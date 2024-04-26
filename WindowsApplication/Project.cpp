#include "Project.h"
#include "json.hpp"

using json = nlohmann::json;
using namespace nlohmann::literals;


Project::Project()
{
	path = "";
	loadedScenePath = "";
	loadedAnimationPath = "";
	sceneViewScrolling = Vector2(0,0);
}

Project::~Project()
{
}

void Project::SetPath(std::string projectPath)
{
	path = projectPath;
}

std::string Project::GetPath()
{
	return path;
}

void Project::SetLoadedScenePath(std::string path)
{
	loadedScenePath = path;
}

std::string Project::GetLoadedScenePath()
{
	return loadedScenePath;
}

void Project::SetLoadedPreviewAnimationPath(std::string path)
{
	loadedAnimationPath = path;
}

std::string Project::GetLoadedPreviewAnimationPath()
{
	return loadedAnimationPath;
}

void Project::SetSceneViewScrolling(Vector2 scrolling)
{
	sceneViewScrolling = scrolling;
}

Vector2 Project::GetSceneViewScrolling()
{
	return sceneViewScrolling;
}

std::string Project::GetData()
{
	json jsonData = {
		{ "path", path },
		{ "loadedScenePath", loadedScenePath },
		{ "loadedAnimationPath", loadedAnimationPath },
		{ "sceneViewScrollingX", sceneViewScrolling.x },
		{ "sceneViewScrollingY", sceneViewScrolling.y },
	};

	std::string data = jsonData.dump();
	// Return dumped json object with required data for saving
	return data;
}
