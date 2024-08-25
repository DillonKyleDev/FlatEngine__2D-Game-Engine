#include "Collider.h"
#include "BoxCollider.h"
#include "CircleCollider.h"
#include "RigidBody.h"
#include "FlatEngine.h"
#include "GameObject.h"
#include "Transform.h"
#include "Project.h"
#include "imgui_internal.h"
#include <cmath>
#include "CompositeCollider.h"

using BoxCollider = FlatEngine::BoxCollider;
using CircleCollider = FlatEngine::CircleCollider;
using GameObject = FlatEngine::GameObject;

namespace FlatEngine
{
	Collider::Collider(long myID, long parentID)
	{		
		SetID(myID);
		SetParentID(parentID);
		
		_isComposite = false;
		OnActiveCollision = nullptr;
		OnCollisionEnter = nullptr;
		OnCollisionLeave = nullptr;
		activeOffset = Vector2(0, 0);
		previousPosition = Vector2(0, 0);
		centerGrid = Vector2(0, 0);
		nextCenterGrid = Vector2(0, 0);
		centerCoord = Vector2(0, 0);
		nextCenterCoord = Vector2(0, 0);
		rotation = 0;
		activeRadiusScreen = 0;
		activeRadiusGrid = 0;
		activeLayer = 0;
		_isColliding = false;
		_isContinuous = true;
		_isStatic = false;
		_isSolid = true;
		_showActiveRadius = false;
		_onActiveCollidingSet = false;
		_onCollisionEnterSet = false;
		_onCollisionLeaveSet = false;

		_isCollidingRight = false;
		_isCollidingLeft = false;
		_isCollidingBottom = false;
		_isCollidingTop = false;

		_isCollidingTopRight = false;
		_isCollidingTopLeft = false;
		_isCollidingBottomRight = false;
		_isCollidingBottomLeft = false;

		_rightCollisionStatic = false;
		_leftCollisionStatic = false;
		_bottomCollisionStatic = false;
		_topCollisionStatic = false;
		_bottomLeftCollisionStatic = false;
		_bottomRightCollisionStatic = false;

		_rightCollisionSolid = false;
		_leftCollisionSolid = false;
		_bottomCollisionSolid = false;
		_topCollisionSolid = false;
		_bottomLeftCollisionSolid = false;
		_bottomRightCollisionSolid = false;
		_topLeftCollisionSolid = false;
		_topRightCollisionSolid = false;

		rightCollision = 0;
		leftCollision = 0;
		bottomCollision = 0;
		topCollision = 0;

		leftCollidedPosition = Vector2(0,0);
		rightCollidedPosition = Vector2(0, 0);
		bottomCollidedPosition = Vector2(0, 0);
		topCollidedPosition = Vector2(0, 0);
		topRightCollidedPosition = Vector2(0, 0);
		bottomRightCollidedPosition = Vector2(0, 0);
		topLeftCollidedPosition = Vector2(0, 0);
		bottomLeftCollidedPosition = Vector2(0, 0);
	}

	Collider::Collider(Collider* toCopy, long newParentID, long myID)
	{		
		if (myID != -1)
			SetID(myID);
		else
			SetID(GetNextComponentID());
		SetParentID(newParentID);
		SetActive(toCopy->IsActive());

		_isComposite = toCopy->_isComposite;
		OnActiveCollision = toCopy->OnActiveCollision;
		OnCollisionEnter = toCopy->OnCollisionEnter;
		OnCollisionLeave = toCopy->OnCollisionLeave;
		activeOffset = toCopy->activeOffset;
		previousPosition = toCopy->previousPosition;
		centerGrid = toCopy->centerGrid;
		nextCenterGrid = toCopy->nextCenterGrid;
		centerCoord = toCopy->centerCoord;
		nextCenterCoord = toCopy->nextCenterCoord;
		rotation = toCopy->rotation;
		activeRadiusScreen = toCopy->activeRadiusScreen;
		activeRadiusGrid = toCopy->activeRadiusGrid;
		activeLayer = toCopy->activeLayer;
		_isColliding = toCopy->_isColliding;
		_isContinuous = toCopy->_isContinuous;
		_isStatic = toCopy->_isStatic;
		_isSolid = toCopy->_isSolid;
		_showActiveRadius = toCopy->_showActiveRadius;
		_onActiveCollidingSet = toCopy->_onActiveCollidingSet;
		_onCollisionEnterSet = toCopy->_onCollisionEnterSet;
		_onCollisionLeaveSet = toCopy->_onCollisionLeaveSet;

		collidedPosition = Vector2(0, 0);

		_isCollidingRight = false;
		_isCollidingLeft = false;
		_isCollidingBottom = false;
		_isCollidingTop = false;

		_isCollidingTopRight = false;
		_isCollidingTopLeft = false;
		_isCollidingBottomRight = false;
		_isCollidingBottomLeft = false;

		_rightCollisionStatic = false;
		_leftCollisionStatic = false;
		_bottomCollisionStatic = false;
		_topCollisionStatic = false;
		_bottomLeftCollisionStatic = false;
		_bottomRightCollisionStatic = false;

		_rightCollisionSolid = false;
		_leftCollisionSolid = false;
		_bottomCollisionSolid = false;
		_topCollisionSolid = false;
		_bottomLeftCollisionSolid = false;
		_bottomRightCollisionSolid = false;
		_topLeftCollisionSolid = false;
		_topRightCollisionSolid = false;

		rightCollision = 0;
		leftCollision = 0;
		bottomCollision = 0;
		topCollision = 0;

		leftCollidedPosition = Vector2(0, 0);
		rightCollidedPosition = Vector2(0, 0);
		bottomCollidedPosition = Vector2(0, 0);
		topCollidedPosition = Vector2(0, 0);
		topRightCollidedPosition = Vector2(0, 0);
		bottomRightCollidedPosition = Vector2(0, 0);
		topLeftCollidedPosition = Vector2(0, 0);
		bottomLeftCollidedPosition = Vector2(0, 0);
	}

	Collider::~Collider()
	{
	}

	void Collider::SetOnActiveCollision(std::function<void(GameObject*, GameObject*)> callback)
	{
		OnActiveCollision = callback;
		_onActiveCollidingSet = true;
	}

	void Collider::SetOnCollisionEnter(std::function<void(GameObject*, GameObject*)> callback)
	{
		OnCollisionEnter = callback;
		_onCollisionEnterSet = true;
	}

	void Collider::SetOnCollisionLeave(std::function<void(GameObject*, GameObject*)> callback)
	{
		OnCollisionLeave = callback;
		_onCollisionLeaveSet = true;
	}

