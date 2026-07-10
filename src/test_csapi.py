#!/usr/bin/env python3
#
# (C) Copyright 2026 Diomidis Spinellis
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

import json
import os
import sqlite3
import tempfile
import threading
import unittest
import urllib.request
import urllib.error
from http.server import HTTPServer

import csapi


class TestCsapi(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        # Create a temporary SQLite database
        cls.db_fd, cls.db_path = tempfile.mkstemp()
        csapi._db_path = cls.db_path

        # Populate SQLite database with mock CScout schema and data
        conn = sqlite3.connect(cls.db_path)
        
        conn.execute("""
            CREATE TABLE IDS(
                EID INTEGER PRIMARY KEY,
                NAME TEXT,
                UNUSED INTEGER,
                MACRO INTEGER,
                FUN INTEGER,
                READONLY INTEGER,
                ORDINARY INTEGER,
                CSCOPE INTEGER
            )
        """)
        conn.execute("""
            CREATE TABLE FILES(
                FID INTEGER PRIMARY KEY,
                NAME TEXT,
                RO INTEGER
            )
        """)
        conn.execute("""
            CREATE TABLE TOKENS(
                FID INTEGER,
                EID INTEGER,
                FOFFSET INTEGER
            )
        """)
        conn.execute("""
            CREATE TABLE LINEPOS(
                FID INTEGER,
                FOFFSET INTEGER,
                LNUM INTEGER
            )
        """)
        conn.execute("""
            CREATE TABLE FILEMETRICS(
                FID INTEGER,
                PRECPP INTEGER,
                POSTCPP INTEGER,
                NONSPACE INTEGER,
                STATEMENTS INTEGER,
                FUNCTIONS INTEGER,
                MAXNESTING INTEGER
            )
        """)
        conn.execute("""
            CREATE TABLE FUNCTIONS(
                ID INTEGER PRIMARY KEY,
                NAME TEXT,
                FID INTEGER,
                FOFFSET INTEGER,
                DEFINED INTEGER,
                FILESCOPED INTEGER
            )
        """)
        conn.execute("""
            CREATE TABLE FUNCTIONMETRICS(
                FUNCTIONID INTEGER,
                PRECPP INTEGER,
                POSTCPP INTEGER,
                NONSPACE INTEGER,
                STATEMENTS INTEGER,
                MAXNESTING INTEGER,
                FANOUT INTEGER,
                METRICID INTEGER
            )
        """)
        conn.execute("""
            CREATE TABLE FCALLS(
                SOURCEID INTEGER,
                DESTID INTEGER
            )
        """)
        conn.execute("""
            CREATE TABLE PROJECTS(
                PID INTEGER PRIMARY KEY,
                NAME TEXT
            )
        """)

        # Insert dummy data
        conn.execute("INSERT INTO IDS VALUES (1, 'my_var', 1, 0, 0, 0, 1, 1)")
        conn.execute("INSERT INTO IDS VALUES (2, 'my_func', 0, 0, 1, 0, 1, 0)")
        conn.execute("INSERT INTO IDS VALUES (3, 'macro_name', 0, 1, 0, 0, 0, 0)")

        conn.execute("INSERT INTO FILES VALUES (1, 'main.c', 0)")
        conn.execute("INSERT INTO FILES VALUES (2, 'utils.c', 0)")

        conn.execute("INSERT INTO TOKENS VALUES (1, 1, 10)")
        conn.execute("INSERT INTO TOKENS VALUES (1, 1, 50)")
        conn.execute("INSERT INTO TOKENS VALUES (1, 2, 20)")

        conn.execute("INSERT INTO LINEPOS VALUES (1, 0, 1)")
        conn.execute("INSERT INTO LINEPOS VALUES (1, 15, 2)")
        conn.execute("INSERT INTO LINEPOS VALUES (1, 45, 3)")

        conn.execute("INSERT INTO FILEMETRICS VALUES (1, 100, 150, 80, 10, 1, 2)")

        conn.execute("INSERT INTO FUNCTIONS VALUES (10, 'my_func', 1, 20, 1, 0)")
        conn.execute("INSERT INTO FUNCTIONS VALUES (11, 'static_func', 2, 5, 1, 1)")

        conn.execute("INSERT INTO FUNCTIONMETRICS VALUES (10, 20, 30, 15, 5, 2, 2, 1)")

        conn.execute("INSERT INTO FCALLS VALUES (11, 10)")

        conn.execute("INSERT INTO PROJECTS VALUES (1, 'TestProject')")

        conn.commit()
        conn.close()

        # Start the HTTP server on a dynamically allocated free port
        cls.server = HTTPServer(("127.0.0.1", 0), csapi.Handler)
        cls.port = cls.server.server_address[1]
        cls.server_thread = threading.Thread(target=cls.server.serve_forever)
        cls.server_thread.daemon = True
        cls.server_thread.start()

    @classmethod
    def tearDownClass(cls):
        # Gracefully shut down the server by hitting the /quit endpoint
        try:
            req = urllib.request.Request(f"http://127.0.0.1:{cls.port}/quit")
            with urllib.request.urlopen(req) as response:
                pass
        except Exception:
            pass

        # Stop HTTP server if still running
        try:
            cls.server.shutdown()
            cls.server_thread.join(timeout=2)
        except Exception:
            pass
        # Clean up database file
        try:
            os.close(cls.db_fd)
            os.remove(cls.db_path)
        except Exception:
            pass

    def get_url(self, path):
        return f"http://127.0.0.1:{self.port}{path}"

    def get_json(self, path):
        req = urllib.request.Request(self.get_url(path))
        with urllib.request.urlopen(req) as response:
            self.assertEqual(response.status, 200)
            return json.loads(response.read().decode('utf-8'))

    def test_status(self):
        data = self.get_json("/status")
        self.assertEqual(data["status"], "ok")
        self.assertTrue(data["indexes_ready"])

    def test_identifiers(self):
        data = self.get_json("/identifiers")
        self.assertEqual(len(data), 3)
        
        # Test unused filter
        data_unused = self.get_json("/identifiers?unused=1")
        self.assertEqual(len(data_unused), 1)
        self.assertEqual(data_unused[0]["NAME"], "my_var")

        # Test name search filter
        data_search = self.get_json("/identifiers?name=func")
        self.assertEqual(len(data_search), 1)
        self.assertEqual(data_search[0]["NAME"], "my_func")

        # Test limit and offset paging
        data_paged = self.get_json("/identifiers?limit=1&offset=1")
        self.assertEqual(len(data_paged), 1)

    def test_identifier(self):
        data = self.get_json("/identifier?eid=1")
        self.assertEqual(data["identifier"]["NAME"], "my_var")
        self.assertEqual(len(data["locations"]), 2)
        # Verify LNUM mapping calculation
        self.assertEqual(data["locations"][0]["LNUM"], 1) # offset 10 <= 15
        self.assertEqual(data["locations"][1]["LNUM"], 3) # offset 50 >= 45

        # Check 404 for missing eid
        with self.assertRaises(urllib.error.HTTPError) as cm:
            self.get_json("/identifier?eid=999")
        self.assertEqual(cm.exception.code, 404)

        # Check 400 for missing parameter
        with self.assertRaises(urllib.error.HTTPError) as cm:
            self.get_json("/identifier")
        self.assertEqual(cm.exception.code, 400)

    def test_files(self):
        data = self.get_json("/files")
        self.assertEqual(len(data), 2)
        self.assertEqual(data[0]["NAME"], "main.c")

    def test_filemetrics(self):
        data = self.get_json("/filemetrics?fid=1")
        self.assertEqual(len(data), 1)
        self.assertEqual(data[0]["STATEMENTS"], 10)

    def test_functions(self):
        data = self.get_json("/functions")
        self.assertEqual(len(data), 2)

        # Test filescoped filter
        data_scoped = self.get_json("/functions?filescoped=1")
        self.assertEqual(len(data_scoped), 1)
        self.assertEqual(data_scoped[0]["NAME"], "static_func")

    def test_funmetrics(self):
        data = self.get_json("/funmetrics?fnid=10")
        self.assertEqual(len(data), 1)
        self.assertEqual(data[0]["FANOUT"], 2)

    def test_callers(self):
        data = self.get_json("/callers?fnid=10")
        self.assertEqual(len(data), 1)
        self.assertEqual(data[0]["NAME"], "static_func")

    def test_callees(self):
        data = self.get_json("/callees?fnid=11")
        self.assertEqual(len(data), 1)
        self.assertEqual(data[0]["NAME"], "my_func")

    def test_projects(self):
        data = self.get_json("/projects")
        self.assertEqual(len(data), 1)
        self.assertEqual(data[0]["NAME"], "TestProject")

    def test_refactor_preview(self):
        data = self.get_json("/refactor/preview?eid=1&newname=renamed_var")
        self.assertEqual(data["eid"], 1)
        self.assertEqual(data["old_name"], "my_var")
        self.assertEqual(data["new_name"], "renamed_var")
        self.assertEqual(data["total_replacements"], 2)
        self.assertEqual(len(data["locations"]), 2)


if __name__ == "__main__":
    unittest.main()
