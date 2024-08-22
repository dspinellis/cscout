#!/usr/bin/awk -f
#
# Convert the output of ccomps -x into a CSV file containing
# node-id, node-type, group-id
#

# Output CSV
BEGIN { OFS="," }

# Set the current group
/^graph/ {
	group = substr($2, 6);
	delete output;
}

# Split input into clean fields
{ gsub(/[";]/, ""); split($0, fields); }

# from -- to: output a CSV record
fields[2] == "--" {
	type_from = substr(fields[1], 1, 1);
	type_to = substr(fields[3], 1, 1);

	# Output the type and group of each node (only once)
	if (!output[fields[1] type_from]++)
		print substr(fields[1], 2), type_from, group;
	if (!output[fields[3] type_to]++)
		print substr(fields[3], 2), type_to, group;
}
