#
# Refactoring Studio for C - User Interface
#
# (C) Copyright 2001, Diomidis Spinellis
#
# $Id: rsc.tcl,v 1.6 2001/09/27 12:35:18 dds Exp $
#

package require Iwidgets 3.0

wm title . "Refactoring Studio for C"


######################################################
# Status bar
label .status -textvariable statusVar -width 40 -anchor w
pack .status -side bottom

menu .menu -tearoff 0

######################################################
# Global widget options
option add *Tabnotebook.backdrop [ . cget -background ]
option add *Tabnotebook.background [ . cget -background ]
option add *Tabnotebook.tabBackground [ . cget -background ]
option add *Tabnotebook.font [ .menu cget -font ]
option add *Tabnotebook.angle 10
option add *Tabnotebook.bevelAmount 2
option add *Tabnotebook.equalTabs false
option add *Tabnotebook.raiseSelect true

option add *Scrolledlistbox.vscrollMode dynamic
option add *Scrolledlistbox.hscrollMode dynamic
option add *Scrolledlistbox.relief sunken
option add *Scrolledlistbox.borderWidth 2
option add *Scrolledlistbox.textBackground white

option add *Hierarchy.relief sunken
option add *Hierarchy.borderWidth 2
option add *Hierarchy.vscrollMode dynamic
option add *Hierarchy.hscrollMode dynamic

option add *Scrolledtext.vscrollMode dynamic
option add *Scrolledtext.hscrollMode dynamic
option add *Scrolledtext.relief sunken
option add *Scrolledtext.borderWidth 2

######################################################
# Menu
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
$m add separator
$m add command -label "Rename Project"

set m .menu.view
menu $m -tearoff 0
.menu add cascade -label "View" -menu $m -underline 0
$m add command -label "Workspace Files"
$m add command -label "Matched Files"
$m add separator
$m add command -label "Contents"
$m add command -label "Dependencies"
$m add command -label "Unused"
$m add command -label "Settings"
$m add command -label "Output"
$m add command -label "Statistics"

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

######################################################
# Toolbar
iwidgets::toolbar .tb -helpvariable statusVar \
-orient horizontal \
-balloonfont [ .menu cget -font ] \
-balloonbackground white \
-borderwidth 2

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

######################################################
# The two vertical panes
iwidgets::panedwindow .panes -width 9i -height 5i -orient vertical
pack .panes -expand yes -fill both

.panes add "files"
set files [.panes childsite "files"]


######################################################
# The tabbed areas
iwidgets::tabnotebook $files.l -tabpos n
set tabfiles [$files.l add -label "Workspace"]
set tabmatches [$files.l add -label "Matches"]
pack $files.l -side left -expand yes -fill both


.panes add "out"
set out [.panes childsite "out"]

iwidgets::tabnotebook $out.l -tabpos n
set tabcontents [$out.l add -label "Contents"]
set tabdependencies [$out.l add -label "Dependencies"]
set tabunused [$out.l add -label "Unused"]
set tabsettings [$out.l add -label "Settings"]
set taboutput [$out.l add -label "Output"]
set tabstatistics [$out.l add -label "Statistics"]
pack $out.l -side left -expand yes -fill both
 
.panes fraction 25 75

######################################################
# Workspace Files
iwidgets::hierarchy $tabfiles.hier -querycommand "get_workspace %n"
pack $tabfiles.hier -expand yes -fill both

######################################################
# Matched Files
iwidgets::scrolledlistbox $tabmatches.list
pack $tabmatches.list -expand yes -fill both

######################################################
# Contents
iwidgets::scrolledtext $tabcontents.text -wrap none
pack $tabcontents.text -side left -expand yes -fill both


######################################################
# Dependencies
label $tabdependencies.label -text "External dependencies of project or file"

frame $tabdependencies.select
radiobutton $tabdependencies.select.symbol -text "List by symbol" -variable size \
    -relief flat -value "path"
radiobutton $tabdependencies.select.file -text "List by file" -variable size \
    -relief flat -value "macro"
pack $tabdependencies.select.symbol $tabdependencies.select.file -side left -pady 2

