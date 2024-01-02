#include "GameObject.h"
#include "FlatEngine.h"



namespace FlatEngine
{
	GameObject::GameObject(GameObject* parent)
	{
		this->ID = FlatEngine::sceneManager->GetLoadedScene()->GetCurrentID();
		this->name = "New GameObject (" + std::to_string(this->ID) + ")";
		this->parent = parent;
		this->components = {};

		FlatEngine::sceneManager->GetLoadedScene()->IncrementID();
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


	Component* GameObject::AddComponent(Component::ComponentTypes type)
	{
		// Look into making these smart pointers so they delete automatically
		FlatEngine::Transform *transformComponent = new FlatEngine::Transform();
		FlatEngine::Sprite *spriteComponent = new FlatEngine::Sprite();

		switch (type)
		{
		case Component::ComponentTypes::Transform:
			this->components.push_back(transformComponent);
			return transformComponent;
			break;

		case Component::ComponentTypes::Sprite:
			this->components.push_back(spriteComponent);
			return spriteComponent;
			break;
		default:
			return nullptr;
			break;
		}
	}


	void GameObject::RemoveComponent(Component* component)
	{

	}


	Component* GameObject::GetComponent(Component::ComponentTypes type)
	{
		for (int i = 0; i < this->components.size(); i++)
		{
			if (this->components[i]->GetType() == type)
			{
				return this->components[i];
			}
		}

		return nullptr;
	}


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

	bool GameObject::HasChildren()
	{
		return this->children.size() > 0;
	}
}
