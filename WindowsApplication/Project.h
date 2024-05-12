#pragma once
#include <string>
#include "Vector2.h"


class Project
{
public:
	Project();
	~Project();

	void SetPath(std::string path);
	std::string GetPath();
	void SetLoadedScenePath(std::string path);
	std::string GetLoadedScenePath();
	void SetFocusedGameObjectID(long ID);
	long GetFocusedGameObjectID();
	void SetLoadedPreviewAnimationPath(std::string path);
	std::string GetLoadedPreviewAnimationPath();
	void SetSceneViewScrolling(Vector2 scrolling);
	Vector2 GetSceneViewScrolling();
	void SetSceneViewGridStep(Vector2 gridStep);
	Vector2 GetSceneViewGridStep();
	std::string GetData();

private:
	std::string path;
	std::string loadedScenePath;
	std::string loadedAnimationPath;
	Vector2 sceneViewScrolling;
	Vector2 sceneViewGridStep;
	long focusedGameObjectID;
	// Add more project properties here
};

