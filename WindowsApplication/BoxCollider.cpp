#include "BoxCollider.h"
#include "RigidBody.h"
#include "FlatEngine.h"
#include "GameObject.h"
#include "Transform.h"


namespace FlatEngine
{
	BoxCollider::BoxCollider(long myID, long parentID)
	{
		SetType(ComponentTypes::BoxCollider);
		SetID(myID);
		SetParentID(parentID);
		activeWidth = 5;
		activeHeight = 3;
		activeOffset = Vector2(0, 0);
		activeEdges = ImVec4(0, 0, 0, 0);
		activeLayer = 0;
		_isContinious = false;
		_isColliding = false;
		_activeEdgesSet = false;
		previousPosition = Vector2(0, 0);

		// Initialize callback functions to nullptr
		OnActiveCollision = nullptr;
		OnCollisionEnter = nullptr;
		OnCollisionLeave = nullptr;

		_onActiveCollidingSet = false;
		_onCollisionEnterSet = false;
		_onCollisionLeaveSet = false;
	}

	BoxCollider::BoxCollider(std::shared_ptr<BoxCollider> toCopy, long newParentID)
	{
		SetType(ComponentTypes::BoxCollider);
		SetID(GetNextComponentID());
		SetParentID(newParentID);
		SetActive(toCopy->IsActive());
		activeWidth = toCopy->GetActiveWidth();
		activeHeight = toCopy->GetActiveHeight();
		activeOffset = toCopy->GetActiveOffset();
		activeLayer = toCopy->GetActiveLayer();
		_isContinious = toCopy->IsContinuous();
		_isColliding = false;
		_activeEdgesSet = false;
		previousPosition = Vector2(0, 0);

		// Initialize callback functions to nullptr
		OnActiveCollision = toCopy->OnActiveCollision;
		OnCollisionEnter = toCopy->OnCollisionEnter;
		OnCollisionLeave = toCopy->OnCollisionLeave;

		_onActiveCollidingSet = false;
		_onCollisionEnterSet = false;
		_onCollisionLeaveSet = false;
	}

	BoxCollider::~BoxCollider()
	{
	}

	void BoxCollider::SetOnColliding(std::function<void(std::shared_ptr<GameObject>, std::shared_ptr<GameObject>)> callback)
	{
		OnActiveCollision = callback;
		_onActiveCollidingSet = true;
	}

	void BoxCollider::SetOnCollisionEnter(std::function<void(std::shared_ptr<GameObject>, std::shared_ptr<GameObject>)> callback)
	{
		OnCollisionEnter = callback;
		_onCollisionEnterSet = true;
	}

	void BoxCollider::SetOnCollisionLeave(std::function<void(std::shared_ptr<GameObject>, std::shared_ptr<GameObject>)> callback)
	{
		OnCollisionLeave = callback;
		_onCollisionLeaveSet = true;
	}

	bool BoxCollider::IsColliding()
	{
		return _isColliding;
	}

	void BoxCollider::SetColliding(bool _colliding)
	{
		_isColliding = _colliding;
	}

	void BoxCollider::UpdatePreviousPosition()
	{
		std::shared_ptr<FlatEngine::Transform> transform = GetParent()->GetTransformComponent();
		previousPosition = transform->GetPosition();
	}

	bool BoxCollider::HasMoved()
	{
		std::shared_ptr<FlatEngine::Transform> transform = GetParent()->GetTransformComponent();
		Vector2 position = transform->GetPosition();
		//LogFloat(previousPosition.y, " " + GetParent()->GetName() + " Inner Previous Pos: ");

		return (previousPosition.x != position.x || previousPosition.y != position.y);
	}

	void BoxCollider::RemoveCollidingObject(std::shared_ptr<GameObject> object)
	{
		for (std::vector<std::shared_ptr<GameObject>>::iterator iterator = collidingObjects.begin(); iterator != collidingObjects.end();)
		{
			if ((*iterator)->GetID() == object->GetID())
				collidingObjects.erase(iterator);

			iterator++;
		}
	}

