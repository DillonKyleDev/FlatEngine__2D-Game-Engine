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
#include "GameScript.h"
#include "Component.h"
#include "ScriptComponent.h"
#include "GameScript.h"
// GameLoop scripts
//#include "./scripts/GameManager.h"
//#include "./scripts/PauseMenu.h"
//#include "./scripts/SettingsButton.h"
//#include "./scripts/StartButton.h"
//#include "./scripts/RestartButton.h"
//#include "./scripts/QuitButton.h"
//#include "./scripts/PlayerController.h"
//#include "./scripts/EnemyController.h"
//#include "./scripts/Health.h"
//#include "./scripts/JumpPad.h"
//#include "./scripts/BlasterRound.h"
//#include "./scripts/BlobParticle.h"

#include <vector>
#include <SDL_mixer.h>
#include <string>
#include <memory>

#include "implot.h"

using GameObject = FlatEngine::GameObject;
using GameScript = FlatEngine::GameScript;
using Component = FlatEngine::Component;
using ScriptComponent = FlatEngine::ScriptComponent;



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
			// Add Update Time Process		
			FlatGui::AddProfilerProcess("GameLoop (variable executions)");
			// Add Update Process for all other time		
			FlatGui::AddProfilerProcess("Not GameLoop");
		}
		FlatEngine::GameLoop::Start();
	};
	void Stop()
	{
		if (FlatEngine::_isDebugMode)
		{
			//// Delete script processes
			//for (int i = 0; i < GameLoop::GetActiveScripts().size(); i++)
			//	FlatGui::RemoveProfilerProcess(activeScripts[i]->GetName() + "-on-" + activeScripts[i]->GetOwner()->GetName());

			FlatGui::RemoveProfilerProcess("GameLoop (variable executions)");
			FlatGui::RemoveProfilerProcess("Not GameLoop");
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
	void InitializeScriptObjects(std::vector<std::shared_ptr<GameObject>> gameObjects)
	{
		std::vector<GameScript> newScripts = std::vector<GameScript>();

		// Find all script components on Scene GameObjects and add those GameObjects
		// to their corresponding script class entity vector members
		for (int i = 0; i < gameObjects.size(); i++)
		{
			std::vector<Component*> components = gameObjects[i]->GetComponents();

			for (int j = 0; j < components.size(); j++)
			{
				// For the script components, loop through and save to a vector, assign respective owners
				if (components[j]->GetTypeString() == "Script")
				{
					ScriptComponent* script = static_cast<ScriptComponent*>(components[j]);
					std::string attachedScript = script->GetAttachedScript();

					//if (attachedScript == "GameManager")
					//{
					//	std::shared_ptr<GameManager> gameManagerScript = std::make_shared<GameManager>(script->GetID());
					//	gameManagerScript->SetOwner(gameObjects[i]);
					//	script->SetScriptInstance(gameManagerScript);
					//	activeScripts.push_back(gameManagerScript);
					//	newScripts.push_back(gameManagerScript);
					//	gameManager = gameManagerScript;
					//	FlatEngine::gameManager = gameManagerScript;
					//}
					//else if (attachedScript == "PauseMenu")
					//{
					//	std::shared_ptr<PauseMenu> pauseMenuScript = std::make_shared<PauseMenu>(script->GetID());
					//	pauseMenuScript->SetOwner(gameObjects[i]);
					//	script->SetScriptInstance(pauseMenuScript);
					//	activeScripts.push_back(pauseMenuScript);
					//	newScripts.push_back(pauseMenuScript);
					//}
					//else if (attachedScript == "SettingsButton")
					//{
					//	std::shared_ptr<SettingsButton> settingsButtonScript = std::make_shared<SettingsButton>(script->GetID());
					//	settingsButtonScript->SetOwner(gameObjects[i]);
					//	script->SetScriptInstance(settingsButtonScript);
					//	activeScripts.push_back(settingsButtonScript);
					//	newScripts.push_back(settingsButtonScript);
					//}
					//else if (attachedScript == "StartButton")
					//{
					//	std::shared_ptr<StartButton> startButtonScript = std::make_shared<StartButton>(script->GetID());
					//	startButtonScript->SetOwner(gameObjects[i]);
					//	script->SetScriptInstance(startButtonScript);
					//	activeScripts.push_back(startButtonScript);
					//	newScripts.push_back(startButtonScript);
					//}
					//else if (attachedScript == "RestartButton")
					//{
					//	std::shared_ptr<RestartButton> restartButtonScript = std::make_shared<RestartButton>(script->GetID());
					//	restartButtonScript->SetOwner(gameObjects[i]);
					//	script->SetScriptInstance(restartButtonScript);
					//	activeScripts.push_back(restartButtonScript);
					//	newScripts.push_back(restartButtonScript);
					//}
					//else if (attachedScript == "QuitButton")
					//{
					//	std::shared_ptr<QuitButton> quitButtonScript = std::make_shared<QuitButton>(script->GetID());
					//	quitButtonScript->SetOwner(gameObjects[i]);
					//	script->SetScriptInstance(quitButtonScript);
					//	activeScripts.push_back(quitButtonScript);
					//	newScripts.push_back(quitButtonScript);
					//}
					//else if (attachedScript == "PlayerController")
					//{
					//	std::shared_ptr<PlayerController> playerControllerScript = std::make_shared<PlayerController>(script->GetID());
					//	playerControllerScript->SetOwner(gameObjects[i]);
					//	script->SetScriptInstance(playerControllerScript);
					//	activeScripts.push_back(playerControllerScript);
					//	newScripts.push_back(playerControllerScript);
					//}
					//else if (attachedScript == "EnemyController")
					//{
					//	std::shared_ptr<EnemyController> enemyControllerScript = std::make_shared<EnemyController>(script->GetID());
					//	enemyControllerScript->SetOwner(gameObjects[i]);
					//	script->SetScriptInstance(enemyControllerScript);
					//	activeScripts.push_back(enemyControllerScript);
					//	newScripts.push_back(enemyControllerScript);
					//}
					//else if (attachedScript == "Health")
					//{
					//	std::shared_ptr<Health> healthScript = std::make_shared<Health>(script->GetID());
					//	healthScript->SetOwner(gameObjects[i]);
					//	script->SetScriptInstance(healthScript);
					//	activeScripts.push_back(healthScript);
					//	newScripts.push_back(healthScript);
					//}
					//else if (attachedScript == "JumpPad")
					//{
					//	std::shared_ptr<JumpPad> jumpPadScript = std::make_shared<JumpPad>(script->GetID());
					//	jumpPadScript->SetOwner(gameObjects[i]);
					//	script->SetScriptInstance(jumpPadScript);
					//	activeScripts.push_back(jumpPadScript);
					//	newScripts.push_back(jumpPadScript);
					//}
					//else if (attachedScript == "BlasterRound")
					//{
					//	std::shared_ptr<BlasterRound> blasterRoundScript = std::make_shared<BlasterRound>(script->GetID());
					//	blasterRoundScript->SetOwner(gameObjects[i]);
					//	script->SetScriptInstance(blasterRoundScript);
					//	activeScripts.push_back(blasterRoundScript);
					//	newScripts.push_back(blasterRoundScript);
					//}
					//else if (attachedScript == "BlobParticle")
					//{
					//	std::shared_ptr<BlobParticle> blobParticleScript = std::make_shared<BlobParticle>(script->GetID());
					//	blobParticleScript->SetOwner(gameObjects[i]);
					//	script->SetScriptInstance(blobParticleScript);
					//	activeScripts.push_back(blobParticleScript);
					//	newScripts.push_back(blobParticleScript);
					//}
				}
			}
		}

		// Call Awake and Start on all newly added scripts
		for (int i = 0; i < newScripts.size(); i++)
		{
			// Create a new Process for each script	
			if (FlatEngine::_isDebugMode)
				FlatGui::AddProfilerProcess(newScripts[i].GetName() + "-on-" + newScripts[i].GetOwner()->GetName());
			newScripts[i].Awake();
			newScripts[i].Start();
		}
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
				FlatGui::AddProcessData("Render", (float)(FlatEngine::GetEngineTime() - renderStartTime)); // Profiler


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
					FlatGui::AddProcessData("Not GameLoop", (float)everythingElseHangTime);
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
					FlatGui::AddProcessData("GameLoop (variable executions)", (float)hangTime);
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
		// 
		// If window was recreated this frame
		if (m_recreateWindow)
		{
			Window::SetScreenDimensions(1900, 900);
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
	EditorApp->SetWindowDimensions(800, 500);
	return EditorApp;
}