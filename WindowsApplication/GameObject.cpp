#include "GameObject.h"
#include "FlatEngine.h"
#include "Transform.h"
#include "Sprite.h"
#include "Camera.h"
#include "Button.h"


namespace FlatEngine
{
	GameObject::GameObject(long parentID)
	{
		this->ID = FlatEngine::sceneManager->GetLoadedScene()->GetNextGameObjectID();
		this->parentID = parentID;
		this->name = "New GameObject (" + std::to_string(this->ID) + ")";
		this->components = {};
		
		// Increment GameObjectID counter in the scene for next GameObject
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


	void GameObject::SetID(long ID)
	{
		this->ID = ID;
	}


	long GameObject::GetID()
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


	void GameObject::RemoveComponent(long componentID)
	{
		for (int i = 0; i < this->components.size(); i++)
		{
			if (this->components[i]->GetID() == componentID)
			{
				if (this->components.size() > 1)
				{
					for (int j = i; j < components.size(); j++)
					{
						if (j == components.size() - 1)
							this->components[j] = this->components[static_cast<std::vector<FlatEngine::Component*, std::allocator<FlatEngine::Component*>>::size_type>(j)];
						else
							this->components[j] = this->components[static_cast<std::vector<FlatEngine::Component*, std::allocator<FlatEngine::Component*>>::size_type>(j) + 1];
					}
					this->components.erase(this->components.begin() + this->components.size() - 1);
				}
				else
				{
					delete this->components[i];
					this->components[i] = nullptr;
					this->components.erase(this->components.begin());
				}				
				break;
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


	void GameObject::SetParentID(long parentID)
	{
		this->parentID = parentID;
	}


	long GameObject::GetParentID()
	{
		return this->parentID;
	}


	void GameObject::AddChild(long childID)
	{
		if (childID != -1)
		{
			this->childrenIDs.push_back(childID);
		}
	}


	void GameObject::RemoveChild(long childID)
	{
		for (int i = 0; i < this->childrenIDs.size(); i++)
		{
			if (this->childrenIDs[i] == childID)
			{
				this->childrenIDs.erase(childrenIDs.begin() + i);
			}
		}
	}


	std::vector<long> GameObject::GetChildren()
	{
		return this->childrenIDs;
	}


	bool GameObject::HasChildren()
	{
		return this->childrenIDs.size() > 0;
	}
}
