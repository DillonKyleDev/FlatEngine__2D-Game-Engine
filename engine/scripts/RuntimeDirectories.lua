----------------------------------------
--- Runtime Directories & File Paths ---
----------------------------------------

F_Dirs = {
<<<<<<< HEAD
	root = "FlatEngine/",	-- For turning absolute paths into relative paths in FlatEngine::OpenLoadFileExplorer()
	projectDir = "../", -- Relative to the solution
	projects = "../assets/projects/",
	mappingContexts = "../assets/mappingContexts/",
	prefabs = "../assets/prefabs/",
	animations = "assets/animations/",
	scenes = "assets/scenes/",
	scripts = "../assets/scripts/",	-- in-game lua scripting
=======
	root = "FlatEngine",	-- For turning absolute paths into relative paths in FlatEngine::OpenLoadFileExplorer()
	-- If you change the name of the FlatEngine folder or the location of the assets folder, you need to update this value in order to get relative paths from above function
	projectDir = "../", -- Relative to the solution
	projects = "../assets/projects",
	mappingContexts = "../assets/mappingContexts",
	prefabs = "../assets/prefabs",
	scripts = "../assets/scripts",	-- in-game lua scripting
>>>>>>> 24d0864ad64ce468c9379edd41450b523a6108f2
}

F_Paths = {
	colors = "../engine/scripts/Colors.lua",
	textures = "../engine/scripts/Textures.lua",

	gameStartupProject = "../assets/projects/OwlTower.json" -- Project that will be started at runtime (includes startup scene)
}