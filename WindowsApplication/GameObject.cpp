#include "GameObject.h"
#include "FlatEngine.h"


GameObject::GameObject(GameObject* parent)
{
	this->ID = FlatEngine::gameObjectManager.GetCurrentID();
	//this->name = "New GameObject (" + std::to_string(this->ID) + ")";
	this->name = "New";
	this->parent = parent;
	this->components = {};
	this->AddComponent(Component::ComponentTypes::Transform);

	FlatEngine::gameObjectManager.IncrementID();
}


GameObject::~GameObject()
{

}


int GameObject::GetID()
{
	return this->ID;
}


void GameObject::SetName(std::string name)
{
	this->name = name;
}

std::string GameObject::GetName()
{
	return name;
}


void GameObject::AddComponent(Component::ComponentTypes type)
{
	Transform transformComponent;
	Sprite spriteComponent;

	switch (type)
	{
	case Component::ComponentTypes::Transform:
		transformComponent = Transform();

		this->components.push_back(transformComponent);
		break;

	case Component::ComponentTypes::Sprite:
		spriteComponent = Sprite();

		this->components.push_back(spriteComponent);
		spriteComponent = spriteComponent;
		break;
	default:
		break;
	}
}


void GameObject::RemoveComponent(Component component)
{

}


Component GameObject::GetComponent(Component::ComponentTypes type)
{
	Component nullComponent;
	nullComponent.SetType(Component::ComponentTypes::Null);

	for (int i = 0; i < this->components.size(); i++)
	{
		if (this->components[i].GetType() == type)
		{
			return this->components[i];
		}
	}

	return nullComponent;
}


std::vector<Component> GameObject::GetComponents()
{
	return this->components;
}


void GameObject::SetParent(GameObject* parent)
{
	if (parent != nullptr)
	{
		this->parent = parent;
	}
}


GameObject* GameObject::GetParent()
{
	return this->parent;
}


void GameObject::AddChild(GameObject* child)
{
	if (child != nullptr)
	{
		this->children.push_back(child);
	}
}


void GameObject::RemoveChild(GameObject child)
{

}


std::vector<GameObject*> GameObject::GetChildren()
{
	return this->children;
}