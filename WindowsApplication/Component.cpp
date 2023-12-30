#include "Component.h"


Component::Component()
{
	//this->type = Component::ComponentTypes::Null;
	//this->position.x = 0;
	//this->position.y = 0;
}


Component::~Component()
{

}


void Component::SetType(ComponentTypes type)
{
	this->type = type;
}


Component::ComponentTypes Component::GetType()
{
	return this->type;
}


std::string Component::GetTypeString()
{
	this->type = this->type;
	switch (this->type)
	{
	case Component::ComponentTypes::Transform:
		return "Transform";
		break;

	case Component::ComponentTypes::Sprite:
		return "Sprite";
		break;

	default:
		return "Null";
	}
}


std::string Component::GetData()
{
	return "{}";
}