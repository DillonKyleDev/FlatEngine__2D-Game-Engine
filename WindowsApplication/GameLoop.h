#pragma once
#include "SDL.h"
#include "GameScript.h"
#include "./scripts/GameManager.h"
#include "GameObject.h"
#include "Collider.h"


namespace FlatEngine
{
	class GameLoop
	{
	public:
		GameLoop();
		~GameLoop();

		void Start();
		void CollectPhysicsBodies();
		void InitializeScriptObjects(std::vector<std::shared_ptr<GameObject>> gameObjects);
		void AddScript(std::shared_ptr<GameObject> owner, std::shared_ptr<ScriptComponent> scriptComponent, std::shared_ptr<GameScript> scriptInstance);
		void RemoveScript(long scriptID);
		void RemoveRigidBody(long rigidBodyID);
		void RemoveCollider(long colliderID);
		void UpdateActiveColliders();
		std::vector<std::pair<std::shared_ptr<FlatEngine::Collider>, std::shared_ptr<FlatEngine::Collider>>> GetColliderPairs();
		void UpdateActiveRigidBodies();
		void Update();
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

		std::shared_ptr<GameManager> gameManager;
		std::string startedScene;
		std::vector<std::shared_ptr<FlatEngine::GameObject>> gameObjects;
		std::vector<std::shared_ptr<FlatEngine::GameScript>> scripts;
		std::vector<std::shared_ptr<FlatEngine::GameScript>> activeScripts;
		std::vector<std::shared_ptr<FlatEngine::RigidBody>> rigidBodies;
		std::vector<std::shared_ptr<FlatEngine::Collider>> colliders;
		std::vector<std::pair<std::shared_ptr<FlatEngine::Collider>, std::shared_ptr<FlatEngine::Collider>>> colliderPairs;
	};
}