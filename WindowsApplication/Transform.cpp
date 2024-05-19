#include "Transform.h"
#include "FlatEngine.h"
#include "BoxCollider.h"

namespace FlatEngine
{
	Transform::Transform(long myID, long parentID)
	{
		SetType(ComponentTypes::Transform);
		SetID(myID);
		SetParentID(parentID);
		origin.x = 0;
		origin.y = 0;
		position.x = 0;
		position.y = 0;
		scale.x = 1;
		scale.y = 1;
		rotation = 0;
	}

	Transform::Transform(std::shared_ptr<Transform> toCopy, long newParentID)
	{
		SetType(ComponentTypes::Transform);
		SetID(GetNextComponentID());
		SetParentID(newParentID);
		SetActive(toCopy->IsActive());
		origin.x = toCopy->GetOrigin().x;
		origin.y = toCopy->GetOrigin().y;
		position.x = toCopy->GetPosition().x;
		position.y = toCopy->GetPosition().y;
		scale.x = toCopy->GetScale().x;
		scale.y = toCopy->GetScale().y;
		rotation = toCopy->GetRotation();
	}

	Transform::~Transform()
	{
	}

	void Transform::SetInitialPosition(Vector2 initialPos)
	{
		position = initialPos;
	}

	void Transform::SetOrigin(Vector2 newOrigin)
	{
		origin = newOrigin;
	}

	Vector2 Transform::GetOrigin()
	{
		return origin;
	}

	Vector2 Transform::GetTruePosition()
	{
		return Vector2(origin.x + position.x, origin.y + position.y);
	}

	std::string Transform::GetData()
	{
		json jsonData = { 
			{ "type", "Transform" },
			{ "id", GetID() },
			{ "_isCollapsed", IsCollapsed() },
			{ "_isActive", IsActive() },
			{ "xOrigin", origin.x },
			{ "yOrigin", origin.y },
			{ "xPos", position.x }, 
			{ "yPos", position.y },
			{ "rotation", rotation },
			{ "xScale", scale.x },
			{ "yScale", scale.y }
		};

		std::string data = jsonData.dump();
		// Return dumped json object with required data for saving
		return data;
	}

	// Setters
	void Transform::SetPosition(Vector2 newPosition)
	{
		//if (GetParent()->HasComponent("BoxCollider"))
		//	GetParent()->GetBoxCollider()->RecalculateBounds();
		position = newPosition;

		if (GetParent() != nullptr && GetParent()->HasChildren())
		{
			for (long id : GetParent()->GetChildren())
			{
				std::shared_ptr<GameObject> child = GetObjectById(id);
				if (child->HasComponent("Transform"))
					child->GetTransformComponent()->UpdateOrigin(GetTruePosition());
			}
		}
	}

	void Transform::UpdateOrigin(Vector2 newOrigin)
	{
		origin = newOrigin;
		UpdateChildOrigins(GetTruePosition());
	}

	void Transform::UpdateChildOrigins(Vector2 newOrigin)
	{
		if (GetParent()->HasChildren())
		{
			for (long id : GetParent()->GetChildren())
			{
				std::shared_ptr<GameObject> child = GetObjectById(id);
				if (child->HasComponent("Transform"))
				{
					std::shared_ptr<Transform> childTransform = child->GetTransformComponent();
					childTransform->SetOrigin(newOrigin);
					childTransform->UpdateChildOrigins(GetTruePosition());
				}
			}
		}
	}

	void Transform::SetScale(Vector2 newScale)
	{
		scale = newScale;
	}

	void Transform::SetRotation(float newRotation)
	{
		if (loadedProject->GetCollisionDetection() == "Separating Axis (Rotational)" && GetParent()->HasComponent("BoxCollider"))
			GetParent()->GetBoxCollider()->SetRotation(newRotation);
		rotation = newRotation;
	}

	// Getters
	Vector2 Transform::GetPosition()
	{
		return position;
	}

	Vector2 Transform::GetScale()
	{
		return scale;
	}

	float Transform::GetRotation()
	{
		return rotation;
	}
}
