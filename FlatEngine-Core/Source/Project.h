#pragma once
#include <string>
#include "Vector2.h"

namespace FlatEngine
{
	class Project
	{
	public:
		Project();
		~Project();

		void SetPath(std::string path);
		std::string GetPath();
		void SetLoadedScenePath(std::string path);
		std::string GetLoadedScenePath();
		void SetRuntimeScene(std::string scene);
		std::string GetRuntimeScene();
		void SetBuildPath(std::string path);
		std::string GetBuildPath();
		void SetCurrentFileDirectory(std::string dirName);
		std::string GetCurrentFileDirectory();
		void SetFocusedGameObjectID(long ID);
		long GetFocusedGameObjectID();
		void SetLoadedPreviewAnimationPath(std::string path);
		std::string GetLoadedPreviewAnimationPath();
		void SetSceneViewScrolling(Vector2 scrolling);
		Vector2 GetSceneViewScrolling();
		void SetSceneViewGridStep(Vector2 gridStep);
		Vector2 GetSceneViewGridStep();
		bool AutoSaveOn();
		void SetAutoSave(bool _newAutoSave);
		void SetResolution(Vector2 resolution);
		Vector2 GetResolution();
		void SetFullscreen(bool _newFullscreen);
		bool IsFullscreen();
		void SetVsyncEnabled(bool _vsync);
		bool IsVsyncEnabled();

		std::string GetData();

	private:
		std::string m_path;
		std::string m_buildPath;
		std::string m_loadedScenePath;
		std::string m_sceneToLoadAtRuntime;
		std::string m_loadedAnimationPath;
		std::string m_currentFileDirectory;
		Vector2 m_sceneViewScrolling;
		Vector2 m_sceneViewGridStep;
		long m_focusedGameObjectID;
		bool m_b_autoSave;
		Vector2 m_resolution;
		bool m_b_fullscreen;
		bool m_b_vsyncEnabled;
		// Add more project properties here
	};
}