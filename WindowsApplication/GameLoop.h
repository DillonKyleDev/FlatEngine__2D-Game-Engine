#pragma once
#include "SDL.h"
#include "GameScript.h"
#include "./scripts/GameManager.h"
#include "GameObject.h"


namespace FlatEngine
{
	class GameLoop
	{
	public:
		GameLoop();
		~GameLoop();

		void Start();
		void InitializeScriptObjects();
		void Update();
		void Stop();
		void Pause();
		void Unpause();
		int TimeEllapsed();
		bool IsStarted();
		bool IsPaused();
		float GetAverageFps();
		float GetFramesCounted();
		void AddFrame();
		float GetDeltaTime();

	private:
		Uint32 startTime;
		int countedTicks;
		int pausedTicks;
		bool _started;
		bool _paused;
		float framesCounted;
		// For deltaTime
		float lastFrameTime;
		float deltaTime;
		std::shared_ptr<GameManager> gameManager;

		std::string startedScene;
		std::vector<std::shared_ptr<FlatEngine::GameObject>> gameObjects;
		std::vector<std::shared_ptr<FlatEngine::GameScript>> scripts;
		std::vector<std::shared_ptr<FlatEngine::GameScript>> activeScripts;
	};
}

