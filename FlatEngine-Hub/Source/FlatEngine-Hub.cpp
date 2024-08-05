#pragma once
#include "FlatHub.h"
#include "FlatEngine.h"
#include "EntryPoint.h"
#include "Application.h"
#include "GameLoop.h"


int main(int argc, char* args[])
{
	// Initializes FlatEngine
	return FlatEngine::Main(argc, args);
}


// Define our Applications main GameLoop
class HubGameLoop : public FlatEngine::GameLoop
{
public:
	HubGameLoop() {};
	~HubGameLoop() {};

	void Start()
	{
		FlatEngine::GameLoop::Start();
	};
	void Stop()
	{
		FlatEngine::GameLoop::Stop();
	};
	void Update()
	{
		// Call base class GameLoop Update function
		//FlatEngine::GameLoop::Update(FlatGui::loadedProject);

		// Other, application specific updates here if needed
		//
	};
private:
};



// Define our Application
class HubApplication : public FlatEngine::Application
{
private:
	HubGameLoop* A_GameLoop;

public:
	HubApplication()
	{
		A_GameLoop = new HubGameLoop();
	}
	~HubApplication()
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
			static double frameStart = FlatEngine::GetEngineTime();
			FlatEngine::HandleEvents(_hasQuit);

			BeginRender();
			EndRender();
		}
	}
	void Quit()
	{
		FlatEngine::Application::Quit();
	}
	void BeginRender()
	{
		Application::BeginRender();

		// Application specific rendering
		FlatHub::RenderHub();
	}
	void EndRender()
	{
		Application::EndRender();
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
};


// Define our CreateApplication() function for the Application
std::shared_ptr<FlatEngine::Application> FlatEngine::CreateApplication(int argc, char** argv)
{
	std::shared_ptr<HubApplication> EditorApp = std::make_shared<HubApplication>();
	EditorApp->SetWindowDimensions(800, 400);
	return EditorApp;
}