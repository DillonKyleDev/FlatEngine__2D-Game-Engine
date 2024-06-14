#include "GameLoop.h"
#include "FlatEngine.h"
#include "Process.h"
#include "Scene.h"
#include "Collider.h"
#include "BoxCollider.h"
#include "CircleCollider.h"
#include "Transform.h"
#include "./scripts/GameManager.h"
#include "./scripts/PauseMenu.h"
#include "./scripts/SettingsButton.h"
#include "./scripts/StartButton.h"
#include "./scripts/RestartButton.h"
#include "./scripts/QuitButton.h"
#include "./scripts/PlayerController.h"
//#include "./scripts/GroundedCheck.h"
#include "./scripts/JumpPad.h"
#include "./scripts/BlasterRound.h"



namespace FlatEngine
{
	GameLoop::GameLoop()
	{
		_started = false;
		_paused = false;
		_frameSkipped = false;
		time = 0;
		activeTime = 0;
		currentTime = 0;
		pausedTime = 0;
		framesCounted = 0;
		deltaTime = 0.005;
		accumulator = 0.0;
		startedScene = "";
		gameManager = nullptr;

		_gamePaused = false;

		gameObjects = std::vector<std::shared_ptr<GameObject>>();
		scripts = std::vector<std::shared_ptr<GameScript>>(); // Not being used??

		activeScripts = std::vector<std::shared_ptr<GameScript>>();
		rigidBodies = std::vector<std::shared_ptr<FlatEngine::RigidBody>>();
		colliders = std::vector<std::shared_ptr<FlatEngine::Collider>>();
		colliderPairs = std::vector<std::pair<std::shared_ptr<FlatEngine::Collider>, std::shared_ptr<FlatEngine::Collider>>>();
	}

	GameLoop::~GameLoop()
	{
	}

	void GameLoop::Start()
	{
		// Handle Game Time
		time = 0;
		activeTime = time - pausedTime;
		_paused = false;
		accumulator = 0.0;
		currentTime = (double)FlatEngine::GetEngineTime();

		// Save the name of the scene we started with so we can load it back up when we stop
		startedScene = FlatEngine::GetLoadedScenePath();

		_started = true;

		// Get currently loaded scenes GameObjects and instantiate script objects for them
		gameObjects = FlatEngine::GetSceneObjects();
		activeScripts.clear();
		InitializeScriptObjects(gameObjects);

		CollectPhysicsBodies();


		if (FlatEngine::_isDebugMode)
		{
			// Add Update Time Process		
			AddProfilerProcess("Update Loop");
			// Add Update Process for all other time		
			AddProfilerProcess("Not Update Loop");
		}
	}

	void GameLoop::CollectPhysicsBodies()
	{
		// Initialize objects for use in GameLoop::Update() with the currently loaded scene
		UpdateActiveColliders();
		UpdateActiveRigidBodies();
	}

	void GameLoop::AddScript(std::shared_ptr<GameObject> owner, std::shared_ptr<ScriptComponent> scriptComponent, std::shared_ptr<GameScript> scriptInstance)
	{
		scriptComponent->SetScriptInstance(scriptInstance);
		scriptInstance->SetOwner(owner);
		activeScripts.push_back(scriptInstance);
		scriptInstance->Awake();
		scriptInstance->Start();
	}

	void GameLoop::RemoveScript(long scriptID)
	{
		for (std::vector<std::shared_ptr<GameScript>>::iterator iter = activeScripts.begin(); iter != activeScripts.end();)
		{
			if ((*iter)->GetOwnerID() == scriptID)
			{
				RemoveProfilerProcess((*iter)->GetName() + "-on-" + (*iter)->GetOwner()->GetName());
				activeScripts.erase(iter);
				return;
			}
			iter++;
		}
	}

	void GameLoop::RemoveRigidBody(long rigidBodyID)
	{
		for (std::vector<std::shared_ptr<RigidBody>>::iterator iter = rigidBodies.begin(); iter != rigidBodies.end();)
		{
			if ((*iter)->GetID() == rigidBodyID)
			{
				rigidBodies.erase(iter);
				return;
			}
			iter++;
		}
	}

	void GameLoop::RemoveCollider(long colliderID)
	{
		for (std::vector<std::shared_ptr<Collider>>::iterator iter = colliders.begin(); iter != colliders.end();)
		{
			if ((*iter)->GetID() == colliderID)
			{
				colliders.erase(iter);
				UpdateActiveColliders();
				return;
			}
			iter++;
		}
	}

