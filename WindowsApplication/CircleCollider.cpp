#include "Collider.h"
#include "CircleCollider.h"
#include "BoxCollider.h"
#include "RigidBody.h"
#include "FlatEngine.h"
#include "GameObject.h"
#include "Transform.h"
#include "imgui_internal.h"
#include <cmath>


namespace FlatEngine {

	CircleCollider::CircleCollider(long myID, long parentID)
	{
		SetType(ComponentTypes::CircleCollider);
		SetID(myID);
		SetParentID(parentID);
		SetActiveRadiusGrid(1.5);

		_activeEdgesSet = false;

		activeLeft = 0;
		activeRight = 0;
		activeBottom = 0;
		activeTop = 0;
		nextActiveLeft = 0;
		nextActiveRight = 0;
		nextActiveBottom = 0;
		nextActiveTop = 0;

		_isCollidingRight = false;
		_isCollidingLeft = false;
		_isCollidingBottom = false;
		_isCollidingTop = false;

		_rightCollisionStatic = false;
		_leftCollisionStatic = false;
		_bottomCollisionStatic = false;
		_topCollisionStatic = false;

		_rightCollisionSolid = false;
		_leftCollisionSolid = false;
		_bottomCollisionSolid = false;
		_topCollisionSolid = false;

		rightCollision = 0;
		leftCollision = 0;
		bottomCollision = 0;
		topCollision = 0;
	}

	CircleCollider::CircleCollider(std::shared_ptr<CircleCollider> toCopy, long newParentID)
	{		
		SetType(ComponentTypes::CircleCollider);
		SetID(GetNextComponentID());
		SetParentID(newParentID);
		SetActive(toCopy->IsActive());
		SetActiveLayer(toCopy->GetActiveLayer());
		SetIsContinuous(toCopy->IsContinuous());
		SetIsStatic(toCopy->IsStatic());
		SetIsSolid(toCopy->IsSolid());
		SetColliding(toCopy->IsColliding());
		SetPreviousPosition(toCopy->GetPreviousPosition());
		SetActiveRadiusScreen(toCopy->GetActiveRadiusScreen());
		SetActiveRadiusGrid(toCopy->GetActiveRadiusGrid());
		SetShowActiveRadius(toCopy->GetShowActiveRadius());
		SetOnActiveCollisionSet(toCopy->OnActiveCollisionSet());
		SetOnCollisionEnterSet(toCopy->OnCollisionEnterSet());
		SetOnCollisionLeaveSet(toCopy->OnCollisionLeaveSet());
		SetOnActiveCollision(toCopy->OnActiveCollision);
		SetOnCollisionEnter(toCopy->OnCollisionEnter);
		SetOnCollisionLeave(toCopy->OnCollisionLeave);

		_activeEdgesSet = false;

		activeLeft = 0;
		activeRight = 0;
		activeBottom = 0;
		activeTop = 0;
		nextActiveLeft = 0;
		nextActiveRight = 0;
		nextActiveBottom = 0;
		nextActiveTop = 0;

		_isCollidingRight = false;
		_isCollidingLeft = false;
		_isCollidingBottom = false;
		_isCollidingTop = false;

		_rightCollisionStatic = false;
		_leftCollisionStatic = false;
		_bottomCollisionStatic = false;
		_topCollisionStatic = false;

		_rightCollisionSolid = false;
		_leftCollisionSolid = false;
		_bottomCollisionSolid = false;
		_topCollisionSolid = false;

		rightCollision = 0;
		leftCollision = 0;
		bottomCollision = 0;
		topCollision = 0;
	}

