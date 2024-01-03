#include "Component.h"


namespace FlatEngine
{
	Component::Component()
	{
		this->type = Component::ComponentTypes::Null;
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

		case Component::ComponentTypes::Camera:
			return "Camera";
			break;

		case Component::ComponentTypes::Script:
			return "Script";
			break;

		default:
			return "Null";
		}
	}


	std::string Component::GetData()
	{
		return "{}";
	}
}