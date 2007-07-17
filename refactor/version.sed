#/bin/sed -f
#
# Create the version.cpp s-list
#
# $Id: version.sed,v 1.3 2007/07/17 05:26:16 dds Exp $
#
/Id/{
	s/^  *//
	s/^/"/
	s/\(.\)$/\1",/
	# Split ident strings to avoid having them replaced during check-in
	s/Id:/" "Id:" "/
	p
}
