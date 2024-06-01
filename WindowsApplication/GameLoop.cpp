#include "GameLoop.h"
#include "FlatEngine.h"
#include "Process.h"
#include "Scene.h"
#include "BoxCollider.h"
#include "Transform.h"
#include "./scripts/GameManager.h"
#include "./scripts/StartButton.h"
#include "./scripts/RestartButton.h"
#include "./scripts/QuitButton.h"
#include "./scripts/PlayerController.h"
#include "./scripts/GroundedCheck.h"
#include "./scripts/JumpPad.h"



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

		gameObjects = std::vector<std::shared_ptr<GameObject>>();
		scripts = std::vector<std::shared_ptr<GameScript>>();
	}

	GameLoop::~GameLoop()
	{
	}

	void GameLoop::Start()
	{
		// Handle Game Time
		time = 0;
		activeTime = time;
		_paused = false;
		accumulator = 0.0;
		currentTime = (double)SDL_GetTicks();

		// Initialize our scripts with the currently loaded scene
		InitializeScriptObjects();
		UpdateActiveColliders();
		UpdateActiveRigidBodies();
		// Save the name of the scene we started with so we can load it back up when we stop
		startedScene = FlatEngine::GetLoadedScenePath();

		_started = true;
	}

	void GameLoop::InitializeScriptObjects()
	{
		// Get currently loaded scenes GameObjects
		gameObjects = FlatEngine::GetSceneObjects();

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
						std::shared_ptr<GameManager> gameManagerScript = std::make_shared<GameManager>();
						gameManagerScript->SetOwner(gameObjects[i]);
						script->SetScriptInstance(gameManagerScript);
						activeScripts.push_back(gameManagerScript);
						gameManager = gameManagerScript;
						FlatEngine::gameManager = gameManagerScript;
					}
					else if (attachedScript == "StartButton")
					{
						std::shared_ptr<StartButton> startButtonScript = std::make_shared<StartButton>();
						startButtonScript->SetOwner(gameObjects[i]);
						script->SetScriptInstance(startButtonScript);
						activeScripts.push_back(startButtonScript);
					}
					else if (attachedScript == "RestartButton")
					{
						std::shared_ptr<RestartButton> restartButtonScript = std::make_shared<RestartButton>();
						restartButtonScript->SetOwner(gameObjects[i]);
						script->SetScriptInstance(restartButtonScript);
						activeScripts.push_back(restartButtonScript);
					}
					else if (attachedScript == "QuitButton")
					{
						std::shared_ptr<QuitButton> quitButtonScript = std::make_shared<QuitButton>();
						quitButtonScript->SetOwner(gameObjects[i]);
						script->SetScriptInstance(quitButtonScript);
						activeScripts.push_back(quitButtonScript);
					}
					else if (attachedScript == "PlayerController")
					{
						std::shared_ptr<PlayerController> playerControllerScript = std::make_shared<PlayerController>();
						playerControllerScript->SetOwner(gameObjects[i]);
						script->SetScriptInstance(playerControllerScript);
						activeScripts.push_back(playerControllerScript);
					}
					else if (attachedScript == "GroundedCheck")
					{
						std::shared_ptr<GroundedCheck> groundCheckScript = std::make_shared<GroundedCheck>();
						groundCheckScript->SetOwner(gameObjects[i]);
						script->SetScriptInstance(groundCheckScript);
						activeScripts.push_back(groundCheckScript);
					}
					else if (attachedScript == "JumpPad")
					{
						std::shared_ptr<JumpPad> jumpPadScript = std::make_shared<JumpPad>();
						jumpPadScript->SetOwner(gameObjects[i]);
						script->SetScriptInstance(jumpPadScript);
						activeScripts.push_back(jumpPadScript);
					}
				}
			}
		}

		// Add Update Time Process		
		AddProfilerProcess("Update Loop");
		// Add Update Process for all other time		
		AddProfilerProcess("Not Update Loop");

		// CALL AWAKE ON ALL SCRIPTS HERE ONCE IT'S IMPLEMENTED //

		for (int i = 0; i < activeScripts.size(); i++)
		{
			// Create a new Process for each script		
			AddProfilerProcess(activeScripts[i]->GetName() + "-on-" + activeScripts[i]->GetOwner()->GetName());
			activeScripts[i]->Awake();
			activeScripts[i]->Start();
		}
	}

	void GameLoop::UpdateActiveColliders()
	{
		// Get currently loaded scenes GameObjects
		gameObjects = FlatEngine::GetSceneObjects();
		boxColliders.clear();
		//circleColliders.clear();

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
					boxColliders.push_back(std::static_pointer_cast<BoxCollider>(components[j]));
				}
				if (components[j]->GetTypeString() == "CircleCollider")
				{
					// Collect all BoxCollider components for collision detection in Update()
					//circleColliders.push_back(std::static_pointer_cast<BoxCollider>(components[j]));
				}
			}
		}

		// Remake boxColliderPairs
		boxColliderPairs.clear();
		for (int i = 0; i < boxColliders.size(); i++)
		{
			for (int j = i + 1; j < boxColliders.size(); j++)
			{
				std::pair<std::shared_ptr<BoxCollider>, std::shared_ptr<BoxCollider>> newPair = { boxColliders.at(i), boxColliders.at(j) };
				boxColliderPairs.push_back(newPair);
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
			float timeStart = (float)SDL_GetTicks();
			activeScripts[i]->Update(deltaTime);

			// Get hang time of each script update function for profiler
			float hangTime = (float)SDL_GetTicks() - timeStart;

			AddProcessData(activeScripts[i]->GetName() + "-on-" + activeScripts[i]->GetOwner()->GetName(), hangTime);
		}
	}

	void GameLoop::Update()
	{
		// Profiler --
		// Save time before Update starts
		float updateLoopStart = (float)SDL_GetTicks();
		static float updateLoopEnd = updateLoopStart;
		// Get hang time of everything after Update Loop for profiler
		float everythingElseHangTime = updateLoopStart - updateLoopEnd;
		AddProcessData("Not Update Loop", everythingElseHangTime);
	

		AddFrame();
		UpdateScripts();
		activeTime = time - pausedTime;

		// TODO: Check here if the Game viewport is focused before getting the mouse data //
		// Check for mouse over on all of our Game Buttons
		static bool _hasLeftClicked = false;
		static bool _hasRightClicked = false;
		if (FlatEngine::uiManager->CheckForMouseOver())
		{
			std::vector<std::shared_ptr<FlatEngine::Button>> hoveredButtons = FlatEngine::uiManager->GetHoveredButtons();
			ImGuiIO inputOutput = ImGui::GetIO();
			std::shared_ptr<Button> topLevelButton = FlatEngine::uiManager->GetTopLevelButton();

			// Call the OnMouseOverFunction() in the top level button that is hovered
			std::shared_ptr<GameObject> thisObject = FlatEngine::GetObjectById(FlatEngine::uiManager->GetTopLevelButton()->GetParentID());

			// If mouse is clicked call the OnLeftClickFunction() in the top level button that is hovered
			if (inputOutput.MouseDown[0] && !_hasLeftClicked && topLevelButton != nullptr && topLevelButton->LeftClickSet())
			{
				_hasLeftClicked = true;
				topLevelButton->OnLeftClickFunction(thisObject);
			}
			// Unclick check
			if (!inputOutput.MouseDown[0])
				_hasLeftClicked = false;

			// If mouse is clicked call the OnRightClickFunction() in the top level button that is hovered
			if (inputOutput.MouseDown[1] && !_hasRightClicked && topLevelButton != nullptr && topLevelButton->RightClickSet())
			{
				_hasRightClicked = true;
				topLevelButton->OnRightClickFunction(thisObject);
			}
			// Unclick check
			if (!inputOutput.MouseDown[1])
				_hasRightClicked = false;
		}

		// Calculate RigidBody physics to use in collisions
		for (std::shared_ptr<RigidBody> rigidBody : rigidBodies)
			rigidBody->CalculatePhysics();

		// Reset BoxCollider collisions before going through all of them again
		for (std::shared_ptr<BoxCollider> boxCollider : boxColliders)
			boxCollider->ResetCollisions();

		// Handle BoxCollision updates here
		static int continuousCounter = 0;
		for (std::pair<std::shared_ptr<BoxCollider>, std::shared_ptr<BoxCollider>> boxColliderPair : boxColliderPairs)
		{
			std::shared_ptr<BoxCollider> collider1 = boxColliderPair.first;
			std::shared_ptr<BoxCollider> collider2 = boxColliderPair.second;

			if (collider1 != nullptr && collider1->IsActive() && collider2 != nullptr && collider2->IsActive() && (!collider1->IsStatic() || !collider2->IsStatic()) && (collider1->IsContinuous() || (!collider1->IsContinuous() && continuousCounter == 10)))
			{
				if (collider2 != nullptr && (collider1->GetID() != collider2->GetID()) && collider2->IsActive())
				{
					collider1->UpdateActiveEdges();
					collider2->UpdateActiveEdges();

					if (collider1->GetActiveLayer() == collider2->GetActiveLayer())
						collider1->CheckForCollision(collider2);
				}
			}
		}
		if (continuousCounter >= 10)
			continuousCounter = 0;
		continuousCounter++;

		// Apply RigidBody physics calculations
		for (std::shared_ptr<RigidBody> rigidBody : rigidBodies)
			rigidBody->ApplyPhysics((float)deltaTime);


		// Profiler --
		// Get hang time of Update Loop for profiler
		float hangTime = (float)SDL_GetTicks() - updateLoopStart;
		AddProcessData("Update Loop", hangTime);
		// Save time after update finishes
		updateLoopEnd = (float)SDL_GetTicks();
	}

	void GameLoop::Stop()
	{
		_started = false;
		_paused = false;

		// Delete script processes
		for (int i = 0; i < activeScripts.size(); i++)
			RemoveProfilerProcess(activeScripts[i]->GetName() + "-on-" + activeScripts[i]->GetOwner()->GetName());

		RemoveProfilerProcess("Update Loop");	
		RemoveProfilerProcess("Not Update Loop");

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
			pausedTime = time - activeTime;
		}
	}

	// In seconds
	double GameLoop::TimeEllapsed()
	{
		if (_started)
		{
			return activeTime;
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
		if (TimeEllapsed() != 0)
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
		_frameSkipped = _skipped;
	}

	bool GameLoop::IsFrameSkipped()
	{
		return _frameSkipped;
	}
}