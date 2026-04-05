#!/usr/bin/env python3
"""Convert CScout custom XML documentation to Markdown.

Reads index.xml for chapter ordering and converts each XML doc file
to GitHub-Flavored Markdown. The CScout docs use SGML-style HTML with
unclosed tags (<p>, <li>, etc.), so we use Python's html.parser which
handles these gracefully.

Usage:
    python3 xml2md.py [--outdir docs/markdown]
"""

import argparse
import re
import xml.etree.ElementTree as ET
from html.parser import HTMLParser
from pathlib import Path


# Map fmtcode ext attributes to markdown language identifiers
LANG_MAP = {
    "c": "c",
    "C": "c",
    "h": "c",
    "cs": "bash",
    "sh": "bash",
    "bash": "bash",
    "SQL": "sql",
    "sql": "sql",
    "py": "python",
    "perl": "perl",
    "pl": "perl",
    "xml": "xml",
    "make": "makefile",
    "Makefile": "makefile",
    "java": "java",
    "dot": "dot",
}

# HTML entities that appear in the docs
ENTITIES = {
    "&mdash;": "\u2014",
    "&ndash;": "\u2013",
    "&nbsp;": "\u00A0",
    "&bull;": "\u2022",
    "&copy;": "\u00A9",
    "&laquo;": "\u00AB",
    "&raquo;": "\u00BB",
    "&hellip;": "\u2026",
}


