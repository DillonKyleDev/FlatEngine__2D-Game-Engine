#include "GameObject.h"
#include "FlatEngine.h"
#include "GameScript.h"
#include "WidgetsManager.h"

#include "Transform.h"
#include "Scene.h"
#include "Sprite.h"
#include "Camera.h"
#include "Text.h"
#include "BoxCollider.h"
#include "ScriptComponent.h"
#include "Text.h"
#include "Animation.h"
#include "Audio.h"
#include "Button.h"
#include "Canvas.h"
#include "CharacterController.h"


namespace FlatEngine
{
	GameObject::GameObject(long parentID)
	{
		this->ID = GetLoadedScene()->GetNextGameObjectID();
		this->parentID = parentID;
		this->name = "New GameObject (" + std::to_string(this->ID) + ")";
		this->components = {};
		this->_isActive = true;
		this->childrenIDs = std::vector<long>();
	
		// Increment GameObjectID counter in the scene for next GameObject
		GetLoadedScene()->IncrementGameObjectID();
	}

	// Copy Constructor
	GameObject::GameObject(std::shared_ptr<GameObject> toCopy, std::vector<std::shared_ptr<GameObject>>& objectVector, long parentID)
	{
		SetParentID(parentID);
		SetID(GetNextGameObjectID());
		SetName(toCopy->GetName());
		SetActive(toCopy->IsActive());

		for (long childID : toCopy->childrenIDs)
		{
			std::shared_ptr<GameObject> childToCopy = GetObjectById(childID);
			// Probably not assigning children right here. Original component with 1 child endsd up with 2 somehow
			std::shared_ptr<GameObject> newChild = std::make_shared<GameObject>(childToCopy, objectVector, GetID());
			objectVector.push_back(newChild);
			AddChild(newChild->GetID());
			std::vector<long> childrenIDs = childToCopy->GetChildren();

			for (long gChildID : childrenIDs) {
				RecursiveChildrenCopy(newChild, objectVector, gChildID);
			}
		}
		
		for (std::shared_ptr<Component> component : toCopy->GetComponents())
		{
			if (component->GetTypeString() == "Transform")
			{
				std::shared_ptr<Transform> newComponent = std::make_shared<Transform>(std::static_pointer_cast<Transform>(component), GetID());
				this->components.push_back(newComponent);
			}
			if (component->GetTypeString() == "Sprite")
			{
				std::shared_ptr<Sprite> newComponent = std::make_shared<Sprite>(std::static_pointer_cast<Sprite>(component), GetID());
				this->components.push_back(newComponent);
			}
			if (component->GetTypeString() == "Animation")
			{
				std::shared_ptr<Animation> newComponent = std::make_shared<Animation>(std::static_pointer_cast<Animation>(component), GetID());
				this->components.push_back(newComponent);
			}
			if (component->GetTypeString() == "Audio")
			{
				std::shared_ptr<Audio> newComponent = std::make_shared<Audio>(std::static_pointer_cast<Audio>(component), GetID());
				this->components.push_back(newComponent);
			}
			if (component->GetTypeString() == "Button")
			{
				std::shared_ptr<Button> newComponent = std::make_shared<Button>(std::static_pointer_cast<Button>(component), GetID());
				this->components.push_back(newComponent);
			}
			if (component->GetTypeString() == "BoxCollider")
			{
				std::shared_ptr<BoxCollider> newComponent = std::make_shared<BoxCollider>(std::static_pointer_cast<BoxCollider>(component), GetID());
				this->components.push_back(newComponent);
			}
			if (component->GetTypeString() == "Camera")
			{
				std::shared_ptr<Camera> newComponent = std::make_shared<Camera>(std::static_pointer_cast<Camera>(component), GetID());
				this->components.push_back(newComponent);
			}
			if (component->GetTypeString() == "Canvas")
			{
				std::shared_ptr<Canvas> newComponent = std::make_shared<Canvas>(std::static_pointer_cast<Canvas>(component), GetID());
				this->components.push_back(newComponent);
			}
			if (component->GetTypeString() == "Script")
			{
				std::shared_ptr<ScriptComponent> newComponent = std::make_shared<ScriptComponent>(std::static_pointer_cast<ScriptComponent>(component), GetID());
				this->components.push_back(newComponent);
			}
			if (component->GetTypeString() == "Text")
			{
				std::shared_ptr<Text> newComponent = std::make_shared<Text>(std::static_pointer_cast<Text>(component), GetID());
				this->components.push_back(newComponent);
			}
			if (component->GetTypeString() == "CharacterController")
			{
				std::shared_ptr<CharacterController> newComponent = std::make_shared<CharacterController>(std::static_pointer_cast<CharacterController>(component), GetID());
				this->components.push_back(newComponent);
			}
		}
	}

