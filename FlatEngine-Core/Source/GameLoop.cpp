#include "GameLoop.h"
#include "FlatEngine.h"
#include "GameObject.h"
#include "Scene.h"
#include "RigidBody.h"
#include "Collider.h"
#include "BoxCollider.h"
#include "CircleCollider.h"
#include "CompositeCollider.h"
#include "Transform.h"
#include "Script.h"
#include "Button.h"
#include "Canvas.h"
#include "TagList.h"
#include "Camera.h"
#include "Project.h"
#include "Scene.h"
#include "CharacterController.h"

#include <vector>
#include <process.h>
#include <crtdefs.h>

namespace FL = FlatEngine;


namespace FlatEngine
{
	GameLoop::GameLoop()
	{
		m_b_started = false;
		m_b_gamePaused = false;
		m_b_paused = false;
		m_b_frameSkipped = false;
		m_time = 0.0f;
		m_activeTime = 0.0f;
		m_currentTime = 0;
		m_pausedTime = 0;
		m_framesCounted = 0;
		m_deltaTime = 0.005f;
		m_accumulator = m_deltaTime;		
		m_hoveredButtons = std::vector<Button>();
		m_objectsQueuedForDelete = std::vector<long>();
	}

	GameLoop::~GameLoop()
	{
	}

	void GameLoop::Start()
	{
		m_time = 0.0f;
		m_activeTime = m_time - m_pausedTime;
		m_b_paused = false;
		m_accumulator = 0.0f;

		// Save the name of the scene we started with so we can load it back up when we stop
		m_b_started = true;
		RunAwakeAndStart();
		m_currentTime = GetEngineTime();
	}

	void GameLoop::Update(float gridstep, Vector2 viewportCenter)
	{
		AddFrame();
		m_activeTime = m_time - m_pausedTime;

		HandleCamera();
		ResetCharacterControllers();
		HandleButtons();
		RunUpdateOnScripts();
		CalculatePhysics();
		HandleCollisions(gridstep, viewportCenter);
		ApplyPhysics();
	}

	void GameLoop::Stop()
	{
		m_b_started = false;
		m_b_paused = false;
		m_framesCounted = 0;
	}

	void GameLoop::Pause()
	{		
		if (m_b_started && !m_b_paused)
		{
			m_b_paused = true;
			m_activeTime = m_time - m_pausedTime;
		}
	}

	void GameLoop::Unpause()
	{		
		if (m_b_started && m_b_paused)
		{
			m_b_paused = false;
			ResetCurrentTime();
			m_pausedTime = m_time - m_activeTime;			
		}
	}

	void GameLoop::ResetCurrentTime()
	{
		m_currentTime = GetEngineTime();
	}

	void GameLoop::HandleCamera()
	{
		if (GetLoadedScene()->GetPrimaryCamera() != nullptr)
		{
			GetLoadedScene()->GetPrimaryCamera()->Follow();
		}
	}

