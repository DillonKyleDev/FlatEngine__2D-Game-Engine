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

		// Get Start "time" in Ticks ellapsed
		this->startTime = SDL_GetTicks();
		this->pausedTicks = 0;

		// Create our script instances
		this->moverScript = new Mover();
		this->upScript = new Up();
		this->sinwaveScript = new Sinwave();

		this->scripts.push_back(moverScript);
		this->scripts.push_back(upScript);
		this->scripts.push_back(sinwaveScript);


		// Find all script components on Scene GameObjects and add those GameObjects
		// to their corresponding script class entity vector members
		std::vector<GameObject*> sceneObjects = FlatEngine::GetSceneObjects();
		for (int i = 0; i < sceneObjects.size(); i++)
		{
			std::vector<Component*> components = sceneObjects[i]->GetComponents();

			for (int j = 0; j < components.size(); j++)
			{
				if (components[j]->GetTypeString() == "Script")
				{
					FlatEngine::ScriptComponent* script = static_cast<FlatEngine::ScriptComponent*>(components[j]);
					std::string attatchedScript = script->GetAttachedScript();

					if (attatchedScript == "Mover")
					{
						moverScript->AddEntity(sceneObjects[i]);
					}
					else if (attatchedScript == "Up")
					{
						upScript->AddEntity(sceneObjects[i]);
					}
					else if (attatchedScript == "Sinwave")
					{
						sinwaveScript->AddEntity(sceneObjects[i]);
					}

					// Add other script name checks here and add them to those script objects
				}
			}
		}


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

		// Run all the script Update() functions
		for (int i = 0; i < scripts.size(); i++)
		{
			if (scripts[i]->GetEntities().size() > 0 && scripts[i]->_isActive)
				scripts[i]->Update(1);
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
}