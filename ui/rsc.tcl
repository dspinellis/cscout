#
# Refactoring Studio for C - User Interface
#
# (C) Copyright 2001, Diomidis Spinellis
#
# $Id: rsc.tcl,v 1.2 2001/09/25 08:33:43 dds Exp $
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
pack .tb -side top -anchor nw -padx 4 -pady 4

set imagedir [file join ${iwidgets::library} demos images]

.tb add button select \
    -helpstr "Select drawing elements" \
    -image [image create photo -file [file join $imagedir select.gif]] \
    -balloonstr "Selection tool" \
    -command {puts "tool: select"}

.tb add button magnify \
    -helpstr "Magnify drawing area" \
    -image [image create photo -file [file join $imagedir mag.gif]] \
    -balloonstr "Zoom tool" \
    -command {puts "tool: magnify"}

.tb add button ruler \
    -helpstr "Measure distances on drawing" \
    -image [image create photo -file [file join $imagedir ruler.gif]] \
    -balloonstr "Ruler tool" \
    -command {puts "tool: ruler"}

.tb add frame filler \
    -borderwidth 1 \
    -width 10 \
    -height 10

.tb add button poly \
    -helpstr "Draw a polygon" \
    -image [image create photo -file [file join $imagedir poly.gif]] \
    -balloonstr "Polygon tool" \
    -command {puts "tool: polygon"}

canvas .worksp -width 2i -height 3i \
    -borderwidth 2 -relief sunken -background white
pack .worksp -side right -expand yes -fill both -padx 4 -pady 4

