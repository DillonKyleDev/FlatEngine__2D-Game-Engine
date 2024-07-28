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
		void CollectPhysicsBodies();
		void InitializeScriptObjects(std::vector<GameObject> gameObjects);
		void AddScript(GameObject& owner, ScriptComponent& scriptComponent, GameScript& scriptInstance);
		void RemoveScript(long scriptID);
		void RemoveRigidBody(long rigidBodyID);
		void RemoveCollider(long colliderID);
		void UpdateActiveColliders();
		std::vector<std::pair<FlatEngine::Collider*, FlatEngine::Collider*>> GetColliderPairs();
		void UpdateActiveRigidBodies();
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

		//FlatEngine::GameManager gameManager;
		std::string startedScene;
		std::vector<FlatEngine::GameObject> gameObjects;
		std::vector<FlatEngine::GameScript*> scripts;
		std::vector<FlatEngine::GameScript*> activeScripts;
		std::vector<FlatEngine::RigidBody*> rigidBodies;
		std::vector<FlatEngine::Collider*> colliders;
		std::vector<std::pair<FlatEngine::Collider*, FlatEngine::Collider*>> colliderPairs;
	};
}