class XMLToMarkdownConverter(HTMLParser):
    """Convert CScout's SGML-style XML to Markdown using html.parser."""

    def __init__(self):
        super().__init__(convert_charrefs=True)
        self.output = []
        self.tag_stack = []
        self.in_fmtcode = False
        self.fmtcode_lang = ""
        self.fmtcode_content = []
        self.in_pre = False
        self.pre_content = []
        self.list_depth = 0
        self.in_table = False
        self.current_row = []
        self.table_rows = []
        self.is_header_row = False
        self.suppress_output = False

    def _write(self, text):
        if self.in_fmtcode:
            self.fmtcode_content.append(text)
        elif self.in_pre:
            self.pre_content.append(text)
        elif not self.suppress_output:
            self.output.append(text)

    def handle_starttag(self, tag, attrs):
        attrs_dict = dict(attrs)
        tag = tag.lower()
        self.tag_stack.append(tag)

        if tag == "notes":
            pass  # Root wrapper, ignore
        elif tag == "em":
            self._write("*")
        elif tag == "strong":
            self._write("**")
        elif tag == "code":
            self._write("`")
        elif tag == "fmtcode":
            ext = attrs_dict.get("ext", "")
            self.fmtcode_lang = LANG_MAP.get(ext, ext)
            self.in_fmtcode = True
            self.fmtcode_content = []
        elif tag == "pre":
            self.in_pre = True
            self.pre_content = []
        elif tag == "a":
            href = attrs_dict.get("href", "")
            name = attrs_dict.get("name", "")
            if name:
                self._write(f'<a id="{name}"></a>')
            elif href:
                self._link_href = re.sub(r"\.html\b", ".md", href)
                self._link_text = []
                self.suppress_output = True
        elif tag == "h2":
            self._write("\n## ")
        elif tag == "h3":
            self._write("\n### ")
        elif tag == "h4":
            self._write("\n#### ")
        elif tag == "p":
            self._write("\n\n")
        elif tag == "br":
            self._write("  \n")
        elif tag == "hr":
            self._write("\n---\n")
        elif tag in ("ul", "ol"):
            self.list_depth += 1
            self._write("\n")
        elif tag == "li":
            indent = "  " * (self.list_depth - 1)
            # Check parent list type
            parent_is_ol = any(t == "ol" for t in self.tag_stack[:-1])
            if parent_is_ol:
                self._write(f"{indent}1. ")
            else:
                self._write(f"{indent}- ")
        elif tag == "dl":
            self._write("\n")
        elif tag == "dt":
            self._write("\n**")
        elif tag == "dd":
            self._write(": ")
        elif tag == "table":
            self.in_table = True
            self.table_rows = []
        elif tag == "tr":
            self.current_row = []
            self.is_header_row = False
        elif tag == "th":
            self.is_header_row = True
            self._cell_content = []
            self.suppress_output = True
        elif tag == "td":
            self._cell_content = []
            self.suppress_output = True
        elif tag == "img":
            src = attrs_dict.get("src", "")
            alt = attrs_dict.get("alt", "")
            self._write(f"![{alt}]({src})")
        elif tag in ("font", "select", "option", "input", "form",
                      "thead", "tbody"):
            pass  # Ignored structural/styling tags

    def handle_endtag(self, tag):
        tag = tag.lower()

        if tag == "notes":
            pass
        elif tag == "em":
            self._write("*")
        elif tag == "strong":
            self._write("**")
        elif tag == "code":
            self._write("`")
        elif tag == "fmtcode":
            code = "".join(self.fmtcode_content).strip()
            self.in_fmtcode = False
            self._write(f"\n```{self.fmtcode_lang}\n{code}\n```\n")
        elif tag == "pre":
            code = "".join(self.pre_content).strip()
            self.in_pre = False
            self._write(f"\n```\n{code}\n```\n")
        elif tag == "a":
            if hasattr(self, "_link_href") and self.suppress_output:
                link_text = "".join(self._link_text).strip()
                self.suppress_output = False
                self._write(f"[{link_text}]({self._link_href})")
                del self._link_href
                del self._link_text
        elif tag in ("h2", "h3", "h4"):
            self._write("\n")
        elif tag == "li":
            self._write("\n")
        elif tag in ("ul", "ol"):
            self.list_depth = max(0, self.list_depth - 1)
            self._write("\n")
        elif tag == "dt":
            self._write("**")
        elif tag == "dd":
            self._write("\n")
        elif tag in ("th", "td"):
            cell_text = "".join(self._cell_content).strip()
            cell_text = cell_text.replace("\n", " ")
            self.current_row.append(cell_text)
            self.suppress_output = False
        elif tag == "tr":
            if self.current_row:
                self.table_rows.append(
                    (list(self.current_row), self.is_header_row)
                )
        elif tag == "table":
            self.in_table = False
            self._write(self._render_table())

        # Pop from stack
        if self.tag_stack and self.tag_stack[-1] == tag:
            self.tag_stack.pop()

    def handle_data(self, data):
        if self.suppress_output and hasattr(self, "_link_text"):
            self._link_text.append(data)
        elif self.suppress_output and hasattr(self, "_cell_content"):
            self._cell_content.append(data)
        else:
            self._write(data)

    def _render_table(self):
        if not self.table_rows:
            return ""
        lines = []
        header_done = False
        for cells, is_header in self.table_rows:
            line = "| " + " | ".join(cells) + " |"
            lines.append(line)
            if (is_header or not header_done) and not header_done:
                sep = "| " + " | ".join("---" for _ in cells) + " |"
                lines.append(sep)
                header_done = True
        return "\n" + "\n".join(lines) + "\n"

    def get_markdown(self):
        text = "".join(self.output)
        # Clean up excessive whitespace
        text = re.sub(r"\n{3,}", "\n\n", text)
        return text.strip()


def convert_file(xml_path, title):
    """Convert a single XML file to Markdown content."""
    with open(xml_path, "r", encoding="utf-8", errors="replace") as f:
        content = f.read()

    # Replace HTML entities before parsing
    for entity, char in ENTITIES.items():
        content = content.replace(entity, char)
    content = re.sub(r"&#(\d+);", lambda m: chr(int(m.group(1))), content)

    # Remove XML declaration
    content = re.sub(r"<\?xml[^?]*\?>", "", content)

    # Remove HTML comments (but preserve content)
    content = re.sub(r"<!--.*?-->", "", content, flags=re.DOTALL)

    converter = XMLToMarkdownConverter()
    converter.feed(content)

    md_content = converter.get_markdown()
    return f"# {title}\n\n{md_content}\n"


