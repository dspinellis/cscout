#/bin/sed -f
#
# Create the version.cpp s-list
#
#
/Id/{
	s/^  *//
	s/^/"/
	s/\(.\)$/\1",/
	# Split ident strings to avoid having them replaced during check-in
	s/Id:/" "Id:" "/
	p
}