	void GameObject::RecursiveChildrenCopy(std::shared_ptr<GameObject> &parent, std::vector<std::shared_ptr<GameObject>> &objectVector, long childID)
	{
		std::shared_ptr<GameObject> childToCopy = GetObjectById(childID);
		std::shared_ptr<GameObject> newChild = std::make_shared<GameObject>(childToCopy, objectVector, parent->GetID());
		objectVector.push_back(newChild);
		parent->AddChild(newChild->GetID());
		std::vector<long> childrenIDs = childToCopy->GetChildren();

		for (long gChildID : childrenIDs)
		{
			RecursiveChildrenCopy(newChild, objectVector, gChildID);
		}


		for (std::shared_ptr<Component> component : parent->GetComponents())
		{
			if (component->GetTypeString() == "Transform")
			{
				std::shared_ptr<Transform> newComponent = std::make_shared<Transform>(std::static_pointer_cast<Transform>(component), GetID());
				parent->components.push_back(newComponent);
			}
			if (component->GetTypeString() == "Sprite")
			{
				std::shared_ptr<Sprite> newComponent = std::make_shared<Sprite>(std::static_pointer_cast<Sprite>(component), GetID());
				parent->components.push_back(newComponent);
			}
			if (component->GetTypeString() == "Animation")
			{
				std::shared_ptr<Animation> newComponent = std::make_shared<Animation>(std::static_pointer_cast<Animation>(component), GetID());
				parent->components.push_back(newComponent);
			}
			if (component->GetTypeString() == "Audio")
			{
				std::shared_ptr<Audio> newComponent = std::make_shared<Audio>(std::static_pointer_cast<Audio>(component), GetID());
				parent->components.push_back(newComponent);
			}
			if (component->GetTypeString() == "Button")
			{
				std::shared_ptr<Button> newComponent = std::make_shared<Button>(std::static_pointer_cast<Button>(component), GetID());
				parent->components.push_back(newComponent);
			}
			if (component->GetTypeString() == "BoxCollider")
			{
				std::shared_ptr<BoxCollider> newComponent = std::make_shared<BoxCollider>(std::static_pointer_cast<BoxCollider>(component), GetID());
				parent->components.push_back(newComponent);
			}
			if (component->GetTypeString() == "Camera")
			{
				std::shared_ptr<Camera> newComponent = std::make_shared<Camera>(std::static_pointer_cast<Camera>(component), GetID());
				parent->components.push_back(newComponent);
			}
			if (component->GetTypeString() == "Canvas")
			{
				std::shared_ptr<Canvas> newComponent = std::make_shared<Canvas>(std::static_pointer_cast<Canvas>(component), GetID());
				parent->components.push_back(newComponent);
			}
			if (component->GetTypeString() == "Script")
			{
				std::shared_ptr<ScriptComponent> newComponent = std::make_shared<ScriptComponent>(std::static_pointer_cast<ScriptComponent>(component), GetID());
				parent->components.push_back(newComponent);
			}
			if (component->GetTypeString() == "Text")
			{
				std::shared_ptr<Text> newComponent = std::make_shared<Text>(std::static_pointer_cast<Text>(component), GetID());
				parent->components.push_back(newComponent);
			}
			if (component->GetTypeString() == "CharacterController")
			{
				std::shared_ptr<CharacterController> newComponent = std::make_shared<CharacterController>(std::static_pointer_cast<CharacterController>(component), GetID());
				this->components.push_back(newComponent);
			}
		}
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
		std::shared_ptr <Transform> transformComponent;
		std::shared_ptr <Sprite> spriteComponent;
		std::shared_ptr <Camera> cameraComponent;
		std::shared_ptr <ScriptComponent> scriptComponent;
		std::shared_ptr <Button> buttonComponent;
		std::shared_ptr<Canvas> canvasComponent;
		std::shared_ptr<Animation> animationComponent;
		std::shared_ptr<Audio> audioComponent;
		std::shared_ptr<Text> textComponent;
		std::shared_ptr<CharacterController> characterControllerComponent;

		// Get next Component ID from the scene
		std::shared_ptr<Scene> scene = GetLoadedScene();
		long nextID = scene->GetNextComponentID();

		switch (type)
		{
		case ComponentTypes::Transform:
			transformComponent = std::make_shared<Transform>(nextID, this->ID);
			scene->IncrementComponentID();
			this->components.push_back(transformComponent);
			return transformComponent;
			break;

		case ComponentTypes::Sprite:
			spriteComponent = std::make_shared < Sprite>(nextID, this->ID);
			this->components.push_back(spriteComponent);
			scene->IncrementComponentID();
			return spriteComponent;
			break;

		case ComponentTypes::Camera:
			cameraComponent = std::make_shared < Camera>(nextID, this->ID);
			this->components.push_back(cameraComponent);
			scene->IncrementComponentID();
			return cameraComponent;
			break;

		case ComponentTypes::Script:
			scriptComponent = std::make_shared < ScriptComponent>(nextID, this->ID);
			this->components.push_back(scriptComponent);
			scene->IncrementComponentID();
			return scriptComponent;
			break;

		case ComponentTypes::Button:
			buttonComponent = uiManager->CreateButton(nextID, this->ID, 0);
			this->components.push_back(buttonComponent);
			scene->IncrementComponentID();
			return buttonComponent;
			break;

		case ComponentTypes::Canvas:
			canvasComponent = uiManager->CreateCanvas(nextID, this->ID, 0);
			this->components.push_back(canvasComponent);
			scene->IncrementComponentID();
			return canvasComponent;
			break;

		case ComponentTypes::Animation:
			animationComponent = std::make_shared<Animation>(nextID, this->ID);
			this->components.push_back(animationComponent);
			scene->IncrementComponentID();
			return animationComponent;
			break;

		case ComponentTypes::Audio:
			audioComponent = std::make_shared<Audio>(nextID, this->ID);
			this->components.push_back(audioComponent);
			scene->IncrementComponentID();
			return audioComponent;
			break;

		case ComponentTypes::Text:
			textComponent = std::make_shared<Text>(nextID, this->ID);
			this->components.push_back(textComponent);
			scene->IncrementComponentID();
			return textComponent;
			break;

		case ComponentTypes::CharacterController:
			characterControllerComponent = std::make_shared<CharacterController>(nextID, this->ID);
			this->components.push_back(characterControllerComponent);
			scene->IncrementComponentID();
			return characterControllerComponent;
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
							this->components[j] = this->components[static_cast<std::vector<Component*, std::allocator<Component*>>::size_type>(j)];
						else
							this->components[j] = this->components[static_cast<std::vector<Component*, std::allocator<Component*>>::size_type>(j) + 1];
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

	std::shared_ptr<Transform> GameObject::GetTransformComponent()
	{
		std::shared_ptr<Transform> transformComponent = std::static_pointer_cast<Transform>(this->GetComponent(ComponentTypes::Transform));
		return transformComponent;
	}

	std::shared_ptr<Sprite> GameObject::GetSpriteComponent()
	{
		std::shared_ptr<Sprite> spriteComponent = std::static_pointer_cast<Sprite>(this->GetComponent(ComponentTypes::Sprite));
		return spriteComponent;
	}

	std::shared_ptr<Camera> GameObject::GetCameraComponent()
	{
		std::shared_ptr<Camera> cameraComponent = std::static_pointer_cast<Camera>(this->GetComponent(ComponentTypes::Camera));
		return cameraComponent;
	}

	std::shared_ptr<Animation> GameObject::GetAnimationComponent()
	{
		std::shared_ptr<Animation> animationComponent = std::static_pointer_cast<Animation>(this->GetComponent(ComponentTypes::Animation));
		return animationComponent;
	}

	std::shared_ptr<Audio> GameObject::GetAudioComponent()
	{
		std::shared_ptr<Audio> audioComponent = std::static_pointer_cast<Audio>(this->GetComponent(ComponentTypes::Audio));
		return audioComponent;
	}

	std::shared_ptr<Button> GameObject::GetButtonComponent()
	{
		std::shared_ptr<Button> buttonComponent = std::static_pointer_cast<Button>(this->GetComponent(ComponentTypes::Button));
		return buttonComponent;
	}

	std::shared_ptr<Canvas> GameObject::GetCanvasComponent()
	{
		std::shared_ptr<Canvas> canvasComponent = std::static_pointer_cast<Canvas>(this->GetComponent(ComponentTypes::Canvas));
		return canvasComponent;
	}

	std::shared_ptr<ScriptComponent> GameObject::GetScriptComponent()
	{
		std::shared_ptr<ScriptComponent> scriptComponent = std::static_pointer_cast<ScriptComponent>(this->GetComponent(ComponentTypes::Script));
		return scriptComponent;
	}

	std::shared_ptr<Text> GameObject::GetTextComponent()
	{
		std::shared_ptr<Text> textComponent = std::static_pointer_cast<Text>(this->GetComponent(ComponentTypes::Text));
		return textComponent;
	}

	std::shared_ptr<CharacterController> GameObject::GetCharacterController()
	{
		std::shared_ptr<CharacterController> characterController = std::static_pointer_cast<CharacterController>(this->GetComponent(ComponentTypes::CharacterController));
		return characterController;
	}

	std::shared_ptr<GameScript> GameObject::GetGameScriptByName(std::string scriptName)
	{
		for (int i = 0; i < this->components.size(); i++)
		{
			if (this->components[i]->GetType() == ComponentTypes::Script)
			{
				std::shared_ptr<ScriptComponent> scriptComponent = std::static_pointer_cast<ScriptComponent>(this->GetComponent(ComponentTypes::Script));
				if (scriptComponent->GetAttachedScript() == scriptName)
					return scriptComponent->GetScriptInstance();
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

	std::shared_ptr<GameObject> GameObject::GetFirstChild()
	{
		return GetObjectById(this->childrenIDs[0]);
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
		std::shared_ptr<Button> button = std::static_pointer_cast<Button>(GetComponent(ComponentTypes::Button));
		if (button != nullptr)
			button->SetActive(_isActive);

		for (long child : GetChildren())
		{
			GetObjectById(child)->SetActive(_isActive);
		}
	}

	bool GameObject::IsActive()
	{
		return this->_isActive;
	}
}