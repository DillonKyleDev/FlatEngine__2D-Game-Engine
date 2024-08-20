---------------------------------------
--- Engine Directories & File Paths ---
---------------------------------------

F_Dirs = {
	root = "FlatEngine",	-- For turning absolute paths into relative paths in FlatEngine::OpenLoadFileExplorer()
	-- If you change the name of the FlatEngine folder or the location of the assets folder, you need to update this value in order to get relative paths from above function
	projectDir = "../", -- Relative to the solution
	projects = "../assets/engine-assets/projects",
	mappingContexts = "../assets/runtime-assets/mappingContexts",
	prefabs = "../assets/runtime-assets/prefabs",
	scripts = "../assets/runtime-assets/scripts",	-- in-game lua scripting
}

F_Paths = {
	colors = "../assets/engine-assets/scripts/Colors.lua",
	textures = "../assets/engine-assets/scripts/Textures.lua",
}