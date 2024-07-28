#include "GameLoop.h"
#include "FlatEngine.h"
#include "Process.h"
#include "Scene.h"
#include "RigidBody.h"
#include "Collider.h"
#include "BoxCollider.h"
#include "CircleCollider.h"
#include "CompositeCollider.h"
#include "Transform.h"
#include "ScriptComponent.h"
#include "TagList.h"
#include "Camera.h"
#include "Project.h"


namespace FlatEngine
{
	GameLoop::GameLoop()
	{
		_started = false;
		_gamePaused = false;
		_paused = false;
		_frameSkipped = false;
		time = 0;
		activeTime = 0;
		currentTime = 0;
		pausedTime = 0;
		framesCounted = 0;
		deltaTime = 0.005;
		accumulator = deltaTime;
		startedScene = "";

		gameObjects = std::vector<FlatEngine::GameObject>();
		activeScripts = std::vector<FlatEngine::GameScript*>();
		rigidBodies = std::vector<FlatEngine::RigidBody*>();
		colliders = std::vector<FlatEngine::Collider*>();
		colliderPairs = std::vector<std::pair<FlatEngine::Collider*, FlatEngine::Collider*>>();
	}

	GameLoop::~GameLoop()
	{
	}

	void GameLoop::Start()
	{
		// Handle Game Time
		time = 0;
		activeTime = time - pausedTime;
		_paused = false;
		accumulator = 0.0;

		// Save the name of the scene we started with so we can load it back up when we stop
		startedScene = FlatEngine::GetLoadedScenePath();

		_started = true;

		// Get currently loaded scenes GameObjects and instantiate script objects for them
		gameObjects = FlatEngine::GetSceneObjects();
		activeScripts.clear();
		InitializeScriptObjects(gameObjects);

		CollectPhysicsBodies();

		currentTime = (double)FlatEngine::GetEngineTime();
	}

	void GameLoop::CollectPhysicsBodies()
	{
		// Initialize objects for use in GameLoop::Update() with the currently loaded scene
		UpdateActiveColliders();
		UpdateActiveRigidBodies();
	}

	void GameLoop::AddScript(GameObject& owner, ScriptComponent &scriptComponent, GameScript &scriptInstance)
	{
		scriptComponent.SetScriptInstance(&scriptInstance);
		scriptInstance.SetOwner(&owner);
		activeScripts.push_back(&scriptInstance);
		scriptInstance.Awake();
		scriptInstance.Start();
	}

	void GameLoop::RemoveScript(long scriptID)
	{
		for (std::vector<GameScript*>::iterator iter = activeScripts.begin(); iter != activeScripts.end();)
		{
			if ((*iter)->GetOwnerID() == scriptID)
			{
				//RemoveProfilerProcess((*iter)->GetName() + "-on-" + (*iter)->GetOwner()->GetName());
				activeScripts.erase(iter);
				return;
			}
			iter++;
		}
	}

	void GameLoop::RemoveRigidBody(long rigidBodyID)
	{
		for (std::vector<RigidBody*>::iterator iter = rigidBodies.begin(); iter != rigidBodies.end();)
		{
			if ((*iter)->GetID() == rigidBodyID)
			{
				rigidBodies.erase(iter);
				return;
			}
			iter++;
		}
	}

	void GameLoop::RemoveCollider(long colliderID)
	{
		for (std::vector<Collider*>::iterator iter = colliders.begin(); iter != colliders.end();)
		{
			if ((*iter)->GetID() == colliderID)
			{
				colliders.erase(iter);
				UpdateActiveColliders();
				return;
			}
			iter++;
		}
	}

	void GameLoop::InitializeScriptObjects(std::vector<GameObject> gameObjects)
	{
	}

