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

		case Component::ComponentTypes::Button:
			return "Button";
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