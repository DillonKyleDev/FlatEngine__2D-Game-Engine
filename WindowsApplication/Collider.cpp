#include "Collider.h"
#include "BoxCollider.h"
#include "CircleCollider.h"
#include "RigidBody.h"
#include "FlatEngine.h"
#include "GameObject.h"
#include "Transform.h"
#include "imgui_internal.h"
#include <cmath>


namespace FlatEngine
{
	Collider::Collider(long myID, long parentID)
	{		
		SetID(myID);
		SetParentID(parentID);
		
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

	Collider::Collider(std::shared_ptr<Collider> toCopy, long newParentID)
	{		
		SetID(GetNextComponentID());
		SetParentID(newParentID);
		SetActive(toCopy->IsActive());

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

	void Collider::SetOnActiveCollision(std::function<void(std::shared_ptr<GameObject>, std::shared_ptr<GameObject>)> callback)
	{
		OnActiveCollision = callback;
		_onActiveCollidingSet = true;
	}

	void Collider::SetOnCollisionEnter(std::function<void(std::shared_ptr<GameObject>, std::shared_ptr<GameObject>)> callback)
	{
		OnCollisionEnter = callback;
		_onCollisionEnterSet = true;
	}

	void Collider::SetOnCollisionLeave(std::function<void(std::shared_ptr<GameObject>, std::shared_ptr<GameObject>)> callback)
	{
		OnCollisionLeave = callback;
		_onCollisionLeaveSet = true;
	}

	bool Collider::CheckForCollision(std::shared_ptr<FlatEngine::Collider> collider1, std::shared_ptr<FlatEngine::Collider> collider2)
	{
		if (loadedProject->GetCollisionDetection() == "Shared Axis")
		{
			return SharedAxisCheckForCollision(collider1, collider2);
		}
		else if (loadedProject->GetCollisionDetection() == "Separating Axis")
		{
			// TODO
			return false;
		}
		return false;
	}

	bool Collider::SharedAxisCheckForCollision(std::shared_ptr<FlatEngine::Collider> collider1, std::shared_ptr<FlatEngine::Collider> collider2)
	{
		bool _colliding = false;
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
			if (!(collider1->GetParent()->HasTag("IgnorePlayer") && collider2->GetParent()->HasTag("Player")) &&
				!(collider1->GetParent()->HasTag("Player") && collider2->GetParent()->HasTag("IgnorePlayer")) && 
				!(collider1->GetParent()->HasTag("IgnoreEnemy") && collider2->GetParent()->HasTag("Enemy")) &&
				!(collider1->GetParent()->HasTag("Enemy") && collider2->GetParent()->HasTag("IgnoreEnemy")) &&
				!(collider1->GetParent()->HasTag("Projectile") && collider2->GetParent()->HasTag("Projectile")))
			{
				// Both BoxColliders
				if (collider1->GetTypeString() == "BoxCollider" && collider2->GetTypeString() == "BoxCollider")
				{
					std::shared_ptr<FlatEngine::BoxCollider> boxCol1 = std::static_pointer_cast<FlatEngine::BoxCollider>(collider1);
					std::shared_ptr<FlatEngine::BoxCollider> boxCol2 = std::static_pointer_cast<FlatEngine::BoxCollider>(collider2);

					_colliding = CheckForCollisionBoxBox(boxCol1, boxCol2);
				}
				// First CircleCollider second BoxCollider
				else if (collider1->GetTypeString() == "CircleCollider" && collider2->GetTypeString() == "BoxCollider")
				{
					std::shared_ptr<FlatEngine::CircleCollider> circleCol = std::static_pointer_cast<FlatEngine::CircleCollider>(collider1);
					std::shared_ptr<FlatEngine::BoxCollider> boxCol = std::static_pointer_cast<FlatEngine::BoxCollider>(collider2);

					_colliding = Collider::CheckForCollisionBoxCircle(boxCol, circleCol);
				}
				// First BoxCollider second CircleCollider
				else if (collider1->GetTypeString() == "BoxCollider" && collider2->GetTypeString() == "CircleCollider")
				{
					std::shared_ptr<FlatEngine::BoxCollider> boxCol = std::static_pointer_cast<FlatEngine::BoxCollider>(collider1);
					std::shared_ptr<FlatEngine::CircleCollider> circleCol = std::static_pointer_cast<FlatEngine::CircleCollider>(collider2);

					_colliding = Collider::CheckForCollisionBoxCircle(boxCol, circleCol);
				}
				// Both CircleColliders ( already true if made if past activeRadius check )
				else if (collider1->GetTypeString() == "CircleCollider" && collider2->GetTypeString() == "CircleCollider")
				{
					std::shared_ptr<FlatEngine::BoxCollider> circleCol1 = std::static_pointer_cast<FlatEngine::BoxCollider>(collider1);
					std::shared_ptr<FlatEngine::BoxCollider> circleCol2 = std::static_pointer_cast<FlatEngine::BoxCollider>(collider2);

					_colliding = true;
				}
			}
		}

		if (_colliding)
		{
			// Set _colliding
			collider1->SetColliding(true);
			collider2->SetColliding(true);

			// Add colliding objects
			collider1->AddCollidingObject(collider2);
			collider2->AddCollidingObject(collider1);

			// For Collider events - Fire OnActiveCollision while there is a collision happening
			if (collider1->OnActiveCollisionSet())
				collider1->OnActiveCollision(collider1->GetParent(), collider2->GetParent());
			if (collider2->OnActiveCollisionSet())
				collider2->OnActiveCollision(collider2->GetParent(), collider1->GetParent());
		}

		return _colliding;
	}

	bool Collider::CheckForCollisionBoxCircle(std::shared_ptr<FlatEngine::BoxCollider> boxCol, std::shared_ptr<FlatEngine::CircleCollider> circleCol)
	{
		bool _colliding = false;
		Vector2 circleCenterGrid = circleCol->GetNextCenterGrid(); // Get next center so no overlap happens this frame
		Vector2 boxCenterGrid = boxCol->GetNextCenterGrid();       // -- ^^^
		Vector2 circlePos = circleCol->GetParent()->GetTransformComponent()->GetTruePosition();
		Vector2 boxPos = boxCol->GetParent()->GetTransformComponent()->GetTruePosition();
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

		double yFromCol;
		double xFromCol;

		// Check how colliders are oriented in relation to each other //

		// check for direct left/right/top/bottom collisions
		// Circle left - Box right
		if (circleCenterGrid.x < boxCenterGrid.x && circleCenterGrid.y < B_TopEdge && circleCenterGrid.y > B_BottomEdge && (B_LeftEdge - circleCenterGrid.x < circleActiveRadius))
		{
			_colliding = true;
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
		// Circle right - Box left
		else if (circleCenterGrid.x > boxCenterGrid.x && circleCenterGrid.y < B_TopEdge && circleCenterGrid.y > B_BottomEdge && (circleCenterGrid.x - B_RightEdge < circleActiveRadius))
		{
			_colliding = true;
			circleCol->_isCollidingLeft = true;
			circleCol->_leftCollisionStatic = boxCol->IsStatic();
			circleCol->_leftCollisionSolid = boxCol->IsSolid();
			boxCol->_isCollidingRight = true;
			boxCol->_rightCollisionStatic = circleCol->IsStatic();
			boxCol->_rightCollisionSolid = boxCol->IsSolid();
			circleCol->leftCollision = B_RightEdge;
			boxCol->rightCollision = A_LeftEdge;

			// Calculate at what Transform positions the collision technically happened and store it for repositioning in RigidBody
			circleCol->leftCollidedPosition = Vector2(B_RightEdge + circleActiveRadius - 0.001f, circlePos.y);
			boxCol->rightCollidedPosition = Vector2(A_LeftEdge - boxHalfWidth + 0.001f, boxPos.y);
		}
		// Circle Top - Box Bottom
		else if (circleCenterGrid.y > boxCenterGrid.y && circleCenterGrid.x < B_RightEdge && circleCenterGrid.x > B_LeftEdge && (circleCenterGrid.y - B_TopEdge < circleActiveRadius))
		{
			_colliding = true;
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
		// Circle Bottom - Box Top
		else if (circleCenterGrid.y < boxCenterGrid.y && circleCenterGrid.x < B_RightEdge && circleCenterGrid.x > B_LeftEdge && (B_BottomEdge - circleCenterGrid.y < circleActiveRadius))
		{
			_colliding = true;
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
					_colliding = true;
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
			// if circleCol is above boxCol
			else if (circleCenterGrid.y > boxCenterGrid.y)
			{
				float rise = std::abs(B_TopEdge - circleCenterGrid.y);
				float run = std::abs(B_RightEdge - circleCenterGrid.x);
				float cornerDistance = std::sqrt((rise * rise) + (run * run));

				// If colliding
				if (cornerDistance < circleActiveRadius)
				{
					_colliding = true;
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
					_colliding = true;
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
			// if circleCol is above boxCol
			else if (circleCenterGrid.y > boxCenterGrid.y)
			{
				float rise = std::abs(B_TopEdge - circleCenterGrid.y);
				float run = std::abs(B_LeftEdge - circleCenterGrid.x);
				float cornerDistance = std::sqrt((rise * rise) + (run * run));

				// If colliding
				if (cornerDistance < circleActiveRadius)
				{
					_colliding = true;
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

		return _colliding;
	}

	bool Collider::CheckForCollisionBoxBox(std::shared_ptr<FlatEngine::BoxCollider> boxCol1, std::shared_ptr<FlatEngine::BoxCollider> boxCol2)
	{
		bool _colliding = false;
		std::shared_ptr<FlatEngine::Transform> box1Transform = boxCol1->GetParent()->GetTransformComponent();
		std::shared_ptr<FlatEngine::Transform> box2Transform = boxCol2->GetParent()->GetTransformComponent();
		Vector2 box1Scale = box1Transform->GetScale();
		Vector2 box2Scale = box2Transform->GetScale();
		Vector2 collider1CenterGrid = boxCol1->GetCenterGrid();
		Vector2 collider2CenterGrid = boxCol2->GetCenterGrid();
		Vector2 col1Pos = boxCol1->GetParent()->GetTransformComponent()->GetTruePosition();
		Vector2 col2Pos = boxCol2->GetParent()->GetTransformComponent()->GetTruePosition();
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
		_colliding = ((A_LeftEdge < B_RightEdge) && (A_RightEdge > B_LeftEdge) && (A_BottomEdge < B_TopEdge) && (A_TopEdge > B_BottomEdge));

		// Get collision details
		if (_colliding)
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
						boxCol1->_isCollidingLeft = true;
						boxCol1->_leftCollisionStatic = boxCol2->IsStatic();
						boxCol1->_leftCollisionSolid = boxCol2->IsSolid();
						boxCol2->_isCollidingRight = true;
						boxCol2->_rightCollisionStatic = boxCol1->IsStatic();
						boxCol2->_rightCollisionSolid = boxCol2->IsSolid();
						boxCol1->leftCollision = B_RightEdge;
						boxCol2->rightCollision = A_LeftEdge;

						// Calculate at what Transform positions the collision technically happened and store it for repositioning in RigidBody
						boxCol1->leftCollidedPosition = Vector2(B_RightEdge + box1HalfWidth - 0.001f, col1Pos.y);
						boxCol2->rightCollidedPosition = Vector2(A_LeftEdge - box2HalfWidth + 0.001f, col2Pos.y);
					}
					// Top/Bottom
					else {
						boxCol1->_isCollidingTop = true;
						boxCol1->_topCollisionStatic = boxCol2->IsStatic();
						boxCol1->_topCollisionSolid = boxCol2->IsSolid();
						boxCol2->_isCollidingBottom = true;
						boxCol2->_bottomCollisionStatic = boxCol1->IsStatic();
						boxCol2->_bottomCollisionSolid = boxCol1->IsSolid();
						boxCol1->topCollision = B_BottomEdge;
						boxCol2->bottomCollision = A_TopEdge;

						// Calculate at what Transform positions the collision technically happened and store it for repositioning in RigidBody
						boxCol1->topCollidedPosition = Vector2(col1Pos.x, B_BottomEdge - box1HalfHeight + 0.001f);
						boxCol2->bottomCollidedPosition = Vector2(col2Pos.x, A_TopEdge + box2HalfHeight - 0.001f);
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
						boxCol1->_isCollidingLeft = true;
						boxCol1->_leftCollisionStatic = boxCol2->IsStatic();
						boxCol1->_leftCollisionSolid = boxCol2->IsSolid();
						boxCol2->_isCollidingRight = true;
						boxCol2->_rightCollisionStatic = boxCol1->IsStatic();
						boxCol2->_rightCollisionSolid = boxCol1->IsSolid();
						boxCol1->leftCollision = B_RightEdge;
						boxCol2->rightCollision = A_LeftEdge;

						// Calculate at what Transform positions the collision technically happened and store it for repositioning in RigidBody
						boxCol1->leftCollidedPosition = Vector2(B_RightEdge + box1HalfWidth - 0.001f, col1Pos.y);
						boxCol2->rightCollidedPosition = Vector2(A_LeftEdge - box2HalfWidth + 0.001f, col2Pos.y);
					}
					// Top/Bottom
					else {
						boxCol1->_isCollidingBottom = true;
						boxCol1->_bottomCollisionStatic = boxCol2->IsStatic();
						boxCol1->_bottomCollisionSolid = boxCol2->IsSolid();
						boxCol2->_isCollidingTop = true;
						boxCol2->_topCollisionStatic = boxCol1->IsStatic();
						boxCol2->_topCollisionSolid = boxCol1->IsSolid();
						boxCol1->bottomCollision = B_TopEdge;
						boxCol2->topCollision = A_BottomEdge;

						// Calculate at what Transform positions the collision technically happened and store it for repositioning in RigidBody
						boxCol1->bottomCollidedPosition = Vector2(col2Pos.x, B_TopEdge + box1HalfHeight - 0.001f);
						boxCol2->topCollidedPosition = Vector2(col1Pos.x, A_BottomEdge - box2HalfHeight + 0.001f);
					}
				}
				// if both are at the same y Pos the it's a left/right collision
				else
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
					boxCol1->leftCollidedPosition = Vector2(B_RightEdge + box1HalfWidth - 0.001f, col1Pos.y);
					boxCol2->rightCollidedPosition = Vector2(A_LeftEdge - box2HalfWidth + 0.001f, col2Pos.y);
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
						boxCol1->_isCollidingRight = true;
						boxCol1->_rightCollisionStatic = boxCol2->IsStatic();
						boxCol1->_rightCollisionSolid = boxCol2->IsSolid();
						boxCol2->_isCollidingLeft = true;
						boxCol2->_leftCollisionStatic = boxCol1->IsStatic();
						boxCol2->_leftCollisionSolid = boxCol1->IsSolid();
						boxCol1->rightCollision = B_LeftEdge;
						boxCol2->leftCollision = A_RightEdge;

						// Calculate at what Transform positions the collision technically happened and store it for repositioning in RigidBody
						boxCol1->rightCollidedPosition = Vector2(B_LeftEdge - box1HalfWidth + 0.001f, col1Pos.y);
						boxCol2->leftCollidedPosition = Vector2(A_RightEdge + box2HalfWidth - 0.001f, col2Pos.y);
					}
					// Top/Bottom
					else {
						boxCol1->_isCollidingTop = true;
						boxCol1->_topCollisionStatic = boxCol2->IsStatic();
						boxCol1->_topCollisionSolid = boxCol2->IsSolid();
						boxCol2->_isCollidingBottom = true;
						boxCol2->_bottomCollisionStatic = boxCol1->IsStatic();
						boxCol2->_bottomCollisionSolid = boxCol1->IsSolid();
						boxCol1->topCollision = B_BottomEdge;
						boxCol2->bottomCollision = A_TopEdge;

						// Calculate at what Transform positions the collision technically happened and store it for repositioning in RigidBody
						boxCol1->topCollidedPosition = Vector2(col1Pos.x, B_BottomEdge - box1HalfHeight + 0.001f);
						boxCol2->bottomCollidedPosition = Vector2(col2Pos.x, A_TopEdge + box2HalfHeight - 0.001f);
					}
				}
				else if (collider1CenterGrid.y > collider2CenterGrid.y)
				{
					float leftRightOverlap = A_RightEdge - B_LeftEdge;
					float topBottomOverlap = B_TopEdge - A_BottomEdge;
					// Left/Right
					if (leftRightOverlap < topBottomOverlap)
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
						boxCol1->rightCollidedPosition = Vector2(B_LeftEdge - box1HalfWidth + 0.001f, col1Pos.y);
						boxCol2->leftCollidedPosition = Vector2(A_RightEdge + box2HalfWidth - 0.001f, col2Pos.y);
					}
					// Top/Bottom
					else {
						boxCol1->_isCollidingBottom = true;
						boxCol1->_bottomCollisionStatic = boxCol2->IsStatic();
						boxCol1->_bottomCollisionSolid = boxCol2->IsSolid();
						boxCol2->_isCollidingTop = true;
						boxCol2->_topCollisionStatic = boxCol1->IsStatic();
						boxCol2->_topCollisionSolid = boxCol1->IsSolid();
						boxCol1->bottomCollision = B_TopEdge;
						boxCol2->topCollision = A_BottomEdge;

						// Calculate at what Transform positions the collision technically happened and store it for repositioning in RigidBody
						boxCol1->bottomCollidedPosition = Vector2(col2Pos.x, B_TopEdge + box1HalfHeight - 0.001f);
						boxCol2->topCollidedPosition = Vector2(col1Pos.x, A_BottomEdge - box2HalfHeight + 0.001f);
					}
				}
				// if both are at the same y Pos the it's a left/right collision
				else
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
					boxCol1->rightCollidedPosition = Vector2(B_LeftEdge - box1HalfWidth + 0.001f, col1Pos.y);
					boxCol2->leftCollidedPosition = Vector2(A_RightEdge + box2HalfWidth - 0.001f, col2Pos.y);
				}
			}
			// Both boxes are at the same x Pos
			else
			{
				// if boxCol1 is below boxCol2
				if (collider1CenterGrid.y < collider2CenterGrid.y)
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
					boxCol1->topCollidedPosition = Vector2(col1Pos.x, B_BottomEdge - box1HalfHeight + 0.001f);
					boxCol2->bottomCollidedPosition = Vector2(col2Pos.x, A_TopEdge + box2HalfHeight - 0.001f);
				}
				// if boxCol1 is above boxCol2
				else
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
					boxCol1->bottomCollidedPosition = Vector2(col2Pos.x, B_TopEdge + box1HalfHeight - 0.001f);
					boxCol2->topCollidedPosition = Vector2(col1Pos.x, A_BottomEdge - box2HalfHeight + 0.001f);
				}
			}
		}

		return _colliding;
	}

	bool Collider::IsColliding()
	{
		return _isColliding;
	}

	void Collider::SetColliding(bool _colliding)
	{
		_isColliding = _colliding;
	}

	void Collider::UpdatePreviousPosition()
	{
		std::shared_ptr<FlatEngine::Transform> transform = GetParent()->GetTransformComponent();
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
		std::shared_ptr<FlatEngine::Transform> transform = GetParent()->GetTransformComponent();
		Vector2 position = transform->GetPosition();

		return (previousPosition.x != position.x || previousPosition.y != position.y);
	}

	void Collider::RemoveCollidingObject(std::shared_ptr<GameObject> object)
	{
		for (std::vector<std::shared_ptr<GameObject>>::iterator iterator = collidingObjects.begin(); iterator != collidingObjects.end();)
		{
			if ((*iterator)->GetID() == object->GetID())
				collidingObjects.erase(iterator);

			iterator++;
		}
	}

	void Collider::AddCollidingObject(std::shared_ptr<Collider> collidedWith)
	{
		// Make sure we haven't already tracked it for this frame
		for (std::shared_ptr<FlatEngine::GameObject> object : collidingObjects)
		{
			// Leave function if the object is already known to be in active collision
			if (object->GetID() == collidedWith->GetParent()->GetID())
				return;
		}
		// else add the collided object
		collidingObjects.push_back(collidedWith->GetParent());

		// See if they were colliding in the last frame as well
		for (std::shared_ptr<FlatEngine::GameObject> object : collidingLastFrame)
		{
			// Leave function if the object has already fired OnCollisionEnter();
			if (object->GetID() == collidedWith->GetParent()->GetID())
				return;
		}

		// else, if OnCollisionEnter is set, fire it now. (upon initially adding the object to collidingObjects for the first time)
		if (OnCollisionEnterSet())
			OnCollisionEnter(GetParent(), collidedWith->GetParent());
	}

	std::vector<std::shared_ptr<GameObject>> Collider::GetCollidingObjects()
	{
		return collidingObjects;
	}

	void Collider::ClearCollidingObjects()
	{
		// Check which objects have left collision state since last frame
		for (std::shared_ptr<FlatEngine::GameObject> collidedLastFrame : collidingLastFrame)
		{		
			bool _objectStillColliding = false;

			for (std::shared_ptr<FlatEngine::GameObject> collidedThisFrame : collidingObjects)
				if (collidedLastFrame->GetID() == collidedThisFrame->GetID())
					_objectStillColliding = true;

			// Fire OnLeave if not colliding
			if (!_objectStillColliding)
				for (std::shared_ptr<FlatEngine::BoxCollider> boxCollider : collidedLastFrame->GetBoxColliders())
					if (boxCollider->OnCollisionLeaveSet())
						OnCollisionLeave(GetParent(), GetParent());
		}

		// Save colliding objects for next frame
		collidingLastFrame = collidingObjects;
		collidingObjects.clear();
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
			FlatEngine::LogString("Collider active layer must be an integer greater than 0.");
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
		rotation = GetParent()->GetTransformComponent()->GetRotation();
	}

	float Collider::GetRotation()
	{
		return rotation;
	}

	void Collider::RecalculateBounds()
	{
	}
}