	bool Collider::CheckForCollision(Collider* collider1, Collider* collider2)
	{
		bool b_colliding = false;
		Vector2 collider1Center = collider1->GetCenterGrid();
		Vector2 collider2Center = collider2->GetCenterGrid();

		//LogString("Checking Collision for " + collider1->GetParent()->GetName() + " and " + collider2->GetParent()->GetName());

		// Calculate center distance with pythag
		float rise = std::abs(collider1Center.y - collider2Center.y);
		float run = std::abs(collider1Center.x - collider2Center.x);
		float centerDistance = std::sqrt((rise * rise) + (run * run));

		// If they are close enough to check for collision ( actually colliding if they are both CircleColliders )
		if (centerDistance < collider1->GetActiveRadiusGrid() + collider2->GetActiveRadiusGrid())
		{
			// Both BoxColliders
			if (collider1->GetTypeString() == "BoxCollider" && collider2->GetTypeString() == "BoxCollider")
			{
				BoxCollider* boxCol1 = static_cast<BoxCollider*>(collider1);
				BoxCollider* boxCol2 = static_cast<BoxCollider*>(collider2);

				b_colliding = CheckForCollisionBoxBox(boxCol1, boxCol2);
			}
			// First CircleCollider second BoxCollider
			else if (collider1->GetTypeString() == "CircleCollider" && collider2->GetTypeString() == "BoxCollider")
			{
				CircleCollider* circleCol = static_cast<CircleCollider*>(collider1);
				BoxCollider* boxCol = static_cast<BoxCollider*>(collider2);

				b_colliding = Collider::CheckForCollisionBoxCircle(boxCol, circleCol);
			}
			// First BoxCollider second CircleCollider
			else if (collider1->GetTypeString() == "BoxCollider" && collider2->GetTypeString() == "CircleCollider")
			{
				BoxCollider* boxCol = static_cast<BoxCollider*>(collider1);
				CircleCollider* circleCol = static_cast<CircleCollider*>(collider2);

				b_colliding = Collider::CheckForCollisionBoxCircle(boxCol, circleCol);
			}
			// Both CircleColliders ( already true if made if past activeRadius check )
			else if (collider1->GetTypeString() == "CircleCollider" && collider2->GetTypeString() == "CircleCollider")
			{
				BoxCollider* circleCol1 = static_cast<BoxCollider*>(collider1);
				BoxCollider* circleCol2 = static_cast<BoxCollider*>(collider2);

				b_colliding = true;
			}
		}

		if (b_colliding)
		{
			// Set b_colliding
			collider1->SetColliding(true);
			collider2->SetColliding(true);

			// Add colliding objects
			if (!collider1->_isComposite)
			{
				collider1->AddCollidingObject(collider2);
				// For Collider events - Fire OnActiveCollision while there is a collision happening
				CallLuaOnActiveCollision(collider1->GetParent(), collider2);
				if (collider1->OnActiveCollisionSet())
					collider1->OnActiveCollision(collider1->GetParent(), collider2->GetParent());
			}
			else
			{
				CompositeCollider* compositeCollider = collider1->GetParent()->GetCompositeCollider();
				compositeCollider->AddCollidingObject(collider2);
				if (compositeCollider->OnActiveCollisionSet())
					compositeCollider->OnActiveCollision(collider1->GetParent(), collider2->GetParent());
			}

			if (!collider2->_isComposite)
			{
				collider2->AddCollidingObject(collider1);
				// For Collider events - Fire OnActiveCollision while there is a collision happening
				CallLuaOnActiveCollision(collider2->GetParent(), collider1);
				if (collider2->OnActiveCollisionSet())
					collider2->OnActiveCollision(collider2->GetParent(), collider1->GetParent());
			}
			else
			{
				CompositeCollider* compositeCollider = collider2->GetParent()->GetCompositeCollider();
				compositeCollider->AddCollidingObject(collider1);
				if (compositeCollider->OnActiveCollisionSet())
					compositeCollider->OnActiveCollision(collider2->GetParent(), collider1->GetParent());
			}
		}

		return b_colliding;
	}

