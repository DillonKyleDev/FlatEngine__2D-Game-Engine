#include "GameObject.h"
#include "FlatEngine.h"
#include "Transform.h"
#include "Sprite.h"
#include "Camera.h"
#include "Button.h"


namespace FlatEngine
{
	GameObject::GameObject(GameObject* parent)
	{
		this->ID = FlatEngine::sceneManager->GetLoadedScene()->GetNextGameObjectID();
		this->name = "New GameObject (" + std::to_string(this->ID) + ")";
		this->parent = parent;
		this->components = {};

		FlatEngine::sceneManager->GetLoadedScene()->IncrementGameObjectID();
	}


	GameObject::~GameObject()
	{
		// Delete all pointers
		for (int i = 0; i < this->components.size(); i++)
		{
			if (this->components[i] != nullptr)
			{
				delete this->components[i];
				this->components[i] = nullptr;
			}
		}
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
		FlatEngine::Transform* transformComponent = nullptr;
		FlatEngine::Sprite* spriteComponent = nullptr;
		FlatEngine::Camera* cameraComponent = nullptr;
		FlatEngine::ScriptComponent* scriptComponent = nullptr;
		FlatEngine::Button* buttonComponent = nullptr;

		// Get next Component ID from the scene
		Scene* scene = FlatEngine::GetLoadedScene();
		long nextID = scene->GetNextComponentID();
		// Increment the Component ID after assigning it to a component

		switch (type)
		{
		case Component::ComponentTypes::Transform:
			transformComponent = new FlatEngine::Transform(nextID, this->ID);
			scene->IncrementComponentID();
			this->components.push_back(transformComponent);
			return transformComponent;
			break;

		case Component::ComponentTypes::Sprite:
			spriteComponent = new FlatEngine::Sprite(nextID, this->ID);
			this->components.push_back(spriteComponent);
			scene->IncrementComponentID();
			return spriteComponent;
			break;

		case Component::ComponentTypes::Camera:
			cameraComponent = new FlatEngine::Camera(nextID, this->ID);
			this->components.push_back(cameraComponent);
			scene->IncrementComponentID();
			return cameraComponent;
			break;

		case Component::ComponentTypes::Script:
			scriptComponent = new FlatEngine::ScriptComponent(nextID, this->ID);
			this->components.push_back(scriptComponent);
			scene->IncrementComponentID();
			return scriptComponent;
			break;

		case Component::ComponentTypes::Button:
			buttonComponent = new FlatEngine::Button(nextID, this->ID);
			this->components.push_back(buttonComponent);
			scene->IncrementComponentID();
			return buttonComponent;
			break;

		default:
			return nullptr;
			break;
		}
	}


	void GameObject::RemoveComponent(Component* component)
	{
		for (int i = 0; i < this->components.size(); i++)
		{
			if (this->components[i] == component)
			{
				delete this->components[i];
				this->components[i] = nullptr;
			}
		}
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
