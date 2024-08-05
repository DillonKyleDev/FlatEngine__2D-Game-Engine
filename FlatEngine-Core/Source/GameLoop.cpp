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
#include "ECSManager.h"


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
		//gameObjects = FlatEngine::GetSceneObjects();
		//activeScripts.clear();
		//InitializeScriptObjects(gameObjects);

		//CollectPhysicsBodies();

		currentTime = (double)FlatEngine::GetEngineTime();
	}

	void GameLoop::UpdateScripts()
	{
		//for (int i = 0; i < activeScripts.size(); i++)
		//{
		//	if (activeScripts[i]->_isActive)
		//	{
		//		// Profiler
		//		float timeStart = 0;
		//		if (_isDebugMode)
		//			timeStart = (float)FlatEngine::GetEngineTime();

		//		activeScripts[i]->Update(deltaTime);

		//		//// Profiler
		//		//if (_isDebugMode)
		//		//{
		//		//	float hangTime = (float)FlatEngine::GetEngineTime() - timeStart;
		//		//	if (activeScripts.size() > i && activeScripts[i])
		//		//		AddProcessData(activeScripts[i]->GetName() + "-on-" + activeScripts[i]->GetOwner()->GetName(), hangTime);
		//		//}
		//	}
		//}
	}

	void GameLoop::Update(std::shared_ptr<Project> loadedProject)
	{
		if (GetLoadedScene()->GetPrimaryCamera() != nullptr)
		{
			GetLoadedScene()->GetPrimaryCamera()->Follow();
		}

		AddFrame();
		activeTime = time - pausedTime;


		float processTime = (float)FlatEngine::GetEngineTime();
		UpdateScripts();
		processTime = (float)FlatEngine::GetEngineTime() - processTime;
		LogFloat(processTime, "Update Scripts: ");


		processTime = (float)FlatEngine::GetEngineTime();
		for (std::pair<RigidBody, long> &rigidBody : GetLoadedScene()->GetRigidBodies())
		{
			if (rigidBody.first.IsActive())
				rigidBody.first.CalculatePhysics(loadedProject->GetPhysicsSystem());
		}
		processTime = (float)FlatEngine::GetEngineTime() - processTime;
		LogFloat(processTime, "CalculatePhysics: ");


		processTime = (float)FlatEngine::GetEngineTime();
		for (std::pair<Collider*, long> collider : GetLoadedScene()->GetColliders())
		{
			collider.first->ResetCollisions();
			collider.first->RecalculateBounds();
		}
		processTime = (float)FlatEngine::GetEngineTime() - processTime;
		LogFloat(processTime, "ResetCollisions & Bounds: ");


		processTime = (float)FlatEngine::GetEngineTime();
		// Handle Collision updates here
		static int continuousCounter = 0;
		for (std::pair<Collider*, Collider*> &colliderPair : GetLoadedScene()->GetColliderPairs())
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
		LogFloat(processTime, "Collision Detection: ");


		processTime = (float)FlatEngine::GetEngineTime();
		// Apply RigidBody physics calculations
		for (std::pair<RigidBody, long> &rigidBody : GetLoadedScene()->GetRigidBodies())
		{
			if (rigidBody.first.IsActive())
			{
				rigidBody.first.ApplyPhysics((float)deltaTime, loadedProject->GetPhysicsSystem());
			}
		}
		processTime = (float)FlatEngine::GetEngineTime() - processTime;
		LogFloat(processTime, "Apply Physics: ");
	}

	void GameLoop::Stop()
	{
		_started = false;
		_paused = false;
		framesCounted = 0;



		// Release all active scripts
		//activeScripts.clear();

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