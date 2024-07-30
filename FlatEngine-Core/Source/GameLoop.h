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
		long TimeEllapsedInSec();
		double TimeEllapsedInMs();
		bool IsStarted();
		bool IsPaused();
		float GetAverageFps();
		void AddFrame();
		float GetDeltaTime();
		void SetFrameSkipped(bool _skipped);
		bool IsFrameSkipped();
		int GetFramesCounted();
		void ResetCurrentTime();

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
		std::string startedScene;
	};
}