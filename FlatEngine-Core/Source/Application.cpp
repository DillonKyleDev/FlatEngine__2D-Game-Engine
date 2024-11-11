#include "FlatEngine.h"
#include "Application.h"
#include "AssetManager.h"
//#include "WindowManager.h"

#include "SDL.h"

namespace FL = FlatEngine;

namespace FlatEngine
{
	void Application::BeginRender()
	{
		FL::BeginImGuiRender();
	}

	void Application::EndRender()
	{
		FL::EndImGuiRender();

		// Handle window resizing and recreating ImGui
		if (m_b_windowResized)
		{
			//FL::SaveCurrentScene();

			//int width;
			//int height;
			//SDL_GetWindowSize(FL::F_Window->GetWindow(), &width, &height);
			//FL::F_Window->SetScreenDimensions(width, height);
			//FL::F_AssetManager.CollectDirectories();
			FL::RestartImGui(); // ImGui setup relies on global colors

			m_b_windowResized = false;
		}

		if (F_b_loadNewScene)
		{
			F_b_loadNewScene = false;
			FL::LoadScene(F_sceneToBeLoaded);
			F_sceneToBeLoaded = "";
		}
	}
}