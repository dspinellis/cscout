#!/bin/sh
#
# $Id: build.sh,v 1.1 2008/08/27 15:57:04 dds Exp $
#
# Create the UMLGraph web pages
#

# For each file appearing in top.html
for file in `gawk -F\" '$4 == "menu" && $6 != ".." {print $6}' top.html`
do
	base=`basename $file .html`
	(
	# Highlight menu marker
	cat top.html |
	sed '\|class="menu" href="'$file'"|s/menu/selmenu/
		s/class="menu" href="doc\//class="menu" href="/'
	if expr $file : doc/ >/dev/null
	then
		source=../doc/$base.xml
		sed -n '/<notes>/,/<\/notes>/{;/notes>/d;s/fmtcode.*>/pre>/;p;}' $source
	else
		source=$file
		cat $file
	fi
	sed "s,XDATE,`ident $source | gawk '{printf "%s %s", $4, $5}'`," bottom.html
	) >build/$base.html
	xml val -d /pub/schema/xhtml1-transitional.dtd build/$base.html
done
