#/bin/sed -f
#
# Create the version.cpp s-list
#
# $Id: version.sed,v 1.2 2006/06/02 08:26:50 dds Exp $
#
/Id/{
	s/^  *//
	s/^/"/
	s/.$/",/
	# Split ident strings to avoid having them replaced during check-in
	s/Id:/" "Id:" "/
	p
}
