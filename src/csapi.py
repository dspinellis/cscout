#!/usr/bin/env python3
#
# (C) Copyright 2008-2026 Diomidis Spinellis
# (C) Copyright 2026 Ujjwal Aggarwal
#
# This file is part of CScout.
#
# CScout is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# CScout is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with CScout.  If not, see <http://www.gnu.org/licenses/>.
#
#
# csapi.py -- REST API server for querying a CScout SQLite database.
#
# After running:
#   cscout -s sqlite project.cs | sqlite3 project.db
# start this server with:
#   csapi project.db
#
# The server listens on localhost:8081 by default and exposes
# CScout analysis results as JSON.
#
# No external dependencies -- uses only the Python standard library.
#

import argparse
import json
import sqlite3
import sys
import threading
from http.server import BaseHTTPRequestHandler, HTTPServer
from urllib.parse import parse_qs, urlparse

class MissingParameterError(ValueError):
    """Exception raised when a required query parameter is missing or invalid."""
    pass


def ensure_index(conn: sqlite3.Connection, table: str, columns: list) -> None:
    """Ensure that an index exists on the specified table and columns.
    
    SQLite DDL statements run in autocommit mode, so no commit is necessary.
    """
    index_name = f"idx_{table.lower()}_{'_'.join(columns).lower()}"
    columns_str = ", ".join(columns)
    conn.execute(f"CREATE INDEX IF NOT EXISTS {index_name} ON {table}({columns_str})")


def get_db() -> sqlite3.Connection:
    """Open a database connection.

    Setting check_same_thread=False allows requests to be queried across
    multiple handler threads.
    """
    conn = sqlite3.connect(_db_path, check_same_thread=False)
    conn.row_factory = sqlite3.Row
    return conn


def rows_to_list(rows) -> list:
    """Convert SQLite Row objects into standard Python dictionaries.

    This ensures they are serializable to JSON when returning data.
    """
    return [dict(r) for r in rows]


def get_param(qs, name, default=None):
    """Get a single string query parameter value by name."""
    vals = qs.get(name)
    return vals[0] if vals else default


def get_bool_param(qs, name):
    """Get a query parameter value and convert it to a boolean."""
    v = get_param(qs, name)
    if v is None:
        return None
    return v.lower() in ("1", "true", "yes")


def get_int_param(qs, name, default):
    """Get a query parameter value and convert it to an integer."""
    v = get_param(qs, name)
    try:
        return int(v) if v is not None else default
    except ValueError:
        return default


def get_required_param(qs, name) -> str:
    """Get a single string query parameter or raise MissingParameterError."""
    v = get_param(qs, name)
    if v is None:
        raise MissingParameterError(f"{name} parameter required")
    return v


def get_required_int_param(qs, name) -> int:
    """Get a query parameter converted to an integer, or raise MissingParameterError."""
    v = get_param(qs, name)
    if v is None:
        raise MissingParameterError(f"{name} parameter required")
    try:
        return int(v)
    except ValueError:
        raise MissingParameterError(f"{name} parameter must be an integer")


def get_paging_params(qs) -> tuple:
    """Get limit and offset parameters for pagination."""
    return get_int_param(qs, "limit", 1000), get_int_param(qs, "offset", 0)


def build_where_clause(qs, filters) -> tuple:
    """Build dynamic WHERE conditions and query parameters from a table of filters.

    filters is a list of tuples: (param_name, column_name, filter_type)
    where filter_type is 'bool', 'str', or 'like'.
    """
    conditions = []
    params = []
    for param_name, sql_cond, param_type in filters:
        if param_type == "bool":
            val = get_bool_param(qs, param_name)
            if val is not None:
                conditions.append(sql_cond)
                params.append(int(val))
        elif param_type == "str":
            val = get_param(qs, param_name)
            if val:
                conditions.append(sql_cond)
                params.append(val)
        elif param_type == "like":
            val = get_param(qs, param_name)
            if val:
                conditions.append(sql_cond)
                params.append(f"%{val}%")
    return conditions, params




