#pragma once
#include "FlatEngine.h"
#include "FlatGui.h"
#include "EntryPoint.h"
#include "Application.h"
#include "GameLoop.h"
#include "AssetManager.h"
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

namespace FL = FlatEngine;

using GameObject = FL::GameObject;
using Component = FL::Component;
using Script = FL::Script;



int main(int argc, char* args[])
{
	// Initializes FlatEngine
	return FL::Main(argc, args);
}


// Define our Applications main GameLoop
class EditorGameLoop : public FL::GameLoop
{
public:
	EditorGameLoop() {};
	~EditorGameLoop() {};

	void Start()
	{
		if (FL::_isDebugMode)
		{				
			FL::AddProfilerProcess("GameLoop (variable executions)");		
			FL::AddProfilerProcess("Not GameLoop");
			FL::AddProfilerProcess("Collision Testing");
		}
		FL::GameLoop::Start();
	};
	void Stop()
	{
		if (FL::_isDebugMode)
		{
			FL::RemoveProfilerProcess("GameLoop (variable executions)");
			FL::RemoveProfilerProcess("Not GameLoop");
			FL::RemoveProfilerProcess("Collision Testing");
		}
		FL::GameLoop::Stop();
	};
	void Update()
	{
		// Call base class GameLoop Update function
		FL::GameLoop::Update(FlatGui::FG_sceneViewGridStep.x, FlatGui::FG_sceneViewCenter);
		
		// Other, application specific updates here if needed
		//
	};
private:
};



// Define our Application
class EditorApplication : public FL::Application
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
			static Uint32 frameStart = FL::GetEngineTime();

			if (FL::_isDebugMode)
				renderStartTime = FL::GetEngineTime(); // Profiler
			_hasQuit = FL::_closeProgram;


			BeginRender();


			if (FL::_isDebugMode)
				FL::AddProcessData("Render", (float)(FL::GetEngineTime() - renderStartTime)); // Profiler


			//// If Release - Start the Game Loop
			if (!FL::_isDebugMode && !A_GameLoop->IsStarted())
				A_GameLoop->Start();


			if (A_GameLoop->IsStarted() && !A_GameLoop->IsGamePaused() || A_GameLoop->IsGamePaused() && A_GameLoop->IsFrameSkipped())
			{
				// Profiler
				Uint32 updateLoopStart = 0;
				static Uint32 updateLoopEnd = 0;
				if (FL::_isDebugMode)
				{
					// Save time before Update starts
					updateLoopStart = FL::GetEngineTime();
					// Get hang time of everything after Update Loop for profiler
					Uint32 everythingElseHangTime = updateLoopStart - updateLoopEnd;
					FL::AddProcessData("Not GameLoop", (float)everythingElseHangTime);
					updateLoopEnd = updateLoopStart;
				}

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

				// Profiler
				if (FL::_isDebugMode)
				{
					// Get hang time of Update Loop for profiler
					Uint32 hangTime = FL::GetEngineTime() - updateLoopStart;
					FL::AddProcessData("GameLoop (variable executions)", (float)hangTime);
					// Save time after update finishes
					updateLoopEnd = FL::GetEngineTime();
				}
			}
			else
				FL::HandleEvents(_hasQuit);

			// If gameloop isn't running, make sure our framestart keeps up with current engine time otherwise it will cause a freeze on initially starting gameloop
			if (!A_GameLoop->IsStarted())
				frameStart = FL::GetEngineTime();

			

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
			if (!FL::_isDebugMode)
			{
				FL::Game_RenderView();
			}
			else
				FlatGui::AddViewports();
		}
	}
	void EndRender()
	{
		Application::EndRender();


		// Application specific rendering tasks
		
		//////////////////////////////
		////////  WARNING!  //////////
		//////////////////////////////
		//  After recreating the window, you need to recreate any assets that were created using that window!!!    // 
		//  This can lead to assets not appearing even though everything seems like it should be working and fine  //
		
		// If window was recreated this frame
		if (m_recreateWindow)
		{
			Window::SetScreenDimensions(1900, 960);
			ImPlot::DestroyContext();
			ImGui::DestroyContext();
			FL::F_AssetManager.CollectDirectories();
			FL::F_AssetManager.CollectColors();
			FL::SetupImGui(); // ImGui setup relies on global colors
			FL::F_AssetManager.CollectTextures(); 
			m_recreateWindow = false;
			FlatGui::OpenProject(m_startupProject);
		}
	}
	void Quit()
	{
		FL::Application::Quit();
		FlatGui::Cleanup();
	}
	void OnLoadScene(std::string sceneName)
	{
		if (FL::GetObjectById(FlatGui::GetFocusedGameObjectID()) != nullptr)
			FlatGui::SetFocusedGameObjectID(-1);

		FlatGui::ResetHierarchyExpanderTracker();
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


// Define our CreateApplication() for the Editor
std::shared_ptr<FL::Application> FL::CreateApplication(int argc, char** argv)
{
	std::shared_ptr<EditorApplication> EditorApp = std::make_shared<EditorApplication>();
	EditorApp->SetWindowDimensions(800, 500);
	return EditorApp;
}