	void GameLoop::InitializeScriptObjects(std::vector<std::shared_ptr<GameObject>> gameObjects)
	{
		std::vector<std::shared_ptr<GameScript>> newScripts = std::vector<std::shared_ptr<GameScript>>();

		// Find all script components on Scene GameObjects and add those GameObjects
		// to their corresponding script class entity vector members
		for (int i = 0; i < gameObjects.size(); i++)
		{
			std::vector<std::shared_ptr<Component>> components = gameObjects[i]->GetComponents();

			for (int j = 0; j < components.size(); j++)
			{
				// For the script components, loop through and save to a vector, assign respective owners
				if (components[j]->GetTypeString() == "Script")
				{
					std::shared_ptr<ScriptComponent> script = std::static_pointer_cast<ScriptComponent>(components[j]);
					std::string attachedScript = script->GetAttachedScript();

					if (attachedScript == "GameManager")
					{
						std::shared_ptr<GameManager> gameManagerScript = std::make_shared<GameManager>(script->GetID());
						gameManagerScript->SetOwner(gameObjects[i]);
						script->SetScriptInstance(gameManagerScript);
						activeScripts.push_back(gameManagerScript);
						newScripts.push_back(gameManagerScript);
						gameManager = gameManagerScript;
						FlatEngine::gameManager = gameManagerScript;
					}
					else if (attachedScript == "PauseMenu")
					{
						std::shared_ptr<PauseMenu> pauseMenuScript = std::make_shared<PauseMenu>(script->GetID());
						pauseMenuScript->SetOwner(gameObjects[i]);
						script->SetScriptInstance(pauseMenuScript);
						activeScripts.push_back(pauseMenuScript);
						newScripts.push_back(pauseMenuScript);
					}
					else if (attachedScript == "SettingsButton")
					{
						std::shared_ptr<SettingsButton> settingsButtonScript = std::make_shared<SettingsButton>(script->GetID());
						settingsButtonScript->SetOwner(gameObjects[i]);
						script->SetScriptInstance(settingsButtonScript);
						activeScripts.push_back(settingsButtonScript);
						newScripts.push_back(settingsButtonScript);
					}
					else if (attachedScript == "StartButton")
					{
						std::shared_ptr<StartButton> startButtonScript = std::make_shared<StartButton>(script->GetID());
						startButtonScript->SetOwner(gameObjects[i]);
						script->SetScriptInstance(startButtonScript);
						activeScripts.push_back(startButtonScript);
						newScripts.push_back(startButtonScript);
					}
					else if (attachedScript == "RestartButton")
					{
						std::shared_ptr<RestartButton> restartButtonScript = std::make_shared<RestartButton>(script->GetID());
						restartButtonScript->SetOwner(gameObjects[i]);
						script->SetScriptInstance(restartButtonScript);
						activeScripts.push_back(restartButtonScript);
						newScripts.push_back(restartButtonScript);
					}
					else if (attachedScript == "QuitButton")
					{
						std::shared_ptr<QuitButton> quitButtonScript = std::make_shared<QuitButton>(script->GetID());
						quitButtonScript->SetOwner(gameObjects[i]);
						script->SetScriptInstance(quitButtonScript);
						activeScripts.push_back(quitButtonScript);
						newScripts.push_back(quitButtonScript);
					}
					else if (attachedScript == "PlayerController")
					{
						std::shared_ptr<PlayerController> playerControllerScript = std::make_shared<PlayerController>(script->GetID());
						playerControllerScript->SetOwner(gameObjects[i]);
						script->SetScriptInstance(playerControllerScript);
						activeScripts.push_back(playerControllerScript);
						newScripts.push_back(playerControllerScript);
					}
					else if (attachedScript == "JumpPad")
					{
						std::shared_ptr<JumpPad> jumpPadScript = std::make_shared<JumpPad>(script->GetID());
						jumpPadScript->SetOwner(gameObjects[i]);
						script->SetScriptInstance(jumpPadScript);
						activeScripts.push_back(jumpPadScript);
						newScripts.push_back(jumpPadScript);
					}
					else if (attachedScript == "BlasterRound")
					{
						std::shared_ptr<BlasterRound> blasterRoundScript = std::make_shared<BlasterRound>(script->GetID());
						blasterRoundScript->SetOwner(gameObjects[i]);
						script->SetScriptInstance(blasterRoundScript);
						activeScripts.push_back(blasterRoundScript);
						newScripts.push_back(blasterRoundScript);
					}
				}
			}
		}

		// Call Awake and Start on all newly added scripts
		for (int i = 0; i < newScripts.size(); i++)
		{
			// Create a new Process for each script	
			 if (FlatEngine::_isDebugMode)	
				AddProfilerProcess(newScripts[i]->GetName() + "-on-" + newScripts[i]->GetOwner()->GetName());
			 newScripts[i]->Awake();
			 newScripts[i]->Start();
		}
	}

