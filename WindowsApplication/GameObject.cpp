#include "GameObject.h"
#include "GameObjectManager.h"

GameObject::GameObject(GameObject* parent)
{
	this->ID = GameObjectManager::currentID;
	name = "New GameObject";
	this->parent = parent;
	this->components = {};

	GameObjectManager::IncrementID();
}


GameObject::~GameObject()
{

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