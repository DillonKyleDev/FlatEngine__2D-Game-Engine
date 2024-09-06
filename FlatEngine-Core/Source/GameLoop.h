#pragma once
#include "Application.h"
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
		void Update(float gridstep, Vector2 viewportCenter);
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

		// Components
		void HandleCamera();
		void ResetCharacterControllers();
		void HandleButtons();
		bool CheckForMouseOver();
		void ResetHoveredButtons();
		int GetFirstUnblockedLayer();
		Canvas GetFirstUnblockedCanvas();
		void CalculatePhysics();
		void HandleCollisions(float gridstep, Vector2 viewportCenter);
		void ApplyPhysics();
		void RunUpdateOnScripts();
		
		float m_time; // Total time in deltaTime increments
		float m_pausedTime;
		float m_activeTime;
		float m_deltaTime;
		float m_accumulator;
		Uint32 m_currentTime;

	private:
		bool m_b_started;
		bool m_b_paused;
		bool m_b_frameSkipped;
		bool m_b_gamePaused;
		long m_framesCounted;
		std::string m_startedScene;

		std::vector<Button> m_hoveredButtons;
	};
}