	void GameLoop::UpdateActiveColliders()
	{
		// Get currently loaded scenes GameObjects
		gameObjects = FlatEngine::GetSceneObjects();
		colliders.clear();

		// Find all script components on Scene GameObjects and add those GameObjects
		// to their corresponding script class entity vector members
		for (int i = 0; i < gameObjects.size(); i++)
		{
			std::vector<std::shared_ptr<Component>> components = gameObjects[i]->GetComponents();

			for (int j = 0; j < components.size(); j++)
			{
				if (components[j]->GetTypeString() == "BoxCollider")
				{
					// Collect all BoxCollider components for collision detection in Update()
					colliders.push_back(std::static_pointer_cast<BoxCollider>(components[j]));
				}
				if (components[j]->GetTypeString() == "CircleCollider")
				{
					// Collect all BoxCollider components for collision detection in Update()
					colliders.push_back(std::static_pointer_cast<CircleCollider>(components[j]));
				}
			}
		}

		// Remake colliderPairs
		colliderPairs.clear();

		for (int i = 0; i < colliders.size(); i++)
		{
			for (int j = i + 1; j < colliders.size(); j++)
			{
				std::pair<std::shared_ptr<Collider>, std::shared_ptr<Collider>> newPair = { colliders.at(i), colliders.at(j) };
				colliderPairs.push_back(newPair);
			}
		}
	}

	void GameLoop::UpdateActiveRigidBodies()
	{
		// Get currently loaded scenes GameObjects
		gameObjects = FlatEngine::GetSceneObjects();
		rigidBodies.clear();

		// Find all script components on Scene GameObjects and add those GameObjects
		// to their corresponding script class entity vector members
		for (int i = 0; i < gameObjects.size(); i++)
		{
			std::vector<std::shared_ptr<Component>> components = gameObjects[i]->GetComponents();

			for (int j = 0; j < components.size(); j++)
			{
				if (components[j]->GetTypeString() == "RigidBody")
				{
					// Collect all BoxCollider components for collision detection in Update()
					rigidBodies.push_back(std::static_pointer_cast<RigidBody>(components[j]));
				}
			}
		}
	}

	void GameLoop::UpdateScripts()
	{
		for (int i = 0; i < activeScripts.size(); i++)
		{
			// Save time before script update
			float timeStart = (float)FlatEngine::GetEngineTime();
			activeScripts[i]->Update(deltaTime);

			// Get hang time of each script update function for profiler
			float hangTime = (float)FlatEngine::GetEngineTime() - timeStart;
			if (activeScripts.size() > i && activeScripts[i])
				AddProcessData(activeScripts[i]->GetName() + "-on-" + activeScripts[i]->GetOwner()->GetName(), hangTime);
		}
	}

