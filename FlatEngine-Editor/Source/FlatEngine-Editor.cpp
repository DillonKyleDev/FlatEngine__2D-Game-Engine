#pragma once
#include "FlatEngine.h"
#include "FlatGui.h"
#include "EntryPoint.h"
#include "Application.h"
#include "GameLoop.h"
#include "Window.h"
#include "Project.h"
#include "Process.h"
#include "GameObject.h"
#include "Component.h"
#include "Script.h"

#include <vector>
#include <SDL_mixer.h>
#include <string>
#include <memory>
#include "implot.h"

using GameObject = FlatEngine::GameObject;
using Component = FlatEngine::Component;
using Script = FlatEngine::Script;



int main(int argc, char* args[])
{
	// Initializes FlatEngine
	return FlatEngine::Main(argc, args);
}


// Define our Applications main GameLoop
class EditorGameLoop : public FlatEngine::GameLoop
{
public:
	EditorGameLoop() {};
	~EditorGameLoop() {};

	void Start()
	{
		if (FlatEngine::_isDebugMode)
		{				
			FlatEngine::AddProfilerProcess("GameLoop (variable executions)");		
			FlatEngine::AddProfilerProcess("Not GameLoop");
			FlatEngine::AddProfilerProcess("Collision Testing");
		}
		FlatEngine::GameLoop::Start();
	};
	void Stop()
	{
		if (FlatEngine::_isDebugMode)
		{
			FlatEngine::RemoveProfilerProcess("GameLoop (variable executions)");
			FlatEngine::RemoveProfilerProcess("Not GameLoop");
			FlatEngine::RemoveProfilerProcess("Collision Testing");
		}
		FlatEngine::GameLoop::Stop();
	};
	void Update()
	{
		// Call base class GameLoop Update function
		FlatEngine::GameLoop::Update(FlatGui::FG_sceneViewGridStep.x, FlatGui::FG_sceneViewCenter);
		
		// Other, application specific updates here if needed
		//
	};
private:
};



// Define our Application
class EditorApplication : public FlatEngine::Application
{
private:
	EditorGameLoop* A_GameLoop;

public:
	EditorApplication()
	{
		A_GameLoop = new EditorGameLoop();
		m_recreateWindow = false;
	}
	~EditorApplication()
	{
		delete A_GameLoop;
		A_GameLoop = nullptr;
	}

	void Init()
	{
	}
	void Run()
	{
		bool& _hasQuit = HasQuit();
		while (!_hasQuit)
		{
			Uint32 renderStartTime = 0;
			static Uint32 frameStart = FlatEngine::GetEngineTime();

			if (FlatEngine::_isDebugMode)
				renderStartTime = FlatEngine::GetEngineTime(); // Profiler
			_hasQuit = FlatEngine::_closeProgram;


			BeginRender();


			if (FlatEngine::_isDebugMode)
				FlatEngine::AddProcessData("Render", (float)(FlatEngine::GetEngineTime() - renderStartTime)); // Profiler


			//// If Release - Start the Game Loop
			if (!FlatEngine::_isDebugMode && !A_GameLoop->IsStarted())
				A_GameLoop->Start();


			if (A_GameLoop->IsStarted() && !A_GameLoop->IsGamePaused() || A_GameLoop->IsGamePaused() && A_GameLoop->IsFrameSkipped())
			{
				// Profiler
				Uint32 updateLoopStart = 0;
				static Uint32 updateLoopEnd = 0;
				if (FlatEngine::_isDebugMode)
				{
					// Save time before Update starts
					updateLoopStart = FlatEngine::GetEngineTime();
					// Get hang time of everything after Update Loop for profiler
					Uint32 everythingElseHangTime = updateLoopStart - updateLoopEnd;
					FlatEngine::AddProcessData("Not GameLoop", (float)everythingElseHangTime);
					updateLoopEnd = updateLoopStart;
				}

				float frameTime = (float)(FlatEngine::GetEngineTime() - frameStart) / 1000.0f; // actual deltaTime (in seconds)

				if (!A_GameLoop->IsGamePaused())
					A_GameLoop->accumulator += frameTime;

				if (!A_GameLoop->IsGamePaused())
				{
					while (A_GameLoop->accumulator >= A_GameLoop->deltaTime)
					{
						FlatEngine::HandleEvents(_hasQuit);
						A_GameLoop->Update();
						A_GameLoop->SetFrameSkipped(false);

						A_GameLoop->time += A_GameLoop->deltaTime;
						A_GameLoop->accumulator -= A_GameLoop->deltaTime;
					}
				}

				// Get time it took to get back to GameLoopUpdate()
				frameStart = FlatEngine::GetEngineTime();

				// Artificially slow GameLoop if frameTime is less than 
				if (!FlatEngine::F_LoadedProject.IsVsyncEnabled() && frameTime < A_GameLoop->deltaTime)
					SDL_Delay((Uint32)(A_GameLoop->deltaTime - frameTime) * 1000);

				// Profiler
				if (FlatEngine::_isDebugMode)
				{
					// Get hang time of Update Loop for profiler
					Uint32 hangTime = FlatEngine::GetEngineTime() - updateLoopStart;
					FlatEngine::AddProcessData("GameLoop (variable executions)", (float)hangTime);
					// Save time after update finishes
					updateLoopEnd = FlatEngine::GetEngineTime();
				}
			}
			else
				FlatEngine::HandleEvents(_hasQuit);

			// If gameloop isn't running, make sure our framestart keeps up with current engine time otherwise it will cause a freeze on initially starting gameloop
			if (!A_GameLoop->IsStarted())
				frameStart = FlatEngine::GetEngineTime();

			

			EndRender();

			// For things we only want to execute once after complete initialization
			FlatGui::RunOnceAfterInitialization();
		}
	}
	void BeginRender()
	{
		Application::BeginRender();


		// Application specific rendering


		// Render the project selection screen
		static bool b_projectSelected = false;
		if (!b_projectSelected)
		{
			FlatGui::RenderProjectHub(b_projectSelected, m_startupProject);
			if (b_projectSelected)
				m_recreateWindow = true;
		}
		else
		{
			// Start the engine and Add viewport(s)
			if (!FlatEngine::_isDebugMode)
			{
				FlatEngine::Game_RenderView();
			}
			else
				FlatGui::AddViewports();
		}
	}
	void EndRender()
	{
		Application::EndRender();


		// Application specific rendering tasks
		

		// If window was recreated this frame
		if (m_recreateWindow)
		{
			Window::SetScreenDimensions(1900, 960);
			ImPlot::DestroyContext();
			ImGui::DestroyContext();
			FlatEngine::SetupImGui();
			FlatEngine::CreateIcons();
			m_recreateWindow = false;
			FlatGui::OpenProject(m_startupProject);
		}
	}
	void Quit()
	{
		FlatEngine::Application::Quit();
		FlatGui::Cleanup();
	}
	void OnLoadScene(std::string sceneName)
	{
		if (FlatEngine::GetObjectById(FlatGui::GetFocusedGameObjectID()) != nullptr)
			FlatGui::SetFocusedGameObjectID(-1);

		FlatGui::ResetHierarchyExpanderTracker();
	}
	FlatEngine::GameLoop* GetGameLoop()
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


// Define our CreateApplication() for the Editor
std::shared_ptr<FlatEngine::Application> FlatEngine::CreateApplication(int argc, char** argv)
{
	std::shared_ptr<EditorApplication> EditorApp = std::make_shared<EditorApplication>();
	EditorApp->SetWindowDimensions(600, 300);
	return EditorApp;
}