	bool Collider::CheckForCollisionBoxCircle(BoxCollider* boxCol, CircleCollider* circleCol)
	{
		bool b_colliding = false;
		Vector2 circleCenterGrid = circleCol->GetNextCenterGrid(); // Get next center so no overlap happens this frame
		Vector2 boxCenterGrid = boxCol->GetNextCenterGrid();       // -- ^^^
		Vector2 circlePos = circleCol->GetParent()->GetTransform()->GetTruePosition();
		Vector2 boxPos = boxCol->GetParent()->GetTransform()->GetTruePosition();
		float boxHalfWidth = boxCol->GetActiveWidth() / 2;
		float boxHalfHeight = boxCol->GetActiveHeight() / 2;
		float circleActiveRadius = circleCol->GetActiveRadiusGrid();

		float A_TopEdge = circleCol->nextActiveTop;
		float A_RightEdge = circleCol->nextActiveRight;
		float A_BottomEdge = circleCol->nextActiveBottom;
		float A_LeftEdge = circleCol->nextActiveLeft;

		float B_TopEdge = boxCol->nextActiveTop;
		float B_RightEdge = boxCol->nextActiveRight;
		float B_BottomEdge = boxCol->nextActiveBottom;
		float B_LeftEdge = boxCol->nextActiveLeft;

		float yFromCol;
		float xFromCol;

		// Check how colliders are oriented in relation to each other //

		// check for direct left/right/top/bottom collisions
		// Circle left - Box right
		if (circleCenterGrid.x < boxCenterGrid.x && circleCenterGrid.y < B_TopEdge && circleCenterGrid.y > B_BottomEdge && (B_LeftEdge - circleCenterGrid.x < circleActiveRadius))
		{
			b_colliding = true;
			if (circleCol->IsSolid() && boxCol->IsSolid())
			{
				circleCol->_isCollidingRight = true;
				circleCol->_rightCollisionStatic = boxCol->IsStatic();
				circleCol->_rightCollisionSolid = boxCol->IsSolid();
				boxCol->_isCollidingLeft = true;
				boxCol->_leftCollisionStatic = circleCol->IsStatic();
				boxCol->_leftCollisionSolid = circleCol->IsSolid();
				circleCol->rightCollision = B_LeftEdge;
				boxCol->leftCollision = A_RightEdge;

				// Calculate at what Transform positions the collision technically happened and store it for repositioning in RigidBody
				circleCol->rightCollidedPosition = Vector2(B_LeftEdge - circleActiveRadius + 0.001f, circlePos.y);
				boxCol->leftCollidedPosition = Vector2(A_RightEdge + boxHalfWidth - 0.001f, boxPos.y);
			}
		}
		// Circle right - Box left
		else if (circleCenterGrid.x > boxCenterGrid.x && circleCenterGrid.y < B_TopEdge && circleCenterGrid.y > B_BottomEdge && (circleCenterGrid.x - B_RightEdge < circleActiveRadius))
		{
			b_colliding = true;
			if (circleCol->IsSolid() && boxCol->IsSolid())
			{
				circleCol->_isCollidingLeft = true;
				circleCol->_leftCollisionStatic = boxCol->IsStatic();
				circleCol->_leftCollisionSolid = boxCol->IsSolid();
				boxCol->_isCollidingRight = true;
				boxCol->_rightCollisionStatic = circleCol->IsStatic();
				boxCol->_rightCollisionSolid = circleCol->IsSolid();
				circleCol->leftCollision = B_RightEdge;
				boxCol->rightCollision = A_LeftEdge;

				// Calculate at what Transform positions the collision technically happened and store it for repositioning in RigidBody
				circleCol->leftCollidedPosition = Vector2(B_RightEdge + circleActiveRadius - 0.001f, circlePos.y);
				boxCol->rightCollidedPosition = Vector2(A_LeftEdge - boxHalfWidth + 0.001f, boxPos.y);
			}
		}
		// Circle Top - Box Bottom
		else if (circleCenterGrid.y > boxCenterGrid.y && circleCenterGrid.x < B_RightEdge && circleCenterGrid.x > B_LeftEdge && (circleCenterGrid.y - B_TopEdge < circleActiveRadius))
		{
			b_colliding = true;
			if (circleCol->IsSolid() && boxCol->IsSolid())
			{
				circleCol->_isCollidingBottom = true;
				circleCol->_bottomCollisionStatic = boxCol->IsStatic();
				circleCol->_bottomCollisionSolid = boxCol->IsSolid();
				boxCol->_isCollidingTop = true;
				boxCol->_topCollisionStatic = circleCol->IsStatic();
				boxCol->_topCollisionSolid = circleCol->IsSolid();
				circleCol->bottomCollision = B_TopEdge;
				boxCol->topCollision = A_BottomEdge;

				// Calculate at what Transform positions the collision technically happened and store it for repositioning in RigidBody
				boxCol->topCollidedPosition = Vector2(boxPos.x, A_BottomEdge - boxHalfHeight - 0.001f);
				circleCol->bottomCollidedPosition = Vector2(circlePos.x, B_TopEdge + circleActiveRadius - 0.001f);
			}
		}
		// Circle Bottom - Box Top
		else if (circleCenterGrid.y < boxCenterGrid.y && circleCenterGrid.x < B_RightEdge && circleCenterGrid.x > B_LeftEdge && (B_BottomEdge - circleCenterGrid.y < circleActiveRadius))
		{
			b_colliding = true;
			if (circleCol->IsSolid() && boxCol->IsSolid())
			{
				circleCol->_isCollidingTop = true;
				circleCol->_topCollisionStatic = boxCol->IsStatic();
				circleCol->_topCollisionSolid = boxCol->IsSolid();
				boxCol->_isCollidingBottom = true;
				boxCol->_bottomCollisionStatic = circleCol->IsStatic();
				boxCol->_bottomCollisionSolid = circleCol->IsSolid();
				circleCol->topCollision = B_BottomEdge;
				boxCol->bottomCollision = A_TopEdge;

				// Calculate at what Transform positions the collision technically happened and store it for repositioning in RigidBody
				boxCol->bottomCollidedPosition = Vector2(boxPos.x, A_TopEdge + boxHalfHeight - 0.001f);
				circleCol->topCollidedPosition = Vector2(circlePos.x, B_BottomEdge - circleActiveRadius + 0.001f);
			}
		}
		// Check for all other (corner) collisions
		// 
		// if circleCol is to the right of boxCol
		else if (circleCenterGrid.x > boxCenterGrid.x)
		{
			// if circleCol is below boxCol
			if (circleCenterGrid.y < boxCenterGrid.y)
			{
				// Calculate distance from corner with pythag
				float rise = std::abs(B_BottomEdge - circleCenterGrid.y);
				float run = std::abs(B_RightEdge - circleCenterGrid.x);
				float cornerDistance = std::sqrt((rise * rise) + (run * run));

				// If colliding
				if (cornerDistance < circleActiveRadius)
				{
					b_colliding = true;
					if (circleCol->IsSolid() && boxCol->IsSolid())
					{
						float leftRightOverlap = B_RightEdge - A_LeftEdge;
						float topBottomOverlap = A_TopEdge - B_BottomEdge;

						// Circle is approaching from the right (keep y pos, calculate x pos)
						if (leftRightOverlap < topBottomOverlap)
						{
							circleCol->_isCollidingLeft = true;
							circleCol->_isCollidingTopLeft = true;
							circleCol->_topLeftCollisionSolid = boxCol->IsSolid(); // new
							circleCol->_leftCollisionStatic = boxCol->IsStatic();
							circleCol->_leftCollisionSolid = boxCol->IsSolid();
							boxCol->_isCollidingRight = true;
							boxCol->_rightCollisionStatic = circleCol->IsStatic();
							boxCol->_rightCollisionSolid = boxCol->IsSolid();
							circleCol->leftCollision = B_RightEdge;
							boxCol->rightCollision = A_LeftEdge;

							// We know y and r, get x with pythag
							yFromCol = circleCenterGrid.y - B_BottomEdge;

							if (-(yFromCol * yFromCol) + (circleActiveRadius * circleActiveRadius) > 0) // no square roots of negatives
								xFromCol = std::sqrt(-(yFromCol * yFromCol) + (circleActiveRadius * circleActiveRadius));
							else
								xFromCol = 0;

							//boxCol->bottomRightCollidedPosition = Vector2(boxPos.x, A_BottomEdge - boxHalfHeight - 0.001f);
							circleCol->topLeftCollidedPosition = Vector2(B_RightEdge + xFromCol - 0.001f, circlePos.y);
						}
						// Circle is approaching from the bottom (keep x pos, calculate y pos)
						else {
							circleCol->_isCollidingTop = true;
							circleCol->_isCollidingTopLeft = true;
							circleCol->_topLeftCollisionSolid = boxCol->IsSolid();
							circleCol->_topCollisionStatic = boxCol->IsStatic();
							circleCol->_topCollisionSolid = boxCol->IsSolid();
							boxCol->_isCollidingBottom = true;
							boxCol->_isCollidingBottomRight = true;
							boxCol->_bottomCollisionStatic = circleCol->IsStatic();
							boxCol->_bottomCollisionSolid = circleCol->IsSolid();
							circleCol->topCollision = B_BottomEdge;
							boxCol->bottomCollision = A_TopEdge;

							// We know x and r, get y with pythag
							xFromCol = circleCenterGrid.x - B_RightEdge;

							if (-(xFromCol * xFromCol) + (circleActiveRadius * circleActiveRadius) > 0)
								yFromCol = std::sqrt(-(xFromCol * xFromCol) + (circleActiveRadius * circleActiveRadius));
							else
								yFromCol = 0;

							//boxCol->bottomRightCollidedPosition = Vector2(boxPos.x, A_BottomEdge - boxHalfHeight - 0.001f);
							circleCol->topLeftCollidedPosition = Vector2(circlePos.x, B_BottomEdge - yFromCol - 0.001f);
						}
					}
				}
			}
			// if circleCol is above boxCol
			else if (circleCenterGrid.y > boxCenterGrid.y)
			{
				float rise = std::abs(B_TopEdge - circleCenterGrid.y);
				float run = std::abs(B_RightEdge - circleCenterGrid.x);
				float cornerDistance = std::sqrt((rise * rise) + (run * run));

				// If colliding
				if (cornerDistance < circleActiveRadius)
				{
					b_colliding = true;
					if (circleCol->IsSolid() && boxCol->IsSolid())
					{
						float leftRightOverlap = B_RightEdge - A_LeftEdge;
						float topBottomOverlap = B_TopEdge - A_BottomEdge;

						// Circle is approaching from the right (keep y pos, calculate x pos)
						if (leftRightOverlap < topBottomOverlap)
						{
							circleCol->_isCollidingLeft = true;
							circleCol->_isCollidingBottomLeft = true;
							circleCol->_bottomLeftCollisionSolid = boxCol->IsSolid();
							circleCol->_leftCollisionStatic = boxCol->IsStatic();
							circleCol->_bottomLeftCollisionStatic = boxCol->IsStatic(); // New
							circleCol->_leftCollisionSolid = boxCol->IsSolid();
							boxCol->_isCollidingRight = true;
							boxCol->_isCollidingTopRight = true;
							boxCol->_rightCollisionStatic = circleCol->IsStatic();
							boxCol->_rightCollisionSolid = circleCol->IsSolid();
							circleCol->leftCollision = B_RightEdge;
							boxCol->rightCollision = A_LeftEdge;

							yFromCol = circleCenterGrid.y - B_TopEdge;

							if (-(yFromCol * yFromCol) + (circleActiveRadius * circleActiveRadius) > 0)
								xFromCol = std::sqrt(-(yFromCol * yFromCol) + (circleActiveRadius * circleActiveRadius));
							else
								xFromCol = 0;

							//FlatGui::PushWindowStyles();
							//ImGui::Begin("Scene View", 0, 16 | 8);
							//FlatGui::PopWindowStyles();
							//DrawLine(Vector2(circleCol->GetCenterCoord().x, circleCol->GetCenterCoord().y + yFromCol * FlatGui::sceneViewGridStep.y), Vector2(circleCol->GetCenterCoord().x + (xFromCol - (circlePos.x - B_RightEdge)) * FlatGui::sceneViewGridStep.x, circleCol->GetCenterCoord().y + yFromCol * FlatGui::sceneViewGridStep.y), Vector4(0, 0, 0, 1), 1, ImGui::GetWindowDrawList());
							//DrawLine(circleCol->GetCenterCoord(), Vector2(circleCol->GetCenterCoord().x, circleCol->GetCenterCoord().y + yFromCol * FlatGui::sceneViewGridStep.y), Vector4(0, 0, 0, 1), 1, ImGui::GetWindowDrawList()); // Vertical
							//ImGui::End();

							//boxCol->topRightCollidedPosition = Vector2(boxPos.x, A_BottomEdge - boxHalfHeight - 0.001f);
							/*circleCol->bottomLeftCollidedPosition = Vector2(circlePos.x, B_TopEdge + yFromCol - 0.001f);*/
							circleCol->bottomLeftCollidedPosition = Vector2(circleCenterGrid.x + (xFromCol - (circleCenterGrid.x - B_RightEdge)), circleCenterGrid.y);
						}
						// Circle is approaching from the top (keep x value, calculate y value)
						else {
							circleCol->_isCollidingBottom = true;
							circleCol->_isCollidingBottomLeft = true;
							circleCol->_bottomCollisionStatic = boxCol->IsStatic();
							circleCol->_bottomLeftCollisionStatic = boxCol->IsStatic();
							circleCol->_bottomLeftCollisionSolid = boxCol->IsSolid();
							circleCol->_bottomCollisionSolid = boxCol->IsSolid();
							boxCol->_isCollidingTop = true;
							boxCol->_topCollisionStatic = circleCol->IsStatic();
							boxCol->_topCollisionSolid = circleCol->IsSolid();
							circleCol->bottomCollision = B_TopEdge;
							boxCol->topCollision = A_BottomEdge;

							yFromCol = circleCenterGrid.y - B_TopEdge;

							if (-(yFromCol * yFromCol) + (circleActiveRadius * circleActiveRadius) > 0)
								xFromCol = std::sqrt(-(yFromCol * yFromCol) + (circleActiveRadius * circleActiveRadius));
							else
								xFromCol = 0;

							/*FlatGui::PushWindowStyles();
							ImGui::Begin("Scene View", 0, 16 | 8);
							FlatGui::PopWindowStyles();
							DrawLine(Vector2(circleCol->GetCenterCoord().x, circleCol->GetCenterCoord().y + yFromCol * FlatGui::sceneViewGridStep.y), Vector2(circleCol->GetCenterCoord().x + (xFromCol - (circlePos.x - B_RightEdge)) * FlatGui::sceneViewGridStep.x, circleCol->GetCenterCoord().y + yFromCol * FlatGui::sceneViewGridStep.y), Vector4(0, 0, 0, 1), 1, ImGui::GetWindowDrawList());
							DrawLine(circleCol->GetCenterCoord(), Vector2(circleCol->GetCenterCoord().x, circleCol->GetCenterCoord().y + yFromCol * FlatGui::sceneViewGridStep.y), Vector4(0, 0, 0, 1), 1, ImGui::GetWindowDrawList());*/ // Vertical
							//ImGui::End();

							//boxCol->topRightCollidedPosition = Vector2(boxPos.x, A_BottomEdge - boxHalfHeight - 0.001f);
							/*circleCol->bottomLeftCollidedPosition = Vector2(circlePos.x, B_TopEdge + yFromCol - 0.001f);*/
							circleCol->bottomLeftCollidedPosition = Vector2(circleCenterGrid.x + (xFromCol - (circleCenterGrid.x - B_RightEdge)), circleCenterGrid.y);
						}
					}
				}
			}
		}
		//LogFloat(xFromCol, "Final X From Coll: ");
		// if circleCol is to the left of boxCol
		else if (circleCenterGrid.x < boxCenterGrid.x)
		{
			// if circleCol is below boxCol
			if (circleCenterGrid.y < boxCenterGrid.y)
			{
				float rise = std::abs(B_BottomEdge - circleCenterGrid.y);
				float run = std::abs(B_LeftEdge - circleCenterGrid.x);
				float cornerDistance = std::sqrt((rise * rise) + (run * run));

				// If colliding
				if (cornerDistance < circleActiveRadius)
				{
					b_colliding = true;
					if (circleCol->IsSolid() && boxCol->IsSolid())
					{
						float leftRightOverlap = A_RightEdge - B_LeftEdge;
						float topBottomOverlap = A_TopEdge - B_BottomEdge;

						// Circle is approaching from the left (keep y value, calculate x value)
						if (leftRightOverlap < topBottomOverlap)
						{
							circleCol->_isCollidingRight = true;
							circleCol->_isCollidingTopRight = true;
							circleCol->_topRightCollisionSolid = boxCol->IsSolid(); // new
							circleCol->_rightCollisionStatic = boxCol->IsStatic();
							circleCol->_rightCollisionSolid = boxCol->IsSolid();
							boxCol->_isCollidingLeft = true;
							boxCol->_leftCollisionStatic = circleCol->IsStatic();
							boxCol->_leftCollisionSolid = circleCol->IsSolid();
							circleCol->rightCollision = B_LeftEdge;
							boxCol->leftCollision = A_RightEdge;

							yFromCol = circleCenterGrid.y - B_BottomEdge;

							if (-(yFromCol * yFromCol) + (circleActiveRadius * circleActiveRadius) > 0)
								xFromCol = std::sqrt(-(yFromCol * yFromCol) + (circleActiveRadius * circleActiveRadius));
							else
								xFromCol = 0;

							circleCol->topRightCollidedPosition = Vector2(B_LeftEdge - xFromCol - 0.001f, circlePos.y);
						}
						// Circle is approaching from the bottom (keep x value, calculate y value)
						else {
							circleCol->_isCollidingTop = true;
							circleCol->_isCollidingTopRight = true;
							circleCol->_topRightCollisionSolid = boxCol->IsSolid();
							circleCol->_topCollisionStatic = boxCol->IsStatic();
							circleCol->_topCollisionSolid = boxCol->IsSolid();
							boxCol->_isCollidingBottom = true;
							boxCol->_bottomCollisionStatic = circleCol->IsStatic();
							boxCol->_bottomCollisionSolid = circleCol->IsSolid();
							circleCol->topCollision = B_BottomEdge;
							boxCol->bottomCollision = A_TopEdge;

							xFromCol = B_LeftEdge - circleCenterGrid.x;

							if (-(xFromCol * xFromCol) + (circleActiveRadius * circleActiveRadius) > 0)
								yFromCol = std::sqrt(-(xFromCol * xFromCol) + (circleActiveRadius * circleActiveRadius));
							else
								yFromCol = 0;

							circleCol->topRightCollidedPosition = Vector2(circlePos.x, B_BottomEdge - yFromCol - 0.001f);
						}
					}
				}
			}
			// if circleCol is above boxCol
			else if (circleCenterGrid.y > boxCenterGrid.y)
			{
				float rise = std::abs(B_TopEdge - circleCenterGrid.y);
				float run = std::abs(B_LeftEdge - circleCenterGrid.x);
				float cornerDistance = std::sqrt((rise * rise) + (run * run));

				// If colliding
				if (cornerDistance < circleActiveRadius)
				{
					b_colliding = true;
					if (circleCol->IsSolid() && boxCol->IsSolid())
					{
						float leftRightOverlap = A_RightEdge - B_LeftEdge;
						float topBottomOverlap = B_TopEdge - A_BottomEdge;

						// Circle is approaching from the left (keep y value, calculate x value)
						if (leftRightOverlap < topBottomOverlap)
						{
							circleCol->_isCollidingRight = true;
							circleCol->_isCollidingBottomRight = true;
							circleCol->_bottomRightCollisionSolid = boxCol->IsSolid(); // new
							circleCol->_rightCollisionStatic = boxCol->IsStatic();
							circleCol->_rightCollisionSolid = boxCol->IsSolid();
							boxCol->_isCollidingLeft = true;
							boxCol->_isCollidingTopLeft = true;
							boxCol->_leftCollisionStatic = circleCol->IsStatic();
							boxCol->_leftCollisionSolid = circleCol->IsSolid();
							circleCol->rightCollision = B_LeftEdge;
							boxCol->leftCollision = A_RightEdge;

							yFromCol = circleCenterGrid.y - B_TopEdge;

							if (-(yFromCol * yFromCol) + (circleActiveRadius * circleActiveRadius) > 0)
								xFromCol = std::sqrt(-(yFromCol * yFromCol) + (circleActiveRadius * circleActiveRadius));
							else
								xFromCol = 0;

							circleCol->bottomRightCollidedPosition = Vector2(B_LeftEdge - xFromCol - 0.001f, circlePos.y);
						}
						// Circle is approaching from the top (keep x value, calculate y value)
						else {
							circleCol->_isCollidingBottom = true;
							circleCol->_isCollidingBottomRight = true;
							circleCol->_bottomRightCollisionSolid = boxCol->IsSolid();
							circleCol->_bottomCollisionStatic = boxCol->IsStatic();
							circleCol->_bottomCollisionSolid = boxCol->IsSolid();
							boxCol->_isCollidingTop = true;
							boxCol->_isCollidingTopLeft = true;
							boxCol->_topCollisionStatic = circleCol->IsStatic();
							boxCol->_topCollisionSolid = circleCol->IsSolid();
							circleCol->bottomCollision = B_TopEdge;
							boxCol->topCollision = A_BottomEdge;

							xFromCol = B_LeftEdge - circleCenterGrid.x;

							if (-(xFromCol * xFromCol) + (circleActiveRadius * circleActiveRadius) > 0)
								yFromCol = std::sqrt(-(xFromCol * xFromCol) + (circleActiveRadius * circleActiveRadius));
							else
								yFromCol = 0;

							circleCol->bottomRightCollidedPosition = Vector2(circlePos.x, B_TopEdge + yFromCol - 0.001f);
						}
					}
				}
			}
		}

		return b_colliding;
	}

