#/bin/sed -f
# 
# Create the version.cpp s-list
#
# $Id: version.sed,v 1.1 2003/06/04 04:39:26 dds Exp $
#
/Id/{
	s/^  *//
	s/^/"/
	s/$/",/
	# Split ident strings to avoid having them replaced during check-in
	s/Id:/" "Id:" "/
	p
}
