---------------------------------------
--- Engine Directories & File Paths ---
---------------------------------------

F_Dirs = {
	root = "FlatEngine",	-- For turning absolute paths into relative paths in FlatEngine::OpenLoadFileExplorer()
	projectDir = "..\\", -- Relative to the solution
	projects = "..\\projects\\"
}

F_Paths = {
	colors = "..\\engine\\scripts\\Colors.lua",
	textures = "..\\engine\\scripts\\Textures.lua",
	cinzelBlack = "..\\engine\\fonts\\Cinzel\\Cinzel-Black.ttf",

	gameStartupProject = "..\\projects\\Clash\\Clash.prj" -- Project that will be started at runtime (includes startup scene)
}