	bool Collider::CheckForCollisionBoxBox(BoxCollider* boxCol1, BoxCollider* boxCol2)
	{
		bool b_colliding = false;
		Transform* box1Transform = boxCol1->GetParent()->GetTransform();
		Transform* box2Transform = boxCol2->GetParent()->GetTransform();
		Vector2 box1Scale = box1Transform->GetScale();
		Vector2 box2Scale = box2Transform->GetScale();
		Vector2 collider1CenterGrid = boxCol1->GetCenterGrid();
		Vector2 collider2CenterGrid = boxCol2->GetCenterGrid();
		Vector2 col1Pos = boxCol1->GetParent()->GetTransform()->GetPosition();
		Vector2 col2Pos = boxCol2->GetParent()->GetTransform()->GetPosition();
		Vector2 col1Offset = boxCol1->GetActiveOffset();
		Vector2 col2Offset = boxCol2->GetActiveOffset();
		float box1HalfHeight = boxCol1->GetActiveHeight() / 2 * box1Scale.y;
		float box1HalfWidth = boxCol1->GetActiveWidth() / 2 * box1Scale.x;
		float box2HalfHeight = boxCol2->GetActiveHeight() / 2 * box2Scale.y;
		float box2HalfWidth = boxCol2->GetActiveWidth() / 2 * box2Scale.x;

		float A_TopEdge = boxCol1->nextActiveTop;
		float A_RightEdge = boxCol1->nextActiveRight;
		float A_BottomEdge = boxCol1->nextActiveBottom;
		float A_LeftEdge = boxCol1->nextActiveLeft;

		float B_TopEdge = boxCol2->nextActiveTop;
		float B_RightEdge = boxCol2->nextActiveRight;
		float B_BottomEdge = boxCol2->nextActiveBottom;
		float B_LeftEdge = boxCol2->nextActiveLeft;

		// Check for collision
		b_colliding = ((A_LeftEdge < B_RightEdge) && (A_RightEdge > B_LeftEdge) && (A_BottomEdge < B_TopEdge) && (A_TopEdge > B_BottomEdge));

		// Get collision details
		if (b_colliding)
		{
			// Check which direction the collision is happening from //
			// 
			// if boxCol1 is to the right of boxCol2
			if (collider1CenterGrid.x > collider2CenterGrid.x)
			{
				// if boxCol1 is below boxCol2
				if (collider1CenterGrid.y < collider2CenterGrid.y)
				{
					float leftRightOverlap = B_RightEdge - A_LeftEdge;
					float topBottomOverlap = A_TopEdge - B_BottomEdge;
					// Left/Right
					if (leftRightOverlap < topBottomOverlap)
					{
						if (boxCol1->IsSolid() && boxCol2->IsSolid())
						{
							boxCol1->_isCollidingLeft = true;
							boxCol1->_leftCollisionStatic = boxCol2->IsStatic();
							boxCol1->_leftCollisionSolid = boxCol2->IsSolid();
							boxCol2->_isCollidingRight = true;
							boxCol2->_rightCollisionStatic = boxCol1->IsStatic();
							boxCol2->_rightCollisionSolid = boxCol2->IsSolid();
							boxCol1->leftCollision = B_RightEdge;
							boxCol2->rightCollision = A_LeftEdge;

							// Calculate at what Transform positions the collision technically happened and store it for repositioning in RigidBody
							boxCol1->leftCollidedPosition = Vector2(B_RightEdge + box1HalfWidth - col1Offset.x - 0.001f, col1Pos.y);
							boxCol2->rightCollidedPosition = Vector2(A_LeftEdge - box2HalfWidth - col2Offset.x + 0.001f, col2Pos.y);
						}
					}
					// Top/Bottom
					else {
						if (boxCol1->IsSolid() && boxCol2->IsSolid())
						{
							boxCol1->_isCollidingTop = true;
							boxCol1->_topCollisionStatic = boxCol2->IsStatic();
							boxCol1->_topCollisionSolid = boxCol2->IsSolid();
							boxCol2->_isCollidingBottom = true;
							boxCol2->_bottomCollisionStatic = boxCol1->IsStatic();
							boxCol2->_bottomCollisionSolid = boxCol1->IsSolid();
							boxCol1->topCollision = B_BottomEdge;
							boxCol2->bottomCollision = A_TopEdge;

							// Calculate at what Transform positions the collision technically happened and store it for repositioning in RigidBody
							boxCol1->topCollidedPosition = Vector2(col1Pos.x, B_BottomEdge - box1HalfHeight - col1Offset.y + 0.001f);
							boxCol2->bottomCollidedPosition = Vector2(col2Pos.x, A_TopEdge + box2HalfHeight - col2Offset.y - 0.001f);
						}
					}
				}
				// if boxCol1 is above boxCol2
				else if (collider1CenterGrid.y > collider2CenterGrid.y)
				{
					float leftRightOverlap = B_RightEdge - A_LeftEdge;
					float topBottomOverlap = B_TopEdge - A_BottomEdge;
					// Left/Right
					if (leftRightOverlap < topBottomOverlap)
					{
						if (boxCol1->IsSolid() && boxCol2->IsSolid())
						{
							boxCol1->_isCollidingLeft = true;
							boxCol1->_leftCollisionStatic = boxCol2->IsStatic();
							boxCol1->_leftCollisionSolid = boxCol2->IsSolid();
							boxCol2->_isCollidingRight = true;
							boxCol2->_rightCollisionStatic = boxCol1->IsStatic();
							boxCol2->_rightCollisionSolid = boxCol1->IsSolid();
							boxCol1->leftCollision = B_RightEdge;
							boxCol2->rightCollision = A_LeftEdge;

							// Calculate at what Transform positions the collision technically happened and store it for repositioning in RigidBody
							boxCol1->leftCollidedPosition = Vector2(B_RightEdge + box1HalfWidth - col1Offset.x - 0.001f, col1Pos.y);
							boxCol2->rightCollidedPosition = Vector2(A_LeftEdge - box2HalfWidth - col2Offset.x + 0.001f, col2Pos.y);
						}
					}
					// Top/Bottom
					else {
						if (boxCol1->IsSolid() && boxCol2->IsSolid())
						{
							boxCol1->_isCollidingBottom = true;
							boxCol1->_bottomCollisionStatic = boxCol2->IsStatic();
							boxCol1->_bottomCollisionSolid = boxCol2->IsSolid();
							boxCol2->_isCollidingTop = true;
							boxCol2->_topCollisionStatic = boxCol1->IsStatic();
							boxCol2->_topCollisionSolid = boxCol1->IsSolid();
							boxCol1->bottomCollision = B_TopEdge;
							boxCol2->topCollision = A_BottomEdge;

							// Calculate at what Transform positions the collision technically happened and store it for repositioning in RigidBody
							boxCol1->bottomCollidedPosition = Vector2(col2Pos.x, B_TopEdge + box1HalfHeight - col1Offset.y - 0.001f);
							boxCol2->topCollidedPosition = Vector2(col1Pos.x, A_BottomEdge - box2HalfHeight - col2Offset.y + 0.001f);
						}
					}
				}
				// if both are at the same y Pos the it's a left/right collision
				else
				{
					if (boxCol1->IsSolid() && boxCol2->IsSolid())
					{
						if (boxCol1->IsSolid() && boxCol2->IsSolid())
						{
							boxCol1->_isCollidingLeft = true;
							boxCol1->_leftCollisionStatic = boxCol2->IsStatic();
							boxCol1->_leftCollisionSolid = boxCol2->IsSolid();
							boxCol2->_isCollidingRight = true;
							boxCol2->_rightCollisionStatic = boxCol1->IsStatic();
							boxCol2->_rightCollisionSolid = boxCol1->IsSolid();
							boxCol1->leftCollision = B_RightEdge;
							boxCol2->rightCollision = A_LeftEdge;

							// Calculate at what Transform positions the collision technically happened and store it for repositioning in RigidBody
							boxCol1->leftCollidedPosition = Vector2(B_RightEdge + box1HalfWidth + col1Offset.x - 0.001f, col1Pos.y);
							boxCol2->rightCollidedPosition = Vector2(A_LeftEdge - box2HalfWidth - col2Offset.x + 0.001f, col2Pos.y);
						}
					}
				}
			}
			// if boxCol1 is to the left of boxCol2
			else if (collider1CenterGrid.x < collider2CenterGrid.x)
			{
				// if boxCol1 is below boxCol2
				if (collider1CenterGrid.y < collider2CenterGrid.y)
				{
					float leftRightOverlap = A_RightEdge - B_LeftEdge;
					float topBottomOverlap = A_TopEdge - B_BottomEdge;
					// Left/Right
					if (leftRightOverlap < topBottomOverlap)
					{
						if (boxCol1->IsSolid() && boxCol2->IsSolid())
						{
							boxCol1->_isCollidingRight = true;
							boxCol1->_rightCollisionStatic = boxCol2->IsStatic();
							boxCol1->_rightCollisionSolid = boxCol2->IsSolid();
							boxCol2->_isCollidingLeft = true;
							boxCol2->_leftCollisionStatic = boxCol1->IsStatic();
							boxCol2->_leftCollisionSolid = boxCol1->IsSolid();
							boxCol1->rightCollision = B_LeftEdge;
							boxCol2->leftCollision = A_RightEdge;

							// Calculate at what Transform positions the collision technically happened and store it for repositioning in RigidBody
							boxCol1->rightCollidedPosition = Vector2(B_LeftEdge - box1HalfWidth - col1Offset.x + 0.001f, col1Pos.y);
							boxCol2->leftCollidedPosition = Vector2(A_RightEdge + box2HalfWidth - col2Offset.x - 0.001f, col2Pos.y);
						}
					}
					// Top/Bottom
					else {
						if (boxCol1->IsSolid() && boxCol2->IsSolid())
						{
							boxCol1->_isCollidingTop = true;
							boxCol1->_topCollisionStatic = boxCol2->IsStatic();
							boxCol1->_topCollisionSolid = boxCol2->IsSolid();
							boxCol2->_isCollidingBottom = true;
							boxCol2->_bottomCollisionStatic = boxCol1->IsStatic();
							boxCol2->_bottomCollisionSolid = boxCol1->IsSolid();
							boxCol1->topCollision = B_BottomEdge;
							boxCol2->bottomCollision = A_TopEdge;

							// Calculate at what Transform positions the collision technically happened and store it for repositioning in RigidBody
							boxCol1->topCollidedPosition = Vector2(col1Pos.x, B_BottomEdge - box1HalfHeight - col1Offset.y + 0.001f);
							boxCol2->bottomCollidedPosition = Vector2(col2Pos.x, A_TopEdge + box2HalfHeight - col2Offset.y - 0.001f);
						}
					}
				}
				else if (collider1CenterGrid.y > collider2CenterGrid.y)
				{
					float leftRightOverlap = A_RightEdge - B_LeftEdge;
					float topBottomOverlap = B_TopEdge - A_BottomEdge;
					// Left/Right
					if (leftRightOverlap < topBottomOverlap)
					{
						if (boxCol1->IsSolid() && boxCol2->IsSolid())
						{
							boxCol1->_isCollidingRight = true;
							boxCol1->_rightCollisionStatic = boxCol2->IsStatic();
							boxCol1->_rightCollisionSolid = boxCol2->IsSolid();
							boxCol2->_isCollidingLeft = true;
							boxCol2->_leftCollisionStatic = boxCol1->IsStatic();
							boxCol2->_leftCollisionSolid = boxCol1->IsSolid();
							boxCol1->rightCollision = B_LeftEdge;
							boxCol2->leftCollision = A_RightEdge;

							// Calculate at what Transform positions the collision technically happened and store it for repositioning in RigidBody
							boxCol1->rightCollidedPosition = Vector2(B_LeftEdge - box1HalfWidth - col1Offset.x + 0.001f, col1Pos.y);
							boxCol2->leftCollidedPosition = Vector2(A_RightEdge + box2HalfWidth - col2Offset.x - 0.001f, col2Pos.y);
						}
					}
					// Top/Bottom
					else {
						if (boxCol1->IsSolid() && boxCol2->IsSolid())
						{
							boxCol1->_isCollidingBottom = true;
							boxCol1->_bottomCollisionStatic = boxCol2->IsStatic();
							boxCol1->_bottomCollisionSolid = boxCol2->IsSolid();
							boxCol2->_isCollidingTop = true;
							boxCol2->_topCollisionStatic = boxCol1->IsStatic();
							boxCol2->_topCollisionSolid = boxCol1->IsSolid();
							boxCol1->bottomCollision = B_TopEdge;
							boxCol2->topCollision = A_BottomEdge;

							// Calculate at what Transform positions the collision technically happened and store it for repositioning in RigidBody
							boxCol1->bottomCollidedPosition = Vector2(col2Pos.x, B_TopEdge + box1HalfHeight - col1Offset.y - 0.001f);
							boxCol2->topCollidedPosition = Vector2(col1Pos.x, A_BottomEdge - box2HalfHeight - col2Offset.y + 0.001f);
						}
					}
				}
				// if both are at the same y Pos the it's a left/right collision
				else
				{
					if (boxCol1->IsSolid() && boxCol2->IsSolid())
					{
						boxCol1->_isCollidingRight = true;
						boxCol1->_rightCollisionStatic = boxCol2->IsStatic();
						boxCol1->_rightCollisionSolid = boxCol2->IsSolid();
						boxCol2->_isCollidingLeft = true;
						boxCol2->_leftCollisionStatic = boxCol1->IsStatic();
						boxCol2->_leftCollisionSolid = boxCol1->IsSolid();
						boxCol1->rightCollision = B_LeftEdge;
						boxCol2->leftCollision = A_RightEdge;

						// Calculate at what Transform positions the collision technically happened and store it for repositioning in RigidBody
						boxCol1->rightCollidedPosition = Vector2(B_LeftEdge - box1HalfWidth - col1Offset.x + 0.001f, col1Pos.y);
						boxCol2->leftCollidedPosition = Vector2(A_RightEdge + box2HalfWidth - col2Offset.x - 0.001f, col2Pos.y);
					}
				}
			}
			// Both boxes are at the same x Pos
			else
			{
				// if boxCol1 is below boxCol2
				if (collider1CenterGrid.y < collider2CenterGrid.y)
				{
					if (boxCol1->IsSolid() && boxCol2->IsSolid())
					{
						boxCol1->_isCollidingTop = true;
						boxCol1->_topCollisionStatic = boxCol2->IsStatic();
						boxCol1->_topCollisionSolid = boxCol2->IsSolid();
						boxCol2->_isCollidingBottom = true;
						boxCol2->_bottomCollisionStatic = boxCol1->IsStatic();
						boxCol2->_bottomCollisionSolid = boxCol1->IsSolid();
						boxCol1->topCollision = B_BottomEdge;
						boxCol2->bottomCollision = A_TopEdge;

						// Calculate at what Transform positions the collision technically happened and store it for repositioning in RigidBody
						boxCol1->topCollidedPosition = Vector2(col1Pos.x, B_BottomEdge - box1HalfHeight - col1Offset.y + 0.001f);
						boxCol2->bottomCollidedPosition = Vector2(col2Pos.x, A_TopEdge + box2HalfHeight - col2Offset.y - 0.001f);
					}
				}
				// if boxCol1 is above boxCol2
				else
				{
					if (boxCol1->IsSolid() && boxCol2->IsSolid())
					{
						boxCol1->_isCollidingBottom = true;
						boxCol1->_bottomCollisionStatic = boxCol2->IsStatic();
						boxCol1->_bottomCollisionSolid = boxCol2->IsSolid();
						boxCol2->_isCollidingTop = true;
						boxCol2->_topCollisionStatic = boxCol1->IsStatic();
						boxCol2->_topCollisionSolid = boxCol1->IsSolid();
						boxCol1->bottomCollision = B_TopEdge;
						boxCol2->topCollision = A_BottomEdge;

						// Calculate at what Transform positions the collision technically happened and store it for repositioning in RigidBody
						boxCol1->bottomCollidedPosition = Vector2(col2Pos.x, B_TopEdge + box1HalfHeight - col1Offset.y - 0.001f);
						boxCol2->topCollidedPosition = Vector2(col1Pos.x, A_BottomEdge - box2HalfHeight - col2Offset.y + 0.001f);
					}
				}
			}
		}

		return b_colliding;
	}

