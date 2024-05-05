#include "GameLoop.h"
#include "FlatEngine.h"
#include "Process.h"
#include "Scene.h"
#include "BoxCollider.h"
#include "./scripts/GameManager.h"


namespace FlatEngine
{
	GameLoop::GameLoop()
	{
		startTime = 0;
		countedTicks = 0;
		pausedTicks = 0;
		_started = false;
		_paused = false;
		framesCounted = 0;
		lastFrameTime = 0;
		deltaTime = 0;
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
		_paused = false;
		lastFrameTime = (float)SDL_GetTicks();
		startTime = SDL_GetTicks();
		pausedTicks = 0;

		// Initialize our scripts with the currently loaded scene
		InitializeScriptObjects();
		// Save the name of the scene we started with so we can load it back up when we stop
		startedScene = FlatEngine::GetLoadedScenePath();

		_started = true;
	}

	void GameLoop::InitializeScriptObjects()
	{
		// Get currently loaded scenes GameObjects
		gameObjects = FlatEngine::GetSceneObjects();
		rigidBodies.clear();
		boxColliders.clear();

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
				}
				if (components[j]->GetTypeString() == "RigidBody")
				{
					// Collect all BoxCollider components for collision detection in Update()
					rigidBodies.push_back(std::static_pointer_cast<RigidBody>(components[j]));
				}
				if (components[j]->GetTypeString() == "BoxCollider")
				{
					// Collect all BoxCollider components for collision detection in Update()
					boxColliders.push_back(std::static_pointer_cast<BoxCollider>(components[j]));
				}
			}
		}


		// CALL AWAKE ON ALL SCRIPTS HERE ONCE IT'S IMPLEMENTED //

		for (int i = 0; i < activeScripts.size(); i++)
		{
			// Create a new Process for each script
			std::shared_ptr<Process> newProcess = std::make_shared<Process>(activeScripts[i]->GetName() + "-on-" + activeScripts[i]->GetOwner()->GetName());
			AddProfilerProcess(newProcess);
			activeScripts[i]->Awake();
			activeScripts[i]->Start();
		}
	}

	void GameLoop::Update()
	{
		AddFrame();

		// The time that this function was called last (the last frame), lastFrameTime, is the marker for how long it has been 
		// (in milliseconds) from that frame to this current one. That is deltaTime.
		deltaTime = (float)SDL_GetTicks() - lastFrameTime;
		// Update lastFrameTime to this frames time for the next time Update() is called to calculate deltaTime again.
		lastFrameTime = (float)SDL_GetTicks();

		for (int i = 0; i < activeScripts.size(); i++)
		{
			// Save time before script update
			float timeStart = (float)SDL_GetTicks();
			activeScripts[i]->Update(deltaTime);

			// Get hang time of each script update function for profiler
			float hangTime = (float)SDL_GetTicks() - timeStart;

			AddProcessData(activeScripts[i]->GetName() + "-on-" + activeScripts[i]->GetOwner()->GetName(), hangTime);
		}

		// TODO: Check here if the Game viewport is focused before getting the mouse data //
		//
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

		// Handle RigidBody physics updates here
		for (std::shared_ptr<RigidBody> rigidBody : rigidBodies)
		{
			rigidBody->ApplyGravity();
			rigidBody->ApplyVelocity();
		}

		static int continuousCounter = 0;

		// Handle BoxCollision updates here
		for (std::shared_ptr<BoxCollider> boxCollider : boxColliders)
		{
			if (boxCollider != nullptr && boxCollider->IsActive() && (boxCollider->IsContinuous() || (!boxCollider->IsContinuous() && continuousCounter == 10)))
			{
				// Update Active Collider bounds
				ImVec4 activeEdges = boxCollider->UpdateActiveEdges(FlatGui::worldCenterPoint, FlatGui::gridStep);
				//ImVec2 screenPos = ImGui::GetCursorScreenPos();
				//ImGui::SetCursorPos(FlatGui::worldCenterPoint);
				//ImGui::GetWindowDrawList()->AddRectFilled(FlatGui::worldCenterPoint, ImVec2(FlatGui::worldCenterPoint.x + 20, FlatGui::worldCenterPoint.y + 20), ImGui::GetColorU32(FlatGui::whiteColor));
				// Get Mouse Position in the viewport space
				ImVec2 mousePos = ImGui::GetIO().MousePos;

				// If Mouse and Button are colliding, add the hovered button
				if (FlatEngine::AreCollidingViewport(activeEdges, ImVec4(mousePos.y, mousePos.x, mousePos.y, mousePos.x)))
				{
					LogString("Colliding with: " + boxCollider->GetParent()->GetName());
					if (boxCollider->GetParent()->HasComponent("RigidBody"))
						boxCollider->GetParent()->GetRigidBody();
				}
				else if (boxCollider->GetParent()->HasComponent("RigidBody"))
					boxCollider->GetParent()->GetRigidBody()->SetIsGrounded(false);

				// Reset continuous counter
				continuousCounter = 0;
			}
			continuousCounter++;
		}
	}

	void GameLoop::Stop()
	{
		_started = false;
		_paused = false;

		// Reset Ticks and frames counted
		countedTicks = SDL_GetTicks();
		pausedTicks = 0;
		framesCounted = 0;

		// Delete script processes
		for (int i = 0; i < activeScripts.size(); i++)
			RemoveProfilerProcess(activeScripts[i]->GetName() + "-on-" + activeScripts[i]->GetOwner()->GetName());

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

			// Store the time that the timer was paused and reset the counted frames
			pausedTicks = SDL_GetTicks() - countedTicks;
			countedTicks = 0;
		}
	}

	void GameLoop::Unpause()
	{
		// If the timer is running and paused
		if (_started && _paused)
		{
			_paused = false;
			// Get the ellapsed time since the pause and remove it from total ticks to get current time
			countedTicks = SDL_GetTicks() - pausedTicks;
			pausedTicks = 0;
		}
	}

	int GameLoop::TimeEllapsed()
	{
		if (_started)
		{
			if (_paused)
			{
				return pausedTicks;
			}
			else
			{
				return SDL_GetTicks() - startTime;
			}
		}
		return countedTicks;
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
		return framesCounted / TimeEllapsed() * 1000;
	}

	float GameLoop::GetFramesCounted()
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
}