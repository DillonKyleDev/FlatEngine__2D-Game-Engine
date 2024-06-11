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

	bool Collider::CheckForCollision(std::shared_ptr<FlatEngine::Collider> other)
	{
		return false;
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

	bool Collider::CheckForCollisionBoxCircle(std::shared_ptr<FlatEngine::BoxCollider> boxCol, std::shared_ptr<FlatEngine::CircleCollider> circleCol)
	{
		bool _colliding = false;
		Vector2 circleCenterGrid = circleCol->GetCenterGrid();
		Vector2 boxCenterGrid = boxCol->GetCenterGrid();

		float activeRadius = circleCol->GetActiveRadiusGrid();

		float A_TopEdge = circleCol->nextActiveTop;
		float A_RightEdge = circleCol->nextActiveRight;
		float A_BottomEdge = circleCol->nextActiveBottom;
		float A_LeftEdge = circleCol->nextActiveLeft;

		float B_TopEdge = boxCol->nextActiveTop;
		float B_RightEdge = boxCol->nextActiveRight;
		float B_BottomEdge = boxCol->nextActiveBottom;
		float B_LeftEdge = boxCol->nextActiveLeft;

		// Check how colliders are oriented in relation to each other //

		// check for direct left/right/top/bottom collisions
		// Right
		if (circleCenterGrid.x < boxCenterGrid.x && circleCenterGrid.y < B_TopEdge && circleCenterGrid.y > B_BottomEdge && (B_LeftEdge - circleCenterGrid.x < activeRadius))
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
		}
		// Left
		else if (circleCenterGrid.x > boxCenterGrid.x && circleCenterGrid.y < B_TopEdge && circleCenterGrid.y > B_BottomEdge && (circleCenterGrid.x - B_RightEdge < activeRadius))
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
		}
		// Top
		else if (circleCenterGrid.y > boxCenterGrid.y && circleCenterGrid.x < B_RightEdge && circleCenterGrid.x > B_LeftEdge && (circleCenterGrid.y - B_TopEdge < activeRadius))
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
		}
		// Bottom
		else if (circleCenterGrid.y < boxCenterGrid.y && circleCenterGrid.x < B_RightEdge && circleCenterGrid.x > B_LeftEdge && (B_BottomEdge - circleCenterGrid.y < activeRadius))
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
		}
		// Check for all other (corner) collisions
		// if circleCol is to the right
		else if (circleCenterGrid.x > boxCenterGrid.x)
		{
			// if self is below other
			if (circleCenterGrid.y < boxCenterGrid.y)
			{
				// Calculate distance from corner with pythag
				float rise = std::abs(B_BottomEdge - circleCenterGrid.y);
				float run = std::abs(B_RightEdge - circleCenterGrid.x);
				float cornerDistance = std::sqrt((rise * rise) + (run * run));

				if (cornerDistance < activeRadius)
				{
					_colliding = true;

					float leftRightOverlap = B_RightEdge - A_LeftEdge;
					float topBottomOverlap = A_TopEdge - B_BottomEdge;
					// Left/Right
					if (leftRightOverlap < topBottomOverlap)
					{
						circleCol->_isCollidingLeft = true;
						circleCol->_leftCollisionStatic = boxCol->IsStatic();
						circleCol->_leftCollisionSolid = boxCol->IsSolid();
						boxCol->_isCollidingRight = true;
						boxCol->_rightCollisionStatic = circleCol->IsStatic();
						boxCol->_rightCollisionSolid = boxCol->IsSolid();
						circleCol->leftCollision = B_RightEdge;
						boxCol->rightCollision = A_LeftEdge;
					}
					// Top/Bottom
					else {
						circleCol->_isCollidingTop = true;
						circleCol->_topCollisionStatic = boxCol->IsStatic();
						circleCol->_topCollisionSolid = boxCol->IsSolid();
						boxCol->_isCollidingBottom = true;
						boxCol->_bottomCollisionStatic = circleCol->IsStatic();
						boxCol->_bottomCollisionSolid = circleCol->IsSolid();
						circleCol->topCollision = B_BottomEdge;
						boxCol->bottomCollision = A_TopEdge;
					}
				}
			}
			// if circleCol is above
			else if (circleCenterGrid.y > boxCenterGrid.y)
			{
				// Calculate distance from corner with pythag
				float rise = std::abs(B_TopEdge - circleCenterGrid.y);
				float run = std::abs(B_RightEdge - circleCenterGrid.x);
				float cornerDistance = std::sqrt((rise * rise) + (run * run));

				if (cornerDistance < activeRadius)
				{
					_colliding = true;

					float leftRightOverlap = B_RightEdge - A_LeftEdge;
					float topBottomOverlap = B_TopEdge - A_BottomEdge;
					// Left/Right
					if (leftRightOverlap < topBottomOverlap)
					{
						circleCol->_isCollidingLeft = true;
						circleCol->_leftCollisionStatic = boxCol->IsStatic();
						circleCol->_leftCollisionSolid = boxCol->IsSolid();
						boxCol->_isCollidingRight = true;
						boxCol->_rightCollisionStatic = circleCol->IsStatic();
						boxCol->_rightCollisionSolid = circleCol->IsSolid();
						circleCol->leftCollision = B_RightEdge;
						boxCol->rightCollision = A_LeftEdge;
					}
					// Top/Bottom
					else {
						circleCol->_isCollidingBottom = true;
						circleCol->_bottomCollisionStatic = boxCol->IsStatic();
						circleCol->_bottomCollisionSolid = boxCol->IsSolid();
						boxCol->_isCollidingTop = true;
						boxCol->_topCollisionStatic = circleCol->IsStatic();
						boxCol->_topCollisionSolid = circleCol->IsSolid();
						circleCol->bottomCollision = B_TopEdge;
						boxCol->topCollision = A_BottomEdge;
					}
				}
			}
		}
		// if circleCol is to the left
		else if (circleCenterGrid.x < boxCenterGrid.x)
		{
			// if circleCol is below boxCol
			if (circleCenterGrid.y < boxCenterGrid.y)
			{
				// Calculate distance from corner with pythag
				float rise = std::abs(B_BottomEdge - circleCenterGrid.y);
				float run = std::abs(B_LeftEdge - circleCenterGrid.x);
				float cornerDistance = std::sqrt((rise * rise) + (run * run));

				if (cornerDistance < activeRadius)
				{
					_colliding = true;

					float leftRightOverlap = A_RightEdge - B_LeftEdge;
					float topBottomOverlap = A_TopEdge - B_BottomEdge;
					// Left/Right
					if (leftRightOverlap < topBottomOverlap)
					{
						circleCol->_isCollidingRight = true;
						circleCol->_rightCollisionStatic = boxCol->IsStatic();
						circleCol->_rightCollisionSolid = boxCol->IsSolid();
						boxCol->_isCollidingLeft = true;
						boxCol->_leftCollisionStatic = circleCol->IsStatic();
						boxCol->_leftCollisionSolid = circleCol->IsSolid();
						circleCol->rightCollision = B_LeftEdge;
						boxCol->leftCollision = A_RightEdge;
					}
					// Top/Bottom
					else {
						circleCol->_isCollidingTop = true;
						circleCol->_topCollisionStatic = boxCol->IsStatic();
						circleCol->_topCollisionSolid = boxCol->IsSolid();
						boxCol->_isCollidingBottom = true;
						boxCol->_bottomCollisionStatic = circleCol->IsStatic();
						boxCol->_bottomCollisionSolid = circleCol->IsSolid();
						circleCol->topCollision = B_BottomEdge;
						boxCol->bottomCollision = A_TopEdge;
					}
				}
			}
			// if circleCol is above
			else if (circleCenterGrid.y > boxCenterGrid.y)
			{
				// Calculate distance from corner with pythag
				float rise = std::abs(B_TopEdge - circleCenterGrid.y);
				float run = std::abs(B_LeftEdge - circleCenterGrid.x);
				float cornerDistance = std::sqrt((rise * rise) + (run * run));

				if (cornerDistance < activeRadius)
				{
					_colliding = true;

					float leftRightOverlap = A_RightEdge - B_LeftEdge;
					float topBottomOverlap = B_TopEdge - A_BottomEdge;
					// Left/Right
					if (leftRightOverlap < topBottomOverlap)
					{
						circleCol->_isCollidingRight = true;
						circleCol->_rightCollisionStatic = boxCol->IsStatic();
						circleCol->_rightCollisionSolid = boxCol->IsSolid();
						boxCol->_isCollidingLeft = true;
						boxCol->_leftCollisionStatic = circleCol->IsStatic();
						boxCol->_leftCollisionSolid = circleCol->IsSolid();
						circleCol->rightCollision = B_LeftEdge;
						boxCol->leftCollision = A_RightEdge;
					}
					// Top/Bottom
					else {
						circleCol->_isCollidingBottom = true;
						circleCol->_bottomCollisionStatic = boxCol->IsStatic();
						circleCol->_bottomCollisionSolid = boxCol->IsSolid();
						boxCol->_isCollidingTop = true;
						boxCol->_topCollisionStatic = circleCol->IsStatic();
						boxCol->_topCollisionSolid = circleCol->IsSolid();
						circleCol->bottomCollision = B_TopEdge;
						boxCol->topCollision = A_BottomEdge;
					}
				}
			}
		}

		return _colliding;
	}

	void Collider::AddCollidingObject(std::shared_ptr<GameObject> collidedWith)
	{
		collidingObjects.push_back(collidedWith);
	}

	std::vector<std::shared_ptr<GameObject>> Collider::GetCollidingObjects()
	{
		return collidingObjects;
	}

	void Collider::ClearCollidingObjects()
	{
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
