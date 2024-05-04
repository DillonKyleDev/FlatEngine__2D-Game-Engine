#include "Component.h"
#include "FlatEngine.h"

namespace FlatEngine
{
	Component::Component()
	{
		type = Component::ComponentTypes::Null;
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

	std::shared_ptr<GameObject> Component::GetParent()
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
		type = type;
		switch (type)
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

		case ComponentTypes::Audio:
			return "Audio";
			break;

		case ComponentTypes::Text:
			return "Text";
			break;

		case ComponentTypes::BoxCollider:
			return "BoxCollider";
			break;

		case ComponentTypes::CircleCollider:
			return "CircleCollider";
			break;

		case ComponentTypes::RigidBody:
			return "RigidBody";
			break;

		case ComponentTypes::CharacterController:
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