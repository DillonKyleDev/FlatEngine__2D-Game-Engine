#include "Transform.h"
#include "FlatEngine.h"

namespace FlatEngine
{
	Transform::Transform(long myID, long parentID)
	{
		SetType(ComponentTypes::Transform);
		SetID(myID);
		SetParentID(parentID);
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
		position.x = toCopy->GetPosition().x;
		position.y = toCopy->GetPosition().y;
		scale.x = toCopy->GetScale().x;
		scale.y = toCopy->GetScale().y;
		rotation = toCopy->GetRotation();
	}

	Transform::~Transform()
	{
	}

	std::string Transform::GetData()
	{
		json jsonData = { 
			{ "type", "Transform" },
			{ "id", GetID() },
			{ "_isCollapsed", IsCollapsed() },
			{ "_isActive", IsActive() },
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
		position = newPosition;
	}

	void Transform::SetScale(Vector2 newScale)
	{
		scale = newScale;
	}

	void Transform::SetRotation(float newRotation)
	{
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
