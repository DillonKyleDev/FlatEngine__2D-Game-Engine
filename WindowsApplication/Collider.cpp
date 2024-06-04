#include "Collider.h"
#include "RigidBody.h"
#include "FlatEngine.h"
#include "GameObject.h"
#include "Transform.h"
#include "imgui_internal.h"
#include <cmath>


namespace FlatEngine
{
	Collider::Collider(long myID, long parentID, ComponentTypes type)
	{
		SetType(type);
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

	Collider::Collider(std::shared_ptr<Collider> toCopy, long newParentID, ComponentTypes type)
	{
		SetType(type);
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

	bool Collider::CheckForCollision(std::shared_ptr<Collider> other)
	{
		return true;
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
