#
# Refactoring Studio for C - User Interface
#
# (C) Copyright 2001, Diomidis Spinellis
#
# $Id: rsc.tcl,v 1.17 2001/09/30 08:23:19 dds Exp $
#

#tk_messageBox -icon info -message "Debug" -type ok

package require Iwidgets 3.0

set workspace_filename "Untitled"



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
$m add command -label "Open Workspace..." -command open_workspace
$m add command -label "Open File..."
$m add command -label "Save Workspace" -command save_workspace
$m add command -label "Save Workspace As..." -command save_workspace_as
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
$m add command -label "Clone Project"

set m .menu.view
menu $m -tearoff 0
.menu add cascade -label "View" -menu $m -underline 0
$m add command -label "Workspace" -command {$files.l select Workspace}
$m add command -label "Matches" -command {$files.l select Matches}
$m add separator
$m add command -label "Contents" -command {$out.l select Contents}
$m add command -label "Dependencies" -command {$out.l select Dependencies}
$m add command -label "Unused" -command {$out.l select Unused}
$m add command -label "Settings" -command {$out.l select Settings}
$m add command -label "Output" -command {$out.l select Output}
$m add command -label "Statistics" -command {$out.l select Statistics}

set m .menu.insert
menu $m -tearoff 0
.menu add cascade -label "Insert" -menu $m -underline 0
$m add command -label "Project" -command insert_project
$m add separator
$m add command -label "File to Project" -command insert_file
$m add command -label "Directory Files to Project" -command insert_dir_files
$m add command -label "Hierarchy Files to Project"
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
    -balloonstr "Save Workspace" \
    -command save_workspace

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
# Warning: the names are used in the view menu
set tabfiles [$files.l add -label "Workspace"]
set tabmatches [$files.l add -label "Matches"]
pack $files.l -side left -expand yes -fill both

$files.l select Workspace

.panes add "out"
set out [.panes childsite "out"]

iwidgets::tabnotebook $out.l -tabpos n
# Warning: the names are used in the view menu
set tabcontents [$out.l add -label "Contents"]
set tabdependencies [$out.l add -label "Dependencies"]
set tabunused [$out.l add -label "Unused"]
set tabsettings [$out.l add -label "Settings"]
set taboutput [$out.l add -label "Output"]
set tabstatistics [$out.l add -label "Statistics"]
pack $out.l -side left -expand yes -fill both
 
$out.l select Settings

.panes fraction 25 75

######################################################
# Workspace Files
iwidgets::hierarchy $tabfiles.hier -querycommand "get_workspace %n" -alwaysquery 1 \
-selectbackground blue -selectcommand "select_workspace {%n} %s"
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
    -relief flat -value "symbol" -variable dependlist
radiobutton $tabdependencies.select.file -text "List by file" -variable size \
    -relief flat -value "file" -variable dependlist
pack $tabdependencies.select.symbol $tabdependencies.select.file -side left -pady 2
set dependlist symbol

iwidgets::hierarchy $tabdependencies.hier -visibleitems 30x15 

pack $tabdependencies.label  -side top -pady 2 -anchor nw
pack $tabdependencies.select -side top -pady 2 -anchor nw
pack $tabdependencies.hier -expand true -fill both -side top -pady 2 -anchor nw

######################################################
# Unused
label $tabunused.label -text "Unused symbols in workspace, project, or file"

frame $tabunused.select
radiobutton $tabunused.select.symbol -text "Order by symbol" -variable size \
    -relief flat -value "symbol" -variable unusedorder
radiobutton $tabunused.select.file -text "Order by file name" -variable size \
    -relief flat -value "name" -variable unusedorder
pack $tabunused.select.symbol $tabunused.select.file -side left -pady 2
set unusedorder symbol

iwidgets::scrolledlistbox $tabunused.hier

pack $tabunused.label  -side top -pady 2 -anchor nw
pack $tabunused.select -side top -pady 2 -anchor nw
pack $tabunused.hier -expand true -fill both -side top -pady 2 -anchor nw

######################################################
# Settings

