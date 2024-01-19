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

		this->gameObjects = std::vector<std::shared_ptr<GameObject>>();
		this->scripts = std::vector<std::shared_ptr<GameScript>>();

		this->moverScript = nullptr;
		this->upScript = nullptr;
		this->sinwaveScript = nullptr;
		this->attackScript = nullptr;
		this->startButtonScript = nullptr;
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

		// Create our script instances
		this->moverScript = std::make_shared<Mover>();
		this->upScript = std::make_shared<Up>();
		this->sinwaveScript = std::make_shared<Sinwave>();
		this->attackScript = std::make_shared<Attack>();
		this->startButtonScript = std::make_shared<StartButton>();
		this->scripts.push_back(moverScript);
		this->scripts.push_back(upScript);
		this->scripts.push_back(sinwaveScript);
		this->scripts.push_back(attackScript);
		this->scripts.push_back(startButtonScript);

		this->InitializeScriptObjects();

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

					if (attachedScript == "Mover")
					{
						moverScript->AddEntity(this->gameObjects[i]);
					}
					else if (attachedScript == "Up")
					{
						upScript->AddEntity(this->gameObjects[i]);
					}
					else if (attachedScript == "Sinwave")
					{
						sinwaveScript->AddEntity(this->gameObjects[i]);
					}
					else if (attachedScript == "Attack")
					{
						attackScript->AddEntity(this->gameObjects[i]);
					}
					else if (attachedScript == "StartButton")
					{
						startButtonScript->AddEntity(this->gameObjects[i]);
					}
					// Add other script name checks here and add them to those script objects
				}
			}
		}


		// CALL AWAKE ON ALL SCRIPTS HERE ONCE IT'S IMPLEMENTED //


		// After all the scripts have gotten all of their scene objects added to them, we can run their Start methods
		for (int i = 0; i < scripts.size(); i++)
		{
			if (scripts[i]->GetEntities().size() > 0 && scripts[i]->_isActive)
				scripts[i]->Start();
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


		// Run all the script Update() functions
		for (int i = 0; i < scripts.size(); i++)
			if (scripts[i]->GetEntities().size() > 0 && scripts[i]->_isActive)
				scripts[i]->Update(this->deltaTime);


		// TODO: Check here if the Game viewport is focused before getting the mouse data //
		//
		// Check for mouse over on all of our Game Buttons

		static bool _hasLeftClicked = false;
		static bool _hasRightClicked = false;
		if (FlatEngine::uiManager->CheckForMouseOver())
		{
			std::vector<std::shared_ptr<FlatEngine::Button>> hoveredButtons = FlatEngine::uiManager->GetHoveredButtons();
			ImGuiIO inputOutput = ImGui::GetIO();

			// Call the OnMouseOverFunction() in the top level button that is hovered
			std::shared_ptr<GameObject> thisObject = FlatEngine::GetObjectById(FlatEngine::uiManager->GetTopLevelButton()->GetParentID());
			//FlatEngine::uiManager->GetTopLevelButton()->OnMouseOverFunction(thisObject);

			// If mouse is clicked call the OnLeftClickFunction() in the top level button that is hovered
			if (inputOutput.MouseDown[0] && !_hasLeftClicked)
			{
				_hasLeftClicked = true;
				FlatEngine::uiManager->GetTopLevelButton()->OnLeftClickFunction(thisObject);
			}
			// Unclick check
			if (!inputOutput.MouseDown[0])
				_hasLeftClicked = false;
				
			// If mouse is clicked call the OnRightClickFunction() in the top level button that is hovered
			if (inputOutput.MouseDown[1] && !_hasRightClicked)
			{
				_hasRightClicked = true;
				FlatEngine::uiManager->GetTopLevelButton()->OnRightClickFunction(thisObject);
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

		// Reset shared_ptrs
		this->moverScript = nullptr;
		this->upScript = nullptr;
		this->sinwaveScript = nullptr;

		// Load back up the saved version of the scene
		FlatEngine::LoadScene(FlatEngine::GetLoadedScene()->GetName());
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