	bool Collider::IsColliding()
	{
		return _isColliding;
	}

	void Collider::SetColliding(bool b_colliding)
	{
		_isColliding = b_colliding;
	}

	void Collider::UpdatePreviousPosition()
	{
		Transform* transform = GetParent()->GetTransform();
		previousPosition = transform->GetTruePosition();
	}

	Vector2 Collider::GetPreviousPosition()
	{
		return previousPosition;
	}

	void Collider::SetPreviousPosition(Vector2 prevPos)
	{
		previousPosition = prevPos;
	}

	bool Collider::HasMoved()
	{
		Transform* transform = GetParent()->GetTransform();
		Vector2 position = transform->GetPosition();

		return (previousPosition.x != position.x || previousPosition.y != position.y);
	}

	// Don't use this for anything I don't think, safe to remove
	void Collider::RemoveCollidingObject(GameObject object)
	{
		for (std::vector<GameObject*>::iterator iterator = m_collidingObjects.begin(); iterator != m_collidingObjects.end();)
		{
			if ((*iterator)->GetID() == object.GetID())
				m_collidingObjects.erase(iterator);

			iterator++;
		}
	}

	void Collider::AddCollidingObject(Collider* collidedWith)
	{
		// Make sure we haven't already tracked it for this frame
		for (GameObject* object : m_collidingObjects)
		{
			// Leave function if the object is already known to be in active collision
			if (object->GetID() == collidedWith->GetParent()->GetID())
				return;
		}
		// else add the collided object
		m_collidingObjects.push_back(collidedWith->GetParent());

		// See if they were colliding in the last frame as well
		for (GameObject *object : collidingLastFrame)
		{
			// Leave function if the object has already fired OnCollisionEnter();
			if (object->GetID() == collidedWith->GetParent()->GetID())
				return;
		}

		// else, if OnCollisionEnter is set, fire it now. (upon initially adding the object to m_collidingObjects for the first time)
		CallLuaOnCollisionEnter(GetParent(), collidedWith);
		if (OnCollisionEnterSet())
			OnCollisionEnter(GetParent(), collidedWith->GetParent());
	}

