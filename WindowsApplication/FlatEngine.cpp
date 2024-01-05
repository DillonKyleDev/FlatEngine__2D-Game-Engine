#include "FlatEngine.h"


/*
######################################
######							######
######         Engine           ######
######                          ######
######################################
*/


namespace FlatEngine
{
	//Variables
	int FlatEngine::FocusedGameObjectIndex = -1;
	FlatEngine::SceneManager* FlatEngine::sceneManager = new FlatEngine::SceneManager();
	FlatEngine::Logger* FlatEngine::logger = new FlatEngine::Logger();
	FlatEngine::GameLoop* FlatEngine::gameLoop = new FlatEngine::GameLoop();

	// FlatEngine
	void FlatEngine::Run(bool& _hasQuit)
	{
		// Keeping track of the frames passed since we started the GameLoop
		if (FlatEngine::GameLoopStarted() && !FlatEngine::GameLoopPaused())
		{
			FlatEngine::GameLoopUpdate();
		}

		FlatEngine::FlatGui::Render(_hasQuit);
	}

	void FlatEngine::SetFocusedGameObjectIndex(int index)
	{
		FlatEngine::FocusedGameObjectIndex = index;
	}

	int FlatEngine::GetFocusedGameObjectIndex()
	{
		return FlatEngine::FocusedGameObjectIndex;
	}


	std::vector<FlatEngine::GameScript*> gameScripts = {};


	// Scene Manager Prettification
	Scene* FlatEngine::GetLoadedScene()
	{
		return FlatEngine::sceneManager->GetLoadedScene();
	}

	void FlatEngine::SaveScene(Scene *scene)
	{
		FlatEngine::sceneManager->SaveScene(scene);
	}

	void FlatEngine::LoadScene(std::string name)
	{
		FlatEngine::sceneManager->LoadScene(name);
	}
	

	// Scene Manager Prettification
	std::vector<GameObject*> FlatEngine::GetSceneObjects()
	{
		return FlatEngine::GetLoadedScene()->GetSceneObjects();
	}

	void FlatEngine::CreateGameObject()
	{
		FlatEngine::GetLoadedScene()->CreateGameObject();
	}

	void FlatEngine::DeleteGameObject(int sceneObjectIndex)
	{
		FlatEngine::GetLoadedScene()->DeleteGameObject(sceneObjectIndex);
	}


	// Logging
	void FlatEngine::LogString(std::string line)
	{
		logger->LogString(line);
	}

	void FlatEngine::LogFloat(float var, std::string line)
	{
		logger->LogFloat(var, line);
	}

	void FlatEngine::LogInt(int var, std::string line)
	{
		logger->LogInt(var, line);
	}

	void FlatEngine::LogVector2(Vector2 vector, std::string line)
	{
		logger->LogVector2(vector, line);
	}

	void FlatEngine::DrawRectangle(Vector2 startingPoint, Vector2 endingPoint)
	{
		logger->DrawRectangle(startingPoint, endingPoint);
	}

	void FlatEngine::DrawRectangle(ImVec2 startingPoint, ImVec2 endingPoint)
	{
		logger->DrawRectangle(startingPoint, endingPoint);
	}


	// Game Loop prettification
	void FlatEngine::StartGameLoop()
	{
		gameLoop->Start();
	}

	void FlatEngine::GameLoopUpdate()
	{
		gameLoop->Update();
	}

	void FlatEngine::PauseGameLoop()
	{
		gameLoop->Pause();
	}

	void FlatEngine::StopGameLoop()
	{
		gameLoop->Stop();
	}

	int FlatEngine::GetEllapsedGameTime()
	{
		return gameLoop->TimeEllapsed();
	}

	bool FlatEngine::GameLoopStarted()
	{
		return gameLoop->IsStarted();
	}

	bool FlatEngine::GameLoopPaused()
	{
		return gameLoop->IsPaused();
	}

	float FlatEngine::GetAverageFps()
	{
		return gameLoop->GetAverageFps();
	}
}