#
# Refactoring Studio for C - User Interface
#
# (C) Copyright 2001, Diomidis Spinellis
#
# $Id: rsc.tcl,v 1.4 2001/09/25 11:13:21 dds Exp $
#

package require Iwidgets 3.0

wm title . "Refactoring Studio for C"

# Status bar
label .status -textvariable statusVar -width 40 -anchor w
pack .status -side bottom

# Menu
menu .menu -tearoff 0

set m .menu.file
menu $m -tearoff 0
.menu add cascade -label "File" -menu $m -underline 0
$m add command -label "New Workspace"
$m add command -label "Open Workspace..."
$m add command -label "Open File..."
$m add command -label "Save Workspace"
$m add command -label "Save Workspace As..."
$m add command -label "Save Changes"
$m add separator
$m add command -label "Print Setup..."
$m add separator
$m add command -label "Recent Files"
$m add command -label "Recent Workspaces"
$m add separator
$m add command -label "Exit"

set m .menu.edit
menu $m -tearoff 0
.menu add cascade -label "Edit" -menu $m -underline 0
$m add command -label "Undo"
$m add command -label "Redo"
$m add command -label "Undo All"
$m add separator
$m add command -label "Copy Matched List"
$m add separator
$m add command -label "Find"
$m add command -label "Add to Find"
$m add command -label "Replace"
$m add separator
$m add command -label "Remove File"
$m add command -label "Remove File from All Projects"
$m add command -label "Remove Project"

set m .menu.view
menu $m -tearoff 0
.menu add cascade -label "View" -menu $m -underline 0
$m add command -label "Workspace Files"
$m add command -label "Matched Files"
$m add separator
$m add command -label "Contents"
$m add command -label "Settings"
$m add command -label "Output"
$m add command -label "Dependencies"
$m add command -label "Unused"

set m .menu.insert
menu $m -tearoff 0
.menu add cascade -label "Insert" -menu $m -underline 0
$m add command -label "Project"
$m add separator
$m add command -label "File to Project"
$m add command -label "File to all Projects"


set m .menu.tools
menu $m -tearoff 0
.menu add cascade -label "Tools" -menu $m -underline 0
	set m2 .menu.tools.report
	menu $m2 -tearoff 0
	$m2 add command -label "Matched Files"
	$m2 add command -label "Source File"
	$m2 add command -label "Output"
	$m2 add command -label "Dependencies"
	$m2 add command -label "Unused"
$m add cascade -label "Report" -menu $m2 -underline 0
$m add command -label "Report Options"
$m add separator
$m add command -label "Display Options"
$m add command -label "Workspace Settings"
$m add command -label "Editing Options"

set m .menu.help
menu $m -tearoff 0
.menu add cascade -label "Help" -menu $m -underline 0
$m add command -label "About"

. configure -menu .menu

iwidgets::toolbar .tb -helpvariable statusVar -orient horizontal
pack .tb -side top -anchor nw

#set imagedir [file join ${iwidgets::library} demos images]
set imagedir "."

.tb add button new \
    -image [image create photo -file [file join $imagedir new.gif]] \
    -helpstr "Create a new workspace" \
    -balloonstr "New Workspace"

.tb add button open \
    -image [image create photo -file [file join $imagedir open.gif]] \
    -helpstr "Open workspace or file" \
    -balloonstr "Open"

.tb add button savewp \
    -image [image create photo -file [file join $imagedir save.gif]] \
    -helpstr "Save current workspace" \
    -balloonstr "Save Workspace"

.tb add button savech \
    -image [image create photo -file [file join $imagedir savech.gif]] \
    -helpstr "Save changes made to workspace files" \
    -balloonstr "Save Changes"

.tb add frame filler1  -borderwidth 1 -width 10 -height 10

.tb add button find \
    -image [image create photo -file [file join $imagedir find.gif]] \
    -helpstr "Find files using identifier under cursor" \
    -balloonstr "Find Files"

.tb add button findadd \
    -image [image create photo -file [file join $imagedir findadd.gif]] \
    -helpstr "Add to matched list files using identifier under cursor" \
    -balloonstr "Add to Found Files"

.tb add button copy \
    -image [image create photo -file [file join $imagedir copy.gif]] \
    -helpstr "Copy matched list to the clipboard" \
    -balloonstr "Copy"

.tb add frame filler2  -borderwidth 1 -width 10 -height 10

.tb add button replace \
    -image [image create photo -file [file join $imagedir replace.gif]] \
    -helpstr "Replace all instances of identifier under cursor" \
    -balloonstr "Replace"

.tb add button undo \
    -image [image create photo -file [file join $imagedir undo.gif]] \
    -helpstr "Undo global replace" \
    -balloonstr "Undo"

.tb add button redo \
    -image [image create photo -file [file join $imagedir redo.gif]] \
    -helpstr "Redo undone global replace" \
    -balloonstr "Redo"


iwidgets::panedwindow .panes -width 9i -height 5i -orient vertical
pack .panes -expand yes -fill both

.panes add "files"
set files [.panes childsite "files"]


iwidgets::tabnotebook $files.l -tabpos n
set tabfiles [$files.l add -label "Workspace"]
set tabmatches [$files.l add -label "Matches"]
pack $files.l -side left -expand yes -fill both

.panes add "out"
set out [.panes childsite "out"]

iwidgets::tabnotebook $out.l -tabpos n
set tabmatches [$out.l add -label "Contents"]
set tabmatches [$out.l add -label "Dependencies"]
set tabmatches [$out.l add -label "Unused"]
set tabmatches [$out.l add -label "Settings"]
set tabmatches [$out.l add -label "Output"]
set tabmatches [$out.l add -label "Statistics"]
pack $out.l -side left -expand yes -fill both
 
.panes fraction 25 75