# Directory selection
frame $tabsettings.dir
label $tabsettings.dir.lab -text "Working directory" -anchor e
label $tabsettings.dir.ent -width 20 \
	-relief sunken -borderwidth 2 -anchor w
#-state disabled
button $tabsettings.dir.but -text "Set ..." -command set_entry_directory
pack $tabsettings.dir.lab -side left
pack $tabsettings.dir.ent -side left -expand yes -fill x
pack $tabsettings.dir.but -padx 4 -side left

# Macro and include file group
frame $tabsettings.mi

# Macros
frame $tabsettings.mi.macro
label $tabsettings.mi.macro.lab -text "Macro definitions" -anchor w

iwidgets::scrolledlistbox $tabsettings.mi.macro.list \
    -selectmode single 

frame $tabsettings.mi.macro.commands
button $tabsettings.mi.macro.commands.add -text "Add" -width 10 -command macro_add
button $tabsettings.mi.macro.commands.delete -text "Delete" -width 10 -command macro_delete
pack	$tabsettings.mi.macro.commands.add $tabsettings.mi.macro.commands.delete \
	-side left -anchor e -expand no

pack	$tabsettings.mi.macro.lab \
	-side top -anchor nw -expand no
pack	$tabsettings.mi.macro.list \
	-side top -anchor nw \
	-expand yes -fill both
pack	$tabsettings.mi.macro.commands \
	-side top -anchor nw -expand no

# Include path
frame $tabsettings.mi.ipath
label $tabsettings.mi.ipath.lab -text "Include paths" -anchor e

iwidgets::scrolledlistbox $tabsettings.mi.ipath.list \
    -selectmode single 

frame $tabsettings.mi.ipath.commands
button $tabsettings.mi.ipath.commands.add -text "Add" -width 10 -command ipath_add
button $tabsettings.mi.ipath.commands.delete -text "Delete" -width 10 -command ipath_delete
pack	$tabsettings.mi.ipath.commands.add $tabsettings.mi.ipath.commands.delete \
	-side left -anchor e -expand no
button $tabsettings.mi.ipath.commands.moveup -text "Move Up" -width 10
button $tabsettings.mi.ipath.commands.movedown -text "Move Down" -width 10
pack	$tabsettings.mi.ipath.commands.moveup $tabsettings.mi.ipath.commands.movedown \
	-side left -anchor e -expand no

pack	$tabsettings.mi.ipath.lab \
	-side top -anchor nw -expand no
pack	$tabsettings.mi.ipath.list \
	-side top -anchor nw \
	-expand yes -fill both
pack	$tabsettings.mi.ipath.commands \
	-side top -anchor nw -expand no

# Combine them
pack $tabsettings.mi.macro \
	$tabsettings.mi.ipath \
	-side left -anchor nw \
	-expand yes -fill both -padx 4

# Other file settings
pack $tabsettings.dir -expand no -fill x -side top -padx 4 -pady 4 -anchor nw
pack $tabsettings.mi -side top -padx 4 -pady 4 -anchor nw -expand yes -fill both

checkbutton $tabsettings.ro -text "File is read-only" -relief flat
button $tabsettings.clearme -text "Clear item's customized settings" -width 30 -command entry_clearme
button $tabsettings.clearsub -text "Clear subitems' customized settings" -width 30 -command entry_clearsub
pack $tabsettings.ro $tabsettings.clearme $tabsettings.clearsub -side top -padx 4 -pady 4 -anchor nw

######################################################
# Output
iwidgets::scrolledtext $taboutput.text -wrap none
pack $taboutput.text -side left -expand yes -fill both

######################################################
# Modal dialogs

# Project name
iwidgets::promptdialog .dialog_project_name -title "Insert Project" -modality application \
    -labeltext "Project name:" -separator false
.dialog_project_name hide Apply
.dialog_project_name hide Help

# Macro definition
iwidgets::promptdialog .dialog_macro_define -title "Macro Definition" -modality application \
    -labeltext "#define " -separator false
.dialog_macro_define hide Apply
.dialog_macro_define hide Help