	void GameLoop::HandleButtons()
	{
		static bool b_hasLeftClicked = false;
		static bool b_hasRightClicked = false;

		if (CheckForMouseOver())
		{
			for (Button& hovered : m_hoveredButtons)
			{
				if (hovered.GetActiveLayer() >= GetFirstUnblockedLayer())
				{
					GameObject* owner = hovered.GetParent();
					
					if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !b_hasLeftClicked)
					{
						b_hasLeftClicked = true;
						CallLuaButtonEventFunction(owner, LuaEventFunction::OnButtonLeftClick);
					}					
					else if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
					{
						b_hasLeftClicked = false;
					}	

					if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && !b_hasRightClicked)
					{
						b_hasRightClicked = true;
						CallLuaButtonEventFunction(owner, LuaEventFunction::OnButtonRightClick);
					}					
					else if (ImGui::IsMouseReleased(ImGuiMouseButton_Right))
					{
						b_hasRightClicked = false;
					}
				}
			}
		}
	}

	bool GameLoop::CheckForMouseOver()
	{
		std::vector<Button> lastHovered = m_hoveredButtons;
		ResetHoveredButtons();

		std::map<long, Button>& buttons = GetLoadedScene()->GetButtons();

		for (std::pair<long, Button> buttonPair : buttons)
		{
			if (buttonPair.second.IsActive())
			{
				Transform* transform = buttonPair.second.GetParent()->GetTransform();
				Vector4 activeEdges = buttonPair.second.GetActiveEdges();
				Vector2 mousePos = ImGui::GetIO().MousePos;

				if (AreCollidingViewport(activeEdges, Vector4(mousePos.y, mousePos.x, mousePos.y, mousePos.x)))
				{
					m_hoveredButtons.push_back(buttonPair.second);
					buttonPair.second.SetMouseIsOver(true);

					if (buttonPair.second.GetActiveLayer() >= GetFirstUnblockedLayer())
					{
						GameObject* owner = buttonPair.second.GetParent();
						CallLuaButtonEventFunction(buttonPair.second.GetParent(), LuaEventFunction::OnButtonMouseOver);
					}
				}
			}
		}

		// Mouse Enter
		for (Button& hoveredButton : m_hoveredButtons)
		{
			bool b_mouseJustEntered = true;
			for (Button& lastHovered : lastHovered)
			{
				if (hoveredButton.GetID() == lastHovered.GetID())
				{
					b_mouseJustEntered = false;
				}
			}
			if (b_mouseJustEntered)
			{
				CallLuaButtonEventFunction(hoveredButton.GetParent(), LuaEventFunction::OnButtonMouseEnter);
			}
		}

		// Mouse Leave
		for (Button& lastHovered : lastHovered)
		{
			bool b_stillHovered = false;
			for (Button& hoveredButton : m_hoveredButtons)
			{
				if (hoveredButton.GetID() == lastHovered.GetID())
				{
					b_stillHovered = true;
				}
			}
			if (!b_stillHovered && lastHovered.GetParent() != nullptr)
			{
				CallLuaButtonEventFunction(lastHovered.GetParent(), LuaEventFunction::OnButtonMouseLeave);
			}
		}

		return m_hoveredButtons.size() > 0;
	}

	void GameLoop::ResetHoveredButtons()
	{
		for (Button button : m_hoveredButtons)
		{
			button.SetMouseIsOver(false);
		}

		m_hoveredButtons.clear();
	}

	int GameLoop::GetFirstUnblockedLayer()
	{
		Canvas canvas = GetFirstUnblockedCanvas();
		if (canvas.GetID() != -1)
		{
			return canvas.GetLayerNumber();
		}
		else
		{
			return -1;
		}
	}

	Canvas GameLoop::GetFirstUnblockedCanvas()
	{
		Canvas lowestUnblockedCanvas = Canvas(-1);
		int lowestUnblockedLayer = 0;
		Vector2 mousePos = ImGui::GetIO().MousePos;
		std::map<long, Canvas> &canvases = GetLoadedScene()->GetCanvases();

		for (std::pair<long, Canvas> canvasPair : canvases)
		{
			Canvas canvas = canvasPair.second;
			Vector4 activeEdges = canvas.GetActiveEdges();
			bool b_blocksLayers = canvas.GetBlocksLayers();
			int layerNumber = canvas.GetLayerNumber();

			if (AreCollidingViewport(activeEdges, Vector4(mousePos.y, mousePos.x, mousePos.y, mousePos.x)) && b_blocksLayers && layerNumber >= lowestUnblockedLayer)
			{
				lowestUnblockedCanvas = canvas;
				lowestUnblockedLayer = canvas.GetLayerNumber();
			}
		}

		return lowestUnblockedCanvas;
	}

	void GameLoop::ResetCharacterControllers()
	{
		for (std::pair<const long, CharacterController>& owner : FL::GetLoadedScene()->GetCharacterControllers())
		{
			owner.second.SetMoving(false);
		}
	}

	void GameLoop::CalculatePhysics()
	{
		float processTime = (float)GetEngineTime();
		for (std::pair<const long, RigidBody>& rigidBody : GetLoadedScene()->GetRigidBodies())
		{
			if (rigidBody.second.IsActive())
			{
				rigidBody.second.CalculatePhysics();
			}
		}
		processTime = (float)GetEngineTime() - processTime;
		//LogFloat(processTime, "CalculatePhysics: ");
	}

	void GameLoop::HandleCollisions(float gridstep, Vector2 viewportCenter)
	{		
		std::map<long, std::map<long, BoxCollider>>& boxColliders = GetLoadedScene()->GetBoxColliders();
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

		float processTime = (float)GetEngineTime();		
		static int continuousCounter = 0;
		for (std::pair<Collider*, Collider*>& colliderPair : GetLoadedScene()->GetColliderPairs())
		{
			Collider* collider1 = colliderPair.first;
			Collider* collider2 = colliderPair.second;

			if (collider1->GetParent() != nullptr && collider1 != nullptr && collider1->IsActive() && collider2->GetParent() != nullptr && collider2 != nullptr && collider2->IsActive() && (!collider1->IsStatic() || !collider2->IsStatic()) && ((collider1->IsContinuous() || (!collider1->IsContinuous() && continuousCounter == 10)) || (collider2->IsContinuous() || (!collider2->IsContinuous() && continuousCounter == 10))))
			{
				if (collider2 != nullptr && (collider1->GetID() != collider2->GetID()) && collider2->IsActive())
				{
					if (collider1->GetActiveLayer() == collider2->GetActiveLayer())
					{
						Collider::CheckForCollision(collider1, collider2);
					}
				}
			}
		}
		if (continuousCounter >= 10)
			continuousCounter = 0;
		continuousCounter++;

		processTime = (float)GetEngineTime() - processTime;
		AddProcessData("Collision Testing", processTime);
		//LogFloat(processTime, "Collision Detection");
	}

	void GameLoop::ApplyPhysics()
	{
		float processTime = (float)GetEngineTime();
		for (std::pair<const long, RigidBody>& rigidBody : GetLoadedScene()->GetRigidBodies())
		{
			if (rigidBody.second.IsActive())
			{
				rigidBody.second.ApplyPhysics((float)m_deltaTime);
			}
		}
		processTime = (float)GetEngineTime() - processTime;
		//LogFloat(processTime, "Apply Physics: ");
	}

	void GameLoop::RunUpdateOnScripts()
	{
		//float processTime = (float)GetEngineTime();
		RunLuaFuncOnAllScripts("Update");
		//processTime = (float)GetEngineTime() - processTime;
		//LogFloat(processTime, "Update Scripts: ");
	}

	void GameLoop::AddObjectToDeleteQueue(long objectID)
	{
		m_objectsQueuedForDelete.push_back(objectID);
	}

	void GameLoop::DeleteObjectsInDeleteQueue()
	{
		for (long objectID : m_objectsQueuedForDelete)
		{
			DeleteGameObject(objectID);
		}
		m_objectsQueuedForDelete.clear();
	}

	bool GameLoop::IsGamePaused()
	{
		return m_b_gamePaused;
	}

	void GameLoop::PauseGame()
	{
		m_b_gamePaused = true;
	}

	void GameLoop::UnpauseGame()
	{
		m_b_gamePaused = false;
	}

	float GameLoop::TimeEllapsedInSec()
	{
		if (m_b_started)
		{
			return m_activeTime;
		}
		return 0;
	}

	long GameLoop::TimeEllapsedInMs()
	{
		if (m_b_started)
		{
			return (long)(m_time * 1000.0f);
		}
		return 0;
	}

	bool GameLoop::IsStarted()
	{
		return m_b_started;
	}

	bool GameLoop::IsPaused()
	{
		return m_b_paused && m_b_started;
	}

	long GameLoop::GetFramesCounted()
	{
		return m_framesCounted;
	}

	void GameLoop::AddFrame()
	{
		m_framesCounted++;
	}

	float GameLoop::GetDeltaTime()
	{
		return m_deltaTime;
	}

	void GameLoop::SetFrameSkipped(bool b_skipped)
	{
		if (b_skipped)
			ResetCurrentTime();

		m_b_frameSkipped = b_skipped;
	}

	bool GameLoop::IsFrameSkipped()
	{
		return m_b_frameSkipped;
	}
}