	void GameLoop::UpdateActiveColliders()
	{
		// Get currently loaded scenes GameObjects
		gameObjects = FlatEngine::GetSceneObjects();
		colliders.clear();

		// Find all script components on Scene GameObjects and add those GameObjects
		// to their corresponding script class entity vector members
		for (int i = 0; i < gameObjects.size(); i++)
		{
			std::vector<Component*> components = gameObjects[i].GetComponents();

			for (int j = 0; j < components.size(); j++)
			{
				if (components[j]->GetTypeString() == "BoxCollider")
				{
					// Collect all BoxCollider components for collision detection in Update()
					colliders.push_back(static_cast<BoxCollider*>(components[j]));
				}
				if (components[j]->GetTypeString() == "CircleCollider")
				{
					// Collect all BoxCollider components for collision detection in Update()
					colliders.push_back(static_cast<CircleCollider*>(components[j]));
				}
				if (components[j]->GetTypeString() == "CompositeCollider")
				{
					// Collect all CompositeCollider components for collision detection in Update()
					colliders.push_back(static_cast<CompositeCollider*>(components[j]));
				}
			}
		}

		// Remake colliderPairs
		colliderPairs.clear();

		for (int i = 0; i < colliders.size(); i++)
		{
			for (int j = i + 1; j < colliders.size(); j++)
			{
				// If colliders don't belong to the same GameObject
				if (colliders.at(i)->GetParentID() != colliders.at(j)->GetParentID() && colliders.at(i)->GetTypeString() != "CompositeCollider" && colliders.at(j)->GetTypeString() != "CompositeCollider")
				{
					TagList coll1TagList = colliders.at(i)->GetParent()->GetTagList();
					TagList coll2TagList = colliders.at(j)->GetParent()->GetTagList();

					std::vector<std::string> coll1Ignored = coll1TagList.GetIgnoredTags();
					std::vector<std::string> coll2Ignored = coll2TagList.GetIgnoredTags();

					bool _ignorePair = false;

					for (std::string ignoredTag : coll1Ignored)
					{
						if (coll2TagList.HasTag(ignoredTag))
						{
							_ignorePair = true;
							break;
						}
					}
					if (!_ignorePair)
					{
						for (std::string ignoredTag : coll2Ignored)
						{
							if (coll1TagList.HasTag(ignoredTag))
							{
								_ignorePair = true;
								break;
							}
						}
					}
					if (!_ignorePair)
					{
						std::pair<Collider*, Collider*> newPair = { colliders.at(i), colliders.at(j) };
						colliderPairs.push_back(newPair);
					}
				}
			}
		}
	}

	std::vector<std::pair<FlatEngine::Collider*, FlatEngine::Collider*>> GameLoop::GetColliderPairs()
	{
		return colliderPairs;
	}

	void GameLoop::UpdateActiveRigidBodies()
	{
		// Get currently loaded scenes GameObjects
		gameObjects = FlatEngine::GetSceneObjects();
		rigidBodies.clear();

		// Find all script components on Scene GameObjects and add those GameObjects
		// to their corresponding script class entity vector members
		for (int i = 0; i < gameObjects.size(); i++)
		{
			std::vector<Component*> components = gameObjects[i].GetComponents();

			for (int j = 0; j < components.size(); j++)
			{
				if (components[j]->GetTypeString() == "RigidBody" && components[j]->IsActive())
				{
					// Collect all BoxCollider components for collision detection in Update()
					rigidBodies.push_back(static_cast<RigidBody*>(components[j]));
				}
			}
		}
	}

	void GameLoop::UpdateScripts()
	{
		for (int i = 0; i < activeScripts.size(); i++)
		{
			if (activeScripts[i]->_isActive)
			{
				// Profiler
				float timeStart = 0;
				if (_isDebugMode)
					timeStart = (float)FlatEngine::GetEngineTime();

				activeScripts[i]->Update(deltaTime);

				//// Profiler
				//if (_isDebugMode)
				//{
				//	float hangTime = (float)FlatEngine::GetEngineTime() - timeStart;
				//	if (activeScripts.size() > i && activeScripts[i])
				//		AddProcessData(activeScripts[i]->GetName() + "-on-" + activeScripts[i]->GetOwner()->GetName(), hangTime);
				//}
			}
		}
	}

