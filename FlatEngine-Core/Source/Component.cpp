#include "Component.h"
#include "FlatEngine.h"

namespace FlatEngine
{
	Component::Component()
	{
		type = Component::ComponentTypes::T_Null;
		ID = -1;
		parentID = -1;
		_isCollapsed = false;
		_isActive = true;
	}

	Component::~Component()
	{
	}

	void Component::SetParentID(long newParentID)
	{
		parentID = newParentID;
	}

	long Component::GetParentID()
	{
		return parentID;
	}

	GameObject* Component::GetParent()
	{
		return GetObjectById(parentID);
	}

	void Component::SetType(ComponentTypes componentType)
	{
		type = componentType;
	}

	void Component::SetID(long newID)
	{
		ID = newID;
	}

	long Component::GetID()
	{
		return ID;
	}

	Component::ComponentTypes Component::GetType()
	{
		return type;
	}

	bool Component::IsCollapsed()
	{
		return _isCollapsed;
	}

	std::string Component::GetTypeString()
	{
		switch (type)
		{
		case ComponentTypes::T_Transform:
			return "Transform";
			break;

		case ComponentTypes::T_Sprite:
			return "Sprite";
			break;

		case ComponentTypes::T_Camera:
			return "Camera";
			break;

		case ComponentTypes::T_Script:
			return "Script";
			break;

		case ComponentTypes::T_Button:
			return "Button";
			break;

		case ComponentTypes::T_Canvas:
			return "Canvas";
			break;

		case ComponentTypes::T_Animation:
			return "Animation";
			break;

		case ComponentTypes::T_Audio:
			return "Audio";
			break;

		case ComponentTypes::T_Text:
			return "Text";
			break;

		case ComponentTypes::T_BoxCollider:
			return "BoxCollider";
			break;

		case ComponentTypes::T_CircleCollider:
			return "CircleCollider";
			break;

		case ComponentTypes::T_CompositeCollider:
			return "CompositeCollider";
			break;

		case ComponentTypes::T_RigidBody:
			return "RigidBody";
			break;

		case ComponentTypes::T_CharacterController:
			return "CharacterController";
			break;

		default:
			return "Null";
		}
	}

	void Component::SetCollapsed(bool collapsed)
	{
		_isCollapsed = collapsed;
	}

	void Component::SetActive(bool _active)
	{
		_isActive = _active;
	}

	bool Component::IsActive()
	{
		return _isActive;
	}

	std::string Component::GetData()
	{
		return "{}";
	}
}