	void GameLoop::Update()
	{
		// Profiler
		float updateLoopStart = 0;
		static float updateLoopEnd = 0;
		if (FlatEngine::_isDebugMode)
		{
			// Save time before Update starts
			updateLoopStart = (float)FlatEngine::GetEngineTime();
			updateLoopEnd = updateLoopStart;
			// Get hang time of everything after Update Loop for profiler
			float everythingElseHangTime = updateLoopStart - updateLoopEnd;
			AddProcessData("Not Update Loop", everythingElseHangTime);
		}
	

		AddFrame();
		UpdateScripts();
		activeTime = time - pausedTime;		

		// Calculate RigidBody physics to use in collisions
		for (std::shared_ptr<RigidBody> rigidBody : rigidBodies)
			rigidBody->CalculatePhysics();

		// Reset Collider collisions before going through all of them again
		for (std::shared_ptr<Collider> collider : colliders)
			collider->ResetCollisions();

		// Handle Collision updates here
		static int continuousCounter = 0;
		for (std::pair<std::shared_ptr<FlatEngine::Collider>, std::shared_ptr<FlatEngine::Collider>> colliderPair : colliderPairs)
		{
			std::shared_ptr<Collider> collider1 = colliderPair.first;
			std::shared_ptr<Collider> collider2 = colliderPair.second;

			if (collider1->GetParent() != nullptr && collider1 != nullptr && collider1->IsActive() && collider2->GetParent() != nullptr && collider2 != nullptr && collider2->IsActive() && (!collider1->IsStatic() || !collider2->IsStatic()) && ((collider1->IsContinuous() || (!collider1->IsContinuous() && continuousCounter == 10)) || (collider2->IsContinuous() || (!collider2->IsContinuous() && continuousCounter == 10))))
			{
				if (collider2 != nullptr && (collider1->GetID() != collider2->GetID()) && collider2->IsActive())
				{
					collider1->RecalculateBounds();
					collider2->RecalculateBounds();

					if (collider1->GetActiveLayer() == collider2->GetActiveLayer())
						Collider::CheckForCollision(collider1, collider2);
				}
			}
		}
		if (continuousCounter >= 10)
			continuousCounter = 0;
		continuousCounter++;

		// Apply RigidBody physics calculations
		for (std::shared_ptr<RigidBody> rigidBody : rigidBodies)
			rigidBody->ApplyPhysics((float)deltaTime);


		// Profiler
		if (FlatEngine::_isDebugMode)
		{
			// Get hang time of Update Loop for profiler
			float hangTime = (float)FlatEngine::GetEngineTime() - updateLoopStart;
			AddProcessData("Update Loop", hangTime);
			// Save time after update finishes
			updateLoopEnd = (float)FlatEngine::GetEngineTime();
		}
	}

	void GameLoop::Stop()
	{
		_started = false;
		_paused = false;
		framesCounted = 0;

		if (FlatEngine::_isDebugMode)
		{
			// Delete script processes
			for (int i = 0; i < activeScripts.size(); i++)
				RemoveProfilerProcess(activeScripts[i]->GetName() + "-on-" + activeScripts[i]->GetOwner()->GetName());

			RemoveProfilerProcess("Update Loop");
			RemoveProfilerProcess("Not Update Loop");
		}

		// Release all active scripts
		activeScripts.clear();

		// Load back up the saved version of the scene
		FlatEngine::LoadScene(startedScene);
	}

	void GameLoop::Pause()
	{
		// If the timer is running and isn't already paused
		if (_started && !_paused)
		{
			_paused = true;
			activeTime = time - pausedTime;
		}
	}

	void GameLoop::Unpause()
	{
		// If the timer is running and paused
		if (_started && _paused)
		{
			_paused = false;
			ResetCurrentTime();
			pausedTime = time - activeTime;
		}
	}

	void GameLoop::ResetCurrentTime()
	{
		currentTime = FlatEngine::GetEngineTime();
	}

	bool GameLoop::IsGamePaused()
	{
		return _gamePaused;
	}

	void GameLoop::PauseGame()
	{
		_gamePaused = true;
	}

	void GameLoop::UnpauseGame()
	{
		_gamePaused = false;
	}

	// In seconds
	long GameLoop::TimeEllapsedInSec()
	{
		if (_started)
		{
			return activeTime;
		}
		return 0;
	}

	// In ms
	double GameLoop::TimeEllapsedInMs()
	{
		if (_started)
		{
			return time * 1000;
		}
		return 0;
	}

	bool GameLoop::IsStarted()
	{
		return _started;
	}

	bool GameLoop::IsPaused()
	{
		return _paused && _started;
	}

	float GameLoop::GetAverageFps()
	{
		if (TimeEllapsedInSec() != 0)
			return (float)framesCounted / (float)activeTime;
		else
			return 200;
	}

	int GameLoop::GetFramesCounted()
	{
		return framesCounted;
	}

	void GameLoop::AddFrame()
	{
		framesCounted++;
	}

	float GameLoop::GetDeltaTime()
	{
		return deltaTime;
	}

	void GameLoop::SetFrameSkipped(bool _skipped)
	{
		if (_skipped)
			ResetCurrentTime();

		_frameSkipped = _skipped;
	}

	bool GameLoop::IsFrameSkipped()
	{
		return _frameSkipped;
	}
}