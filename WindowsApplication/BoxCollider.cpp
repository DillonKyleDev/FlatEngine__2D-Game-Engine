#include "BoxCollider.h"
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
		activeLayer = 0;
		_isContinious = false;
		_isColliding = false;

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
	}

	ImVec4 BoxCollider::GetActiveEdges()
	{
		return activeEdges;
	}

	ImVec4 BoxCollider::UpdateActiveEdges(ImVec2 centerPoint, float gridStep)
	{
		std::shared_ptr<FlatEngine::Transform> transform = this->GetParent()->GetTransformComponent();
		Vector2 position = transform->GetPosition();
		Vector2 scale = transform->GetScale();

		float activeLeft = FlatGui::WorldToViewport(centerPoint.x, position.x + activeOffset.x - (activeWidth / 2 * scale.x), gridStep, false);
		float activeRight = FlatGui::WorldToViewport(centerPoint.x, position.x + activeOffset.x + (activeWidth / 2 * scale.x), gridStep, false);
		float activeTop = FlatGui::WorldToViewport(centerPoint.y, position.y + activeOffset.y + (activeHeight / 2 * scale.y), gridStep, true);
		float activeBottom = FlatGui::WorldToViewport(centerPoint.y, position.y + activeOffset.y - (activeHeight / 2 * scale.y), gridStep, true);

		SetActiveEdges(ImVec4(activeTop, activeRight, activeBottom, activeLeft));

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