	void BoxCollider::AddCollidingObject(std::shared_ptr<GameObject> object)
	{
		collidingObjects.push_back(object);
	}

	std::vector<std::shared_ptr<GameObject>> BoxCollider::GetCollidingObjects()
	{
		return collidingObjects;
	}

	void BoxCollider::SetActiveDimensions(float width, float height)
	{
		if (width >= 0 && height >= 0)
		{
			activeWidth = width;
			activeHeight = height;
		}
		else
			FlatEngine::LogString("The active width or height you tried to set to BoxCollider component was < 0. Try again.");
	}

	void BoxCollider::SetActiveOffset(Vector2 offset)
	{
		activeOffset = offset;
	}

	void BoxCollider::SetActiveLayer(int layer)
	{
		if (layer >= 0)
			activeLayer = layer;
		else
			FlatEngine::LogString("BoxCollider active layer must be an integer greater than 0.");
	}

	int BoxCollider::GetActiveLayer()
	{
		return activeLayer;
	}

	float BoxCollider::GetActiveWidth()
	{
		return activeWidth;
	}

	float BoxCollider::GetActiveHeight()
	{
		return activeHeight;
	}

	Vector2 BoxCollider::GetActiveOffset()
	{
		return activeOffset;
	}

	void BoxCollider::SetActiveEdges(ImVec4 edges)
	{
		activeEdges = edges;
		_activeEdgesSet = true;
	}

	ImVec4 BoxCollider::GetActiveEdges()
	{
		return activeEdges;
	}

	// Just based on actual pixel locations (0,0) being the top left of the window
	// You can use it for either game view or scene view, you just need the correct center location of whichever you choose
	ImVec4 BoxCollider::UpdateActiveEdges(ImVec2 centerPoint, float gridStep)
	{
		// Only if the activeEdges has not been set or if the velocity is not 0 do we update the active edges
		bool _shouldUpdate = false;

		std::shared_ptr<FlatEngine::RigidBody> rigidBody;
		if (GetParent()->HasComponent("RigidBody"))
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
			Vector2 position;

			// If there is a RigidBody attached, take the next position it will be in as a reference for collision,
			// Else just take the Transforms position because it will be stationary and we don't need precise position checking
			if (rigidBody != nullptr)
				position = rigidBody->GetNextPosition();
			else
			{
				std::shared_ptr<FlatEngine::Transform> transform = this->GetParent()->GetTransformComponent();
				position = transform->GetPosition();
			}			

			float activeLeft = centerPoint.x + (position.x - (activeWidth / 2) + activeOffset.x) * gridStep;
			float activeTop = centerPoint.y + (-position.y - (activeHeight / 2) + activeOffset.y) * gridStep;
			float activeRight = centerPoint.x + (position.x + (activeWidth / 2) + activeOffset.x) * gridStep;
			float activeBottom = centerPoint.y + (-position.y + (activeHeight / 2) + activeOffset.y) * gridStep;

			SetActiveEdges(ImVec4(activeTop, activeRight, activeBottom, activeLeft));
		}

		return activeEdges;
	}

	void BoxCollider::SetIsContinuous(bool _continuous)
	{
		_isContinious = _continuous;
	}

	bool BoxCollider::IsContinuous()
	{
		return _isContinious;
	}

	std::string BoxCollider::GetData()
	{
		json jsonData = {
			{ "type", "BoxCollider" },
			{ "id", GetID() },
			{ "_isCollapsed", IsCollapsed() },
			{ "_isActive", IsActive() },
			{ "activeWidth", activeWidth },
			{ "activeHeight", activeHeight },
			{ "activeOffsetX", activeOffset.x },
			{ "activeOffsetY", activeOffset.y },
			{ "_isContinious", IsContinuous() },
			{ "activeLayer", activeLayer },
		};

		std::string data = jsonData.dump();
		// Return dumped json object with required data for saving
		return data;
	}
}
