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
		void Init();
		void InitializeScriptObjects();
		void UpdateActiveColliders();
		void UpdateActiveRigidBodies();
		void Update();
		void UpdateScripts();
		void Stop();
		void Pause();
		void Unpause();
		bool IsGamePaused();
		void PauseGame();
		void UnpauseGame();
		double TimeEllapsed();
		bool IsStarted();
		bool IsPaused();
		float GetAverageFps();
		void AddFrame();
		float GetDeltaTime();
		void SetFrameSkipped(bool _skipped);
		bool IsFrameSkipped();
		int GetFramesCounted();

		void IncrementTime();
		void AddToAccumulator(float frameTime);
		void DecrementAccumulator();
		void ResetCurrentTime();
		float GetGLCurrentTime();

		double time;
		double deltaTime;
		double currentTime;
		double accumulator;
		
	private:
		bool _started;
		bool _paused;
		bool _frameSkipped;
		int framesCounted;
		double pausedTime;
		double activeTime;

		bool _gamePaused;

		std::shared_ptr<GameManager> gameManager;
		std::string startedScene;
		std::vector<std::shared_ptr<FlatEngine::GameObject>> gameObjects;
		std::vector<std::shared_ptr<FlatEngine::GameScript>> scripts;
		std::vector<std::shared_ptr<FlatEngine::GameScript>> activeScripts;
	};
}