def parse_index(index_path):
    """Parse index.xml and return ordered list of (title, filename) tuples."""
    tree = ET.parse(index_path)
    root = tree.getroot()
    chapters = []
    for ch in root.findall("ch"):
        title = ch.find("ti").text
        filename = ch.find("fi").text
        chapters.append((title, filename))
    return chapters


def generate_toc(chapters):
    """Generate a Markdown table of contents."""
    lines = ["# CScout Documentation\n"]
    lines.append("## Table of Contents\n")
    for i, (title, filename) in enumerate(chapters, 1):
        md_filename = f"{i:02d}-{filename}.md"
        lines.append(f"{i}. [{title}]({md_filename})")
    lines.append("")
    return "\n".join(lines)


def convert_manpage(man_path, title):
    """Convert a troff man page to Markdown."""
    with open(man_path, "r", encoding="utf-8") as f:
        content = f.read()

    lines = content.split("\n")
    md_lines = [f"# {title}\n"]

    for line in lines:
        if line.startswith('.\\"') or line.startswith('.\"'):
            continue
        if line.startswith(".TH "):
            continue
        if line.startswith(".SH "):
            section = line[4:].strip().strip('"')
            md_lines.append(f"\n## {section.title()}\n")
            continue
        if line.startswith(".SS "):
            section = line[4:].strip().strip('"')
            md_lines.append(f"\n### {section}\n")
            continue
        if line.startswith(".TP"):
            continue
        if line.startswith(".PP") or line.startswith(".P ") or line == ".":
            md_lines.append("")
            continue
        if line.startswith(".IP"):
            continue
        if line.startswith(".br"):
            md_lines.append("  ")
            continue

        text = line
        text = re.sub(r'\\fB(.*?)\\f[PR]', r'**\1**', text)
        text = re.sub(r'\\fI(.*?)\\f[PR]', r'*\1*', text)
        text = re.sub(r'^\\fB', '**', text)
        text = re.sub(r'^\\fI', '*', text)
        text = re.sub(r'\\f[PR]', '', text)
        text = text.replace("\\-", "-")
        text = text.replace("\\(en", "-")
        text = text.replace("\\e", "\\")
        text = text.replace('\\"', '')

        if text.strip():
            md_lines.append(text)

    return "\n".join(md_lines)


def generate_dbschema_md():
    """Generate the database schema chapter."""
    return """# Schema of the Generated Database

The database schema is automatically generated by running
*CScout* with the `-s` option.
The exact schema depends on the SQL dialect specified
(SQLite, MySQL, PostgreSQL, or HSQLDB).

To view the schema for your setup, run:

```bash
cscout -s sqlite myproject.cs | head -100
```

The generated SQL script creates tables for identifiers and their
properties (IDS), individual token occurrences (TOKENS), projects
in the workspace (PROJECTS), source files and their attributes
(FILES), per-file metrics (FILEMETRICS), file copy relationships
(FILECOPIES), function definitions (FUNCTIONS), per-function
metrics (FUNCTIONMETRICS), function call relationships (FCALLS),
and file inclusion relationships (INCLUDES).

Refer to the [SQL Back-end](33-sql.md) chapter for instructions on
generating and loading the database.
"""


def generate_qeg_md():
    """Generate the SQL query examples chapter."""
    return """# Examples of SQL Queries

Once you have loaded the *CScout*-generated SQL data into a relational
database (see [SQL Back-end](33-sql.md)), you can run queries to
analyze your codebase.

The following query lists functions ordered by their number of
statements.

```sql
SELECT name, nstmt
FROM FUNCTIONS
INNER JOIN FUNCTIONMETRICS ON FUNCTIONS.ID = FUNCTIONMETRICS.FUNCTIONID
ORDER BY nstmt DESC;
```

To find files with the most lines of code:

```sql
SELECT name, nline
FROM FILES
INNER JOIN FILEMETRICS ON FILES.FID = FILEMETRICS.FILEID
ORDER BY nline DESC;
```

To list function call relationships:

```sql
SELECT f1.name AS caller, f2.name AS callee
FROM FCALLS
INNER JOIN FUNCTIONS f1 ON FCALLS.SOURCEID = f1.ID
INNER JOIN FUNCTIONS f2 ON FCALLS.DESTID = f2.ID
ORDER BY f1.name, f2.name;
```

For the full database schema, see
[Schema of the Generated Database](34-dbschema.md).
For instructions on generating and loading the SQL data, see
[SQL Back-end](33-sql.md).
"""


