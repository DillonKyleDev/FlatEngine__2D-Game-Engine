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
		void UpdateActiveColliders();
		void UpdateActiveRigidBodies();
		void Update();
		void Stop();
		void Pause();
		void Unpause();
		int TimeEllapsed();
		bool IsStarted();
		bool IsPaused();
		float GetAverageFps();
		void AddFrame();
		int GetDeltaTime();
		void SetFrameSkipped(bool _skipped);
		bool IsFrameSkipped();
		int GetFramesCounted();
		int GetCountedTicks();
		int GetPausedTicks();
		int GetLastFrameTime();
		
	private:
		Uint32 startTime;
		bool _started;
		bool _paused;
		bool _frameSkipped;
		int framesCounted;
		int countedTicks;
		int pausedTicks;
		// For deltaTime
		int lastFrameTime;

		int deltaTime;
		std::shared_ptr<GameManager> gameManager;

		std::string startedScene;
		std::vector<std::shared_ptr<FlatEngine::GameObject>> gameObjects;
		std::vector<std::shared_ptr<FlatEngine::GameScript>> scripts;
		std::vector<std::shared_ptr<FlatEngine::GameScript>> activeScripts;
	};
}

