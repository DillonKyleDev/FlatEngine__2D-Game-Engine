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

		this->gameObjects = std::vector<GameObject*>();
		this->buttonComponents = {};
		//this->animationObjects = {};
		this->scripts = {};

		this->moverScript = nullptr;
		this->upScript = nullptr;
		this->sinwaveScript = nullptr;
	}

	GameLoop::~GameLoop()
	{

	}

	void GameLoop::Start()
	{
		this->_started = true;
		this->_paused = false;
		this->lastFrameTime = (float)SDL_GetTicks();

		// Get Start "time" in Ticks ellapsed
		this->startTime = SDL_GetTicks();
		this->pausedTicks = 0;

		this->gameObjects = FlatEngine::GetSceneObjects();

		// Create our script instances
		this->moverScript = new Mover();
		this->upScript = new Up();
		this->sinwaveScript = new Sinwave();
		// this->buttonScript = new ButtonScript();

		this->scripts.push_back(moverScript);
		this->scripts.push_back(upScript);
		this->scripts.push_back(sinwaveScript);
		// this->scripts.push_back(buttonScript);

		// Find all script components on Scene GameObjects and add those GameObjects
		// to their corresponding script class entity vector members
		for (int i = 0; i < this->gameObjects.size(); i++)
		{
			FlatEngine::LogString("GameObject: " + this->gameObjects[i]->GetName());
			std::vector<Component*> components = this->gameObjects[i]->GetComponents();

			for (int j = 0; j < components.size(); j++)
			{
				if (components[j]->GetTypeString() == "Script")
				{
					FlatEngine::ScriptComponent* script = static_cast<FlatEngine::ScriptComponent*>(components[j]);
					std::string attatchedScript = script->GetAttachedScript();

					FlatEngine::LogString("Attached Script: " + attatchedScript);

					if (attatchedScript == "Mover")
					{
						moverScript->AddEntity(this->gameObjects[i]);
					}
					else if (attatchedScript == "Up")
					{
						upScript->AddEntity(this->gameObjects[i]);
					}
					else if (attatchedScript == "Sinwave")
					{
						FlatEngine::LogString("Object Added to Sinewave.");
						sinwaveScript->AddEntity(this->gameObjects[i]);
					}
					// Add the Button GameObject(s) to the buttonScript;

					// Add other script name checks here and add them to those script objects
				}
				// Also
				if (components[j]->GetTypeString() == "Button")
				{
					this->buttonComponents.push_back(static_cast<FlatEngine::Button*>(components[j]));
				}
				if (components[j]->GetTypeString() == "Animation")
				{
					//this->animationComponents.push_back(components[j]);
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

		// In the Start() function of the buttonScript script:
		// startButton->OnMouseOver(callbackFunctionFromScript);
		// This will register the callback function inside the Button Component to be called during the gameloop when the button gets its event.
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
		{
			if (scripts[i]->GetEntities().size() > 0 && scripts[i]->_isActive)
				scripts[i]->Update(this->deltaTime);
		}


		// Run logic for other gameplay components here too like hover and click events for Buttons and Animation components
		// For all the Buttons:

		// int params = 204;
		// 
		// If FlatEngine::AreColliding(ButtonPositions, MousePositions)
			// button->OnMouseOverFunction(params);
	}


	void GameLoop::Stop()
	{
		this->_started = false;
		this->_paused = false;

		// Reset Ticks and frames counted
		this->countedTicks = SDL_GetTicks();
		this->pausedTicks = 0;
		this->framesCounted = 0;

		// Delete the script objects
		delete this->moverScript;
		this->moverScript = nullptr;
		delete this->upScript;
		this->upScript = nullptr;
		delete this->sinwaveScript;
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