	CircleCollider::CircleCollider(CircleCollider* exactCopy)
	{
		SetType(ComponentTypes::CircleCollider);
		SetID(exactCopy->GetID());
		SetParentID(exactCopy->GetParentID());
		SetActive(exactCopy->IsActive());
		SetActiveLayer(exactCopy->GetActiveLayer());
		SetIsContinuous(exactCopy->IsContinuous());
		SetIsStatic(exactCopy->IsStatic());
		SetIsSolid(exactCopy->IsSolid());
		SetColliding(exactCopy->IsColliding());
		SetPreviousPosition(exactCopy->GetPreviousPosition());
		SetActiveRadiusScreen(exactCopy->GetActiveRadiusScreen());
		SetActiveRadiusGrid(exactCopy->GetActiveRadiusGrid());
		SetShowActiveRadius(exactCopy->GetShowActiveRadius());
		SetOnActiveCollisionSet(exactCopy->OnActiveCollisionSet());
		SetOnCollisionEnterSet(exactCopy->OnCollisionEnterSet());
		SetOnCollisionLeaveSet(exactCopy->OnCollisionLeaveSet());
		SetOnActiveCollision(exactCopy->OnActiveCollision);
		SetOnCollisionEnter(exactCopy->OnCollisionEnter);
		SetOnCollisionLeave(exactCopy->OnCollisionLeave);
		SetCenterGrid(exactCopy->GetCenterGrid());
		SetCenterCoord(exactCopy->GetCenterCoord());
		SetNextCenterGrid(exactCopy->GetNextCenterGrid());
		SetNextCenterCoord(exactCopy->GetNextCenterCoord());

		_activeEdgesSet = exactCopy->_activeEdgesSet;

		activeLeft = exactCopy->activeLeft;
		activeRight = exactCopy->activeRight;
		activeBottom = exactCopy->activeBottom;
		activeTop = exactCopy->activeTop;
		nextActiveLeft = exactCopy->nextActiveLeft;
		nextActiveRight = exactCopy->nextActiveRight;
		nextActiveBottom = exactCopy->nextActiveBottom;
		nextActiveTop = exactCopy->nextActiveTop;

		_isCollidingRight = exactCopy->_isCollidingRight;
		_isCollidingLeft = exactCopy->_isCollidingLeft;
		_isCollidingBottom = exactCopy->_isCollidingBottom;
		_isCollidingTop = exactCopy->_isCollidingTop;

		_rightCollisionStatic = exactCopy->_rightCollisionStatic;
		_leftCollisionStatic = exactCopy->_leftCollisionStatic;
		_bottomCollisionStatic = exactCopy->_bottomCollisionStatic;
		_topCollisionStatic = exactCopy->_topCollisionStatic;

		_rightCollisionSolid = exactCopy->_rightCollisionSolid;
		_leftCollisionSolid = exactCopy->_leftCollisionSolid;
		_bottomCollisionSolid = exactCopy->_bottomCollisionSolid;
		_topCollisionSolid = exactCopy->_topCollisionSolid;

		rightCollision = exactCopy->rightCollision;
		leftCollision = exactCopy->leftCollision;
		bottomCollision = exactCopy->bottomCollision;
		topCollision = exactCopy->topCollision;
	}

	CircleCollider::~CircleCollider()
	{
	}

	bool CircleCollider::CheckForCollision(std::shared_ptr<FlatEngine::Collider> other)
	{
		if (loadedProject->GetCollisionDetection() == "Shared Axis")
		{
			return SharedAxisCheckForCollision(other);
		}
		else if (loadedProject->GetCollisionDetection() == "Separating Axis")
		{
			// TODO
			return false;
		}
		return false;
	}

	bool CircleCollider::SharedAxisCheckForCollision(std::shared_ptr<FlatEngine::Collider> other)
	{
		bool _colliding = false;
		Vector2 circleCenterGrid = GetCenterGrid();
		Vector2 boxCenterGrid = other->GetCenterGrid();
		float activeRadius = GetActiveRadiusGrid();

		// Calculate activeRadius with pythag
		float rise = std::abs(boxCenterGrid.y - circleCenterGrid.y);
		float run = std::abs(boxCenterGrid.x - circleCenterGrid.x);
		float centerDistance = std::sqrt((rise * rise) + (run * run));

		if (other->GetTypeString() == "BoxCollider")
		{
			std::shared_ptr<FlatEngine::BoxCollider> boxCol = std::static_pointer_cast<FlatEngine::BoxCollider>(other);
			
			if (centerDistance < boxCol->GetActiveRadiusGrid() + activeRadius)
			{
				_colliding = Collider::CheckForCollisionBoxCircle(boxCol, std::make_shared<CircleCollider>(this));

				if (_colliding)
				{
					SetColliding(_colliding);
					boxCol->SetColliding(_colliding);
				}
			}
		}
		else if (other->GetTypeString() == "CircleCollider")
		{
			std::shared_ptr<FlatEngine::BoxCollider> circleCol = std::static_pointer_cast<FlatEngine::BoxCollider>(other);

			if (centerDistance < other->GetActiveRadiusGrid() + activeRadius)
			{
				_colliding = true;
				SetColliding(_colliding);
				circleCol->SetColliding(_colliding);
			}
		}

		return _colliding;
	}

	void CircleCollider::AddCollidingObject(std::shared_ptr<FlatEngine::GameObject> collidedWith)
	{
		for (std::shared_ptr<FlatEngine::GameObject> object : GetCollidingObjects())
		{
			// Leave function if the object is already known to be in active collision
			if (object->GetID() == collidedWith->GetID())
				return;
		}
		Collider::AddCollidingObject(collidedWith);

		// If OnCollisionEnter is set, fire it now. (upon initially adding the object to collidingObjects for the first time)
		if (OnCollisionEnterSet())
			OnCollisionEnter(GetParent(), collidedWith);
		if (collidedWith->GetCircleCollider()->OnCollisionEnterSet())
			collidedWith->GetCircleCollider()->OnCollisionEnter(collidedWith, GetParent());
	}

	void CircleCollider::ResetCollisions()
	{
		// TOOOOO DOOOOOO
		// Change this later to not reset grounded but instead inside ApplyColliderPhysics use _Collidingbottom bools and velocity to decide if a CircleCollider is grounded or not.

		if (GetParent() != nullptr && GetParent()->HasComponent("RigidBody"))
			GetParent()->GetRigidBody()->SetIsGrounded(false);
		std::string name = GetParent()->GetName();

		ClearCollidingObjects();
		SetColliding(false);

		_isCollidingRight = false;
		_isCollidingLeft = false;
		_isCollidingBottom = false;
		_isCollidingTop = false;

		rightCollision = 0;
		leftCollision = 0;
		bottomCollision = 0;
		topCollision = 0;
	}