class Handler(BaseHTTPRequestHandler):
    """Custom HTTP request handler for the CScout REST API endpoints."""

    def log_message(self, fmt, *args):
        """Override standard logging to suppress console noise.

        The server runs as a background IDE helper, so logging every HTTP
        request is unnecessary and clutters the user's terminal.
        """
        # Suppress per-request access log noise.
        pass

    def send_json(self, data, status=200):
        """Send a JSON response to the client.

        Includes headers for CORS support so the VS Code extension can
        query it directly from the editor environment.
        """
        body = json.dumps(data).encode()
        self.send_response(status)
        self.send_header("Content-Type", "application/json")
        self.send_header("Content-Length", str(len(body)))
        self.send_header("Access-Control-Allow-Origin", "*")
        self.end_headers()
        self.wfile.write(body)

    def send_error_json(self, status, message):
        """Send a JSON formatted error response with the specified status code."""
        self.send_json({"error": message}, status)

    def do_GET(self):
        """Parse query string parameters and route GET requests to the endpoints."""
        parsed = urlparse(self.path)
        path = parsed.path
        qs = parse_qs(parsed.query)

        conn = get_db()

        try:
            if path == "/status":
                self.handle_status(conn, qs)
            elif path == "/quit":
                self.handle_quit()
            elif path == "/identifiers":
                self.handle_identifiers(conn, qs)
            elif path == "/identifier":
                self.handle_identifier(conn, qs)
            elif path == "/files":
                self.handle_files(conn, qs)
            elif path == "/filemetrics":
                self.handle_filemetrics(conn, qs)
            elif path == "/functions":
                self.handle_functions(conn, qs)
            elif path == "/funmetrics":
                self.handle_funmetrics(conn, qs)
            elif path == "/callers":
                self.handle_callers(conn, qs)
            elif path == "/callees":
                self.handle_callees(conn, qs)
            elif path == "/projects":
                self.handle_projects(conn, qs)
            elif path == "/refactor/preview":
                self.handle_refactor_preview(conn, qs)
            else:
                self.send_error_json(404, f"Unknown endpoint: {path}")
        except MissingParameterError as e:
            self.send_error_json(400, str(e))
        except Exception as e:
            self.send_error_json(500, str(e))
        finally:
            conn.close()

    def handle_status(self, conn, qs):
        """Handle /status requests."""
        self.send_json({
            "status": "ok",
            "indexes_ready": True,
        })

    def handle_quit(self):
        """Handle /quit requests to shut down the server gracefully."""
        self.send_json({"status": "quit"})
        # Shut down the server in a separate thread to avoid deadlock.
        threading.Thread(target=self.server.shutdown).start()

    def handle_identifiers(self, conn, qs):
        """Handle /identifiers requests."""
        ensure_index(conn, "IDS", ["NAME"])
        filters = [
            ("unused", "UNUSED = ?", "bool"),
            ("macro", "MACRO = ?", "bool"),
            ("fun", "FUN = ?", "bool"),
            ("readonly", "READONLY = ?", "bool"),
            ("name", "NAME LIKE ?", "like"),
        ]
        conditions, params = build_where_clause(qs, filters)

        if get_bool_param(qs, "should_be_static"):
            conditions.append("READONLY = 0 AND ORDINARY = 1 AND LSCOPE = 1 AND NAME != 'main' AND EID NOT IN (SELECT EID FROM TOKENS GROUP BY EID HAVING COUNT(DISTINCT FID) > 1)")

        limit, offset = get_paging_params(qs)

        where = ("WHERE " + " AND ".join(conditions)
                 if conditions else "")
        params += [limit, offset]
        rows = conn.execute(
            f"SELECT * FROM IDS {where} LIMIT ? OFFSET ?",
            params).fetchall()
        self.send_json(rows_to_list(rows))

    def handle_identifier(self, conn, qs):
        """Handle /identifier requests."""
        ensure_index(conn, "TOKENS", ["EID"])
        ensure_index(conn, "LINEPOS", ["FID", "FOFFSET"])
        eid = get_required_int_param(qs, "eid")
        row = conn.execute(
            "SELECT * FROM IDS WHERE EID = ?",
            (eid,)).fetchone()
        if row is None:
            self.send_error_json(404, "Identifier not found")
            return

        limit, offset = get_paging_params(qs)
        tokens = conn.execute(
            """
            SELECT t.FID, f.NAME AS FILE, t.FOFFSET, l.LNUM
            FROM TOKENS t
            JOIN FILES f ON t.FID = f.FID
            LEFT JOIN LINEPOS l
                   ON l.FID = t.FID AND l.FOFFSET = (
                       SELECT MAX(FOFFSET) FROM LINEPOS
                       WHERE FID = t.FID AND FOFFSET <= t.FOFFSET)
            WHERE t.EID = ?
            ORDER BY t.FID, t.FOFFSET
            LIMIT ? OFFSET ?
            """,
            (eid, limit, offset)).fetchall()
        self.send_json({
            "identifier": dict(row),
            "locations": rows_to_list(tokens),
        })

    def handle_files(self, conn, qs):
        """Handle /files requests."""
        rows = conn.execute(
            "SELECT * FROM FILES ORDER BY NAME").fetchall()
        self.send_json(rows_to_list(rows))

    def handle_filemetrics(self, conn, qs):
        """Handle /filemetrics requests."""
        ensure_index(conn, "FILEMETRICS", ["FID"])
        fid = get_required_int_param(qs, "fid")
        rows = conn.execute(
            "SELECT * FROM FILEMETRICS WHERE FID = ?",
            (fid,)).fetchall()
        if not rows:
            self.send_error_json(404, "File not found")
            return
        self.send_json(rows_to_list(rows))

    def handle_functions(self, conn, qs):
        """Handle /functions requests."""
        ensure_index(conn, "LINEPOS", ["FID", "FOFFSET"])
        filters = [
            ("defined", "DEFINED = ?", "bool"),
            ("filescoped", "FILESCOPED = ?", "bool"),
        ]
        conditions, params = build_where_clause(qs, filters)

        limit, offset = get_paging_params(qs)

        where = ("WHERE " + " AND ".join(conditions)
                 if conditions else "")
        params += [limit, offset]
        rows = conn.execute(
            f"""SELECT f.ID, f.NAME, f.ISMACRO, f.DEFINED, f.DECLARED, f.FILESCOPED, f.FID, f.FOFFSET, f.FANIN, fm.FANOUT, fm.CCYCL1 FROM FUNCTIONS f LEFT JOIN FUNCTIONMETRICS fm ON fm.FUNCTIONID = f.ID AND fm.PRECPP = 0 {where} LIMIT ? OFFSET ?""",
            params).fetchall()
        self.send_json(rows_to_list(rows))

    def handle_funmetrics(self, conn, qs):
        """Handle /funmetrics requests."""
        ensure_index(conn, "FUNCTIONMETRICS", ["FUNCTIONID"])
        fnid = get_required_int_param(qs, "fnid")
        rows = conn.execute(
            "SELECT * FROM FUNCTIONMETRICS WHERE FUNCTIONID = ?",
            (fnid,)).fetchall()
        if not rows:
            self.send_error_json(404, "Function not found")
            return
        self.send_json(rows_to_list(rows))

    def handle_callers(self, conn, qs):
        """Handle /callers requests."""
        ensure_index(conn, "FCALLS", ["DESTID"])
        fnid = get_required_int_param(qs, "fnid")
        rows = conn.execute(
            """
            SELECT f.ID, f.NAME, f.FID, f.FOFFSET,
                   fi.NAME AS FILE
            FROM FCALLS fc
            JOIN FUNCTIONS f ON fc.SOURCEID = f.ID
            JOIN FILES fi ON f.FID = fi.FID
            WHERE fc.DESTID = ?
            ORDER BY f.NAME
            """,
            (fnid,)).fetchall()
        self.send_json(rows_to_list(rows))

    def handle_callees(self, conn, qs):
        """Handle /callees requests."""
        ensure_index(conn, "FCALLS", ["SOURCEID"])
        fnid = get_required_int_param(qs, "fnid")
        rows = conn.execute(
            """
            SELECT f.ID, f.NAME, f.FID, f.FOFFSET,
                   fi.NAME AS FILE
            FROM FCALLS fc
            JOIN FUNCTIONS f ON fc.DESTID = f.ID
            JOIN FILES fi ON f.FID = fi.FID
            WHERE fc.SOURCEID = ?
            ORDER BY f.NAME
            """,
            (fnid,)).fetchall()
        self.send_json(rows_to_list(rows))

    def handle_projects(self, conn, qs):
        """Handle /projects requests."""
        rows = conn.execute(
            "SELECT * FROM PROJECTS").fetchall()
        self.send_json(rows_to_list(rows))

    def handle_refactor_preview(self, conn, qs):
        """Handle /refactor/preview requests."""
        ensure_index(conn, "TOKENS", ["EID"])
        ensure_index(conn, "LINEPOS", ["FID", "FOFFSET"])
        eid = get_required_int_param(qs, "eid")
        newname = get_required_param(qs, "newname")
        id_row = conn.execute(
            "SELECT NAME FROM IDS WHERE EID = ?",
            (eid,)).fetchone()
        if id_row is None:
            self.send_error_json(404, "Identifier not found")
            return
        tokens = conn.execute(
            """
            SELECT t.FID, f.NAME AS FILE, t.FOFFSET, l.LNUM
            FROM TOKENS t
            JOIN FILES f ON t.FID = f.FID
            LEFT JOIN LINEPOS l
                   ON l.FID = t.FID AND l.FOFFSET = (
                       SELECT MAX(FOFFSET) FROM LINEPOS
                       WHERE FID = t.FID AND FOFFSET <= t.FOFFSET)
            WHERE t.EID = ?
            ORDER BY t.FID, t.FOFFSET
            """,
            (eid,)).fetchall()
        self.send_json({
            "eid": eid,
            "old_name": id_row["NAME"],
            "new_name": newname,
            "total_replacements": len(tokens),
            "locations": rows_to_list(tokens),
        })


def main():
    global _db_path

    parser = argparse.ArgumentParser(
        description="REST API server for querying a CScout SQLite database.")
    parser.add_argument("db",
                        help="Path to the CScout SQLite database file")
    parser.add_argument("-p", "--port", type=int, default=8081,
                        help="Port to listen on (default: 8081)")
    args = parser.parse_args()

    _db_path = args.db

    # Verify DB is readable before starting.
    try:
        conn = sqlite3.connect(_db_path)
        conn.execute("SELECT COUNT(*) FROM IDS")
        conn.close()
    except Exception as e:
        print(f"Error: cannot open database {_db_path}: {e}",
              file=sys.stderr)
        sys.exit(1)
    server = HTTPServer(("127.0.0.1", args.port), Handler)
    print(f"CScout API ready at http://127.0.0.1:{args.port}",
          file=sys.stderr)
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        pass


if __name__ == "__main__":
    main()
