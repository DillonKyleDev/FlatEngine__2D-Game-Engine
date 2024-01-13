#include "Component.h"


namespace FlatEngine
{
	Component::Component()
	{
		this->type = Component::ComponentTypes::Null;
		this->ID = -1;
		this->parentID = -1;
		this->_isCollapsed = false;
	}


	Component::~Component()
	{
	}


	void Component::SetParentID(long parentID)
	{
		this->parentID = parentID;
	}


	long Component::GetParentID()
	{
		return this->parentID;
	}


	void Component::SetType(ComponentTypes type)
	{
		this->type = type;
	}

	void Component::SetID(long ID)
	{
		this->ID = ID;
	}

	long Component::GetID()
	{
		return this->ID;
	}

	Component::ComponentTypes Component::GetType()
	{
		return this->type;
	}

	bool Component::IsCollapsed()
	{
		return this->_isCollapsed;
	}


	std::string Component::GetTypeString()
	{
		this->type = this->type;
		switch (this->type)
		{
		case ComponentTypes::Transform:
			return "Transform";
			break;

		case ComponentTypes::Sprite:
			return "Sprite";
			break;

		case ComponentTypes::Camera:
			return "Camera";
			break;

		case ComponentTypes::Script:
			return "Script";
			break;

		case ComponentTypes::Button:
			return "Button";
			break;

		case ComponentTypes::Canvas:
			return "Canvas";
			break;

		case ComponentTypes::Animation:
			return "Animation";
			break;

		default:
			return "Null";
		}
	}

	void Component::SetCollapsed(bool collapsed)
	{
		this->_isCollapsed = collapsed;
	}


	std::string Component::GetData()
	{
		return "{}";
	}

}