# Include directory path
iwidgets::promptdialog .dialog_ipath_define -title "Include Path" -modality application \
    -labeltext "Directory: " -separator false
.dialog_ipath_define hide Apply
.dialog_ipath_define hide Help

######################################################
# Global variables

# Settings per workspace node (e.g. wp/demo/main.c)
# Read-only
set readonly(wp) 1
# Working directory
set dir(wp) [pwd]
# Preprocessor macros
set macro(wp) {}
# Preprocessor include paths
set ipath(wp) {}
# Workspace entry names (name(wp) isn't really used)
set name(wp) Workspace
# Working directory


######################################################
# Subroutines


# Set window title
proc set_window_title {} {
	global workspace_filename

	wm title . "Refactoring Studio for C - $workspace_filename"
}

# Display an error dialog box with the given mesage
proc ierror {msg} {
	tk_messageBox -icon error -type ok -title "Error" -message $msg
}

# Called when the user clicks on a workspace node
# sel is the selection value of that node
proc select_workspace {uid sel} {
	global tabfiles
	global out

	# Update selection
	$tabfiles.hier selection clear
	$tabfiles.hier selection add $uid

	# Update settings tab
	if {[regexp ^wp $uid]} {
		# A workspace entry has been selected; enable settings
		$out.l pageconfigure Settings -state normal
		settings_refresh
	} else {
		$out.l pageconfigure Settings -state disabled
		$out.l select Output
	}

	# Enable/disable project menus based on selection
	if {[regexp {^wp/[^/]*$} $uid]} {
		set projstate normal
	} else {
		set projstate disabled
	}
	.menu.edit entryconfigure "Remove Project" -state $projstate
	.menu.edit entryconfigure "Rename Project" -state $projstate
	.menu.insert entryconfigure "File to Project" -state $projstate
	.menu.insert entryconfigure "Directory Files to Project" -state $projstate
	.menu.insert entryconfigure "Hierarchy Files to Project" -state $projstate
}


# Called to populate the workspace hierarchy
proc get_workspace {uid} {
	global name
	global tabfiles
	if {$uid == ""} {
			# uid Text branch/leaf
		return {
			{wp	"Workspace" branch} 
			{dep	"Dependent Files" branch} 
			{ro	"Read-only Files" branch}
			{wr	"Writable Files" branch}
			{edit	"Edited Files" branch}
			{all	"All Files" branch}
			{int	"Internal Settings" leaf}
		}
	} elseif {$uid == "wp"} {
		set r {}
		foreach i [array names name] {
			if {[regexp {^wp/[^/]*$} $i]} {
				lappend r [list $i $name($i) branch]
			}
		}
		return $r
	} elseif {[regexp {^wp/[^/]*$} $uid]} {
		# uid is of the type wp/project
		set r {}
		foreach i [array names name] {
			if {[regexp "^$uid/" $i]} {
				lappend r [list $i $name($i) branch]
			}
		}
		return $r
	} else {
		return ""
	}
}


# Called from the menu insert_project command
proc insert_project {} {
	global name
	global tabfiles
	if {[.dialog_project_name activate]} {
		# Invoke dialog box to get the project's name
		set projname [.dialog_project_name get]
		if {[info exists name(wp/$projname)]} {
			ierror "Project $projname is already defined in this workspace"
			return
		}
		if {[regexp {/} $projname]} {
			ierror "Project names can not contain an embedded slash"
			return
		}
		set name(wp/$projname) $projname
		# Expand is needed to internally refresh _nodes so that we do not
		# get a node does not exist error!
		$tabfiles.hier expand wp
		$tabfiles.hier refresh wp
	}	
	# else cancelled
}

# Return an entry's parent node
proc parent {entry} {
	regsub {/[^/]*$} $entry {} parent
	return $parent
}

# Clear this entry's customized settings
proc entry_clearme {} {
	global readonly
	global dir
	global macro
	global ipath

	set thisentry [wp_getentry]
	unset readonly($thisentry)
	unset dir($thisentry)
	unset macro($thisentry)
	unset ipath($thisentry)
	settings_refresh
}