	std::vector<GameObject*> Collider::GetCollidingObjects()
	{
		return m_collidingObjects;
	}

	void Collider::ClearCollidingObjects()
	{
		// Check which objects have left collision state since last frame
		for (GameObject *collidedLastFrame : collidingLastFrame)
		{		
			bool _objectStillColliding = false;

			for (GameObject *collidedThisFrame : m_collidingObjects)
				if (collidedLastFrame->GetID() == collidedThisFrame->GetID())
					_objectStillColliding = true;

			// Fire OnLeave if not colliding
			if (!_objectStillColliding)
				for (BoxCollider* boxCollider : collidedLastFrame->GetBoxColliders())
				{
					CallLuaOnCollisionLeave(GetParent(), boxCollider);
					if (boxCollider->OnCollisionLeaveSet())
						OnCollisionLeave(GetParent(), boxCollider->GetParent());
				}
		}

		// Save colliding objects for next frame
		collidingLastFrame = m_collidingObjects;
		m_collidingObjects.clear();
	}

	void Collider::SetActiveOffset(Vector2 offset)
	{
		activeOffset = offset;
	}

	void Collider::SetActiveLayer(int layer)
	{
		if (layer >= 0)
			activeLayer = layer;
		else
			LogString("Collider active layer must be an integer greater than 0.");
	}

