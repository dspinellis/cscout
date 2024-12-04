#!/bin/sh
#
# Create a documentation page with the SQL examples
#

set -eu

cat <<\EOF
<?xml version="1.0" ?>
<notes>
Once data have been uploaded onto a SQL database they can be queried in a
number of different ways.
Here are some example queries.
All queries have been tested with SQLite, but
note that some queries may not run on a particular relational database
engine.
<p />
EOF

for f in ../src/test/sql/*.sql ; do
  sed -n '1s/-- \(.*\)/<h3>\1<\/h3>/p' $f
  echo "<!-- $(basename $f) -->"
  echo '<fmtcode ext="SQL">'
  sed 1,2d $f
  echo '</fmtcode>'
  echo '<p />'
done

echo '</notes>'
