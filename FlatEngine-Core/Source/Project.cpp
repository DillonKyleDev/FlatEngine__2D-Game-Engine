#include "Project.h"
#include "Window.h"
#include "json.hpp"
#include <SDL.h>

using json = nlohmann::json;
using namespace nlohmann::literals;

namespace FlatEngine
{
	Project::Project()
	{
		m_path = "";
		m_loadedScenePath = "";
		m_loadedAnimationPath = "";
		m_sceneViewScrolling = Vector2(0, 0);
		m_focusedGameObjectID = -1;
		m_b_autoSave = true;
		m_resolution = Vector2(1920, 1080);
		m_b_vsyncEnabled = true;
		m_b_fullscreen = false;
	}

	Project::~Project()
	{
	}

	void Project::SetPath(std::string projectPath)
	{
		m_path = projectPath;
	}

	std::string Project::GetPath()
	{
		return m_path;
	}

	void Project::SetLoadedScenePath(std::string path)
	{
		m_loadedScenePath = path;
	}

	std::string Project::GetLoadedScenePath()
	{
		return m_loadedScenePath;
	}

	void Project::SetBuildPath(std::string path)
	{
		m_buildPath = path;
	}

	std::string Project::GetBuildPath()
	{
		return m_buildPath;
	}

	void Project::SetFocusedGameObjectID(long ID)
	{
		m_focusedGameObjectID = ID;
	}

	long Project::GetFocusedGameObjectID()
	{
		return m_focusedGameObjectID;
	}

	void Project::SetLoadedPreviewAnimationPath(std::string path)
	{
		m_loadedAnimationPath = path;
	}

	std::string Project::GetLoadedPreviewAnimationPath()
	{
		return m_loadedAnimationPath;
	}

	std::string Project::GetRuntimeScene()
	{
		return m_sceneToLoadAtRuntime;
	}

	void Project::SetRuntimeScene(std::string scene)
	{
		m_sceneToLoadAtRuntime = scene;
	}

	void Project::SetSceneViewScrolling(Vector2 scrolling)
	{
		m_sceneViewScrolling = scrolling;
	}

	Vector2 Project::GetSceneViewScrolling()
	{
		return m_sceneViewScrolling;
	}

	void Project::SetSceneViewGridStep(Vector2 gridStep)
	{
		m_sceneViewGridStep = gridStep;
	}

	Vector2 Project::GetSceneViewGridStep()
	{
		return m_sceneViewGridStep;
	}

	bool Project::AutoSaveOn()
	{
		return m_b_autoSave;
	}

	void Project::SetAutoSave(bool b_newAutoSave)
	{
		m_b_autoSave = b_newAutoSave;
	}

	void Project::SetResolution(Vector2 newResolution)
	{
		m_resolution = newResolution;
		//Window::SetScreenDimensions(resolution.x, resolution.y);
		//SDL_bool setIntegerScale = SDL_bool(true);
		//SDL_RenderSetIntegerScale(Window::renderer, setIntegerScale);
		//SDL_RenderSetLogicalSize(Window::renderer, resolution.x, resolution.y);
	}

	Vector2 Project::GetResolution()
	{
		return m_resolution;
	}

	void Project::SetFullscreen(bool b_newFullscreen)
	{
		m_b_fullscreen = b_newFullscreen;
		Window::SetFullscreen(m_b_fullscreen);
	}

	bool Project::IsFullscreen()
	{
		return m_b_fullscreen;
	}

	void Project::SetVsyncEnabled(bool b_vsync)
	{
		m_b_vsyncEnabled = b_vsync;
		int interval = 0;

		if (m_b_vsyncEnabled)
			interval = 1;

		SDL_RenderSetVSync(Window::W_Renderer, interval); // vsync disabled -- 1 to activate
	}

	bool Project::IsVsyncEnabled()
	{
		return m_b_vsyncEnabled;
	}


	std::string Project::GetData()
	{
		json jsonData = {
			{ "path", m_path },
			{ "loadedScenePath", m_loadedScenePath },
			{ "sceneToLoadAtRuntime", m_sceneToLoadAtRuntime },
			{ "buildPath", m_buildPath },
			{ "loadedAnimationPath", m_loadedAnimationPath },
			{ "sceneViewScrollingX", m_sceneViewScrolling.x },
			{ "sceneViewScrollingY", m_sceneViewScrolling.y },
			{ "_autoSave", m_b_autoSave },
			{ "resolutionWidth", m_resolution.x },
			{ "resolutionHeight", m_resolution.y },
			{ "_fullscreen", m_b_fullscreen },
			{ "_vsyncEnabled", m_b_vsyncEnabled },
		};

		std::string data = jsonData.dump();
		// Return dumped json object with required data for saving
		return data;
	}
}