	int Collider::GetActiveLayer()
	{
		return activeLayer;
	}

	void Collider::SetActiveRadiusScreen(float radius)
	{
		activeRadiusScreen = radius;
	}

	Vector2 Collider::GetActiveOffset()
	{
		return activeOffset;
	}

	float Collider::GetActiveRadiusScreen()
	{
		return activeRadiusScreen;
	}

	void Collider::SetActiveRadiusGrid(float radius)
	{
		activeRadiusGrid = radius;
	}

	float Collider::GetActiveRadiusGrid()
	{
		return activeRadiusGrid;
	}

	void  Collider::SetShowActiveRadius(bool _show)
	{
		_showActiveRadius = _show;
	}

	bool  Collider::GetShowActiveRadius()
	{
		return _showActiveRadius;
	}

	bool Collider::OnActiveCollisionSet()
	{
		return _onActiveCollidingSet;
	}

	bool Collider::OnCollisionEnterSet()
	{
		return _onCollisionEnterSet;
	}

	bool Collider::OnCollisionLeaveSet()
	{
		return _onCollisionLeaveSet;
	}

	void Collider::SetOnActiveCollisionSet(bool _set)
	{
		_onActiveCollidingSet = _set;
	}

	void Collider::SetOnCollisionEnterSet(bool _set)
	{
		_onCollisionEnterSet = _set;
	}