	void GameLoop::Update(std::shared_ptr<Project> loadedProject)
	{
		if (GetLoadedScene()->GetPrimaryCamera() != nullptr)
		{
			GetLoadedScene()->GetPrimaryCamera()->Follow();
		}

		AddFrame();
		activeTime = time - pausedTime;


		//processTime = (float)FlatEngine::GetEngineTime();
		UpdateScripts();
		//processTime = (float)FlatEngine::GetEngineTime() - processTime;
		//LogFloat(processTime, "Update Scripts: ");


		float processTime = (float)FlatEngine::GetEngineTime();
		// Calculate RigidBody physics to use in collisions
		for (RigidBody* rigidBody : rigidBodies)
		{
			if (rigidBody->IsActive())
				rigidBody->CalculatePhysics(loadedProject->GetPhysicsSystem());
		}
		processTime = (float)FlatEngine::GetEngineTime() - processTime;
		//LogFloat(processTime, "CalculatePhysics: ");


		processTime = (float)FlatEngine::GetEngineTime();
		for (Collider* collider : colliders)
		{
			collider->ResetCollisions();
			collider->RecalculateBounds();
		}
		processTime = (float)FlatEngine::GetEngineTime() - processTime;
		//LogFloat(processTime, "ResetCollisions & Bounds: ");


		processTime = (float)FlatEngine::GetEngineTime();
		// Handle Collision updates here
		static int continuousCounter = 0;
		for (std::pair<Collider*, Collider*> colliderPair : colliderPairs)
		{
			Collider* collider1 = colliderPair.first;
			Collider* collider2 = colliderPair.second;

			if (collider1->GetParent()->IsValid() && collider1 != nullptr && collider1->IsActive() && collider2->GetParent()->IsValid() && collider2 != nullptr && collider2->IsActive() && (!collider1->IsStatic() || !collider2->IsStatic()) && ((collider1->IsContinuous() || (!collider1->IsContinuous() && continuousCounter == 10)) || (collider2->IsContinuous() || (!collider2->IsContinuous() && continuousCounter == 10))))
			{
				if (collider2 != nullptr && (collider1->GetID() != collider2->GetID()) && collider2->IsActive())
				{
					if (collider1->GetActiveLayer() == collider2->GetActiveLayer())
						Collider::CheckForCollision(loadedProject->GetCollisionDetection(), collider1, collider2);
				}
			}
		}
		if (continuousCounter >= 10)
			continuousCounter = 0;
		continuousCounter++;

		processTime = (float)FlatEngine::GetEngineTime() - processTime;
		//LogFloat(processTime, "Collision Detection: ");



		processTime = (float)FlatEngine::GetEngineTime();
		// Apply RigidBody physics calculations
		for (RigidBody* rigidBody : rigidBodies)
			if (rigidBody->IsActive())
				rigidBody->ApplyPhysics((float)deltaTime, loadedProject->GetPhysicsSystem());

		processTime = (float)FlatEngine::GetEngineTime() - processTime;
		//LogFloat(processTime, "Apply Physics: ");
	}

	void GameLoop::Stop()
	{
		_started = false;
		_paused = false;
		framesCounted = 0;



		// Release all active scripts
		activeScripts.clear();

		// Load back up the saved version of the scene
		FlatEngine::LoadScene(startedScene);
	}

	void GameLoop::Pause()
	{
		// If the timer is running and isn't already paused
		if (_started && !_paused)
		{
			_paused = true;
			activeTime = time - pausedTime;
		}
	}

	void GameLoop::Unpause()
	{
		// If the timer is running and paused
		if (_started && _paused)
		{
			_paused = false;
			ResetCurrentTime();
			pausedTime = time - activeTime;
		}
	}

	void GameLoop::ResetCurrentTime()
	{
		currentTime = FlatEngine::GetEngineTime();
	}

	bool GameLoop::IsGamePaused()
	{
		return _gamePaused;
	}

	void GameLoop::PauseGame()
	{
		_gamePaused = true;
	}

	void GameLoop::UnpauseGame()
	{
		_gamePaused = false;
	}

	// In seconds
	long GameLoop::TimeEllapsedInSec()
	{
		if (_started)
		{
			return activeTime;
		}
		return 0;
	}

	// In ms
	double GameLoop::TimeEllapsedInMs()
	{
		if (_started)
		{
			return time * 1000;
		}
		return 0;
	}

	bool GameLoop::IsStarted()
	{
		return _started;
	}

	bool GameLoop::IsPaused()
	{
		return _paused && _started;
	}

	float GameLoop::GetAverageFps()
	{
		if (TimeEllapsedInSec() != 0)
			return (float)framesCounted / (float)activeTime;
		else
			return 200;
	}

	int GameLoop::GetFramesCounted()
	{
		return framesCounted;
	}

	void GameLoop::AddFrame()
	{
		framesCounted++;
	}

	float GameLoop::GetDeltaTime()
	{
		return deltaTime;
	}

	void GameLoop::SetFrameSkipped(bool _skipped)
	{
		if (_skipped)
			ResetCurrentTime();

		_frameSkipped = _skipped;
	}

	bool GameLoop::IsFrameSkipped()
	{
		return _frameSkipped;
	}
}