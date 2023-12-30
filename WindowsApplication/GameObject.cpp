#include "GameObject.h"
#include "FlatEngine.h"



namespace FlatEngine
{
	GameObject::GameObject(GameObject* parent)
	{
		this->ID = FlatEngine::gameObjectManager.GetCurrentID();
		this->name = "New GameObject (" + std::to_string(this->ID) + ")";
		this->parent = parent;
		this->components = {};

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
		// Look into making these smart pointers so they delete automatically
		FlatEngine::Transform *transformComponent = new FlatEngine::Transform();
		FlatEngine::Sprite *spriteComponent = new FlatEngine::Sprite();

		switch (type)
		{
		case Component::ComponentTypes::Transform:
			this->components.push_back(transformComponent);
			break;

		case Component::ComponentTypes::Sprite:
			this->components.push_back(spriteComponent);
			break;
		default:
			break;
		}
	}


	void GameObject::RemoveComponent(Component* component)
	{

	}


	//Component GameObject::GetComponent(Component::ComponentTypes type)
	//{
	//	FlatEngine::Transform nullComponent;
	//	nullComponent.SetType(Component::ComponentTypes::Null);

	//	for (int i = 0; i < this->components.size(); i++)
	//	{
	//		if (this->components[i].GetType() == type)
	//		{
	//			return this->components[i];
	//		}
	//	}

	//	// This is returning address of a local variable. Must not do this in final implementation
	//	return nullComponent;
	//}


	std::vector<Component*> &GameObject::GetComponents()
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
}
