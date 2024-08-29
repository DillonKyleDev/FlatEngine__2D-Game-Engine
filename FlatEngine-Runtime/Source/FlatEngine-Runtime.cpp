#pragma once
#include "FlatEngine.h"
#include "EntryPoint.h"
#include "Application.h"
#include "GameLoop.h"
#include "AssetManager.h"
#include "PrefabManager.h"
#include "Window.h"
#include "Project.h"

#include <vector>
#include <string>
#include <memory>
#include "imgui_internal.h"

namespace FL = FlatEngine;

int main(int argc, char* args[])
{
	// Initializes FlatEngine
	return FL::Main(argc, args);
}


// Define our Applications main GameLoop
class RuntimeGameLoop : public FL::GameLoop
{
public:
	RuntimeGameLoop() {};
	~RuntimeGameLoop() {};

	void Start()
	{
		FL::GameLoop::Start();
	};
	void Stop()
	{
		FL::GameLoop::Stop();
	};
	void Update()
	{
		// Call base class GameLoop Update function
		FL::GameLoop::Update(FL::F_gameViewGridStep.x, FL::F_gameViewCenter);


		// Other, application specific updates here if needed
		//
	};
private:
};



// Define our Application
class RuntimeApplication : public FL::Application
{
private:
	RuntimeGameLoop* A_GameLoop;

public:
	RuntimeApplication()
	{
		A_GameLoop = new RuntimeGameLoop();
		m_recreateWindow = false;
		SetDirectoriesType(FL::RuntimeDir);
	}
	~RuntimeApplication()
	{
		delete A_GameLoop;
		A_GameLoop = nullptr;
	}

	void Init()
	{
		// Remove the reference to the imgui.ini file for layout since we only need that in Engine mode and
		// we don't want to have to include it in the final release build anyway.
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.IniFilename = NULL;

		//Window::SetFullscreen(false);
	}
	void Run()
	{
		bool& _hasQuit = HasQuit();
		while (!_hasQuit)
		{
			Uint32 renderStartTime = 0;
			static Uint32 frameStart = FL::GetEngineTime();

			if (FL::_isDebugMode)
				renderStartTime = FL::GetEngineTime(); // Profiler
			_hasQuit = FL::_closeProgram;


			// For things we only want to execute once after complete initialization
			RunOnceAfterInitialization();


			BeginRender();


			//// If Release - Start the Game Loop
			if (!A_GameLoop->IsStarted())
				A_GameLoop->Start();


			if (A_GameLoop->IsStarted() && !A_GameLoop->IsGamePaused() || A_GameLoop->IsGamePaused() && A_GameLoop->IsFrameSkipped())
			{
				float frameTime = (float)(FL::GetEngineTime() - frameStart) / 1000.0f; // actual deltaTime (in seconds)

				if (!A_GameLoop->IsGamePaused())
					A_GameLoop->accumulator += frameTime;

				if (!A_GameLoop->IsGamePaused())
				{
					while (A_GameLoop->accumulator >= A_GameLoop->deltaTime)
					{
						FL::HandleEvents(_hasQuit);
						A_GameLoop->Update();
						A_GameLoop->SetFrameSkipped(false);

						A_GameLoop->time += A_GameLoop->deltaTime;
						A_GameLoop->accumulator -= A_GameLoop->deltaTime;
					}
				}

				// Get time it took to get back to GameLoopUpdate()
				frameStart = FL::GetEngineTime();

				// Artificially slow GameLoop if frameTime is less than 
				if (!FL::F_LoadedProject.IsVsyncEnabled() && frameTime < A_GameLoop->deltaTime)
					SDL_Delay((Uint32)(A_GameLoop->deltaTime - frameTime) * 1000);
			}
			else
				FL::HandleEvents(_hasQuit);

			// If gameloop isn't running, make sure our framestart keeps up with current engine time otherwise it will cause a freeze on initially starting gameloop
			if (!A_GameLoop->IsStarted())
				frameStart = FL::GetEngineTime();



			EndRender();
		}
	}
	// For things we only want to execute once after complete initialization
	void RunOnceAfterInitialization()
	{
		static bool b_hasRunOnce = false;

		if (!b_hasRunOnce)
		{
			json projectJson;
			FL::InitializeTileSets();
			FL::InitializeMappingContexts();
			FL::prefabManager->InitializePrefabs();
			FL::LoadGameProject(FL::GetFilePath("gameStartupProject"), projectJson);
			FL::LoadScene(FL::F_LoadedProject.GetRuntimeScene());

			b_hasRunOnce = true;
		}
	}
	void BeginRender()
	{
		Application::BeginRender();



		// Application specific rendering	
		FL::SetNextViewportToFillWindow();  // Maximize viewport
		bool b_inRuntime = true;
		FL::Game_RenderView(b_inRuntime);
			
	}
	void EndRender()
	{
		Application::EndRender();


		// Application specific rendering tasks
	}
	void Quit()
	{
		FL::Application::Quit();
	}
	void OnLoadScene(std::string sceneName)
	{
	}
	FL::GameLoop* GetGameLoop()
	{
		return A_GameLoop;
	};
	bool GameLoopStarted()
	{
		return A_GameLoop->IsStarted();
	};
	bool GameLoopPaused()
	{
		return A_GameLoop->IsPaused();
	};
	void StartGameLoop()
	{
		A_GameLoop->Start();
	};
	void UpdateGameLoop()
	{
		A_GameLoop->Update();
	};
	void PauseGameLoop()
	{
		if (A_GameLoop->IsPaused())
			A_GameLoop->Unpause();
		else
			A_GameLoop->Pause();
	};
	void StopGameLoop()
	{
		A_GameLoop->Stop();
	};
	void PauseGame()
	{
		if (A_GameLoop->IsGamePaused())
			A_GameLoop->UnpauseGame();
		else
			A_GameLoop->PauseGame();
	}

	bool m_recreateWindow;
	std::string m_startupProject;
};


// Define our CreateApplication() for the Runtime
std::shared_ptr<FL::Application> FL::CreateApplication(int argc, char** argv)
{
	std::shared_ptr<RuntimeApplication> RuntimeApp = std::make_shared<RuntimeApplication>();
	RuntimeApp->SetWindowDimensions(1920, 1080);
	return RuntimeApp;
}