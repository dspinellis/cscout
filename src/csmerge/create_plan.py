#!/usr/bin/env python
#
# Output a shell script to merge the 32 generated database files into one
#

temp_db_number=0

def prefix_lines(prefix, multiline_string):
    """
    Prefix each line of a multi-line string with the specified string.

    Args:
        prefix (str): The string to prefix each line with.
        multiline_string (str): The multi-line string to process.

    Returns:
        str: The resulting string with each line prefixed.
    """
    lines = multiline_string.splitlines()
    prefixed_lines = [f"{prefix}{line}" for line in lines]
    return '\n'.join(prefixed_lines)

def merge(dbs, indent):
    """Output a plan for merging the specified databases and return the name
    of the merged database.

    Args:
        dbs (list): Databases to merge
        indent (str): Indentation to use

    Returns:
        tuple: A tuple containing two strings
        - str: The code for merging the databases
        - str: The name of the merged database file
    """
    global temp_db_number
    if len(dbs) == 2:
        temp_db_number += 1
        temp_db_name = f"{temp_db_number}.db"
        code = f"""  create_empty {temp_db_name}
  merge {temp_db_name} {dbs[0]}
  merge {temp_db_name} {dbs[1]}
"""
        return (code, temp_db_name)

    midpoint = len(dbs) // 2

    (left_code, left_db_name) = merge(dbs[:midpoint], indent + '  ')
    (right_code, right_db_name) = merge(dbs[midpoint:], indent + '  ')
    code = prefix_lines(indent, f"""(
{left_code}
) &
pid_left=$!

(
{right_code}
) &
pid_right=$!

wait $pid_left
wait $pid_right
merge {left_db_name} {right_db_name}
""")
    return (code, left_db_name)

file_list = [f"file{str(i).zfill(4)}.db" for i in range(1, 33)]
(code, db_name) = merge(file_list, '')
print(f"{code}\necho Result is in {db_name}")
