#include "GameObject.h"
#include "FlatEngine.h"
#include "Transform.h"
#include "Sprite.h"
#include "Camera.h"
#include "Button.h"
#include "Canvas.h"
#include "Animation.h"
#include "Audio.h"
#include "Text.h"
#include "WidgetsManager.h"


namespace FlatEngine
{
	GameObject::GameObject(long parentID)
	{
		this->ID = FlatEngine::sceneManager->GetLoadedScene()->GetNextGameObjectID();
		this->parentID = parentID;
		this->name = "New GameObject (" + std::to_string(this->ID) + ")";
		this->components = {};
		this->_isActive = true;
		this->childrenIDs = std::vector<long>();
		
		// Increment GameObjectID counter in the scene for next GameObject
		FlatEngine::sceneManager->GetLoadedScene()->IncrementGameObjectID();
	}

	GameObject::~GameObject()
	{
		// Reset all pointers
		for (int i = 0; i < this->components.size(); i++)
		{
			if (this->components[i] != nullptr)
			{
				this->components[i] = nullptr;
			}
		}
	}

	// Copy Constructor
	GameObject::GameObject(GameObject& original)
	{
		FlatEngine::LogString("Name: " + original.GetName());
		this->ID = original.GetID();
		this->parentID = original.GetParentID();
		this->name = original.GetName();
		this->components = original.GetComponents();
		this->_isActive = original.IsActive();
		this->childrenIDs = original.childrenIDs;
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


	std::shared_ptr<Component> GameObject::AddComponent(Component::ComponentTypes type)
	{
		// Look into making these smart pointers so they delete automatically
		std::shared_ptr <FlatEngine::Transform> transformComponent;
		std::shared_ptr <FlatEngine::Sprite> spriteComponent;
		std::shared_ptr <FlatEngine::Camera> cameraComponent;
		std::shared_ptr <FlatEngine::ScriptComponent> scriptComponent;
		std::shared_ptr <FlatEngine::Button> buttonComponent;
		std::shared_ptr<FlatEngine::Canvas> canvasComponent;
		std::shared_ptr<FlatEngine::Animation> animationComponent;
		std::shared_ptr<FlatEngine::Audio> audioComponent;
		std::shared_ptr<FlatEngine::Text> textComponent;

		// Get next Component ID from the scene
		std::shared_ptr<Scene> scene = FlatEngine::GetLoadedScene();
		long nextID = scene->GetNextComponentID();

		switch (type)
		{
		case ComponentTypes::Transform:
			transformComponent = std::make_shared<FlatEngine::Transform>(nextID, this->ID);
			scene->IncrementComponentID();
			this->components.push_back(transformComponent);
			return transformComponent;
			break;

		case ComponentTypes::Sprite:
			spriteComponent = std::make_shared < FlatEngine::Sprite>(nextID, this->ID);
			this->components.push_back(spriteComponent);
			scene->IncrementComponentID();
			return spriteComponent;
			break;

		case ComponentTypes::Camera:
			cameraComponent = std::make_shared < FlatEngine::Camera>(nextID, this->ID);
			this->components.push_back(cameraComponent);
			scene->IncrementComponentID();
			return cameraComponent;
			break;

		case ComponentTypes::Script:
			scriptComponent = std::make_shared < FlatEngine::ScriptComponent>(nextID, this->ID);
			this->components.push_back(scriptComponent);
			scene->IncrementComponentID();
			return scriptComponent;
			break;

		case ComponentTypes::Button:
			buttonComponent = FlatEngine::uiManager->CreateButton(nextID, this->ID, 0);
			this->components.push_back(buttonComponent);
			scene->IncrementComponentID();
			return buttonComponent;
			break;

		case ComponentTypes::Canvas:
			canvasComponent = FlatEngine::uiManager->CreateCanvas(nextID, this->ID, 0);
			this->components.push_back(canvasComponent);
			scene->IncrementComponentID();
			return canvasComponent;
			break;

		case ComponentTypes::Animation:
			animationComponent = std::make_shared<FlatEngine::Animation>(nextID, this->ID);
			this->components.push_back(animationComponent);
			scene->IncrementComponentID();
			return animationComponent;
			break;

		case ComponentTypes::Audio:
			audioComponent = std::make_shared<FlatEngine::Audio>(nextID, this->ID);
			this->components.push_back(audioComponent);
			scene->IncrementComponentID();
			return audioComponent;
			break;

		case ComponentTypes::Text:
			textComponent = std::make_shared<FlatEngine::Text>(nextID, this->ID);
			this->components.push_back(textComponent);
			scene->IncrementComponentID();
			return textComponent;
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
					this->components.erase(this->components.begin());
				}				
				break;
			}
		}
	}


	std::shared_ptr<Component> GameObject::GetComponent(Component::ComponentTypes type)
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


	std::vector<std::shared_ptr<Component>> GameObject::GetComponentsOfType(Component::ComponentTypes type)
	{
		std::vector<std::shared_ptr<Component>> componentsOfType = std::vector<std::shared_ptr<Component>>();

		for (int i = 0; i < this->components.size(); i++)
		{
			if (this->components[i]->GetType() == type)
			{
				componentsOfType.push_back(this->components[i]);
				
			}
		}

		return componentsOfType;
	}


	std::vector<std::shared_ptr<Component>> &GameObject::GetComponents()
	{
		return this->components;
	}

	std::vector<std::shared_ptr<Component>> GameObject::GetScriptInstances()
	{
		std::vector<std::shared_ptr<Component>> scriptComponents = this->GetComponentsOfType(ComponentTypes::Script);
		std::vector<std::shared_ptr<Component>> gameScripts;

		for (std::shared_ptr<Component> script : scriptComponents)
			gameScripts.push_back(script);
		
		return gameScripts;
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

	void GameObject::SetActive(bool _isActive)
	{
		this->_isActive = _isActive;
	}

	bool GameObject::IsActive()
	{
		return this->_isActive;
	}
}