# Clear customized settings of all subentries
proc entry_clearsub {} {
	global readonly
	global dir
	global macro
	global ipath
	global name

	set thisentry [wp_getentry]
	foreach i [array names name] {
		if {[regexp "^$thisentry/" $i] && [info exists macro($i)]} {
			unset readonly($i)
			unset dir($i)
			unset macro($i)
			unset ipath($i)
		}
	}
	settings_refresh
}

# Create settings for an entry, independent from its parent
proc emancipate {entry} {
	global readonly
	global dir
	global macro
	global ipath

	if {![info exists macro($entry)]} {
		# So far we have been inheriting our parent; get our own
		set parent [parent $entry]
		if {![info exists macro($parent)]} {
			emancipate $parent
		}
		set readonly($entry) $readonly($parent)
		set dir($entry) $dir($parent)
		set macro($entry) $macro($parent)
		set ipath($entry) $ipath($parent)
	}
}

# Return the current workspace selection
proc wp_getentry {} {
	global tabfiles

	return [lindex [$tabfiles.hier selection get] 0]
}

# Return the current project (assume that a project is selected)
proc wp_getproject {} {
	set entry [wp_getentry]
	regsub {^wp/} $entry {} entry
	regsub {/.*} $entry {} entry
	return $entry
}

# Add a new macro in an entry's settings
proc macro_add {} {
	global macro

	if {[.dialog_macro_define activate]} {
		set entry [wp_getmodsettings]
		lappend macro($entry) [.dialog_macro_define get]
	}
	settings_refresh
}

# Delete a macro definition
proc macro_delete {} {
	global tabsettings
	global macro

	set sel [$tabsettings.mi.macro.list curselection]
	if {$sel != {}} {
		set entry [wp_getmodsettings]
		set macro($entry) [lreplace $macro($entry) $sel $sel]
	}
	settings_refresh
}

# Add a new ipath in an entry's settings
proc ipath_add {} {
	global ipath

	if {[.dialog_ipath_define activate]} {
		set entry [wp_getmodsettings]
		lappend ipath($entry) [.dialog_ipath_define get]
	}
	settings_refresh
}

# Delete a ipath definition
proc ipath_delete {} {
	global tabsettings
	global ipath

	set sel [$tabsettings.mi.ipath.list curselection]
	if {$sel != {}} {
		set entry [wp_getmodsettings]
		set ipath($entry) [lreplace $ipath($entry) $sel $sel]
	}
	settings_refresh
}

# Return the entry to get the settings from
proc wp_getsettings {} {
	global macro

	set entry [wp_getentry]
	while {![info exists macro($entry)]} {
		# We are inheriting our parent
		set entry [parent $entry]
	}
	return $entry
}

# Return an entry for changing settings 
proc wp_getmodsettings {} {
	set entry [wp_getentry]
	emancipate $entry
	return $entry
}

# Refresh the Settings tab contents based on the current workspace selection
proc settings_refresh {} {
	global tabsettings
	global macro
	global ipath
	global dir
	global name

	set entry [wp_getsettings]

	# Update macro definitions
	$tabsettings.mi.macro.list delete 0 end
	foreach i $macro($entry) {
		$tabsettings.mi.macro.list insert end $i
	}

	# Update include path
	$tabsettings.mi.ipath.list delete 0 end
	foreach i $ipath($entry) {
		$tabsettings.mi.ipath.list insert end $i
	}

	# Set directory
	$tabsettings.dir.ent configure -text $dir($entry)
	
	# Enable/disable the clear this entry button
	set thisentry [wp_getentry]
	if {[info exists macro($thisentry)] && $thisentry != {wp}} {
		$tabsettings.clearme configure -state normal
	} else {
		$tabsettings.clearme configure -state disabled
	}
	# Enable/disable the clear subentries button
	$tabsettings.clearsub configure -state disabled
	foreach i [array names name] {
#tk_messageBox -icon info -message "Thisentry:$thisentry i:$i" -type ok
		if {[regexp "^$thisentry/" $i] && [info exists macro($i)]} {
			$tabsettings.clearsub configure -state normal
			break
		}
	}
}

