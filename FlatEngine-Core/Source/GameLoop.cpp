#include "GameLoop.h"
#include "FlatEngine.h"
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

// Multithreading
#include <vector>
#include <process.h>
//#include <thread>
#include <crtdefs.h>


namespace FlatEngine
{
	GameLoop::GameLoop()
	{
		_started = false;
		_gamePaused = false;
		_paused = false;
		_frameSkipped = false;
		time = 0.0f;
		activeTime = 0.0f;
		currentTime = 0;
		pausedTime = 0;
		framesCounted = 0;
		deltaTime = 0.005f;
		accumulator = deltaTime;
		startedScene = "";
	}

	GameLoop::~GameLoop()
	{
	}

	void GameLoop::Start()
	{
		// Handle Game Time
		time = 0.0f;
		activeTime = time - pausedTime;
		_paused = false;
		accumulator = 0.0f;

		// Save the name of the scene we started with so we can load it back up when we stop
		startedScene = FlatEngine::GetLoadedScenePath();

		_started = true;

		std::map<long, std::map<std::string, GameScript>>& gameScripts = GetLoadedScene()->GetScripts();

		RunAwakeAndStart();

		// Get currently loaded scenes GameObjects and instantiate script objects for them
		//gameObjects = FlatEngine::GetSceneObjects();
		//activeScripts.clear();
		//InitializeScriptObjects(gameObjects);

		//CollectPhysicsBodies();

		currentTime = FlatEngine::GetEngineTime();
		
		for (std::thread* thread : m_threads)
		{
			delete thread;
			thread = nullptr;
		}
		m_threads.clear();

		//for (int i = 0; i < GetLoadedScene()->GetColliderPairs().size(); i++)
		//{
		//	std::thread thread = new std::thread();
		//	m_threads,
		//}
	}

	void GameLoop::UpdateScripts()
	{
		std::map<long, std::map<std::string, GameScript>> &gameScripts = GetLoadedScene()->GetScripts();

		for (std::pair<long, std::map<std::string, GameScript>> owner : gameScripts)
		{
			for (std::pair<std::string, GameScript> script : owner.second)
			{
				if (script.second._isActive)
				{
					// Profiler
					//float timeStart = 0;
					//if (_isDebugMode)
					//	timeStart = (float)FlatEngine::GetEngineTime();

					script.second.Update(deltaTime);

					// Profiler
					//if (_isDebugMode)
					//{
					//	float hangTime = (float)FlatEngine::GetEngineTime() - timeStart;
					//	if (script.second.size() > i && activeScripts[i])
					//		AddProcessData(activeScripts[i]->GetName() + "-on-" + activeScripts[i]->GetOwner()->GetName(), hangTime);
					//}
				}
			}
		}
	}

	void GameLoop::Update(float gridstep, Vector2 viewportCenter)
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
		//LogFloat(processTime, "Update Scripts: ");


		processTime = (float)FlatEngine::GetEngineTime();
		for (std::pair<const long, RigidBody> &rigidBody : GetLoadedScene()->GetRigidBodies())
		{
			if (rigidBody.second.IsActive())
				rigidBody.second.CalculatePhysics();
		}
		processTime = (float)FlatEngine::GetEngineTime() - processTime;
		//LogFloat(processTime, "CalculatePhysics: ");


		//processTime = (float)FlatEngine::GetEngineTime();
		std::map<long, std::map<long, BoxCollider>> &boxColliders = GetLoadedScene()->GetBoxColliders();
		for (std::map<long, std::map<long, BoxCollider>>::iterator outerIter = boxColliders.begin(); outerIter != boxColliders.end();)
		{
			for (std::map<long, BoxCollider>::iterator innerIter = outerIter->second.begin(); innerIter != outerIter->second.end();)
			{
				innerIter->second.ResetCollisions();
				innerIter->second.RecalculateBounds(gridstep, viewportCenter);
				innerIter++;
			}
			outerIter++;
		}
		processTime = (float)FlatEngine::GetEngineTime() - processTime;
		//LogFloat(processTime, "ResetCollisions & Bounds: ");

		std::vector<std::thread*> threads = std::vector<std::thread*>();

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
					{
						//std::thread thread = std::thread(Collider::CheckForCollision, std::ref(collider1), std::ref(collider2));
						//threads.push_back(&thread);
						Collider::CheckForCollision(collider1, collider2);
					}
				}
			}
		}
		if (continuousCounter >= 10)
			continuousCounter = 0;
		continuousCounter++;

		for (std::thread *thread : threads)
		{
			thread->join();
		}

		m_threads.clear();
		//for (std::thread* thread : threads)
		//{
		//	delete thread;
		//	thread = nullptr;
		//}

		processTime = (float)FlatEngine::GetEngineTime() - processTime;
		FlatEngine::AddProcessData("Collision Testing", processTime);
		//LogFloat(processTime, "Collision Detection");


		//processTime = (float)FlatEngine::GetEngineTime();
		// Apply RigidBody physics calculations
		for (std::pair<const long, RigidBody> &rigidBody : GetLoadedScene()->GetRigidBodies())
		{
			if (rigidBody.second.IsActive())
			{
				rigidBody.second.ApplyPhysics((float)deltaTime);
			}
		}
		processTime = (float)FlatEngine::GetEngineTime() - processTime;
		//LogFloat(processTime, "Apply Physics: ");
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
	float GameLoop::TimeEllapsedInSec()
	{
		if (_started)
		{
			return activeTime;
		}
		return 0;
	}

	// In ms
	long GameLoop::TimeEllapsedInMs()
	{
		if (_started)
		{
			return (long)(time * 1000.0f);
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

	long GameLoop::GetFramesCounted()
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