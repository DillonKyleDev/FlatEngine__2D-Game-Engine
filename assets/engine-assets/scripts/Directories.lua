----------------------------------------
----- Engine Directories Locations -----
----------------------------------------

-- These paths provide the Engine with the directories script file to use for whichever build type is needed.
-- Each directories file houses unique values for the specific build type
-- You can but probably shouldn't change these locations, but you're free to change the data inside each of these files to suit your needs, ie. startupProject or colors.lua file used for custom engine coloring

F_DirectoryPaths = {
	runtime = "../assets/engine-assets/scripts/RuntimeDirectories.lua",
	editor = "../assets/engine-assets/scripts/EditorDirectories.lua",
	debug = "../assets/engine-assets/scripts/DebugDirectories.lua",
}