iwidgets::hierarchy $tabdependencies.hier -visibleitems 30x15 

pack $tabdependencies.label  -side top -pady 2 -anchor nw
pack $tabdependencies.select -side top -pady 2 -anchor nw
pack $tabdependencies.hier -expand true -fill both -side top -pady 2 -anchor nw

######################################################
# Unused
label $tabunused.label -text "Unused symbols in workspace, project, or file"

frame $tabunused.select
radiobutton $tabunused.select.symbol -text "Order by symbol" -variable size \
    -relief flat -value "path"
radiobutton $tabunused.select.file -text "Order by file name" -variable size \
    -relief flat -value "macro"
pack $tabunused.select.symbol $tabunused.select.file -side left -pady 2

iwidgets::scrolledlistbox $tabunused.hier

pack $tabunused.label  -side top -pady 2 -anchor nw
pack $tabunused.select -side top -pady 2 -anchor nw
pack $tabunused.hier -expand true -fill both -side top -pady 2 -anchor nw

######################################################
# Settings
frame $tabsettings.select
radiobutton $tabsettings.select.path -text "Include paths" -variable size \
    -relief flat -value "path"
radiobutton $tabsettings.select.macro -text "Macro definitions" -variable size \
    -relief flat -value "macro"
pack $tabsettings.select.macro $tabsettings.select.path -side left -pady 2

frame $tabsettings.listgroup
iwidgets::scrolledlistbox $tabsettings.listgroup.list \
    -selectmode single 
$tabsettings.listgroup.list insert 0 "/usr/include"

frame $tabsettings.listgroup.commands
frame $tabsettings.listgroup.commands.add
button $tabsettings.listgroup.commands.add.command -text "Add" -width 10
entry $tabsettings.listgroup.commands.add.entry -text "Add" 
pack	$tabsettings.listgroup.commands.add.command -side left -anchor e
pack	$tabsettings.listgroup.commands.add.entry -expand true -fill x -padx 4 -side left -anchor e
button $tabsettings.listgroup.commands.moveup -text "Move Up" -width 10
button $tabsettings.listgroup.commands.movedown -text "Move Down" -width 10
button $tabsettings.listgroup.commands.delete -text "Delete" -width 10
button $tabsettings.listgroup.commands.reset -text "Reset" -width 10
button $tabsettings.listgroup.commands.clearall -text "Clear All" -width 10

pack  $tabsettings.listgroup.commands.add  -expand true -fill x \
	-side top -padx 4 -pady 4 -anchor w
pack $tabsettings.listgroup.commands.moveup \
	$tabsettings.listgroup.commands.movedown \
	$tabsettings.listgroup.commands.delete \
	$tabsettings.listgroup.commands.reset \
	$tabsettings.listgroup.commands.clearall \
	-side top -padx 4 -pady 4 -anchor w

pack $tabsettings.select -side top -padx 4 -pady 4 -anchor nw
pack $tabsettings.listgroup.list -expand true -fill both -side left -padx 4 -pady 4 -anchor nw
pack $tabsettings.listgroup.commands -side left -padx 4 -pady 4 -anchor nw -expand true -fill x
checkbutton $tabsettings.ro -text "File is read-only" -relief flat
button $tabsettings.clearsub -text "Clear Subitem Settings" 
pack $tabsettings.listgroup -expand true -fill both -side top -padx 4 -pady 4 -anchor nw
pack $tabsettings.ro $tabsettings.clearsub -side top -padx 4 -pady 4 -anchor nw

######################################################
# Output
iwidgets::scrolledtext $taboutput.text -wrap none
pack $taboutput.text -side left -expand yes -fill both

######################################################
# Subroutines

proc get_workspace {uid} {
	if {$uid == ""} {
		return {
			# uid Text branch/leaf
			{wp	"Workspace" branch} 
			{dep	"Dependent Files" branch} 
			{ro	"Read-only Files" branch}
			{wr	"Writable Files" branch}
			{edit	"Edited Files" branch}
			{all	"All Files" branch}
			{int	"Internal Settings" leaf}
		};
	} else {
		return "";
	}
}
