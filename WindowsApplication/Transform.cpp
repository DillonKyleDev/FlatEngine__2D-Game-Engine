#include "Transform.h"
#include "FlatEngine.h"

namespace FlatEngine
{
	Transform::Transform(long myID, long parentID)
	{
		this->SetType(ComponentTypes::Transform);
		this->SetID(myID);
		this->SetParentID(parentID);
		this->position.x = 0;
		this->position.y = 0;
		this->scale.x = 1;
		this->scale.y = 1;
		this->rotation = 0;
	}

	Transform::Transform(std::shared_ptr<Transform> toCopy, long newParentID)
	{
		this->SetType(ComponentTypes::Transform);
		this->SetID(GetNextComponentID());
		this->SetParentID(newParentID);
		this->position.x = toCopy->GetPosition().x;
		this->position.y = toCopy->GetPosition().y;
		this->scale.x = toCopy->GetScale().x;
		this->scale.y = toCopy->GetScale().y;
		this->rotation = toCopy->GetRotation();
	}

	Transform::~Transform()
	{
	}

	std::string Transform::GetData()
	{
		json jsonData = { 
			{ "type", "Transform" },
			{ "id", this->GetID() },
			{ "_isCollapsed", this->IsCollapsed() },
			{ "xPos", this->position.x }, 
			{ "yPos", this->position.y },
			{ "rotation", this->rotation },
			{ "xScale", this->scale.x },
			{ "yScale", this->scale.y }
		};

		std::string data = jsonData.dump();
		// Return dumped json object with required data for saving
		return data;
	}

	// Setters
	void Transform::SetPosition(Vector2 position)
	{
		this->position = position;
	}

	void Transform::SetScale(Vector2 scale)
	{
		this->scale = scale;
	}

	void Transform::SetRotation(float rotation)
	{
		this->rotation = rotation;
	}

	// Getters
	Vector2 Transform::GetPosition()
	{
		return this->position;
	}

	Vector2 Transform::GetScale()
	{
		return this->scale;
	}

	float Transform::GetRotation()
	{
		return this->rotation;
	}
}
