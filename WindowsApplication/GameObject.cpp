#include "GameObject.h"
#include "GameObjectManager.h"


GameObject::GameObject()
{
	this->ID = GameObjectManager::currentID;
	this->name = "New GameObject";
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
	return this->name;
}


void GameObject::AddComponent(Component component)
{
	this->components.push_back(component);
}


void GameObject::RemoveComponent(Component component)
{

}


Component GameObject::GetComponent()
{
	Component temp;
	return temp;
}