# Map of man page base name to (chapter_number, title)
MAN_PAGES = {
    "cscc": (42, "The cscc Command Manual Page"),
    "cscout": (43, "The cscout Command Manual Page"),
    "cscut": (44, "The cscut Command Manual Page"),
    "csmake": (45, "The csmake Command Manual Page"),
    "csreconst": (46, "The csreconst Command Manual Page"),
    "cssplit": (47, "The cssplit Command Manual Page"),
    "csmerge": (48, "The csmerge Command Manual Page"),
    "cswc": (49, "The cswc Command Manual Page"),
}

# Chapters that are generated rather than converted from XML
GENERATED_CHAPTERS = {
    "dbschema": generate_dbschema_md,
    "qeg": generate_qeg_md,
}


def main():
    parser = argparse.ArgumentParser(
        description="Convert CScout XML documentation to Markdown"
    )
    parser.add_argument(
        "--outdir",
        default="docs/markdown",
        help="Output directory for Markdown files (default: docs/markdown)",
    )
    args = parser.parse_args()

    doc_dir = Path(__file__).parent
    out_dir = doc_dir.parent / args.outdir
    out_dir.mkdir(parents=True, exist_ok=True)

    # Parse index
    index_path = doc_dir / "index.xml"
    chapters = parse_index(index_path)

    # Generate TOC
    toc = generate_toc(chapters)
    toc_path = out_dir / "README.md"
    toc_path.write_text(toc, encoding="utf-8")
    print(f"Generated: {toc_path}")

    # Convert each chapter
    converted = 0
    generated = 0
    skipped = 0
    for i, (title, filename) in enumerate(chapters, 1):
        md_filename = f"{i:02d}-{filename}.md"
        md_path = out_dir / md_filename

        # Check if this chapter has a generator
        if filename in GENERATED_CHAPTERS:
            md_content = GENERATED_CHAPTERS[filename]()
            md_path.write_text(md_content, encoding="utf-8")
            generated += 1
            print(f"  GEN: {md_filename}")
            continue

        # Check if this is a man page
        man_name = filename.removeprefix("man")
        man_path = doc_dir.parent / "man" / f"{man_name}.1"
        if filename.startswith("man") and man_path.exists():
            md_content = convert_manpage(man_path, title)
            md_path.write_text(md_content, encoding="utf-8")
            generated += 1
            print(f"  MAN: {man_path.name} -> {md_filename}")
            continue

        # Convert from XML
        xml_path = doc_dir / f"{filename}.xml"
        if not xml_path.exists():
            print(f"SKIP: {filename}.xml (not found)")
            skipped += 1
            continue

        md_content = convert_file(xml_path, title)
        md_path.write_text(md_content, encoding="utf-8")
        converted += 1
        print(f"  OK: {xml_path.name} -> {md_filename}")

    # Copy image assets and simul.html
    import shutil

    img_count = 0
    for ext in ("*.gif", "*.png"):
        for img in doc_dir.glob(ext):
            shutil.copy2(img, out_dir / img.name)
            img_count += 1

    # Copy simul.html for interactive demo links
    simul_path = doc_dir / "simul.html"
    if simul_path.exists():
        shutil.copy2(simul_path, out_dir / "simul.html")
        print(f"  Copied: simul.html")

    print(f"\nDone: {converted} converted, {generated} generated, "
          f"{skipped} skipped, {img_count} images copied")
    print(f"Output: {out_dir}")
    print(f"\nTo build HTML: cd docs && mkdocs build")
    print(f"To preview:    cd docs && mkdocs serve")


if __name__ == "__main__":
    main()