# Various initialization bits and pieces
# Default selection is the workspace
select_workspace wp 0
set_window_title

# Insert file to a project
proc insert_file {} {
	global name
	global tabfiles
	global dir

	set mydir $dir([wp_getsettings])
	set filename [tk_getOpenFile -initialdir $mydir -filetypes {{"C Source Files" {.c}}}]
	if {$filename != ""} {
		set projname [wp_getproject]
		if {$mydir == [string range $filename 0 [expr [string length $mydir] - 1]]} {
			# We can make it relative
			set filename [string range $filename [expr [string length $mydir] + 1] end]
		}
		if {[info exists name(wp/$projname/$filename)]} {
			ierror "File $filename is already used in this project"
			return
		}
		set name(wp/$projname/$filename) $filename
		# Expand is needed to internally refresh _nodes so that we do not
		# get a node does not exist error!
		$tabfiles.hier expand wp/$projname
		$tabfiles.hier refresh wp/$projname
	}	
	# else cancelled
}

# Save current workspace asking used for name
proc save_workspace_as {} {
	set filename [tk_getSaveFile -defaultextension .rsw \
		-filetypes {{"Refactoring Studio Workspaces" {.rsw}}}]
	if {$filename == ""} {
		return
	} else {
		save_workspace_to $filename
		set workspace_filename $filename
		set_window_title
	}
}

# Save current workspace using workspace_filename
proc save_workspace {} {
	global workspace_filename
	if {$workspace_filename == "Untitled"} {
		save_workspace_as
	} else {
		save_workspace_to $workspace_filename
	}
}

# Save current workspace to filename specified
proc save_workspace_to {filename} {
	global readonly
	global dir
	global macro
	global ipath
	global name
	
	set f [open $filename w]
	puts $f "#RSC 1.0 Workspace"
	puts $f [list array set name [array get name]]
	puts $f [list array set readonly [array get readonly]]
	puts $f [list array set dir [array get dir]]
	puts $f [list array set macro [array get macro]]
	puts $f [list array set ipath [array get ipath]]
	close $f
}

# Open workspace
proc open_workspace {} {
	global tabfiles
	global readonly
	global dir
	global macro
	global ipath
	global name
	global workspace_filename

	set filename [tk_getOpenFile -defaultextension .rsw \
		-filetypes {{"Refactoring Studio Workspaces" {.rsw}}}]
	if {$filename == ""} {
		return
	} else {
		source $filename
		set workspace_filename $filename
		set_window_title
		$tabfiles.hier expand wp
		$tabfiles.hier refresh wp
		settings_refresh
	}
}

# Insert directory files to a project
proc insert_dir_files {} {
	global name
	global tabfiles
	global dir

	set mydir $dir([wp_getsettings])
	set mydir [tk_chooseDirectory -title "Directory to search for .c files" \
		-initialdir $mydir -mustexist true]
	if {$mydir != ""} {
		set projname [wp_getproject]
		foreach filename [glob -nocomplain -directory $mydir -types {f} {*.[Cc]}] {
			if {$mydir == [string range $filename 0 [expr [string length $mydir] - 1]]} {
				# We can make it relative
				set filename [string range $filename [expr [string length $mydir] + 1] end]
			}
			if {![info exists name(wp/$projname/$filename)]} {
				set name(wp/$projname/$filename) $filename
			}
		}
		# Expand is needed to internally refresh _nodes so that we do not
		# get a node does not exist error!
		$tabfiles.hier expand wp/$projname
		$tabfiles.hier refresh wp/$projname
	}	
	# else cancelled
}

# Set the current entry's directory
proc set_entry_directory {} {
	global name
	global tabfiles
	global dir

	set mydir $dir([wp_getsettings])
	set newdir [tk_chooseDirectory -title "Working directory" -initialdir $mydir ]
	if {$newdir != ""} {
		set entry [wp_getmodsettings]
		set dir($entry) $newdir
		settings_refresh
	}	
	# else cancelled
}

