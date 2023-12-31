#include "Transform.h"


namespace FlatEngine
{
	Transform::Transform()
	{
		this->SetType(Component::ComponentTypes::Transform);
		this->position.x = 0;
		this->position.y = 0;
		this->rotation = 0;
	}


	Transform::~Transform()
	{

	}


	std::string Transform::GetData()
	{
		json jsonData = { 
			{ "type", "Transform" },
			{ "xPos", this->position.x }, 
			{ "yPos", this->position.y },
			{ "rotation", this->rotation }
		};

		std::string data = jsonData.dump();
		// Return dumped json object with required data for saving
		return data;
	}


	void Transform::SetPosition(Vector2 position)
	{
		this->position = position;
	}


	void Transform::SetRotation(float rotation)
	{
		this->rotation = rotation;
	}


	Vector2 Transform::GetPosition()
	{
		return this->position;
	}


	float Transform::GetRotation()
	{
		return this->rotation;
	}
}
