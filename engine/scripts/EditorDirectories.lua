---------------------------------------
--- Engine Directories & File Paths ---
---------------------------------------

F_Dirs = {
	root = "FlatEngine",	-- For turning absolute paths into relative paths in FlatEngine::OpenLoadFileExplorer()
	projectDir = "..\\", -- Relative to the solution
	projects = "..\\assets\\projects\\",
	mappingContexts = "..\\assets\\mappingContexts\\",
	prefabs = "..\\assets\\prefabs\\",
	animations = "..\\assets\\animations\\",
	scenes = "..\\assets\\scenes\\",
	scripts = "..\\assets\\scripts\\",	-- in-game lua scripting
	tileTextures = "..\\engine\\images\\tileTextures\\",
	tileSets = "..\\assets\\tileSets\\"
}

F_Paths = {
	colors = "..\\engine\\scripts\\Colors.lua",
	textures = "..\\engine\\scripts\\Textures.lua",
	cinzelBlack = "..\\engine\\fonts\\Cinzel\\Cinzel-Black.ttf",

	gameStartupProject = "..\\assets\\projects\\OwlTower.prj" -- Project that will be started at runtime (includes startup scene)
}