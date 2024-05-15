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
	focusedGameObjectID = -1;
	_autoSave = true;
	physicsSystem = "Euler";
	collisionDetection = "Simple Box";
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

void Project::SetFocusedGameObjectID(long ID)
{
	focusedGameObjectID = ID;
}

long Project::GetFocusedGameObjectID()
{
	return focusedGameObjectID;
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

void Project::SetSceneViewGridStep(Vector2 gridStep)
{
	sceneViewGridStep = gridStep;
}

Vector2 Project::GetSceneViewGridStep()
{
	return sceneViewGridStep;
}

bool Project::AutoSaveOn()
{
	return _autoSave;
}

void Project::SetAutoSave(bool _newAutoSave)
{
	_autoSave = _newAutoSave;
}

void Project::SetPhysicsSystem(std::string system)
{
	physicsSystem = system;
}

std::string Project::GetPhysicsSystem()
{
	return physicsSystem;
}

void Project::SetCollisionDetection(std::string system)
{
	collisionDetection = system;
}

std::string Project::GetCollisionDetection()
{
	return collisionDetection;
}


std::string Project::GetData()
{
	json jsonData = {
		{ "path", path },
		{ "loadedScenePath", loadedScenePath },
		{ "loadedAnimationPath", loadedAnimationPath },
		{ "sceneViewScrollingX", sceneViewScrolling.x },
		{ "sceneViewScrollingY", sceneViewScrolling.y },
		{ "_autoSave", _autoSave },
		{ "physicsSystem", physicsSystem },
		{ "collisionDetection", collisionDetection },
	};

	std::string data = jsonData.dump();
	// Return dumped json object with required data for saving
	return data;
}