	void CircleCollider::UpdateCenter()
	{
		float step = FlatGui::sceneViewGridStep.x;
		Vector2 centerPoint = FlatGui::sceneViewCenter;
		std::shared_ptr<FlatEngine::Transform> transform = GetParent()->GetTransformComponent();
		Vector2 position = transform->GetTruePosition();
		float activeRadius = GetActiveRadiusGrid();

		float xCenter = centerPoint.x + (position.x + activeOffset.x) * step;
		float yCenter = centerPoint.y - (position.y + activeOffset.y) * step;
		
		SetCenterCoord(Vector2(xCenter, yCenter));
		SetCenterGrid(Vector2(position.x + activeOffset.x, position.y + activeOffset.y));
	}

	// Just based on actual pixel locations (0,0) being the top left of the window
// You can use it for either game view or scene view, you just need the correct center location of whichever you choose
	void CircleCollider::UpdateActiveEdges()
	{
		if (loadedProject->GetCollisionDetection() == "Shared Axis")
		{
			SharedAxisUpdateActiveEdges();
		}
		else if (loadedProject->GetCollisionDetection() == "Separating Axis")
		{
			// TODO
		}
	}

	void CircleCollider::SharedAxisUpdateActiveEdges()
	{
		// Only if the activeEdges has not been set or if the velocity is not 0 do we update the active edges
		bool _shouldUpdate = false;

		std::shared_ptr<FlatEngine::GameObject> parent = GetParent();
		std::shared_ptr<FlatEngine::Transform> transform = nullptr;
		Vector2 scale = Vector2(1, 1);
		float activeRadius = GetActiveRadiusGrid();

		if (parent != nullptr)
			transform = parent->GetTransformComponent();
		if (transform != nullptr)
			scale = transform->GetScale();

		std::shared_ptr<FlatEngine::RigidBody> rigidBody;
		if (GetParent() != nullptr && GetParent()->HasComponent("RigidBody"))
		{
			rigidBody = GetParent()->GetRigidBody();
			Vector2 velocity = rigidBody->GetVelocity();

			if (velocity.x != 0 || velocity.y != 0 || !_activeEdgesSet || HasMoved())
				_shouldUpdate = true;
		}
		else
		{
			if (!_activeEdgesSet || HasMoved())
				_shouldUpdate = true;
		}

		if (_shouldUpdate)
		{
			std::shared_ptr<FlatEngine::RigidBody> rigidBody = parent->GetRigidBody();

			float step = FlatGui::sceneViewGridStep.x;
			Vector2 centerPoint = FlatGui::sceneViewCenter;
			std::shared_ptr<FlatEngine::Transform> transform = GetParent()->GetTransformComponent();
			Vector2 scale = transform->GetScale();

			activeLeft = centerPoint.x + (GetCenterGrid().x - (activeRadius * scale.x) + activeOffset.x) * step;
			activeTop = centerPoint.y + (-GetCenterGrid().y - (activeRadius * scale.y) + activeOffset.y) * step;
			activeRight = centerPoint.x + (GetCenterGrid().x + (activeRadius * scale.x) + activeOffset.x) * step;
			activeBottom = centerPoint.y + (-GetCenterGrid().y + (activeRadius * scale.y) + activeOffset.y) * step;

			// For collision detection ( grid space values )
			if (rigidBody != nullptr)
				SetNextCenterGrid(rigidBody->GetNextPosition());
			else
				SetNextCenterGrid(transform->GetTruePosition());

			nextActiveLeft = GetNextCenterGrid().x - (activeRadius * scale.x) + activeOffset.x;
			nextActiveTop = GetNextCenterGrid().y + (activeRadius * scale.y) + activeOffset.y;
			nextActiveRight = GetNextCenterGrid().x + (activeRadius * scale.x) + activeOffset.x;
			nextActiveBottom = GetNextCenterGrid().y - (activeRadius * scale.y) + activeOffset.y;

			//SetNextCenterCoord(Vector2(nextActiveLeft + (nextActiveRight - nextActiveLeft) / 2, nextActiveTop + (nextActiveBottom - nextActiveTop) / 2));			
			_activeEdgesSet = true;
		}
	}

	std::string CircleCollider::GetData()
	{
		json jsonData = {
			{ "type", "CircleCollider" },
			{ "id", GetID() },
			{ "_isCollapsed", IsCollapsed() },
			{ "_isActive", IsActive() },
			{ "_isContinuous", IsContinuous()},
			{ "_isSolid", IsSolid() },
			{ "_isStatic", IsStatic() },
			{ "activeLayer", GetActiveLayer() },
			{ "activeRadius", GetActiveRadiusGrid() },
			{ "_showActiveRadius", GetShowActiveRadius()},
		};

		std::string data = jsonData.dump();
		// Return dumped json object with required data for saving
		return data;
	}

	void CircleCollider::RecalculateBounds()
	{
		UpdateCenter();
		UpdateActiveEdges();
	}
}
