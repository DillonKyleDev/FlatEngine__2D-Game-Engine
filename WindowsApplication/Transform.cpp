#include "Transform.h"


namespace FlatEngine
{
	Transform::Transform()
	{
		this->SetType(Component::ComponentTypes::Transform);
		this->position.x = 0;
		this->position.y = 0;
		this->posX = 0;
		this->posY = 0;
		this->rotation = 5;
	}


	Transform::~Transform()
	{

	}


	std::string Transform::GetData()
	{
		this->position.x = this->position.x;
		this->position.x = this->position.x;
		this->position.y = this->position.y;
		this->position.y = this->position.y;
		json jsonData = { 
			{ "type", this->GetTypeString() },
			{ "xPos", this->posX }, 
			{ "yPos", this->posY },
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


	Vector2 Transform::GetPosition()
	{
		this->position = this->position;
		return this->position;
	}
}