	void Collider::SetOnCollisionLeaveSet(bool _set)
	{
		_onCollisionLeaveSet = _set;
	}

	void Collider::ResetCollisions()
	{
		ClearCollidingObjects();
		SetColliding(false);

		_isCollidingRight = false;
		_isCollidingLeft = false;
		_isCollidingBottom = false;
		_isCollidingTop = false;

		_isCollidingTopRight = false;
		_isCollidingTopLeft = false;
		_isCollidingBottomRight = false;
		_isCollidingBottomLeft = false;

		_rightCollisionStatic = false;
		_leftCollisionStatic = false;
		_bottomCollisionStatic = false;
		_topCollisionStatic = false;
		_bottomLeftCollisionStatic = false;
		_bottomRightCollisionStatic = false;

		_rightCollisionSolid = false;
		_leftCollisionSolid = false;
		_bottomCollisionSolid = false;
		_topCollisionSolid = false;
		_bottomLeftCollisionSolid = false;
		_bottomRightCollisionSolid = false;
		_topLeftCollisionSolid = false;
		_topRightCollisionSolid = false;

		rightCollision = 0;
		leftCollision = 0;
		bottomCollision = 0;
		topCollision = 0;

		leftCollidedPosition = Vector2(0, 0);
		rightCollidedPosition = Vector2(0, 0);
		bottomCollidedPosition = Vector2(0, 0);
		topCollidedPosition = Vector2(0, 0);
		topRightCollidedPosition = Vector2(0, 0);
		bottomRightCollidedPosition = Vector2(0, 0);
		topLeftCollidedPosition = Vector2(0, 0);
		bottomLeftCollidedPosition = Vector2(0, 0);
	}

	void Collider::UpdateCenter()
	{
	}

	void Collider::SetCenterGrid(Vector2 newCenter)
	{
		centerGrid = newCenter;
	}

	Vector2 Collider::GetCenterGrid()
	{
		return centerGrid;
	}

	void Collider::SetCenterCoord(Vector2 newCenter)
	{
		centerCoord = newCenter;
	}

	Vector2 Collider::GetCenterCoord()
	{
		return centerCoord;
	}

	void Collider::SetNextCenterGrid(Vector2 nextCenter)
	{
		nextCenterGrid = nextCenter;
	}

	Vector2 Collider::GetNextCenterGrid()
	{
		return nextCenterGrid;
	}

	void Collider::SetNextCenterCoord(Vector2 nextCenter)
	{
		nextCenterCoord = nextCenter;
	}

	Vector2 Collider::GetNextCenterCoord()
	{
		return nextCenterCoord;
	}

	void Collider::SetIsContinuous(bool _continuous)
	{
		_isContinuous = _continuous;
	}

	bool Collider::IsContinuous()
	{
		return _isContinuous;
	}

	void Collider::SetIsStatic(bool _newStatic)
	{
		_isStatic = _newStatic;
	}

	bool Collider::IsStatic()
	{
		return _isStatic;
	}

	void Collider::SetIsSolid(bool _solid)
	{
		_isSolid = _solid;
	}

	bool Collider::IsSolid()
	{
		return _isSolid;
	}

	void Collider::SetRotation(float newRotation)
	{
		rotation = newRotation;
	}

	void Collider::UpdateRotation()
	{
		rotation = GetParent()->GetTransform()->GetRotation();
	}

	float Collider::GetRotation()
	{
		return rotation;
	}

	void Collider::SetIsComposite(bool _newComposite)
	{
		_isComposite = _newComposite;
	}

	bool Collider::IsComposite()
	{
		return _isComposite;
	}

	void Collider::RecalculateBounds()
	{
	}
}
