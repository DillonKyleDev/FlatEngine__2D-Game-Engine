#include "GameLoop.h"
#include "FlatEngine.h"


namespace FlatEngine
{
	GameLoop::GameLoop()
	{
		this->startTime = 0;
		this->countedTicks = 0;
		this->pausedTicks = 0;
		this->_started = false;
		this->_paused = false;
		this->framesCounted = 0;
		this->lastFrameTime = 0;
		this->deltaTime = 0;
		this->startedScene = "";
		this->gameManager = nullptr;

		this->gameObjects = std::vector<std::shared_ptr<GameObject>>();
		this->scripts = std::vector<std::shared_ptr<GameScript>>();
	}

	GameLoop::~GameLoop()
	{
	}

	void GameLoop::Start()
	{
		// Handle Game Time
		this->_paused = false;
		this->lastFrameTime = (float)SDL_GetTicks();
		this->startTime = SDL_GetTicks();
		this->pausedTicks = 0;

		// Initialize our scripts with the currently loaded scene
		this->InitializeScriptObjects();
		// Save the name of the scene we started with so we can load it back up when we stop
		this->startedScene = FlatEngine::GetLoadedScene()->GetName();

		this->_started = true;
	}

	void GameLoop::InitializeScriptObjects()
	{
		// Get currently loaded scenes GameObjects
		this->gameObjects = FlatEngine::GetSceneObjects();

		// Find all script components on Scene GameObjects and add those GameObjects
		// to their corresponding script class entity vector members
		for (int i = 0; i < this->gameObjects.size(); i++)
		{
			std::vector<std::shared_ptr<Component>> components = this->gameObjects[i]->GetComponents();

			for (int j = 0; j < components.size(); j++)
			{
				// For the script components, we need to go through and add their owned objects
				if (components[j]->GetTypeString() == "Script")
				{
					std::shared_ptr<ScriptComponent> script = std::static_pointer_cast<ScriptComponent>(components[j]);
					std::string attachedScript = script->GetAttachedScript();

					if (attachedScript == "GameBoard")
					{
						std::shared_ptr<GameBoard> gameBoardScript = std::make_shared<GameBoard>();
						gameBoardScript->SetOwner(this->gameObjects[i]);
						script->SetScriptInstance(gameBoardScript);
						this->activeScripts.push_back(gameBoardScript);
					}
					else if (attachedScript == "BoardSquare")
					{
						std::shared_ptr<BoardSquare> boardSquare = std::make_shared<BoardSquare>();
						boardSquare->SetOwner(this->gameObjects[i]);
						script->SetScriptInstance(boardSquare);
						this->activeScripts.push_back(boardSquare);
					}
					else if (attachedScript == "GameManager")
					{
						std::shared_ptr<GameManager> gameManagerScript = std::make_shared<GameManager>();
						gameManagerScript->SetOwner(this->gameObjects[i]);
						script->SetScriptInstance(gameManagerScript);
						this->activeScripts.push_back(gameManagerScript);
						this->gameManager = gameManagerScript;
						FlatEngine::gameManager = gameManagerScript;
					}
				}
			}
		}


		// CALL AWAKE ON ALL SCRIPTS HERE ONCE IT'S IMPLEMENTED //

		for (int i = 0; i < activeScripts.size(); i++)
		{
			activeScripts[i]->Awake();
			activeScripts[i]->Start();
		}
	}

	void GameLoop::Update()
	{
		this->AddFrame();

		// The time that this function was called last (the last frame), this->lastFrameTime, is the marker for how long it has been 
		// (in milliseconds) from that frame to this current one. That is deltaTime.
		this->deltaTime = (float)SDL_GetTicks() - this->lastFrameTime;
		// Update this->lastFrameTime to this frames time for the next time Update() is called to calculate deltaTime again.
		this->lastFrameTime = (float)SDL_GetTicks();

		for (int i = 0; i < activeScripts.size(); i++)
		{
			activeScripts[i]->Update(this->deltaTime);
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
	}

	void GameLoop::Stop()
	{
		this->_started = false;
		this->_paused = false;

		// Reset Ticks and frames counted
		this->countedTicks = SDL_GetTicks();
		this->pausedTicks = 0;
		this->framesCounted = 0;

		// Load back up the saved version of the scene
		FlatEngine::LoadScene(this->startedScene);
	}

	void GameLoop::Pause()
	{
		// If the timer is running and isn't already paused
		if (this->_started && !this->_paused)
		{
			this->_paused = true;

			// Store the time that the timer was paused and reset the counted frames
			this->pausedTicks = SDL_GetTicks() - this->countedTicks;
			this->countedTicks = 0;
		}
	}

	void GameLoop::Unpause()
	{
		// If the timer is running and paused
		if (this->_started && this->_paused)
		{
			this->_paused = false;
			// Get the ellapsed time since the pause and remove it from total ticks to get current time
			this->countedTicks = SDL_GetTicks() - this->pausedTicks;
			this->pausedTicks = 0;
		}
	}

	int GameLoop::TimeEllapsed()
	{
		if (this->_started)
		{
			if (this->_paused)
			{
				return this->pausedTicks;
			}
			else
			{
				return SDL_GetTicks() - this->startTime;
			}
		}
		return this->countedTicks;
	}

	bool GameLoop::IsStarted()
	{
		return this->_started;
	}

	bool GameLoop::IsPaused()
	{
		return this->_paused && this->_started;
	}

	float GameLoop::GetAverageFps()
	{
		return this->framesCounted / this->TimeEllapsed() * 1000;
	}

	float GameLoop::GetFramesCounted()
	{
		return this->framesCounted;
	}

	void GameLoop::AddFrame()
	{
		this->framesCounted++;
	}

	float GameLoop::GetDeltaTime()
	{
		return this->deltaTime;
	}
}