#include "Component.h"
#include "FlatEngine.h"

namespace FlatEngine
{
	Component::Component()
	{
		type = T_Null;
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

	ComponentTypes Component::GetType()
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
		case T_Transform:
			return "Transform";
			break;

		case T_Sprite:
			return "Sprite";
			break;

		case T_Camera:
			return "Camera";
			break;

		case T_Script:
			return "Script";
			break;

		case T_Button:
			return "Button";
			break;

		case T_Canvas:
			return "Canvas";
			break;

		case T_Animation:
			return "Animation";
			break;

		case T_Audio:
			return "Audio";
			break;

		case T_Text:
			return "Text";
			break;

		case T_BoxCollider:
			return "BoxCollider";
			break;

		case T_CircleCollider:
			return "CircleCollider";
			break;

		case T_CompositeCollider:
			return "CompositeCollider";
			break;

		case T_RigidBody:
			return "RigidBody";
			break;

		case T_CharacterController:
			return "CharacterController";
			break;

		case T_TileMap:
			return "TileMap";
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