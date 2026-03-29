# Option Files

The link on the right of global options allows you to
save the *CScout* global options into a file.
A directory `.cscout` will be created in the
current directory (if it does not already exist),
and a file named options will be written in it,
listing the options you specified.

When *CScout* starts-up it will attempt to load the `options`
file by searching in
`$CSCOUT_HOME`,
`$HOME/.cscout`, or
`.cscout` in the current directory.

The  `options` file is text based and contains key-value pairs.
The order of the entries is not significant.
This is an example of an `options` file.

```
show_true: 1
show_projects: 1
show_identical_files: 1
show_line_number: 0
tab_width: 8
rename_override_ro: 1
refactor_fun_arg_override_ro: 1
file_icase: 0
entries_per_page: 20
fname_in_context: 1
sort_rev: 0
cgraph_type: s
cgraph_show: n
fgraph_show: n
cgraph_depth: 5
fgraph_depth: 5
cgraph_dot_url: 0
sfile_re_string: sfile_repl_string:
sfile_repl_string: entries_per_page:
start_editor_cmd: start  C:\Progra~1\Vim\vim71\gvim.exe +/"%s" "%s"
```
