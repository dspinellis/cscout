#!/usr/bin/awk -f
#
# Convert the output of ccomp into a CSV file containing
# node-id, node-type, group-id
#

# Output CSV
BEGIN { OFS="," }

# Set the current group
/^\tsubgraph/ { 
	group = substr($2, 6);
	delete output;
	delete type;
}

# Split input into clean fields
{ gsub(/[\[\]"n;T=]/, ""); split($0, fields); }

# node-id type: record the type
fields[2] == "a" || fields[2] == "t" { type[fields[1]] = fields[2]; }

# from -- to: output a CSV record
fields[2] == "--" {
	type_from = type[fields[1]];
	type_to = type[fields[3]];

	# Output the type and group of each node (only once)
	if (!output[fields[1] type_from]++)
		print fields[1], type_from, group;
	if (!output[fields[3] type_to]++)
		print fields[3], type_to, group;
}
