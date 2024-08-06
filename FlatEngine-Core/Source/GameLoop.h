#pragma once
#include "GameScript.h"
#include "GameObject.h"
#include "Collider.h"


namespace FlatEngine
{
	class Project;
	class GameManager;
	class GameObject;
	class GameScript;
	class RigidBody;
	class Collider;

	class GameLoop
	{
	public:
		GameLoop();
		~GameLoop();

		void Start();
		void Update(std::shared_ptr<Project> loadedProject);
		void UpdateScripts();
		void Stop();
		void Pause();
		void Unpause();
		bool IsGamePaused();
		void PauseGame();
		void UnpauseGame();
		float TimeEllapsedInSec();
		long TimeEllapsedInMs();
		bool IsStarted();
		bool IsPaused();
		float GetAverageFps();
		void AddFrame();
		float GetDeltaTime();
		void SetFrameSkipped(bool _skipped);
		bool IsFrameSkipped();
		long GetFramesCounted();
		void ResetCurrentTime();

		float time; // Total time in deltaTime increments
		float pausedTime;
		float activeTime;
		float deltaTime;
		float accumulator;
		Uint32 currentTime;
		
	private:
		bool _started;
		bool _paused;
		bool _frameSkipped;
		long framesCounted;
		bool _gamePaused;
		std::string startedScene;
	};
}