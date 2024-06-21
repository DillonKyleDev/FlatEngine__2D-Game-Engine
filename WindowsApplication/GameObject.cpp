#include "GameObject.h"
#include "FlatEngine.h"
#include "GameScript.h"
#include "WidgetsManager.h"

#include "TagList.h"
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
#include "BoxCollider.h"
#include "CircleCollider.h"
#include "CompositeCollider.h"


namespace FlatEngine
{
	GameObject::GameObject(long newParentID, long myID)
	{
		if (myID != -1)
			ID = myID;
		else
		{
			ID = GetLoadedScene()->GetNextGameObjectID();
		}
	
		_isPrefab = false;
		prefabName = "";
		prefabSpawnLocation = Vector2(0, 0);
		tagList = std::make_shared<TagList>();
		parentID = newParentID;
		name = "GameObject(" + std::to_string(ID) + ")";
		components = std::vector<std::shared_ptr<Component>>();
		_isActive = true;
		childrenIDs = std::vector<long>();
	}

	// Copy Constructor
	GameObject::GameObject(std::shared_ptr<GameObject> toCopy, std::vector<std::shared_ptr<GameObject>>& childObjectVector, std::vector<std::shared_ptr<GameObject>> objectPool, long parentID, long ID)
	{
		_isPrefab = toCopy->_isPrefab;
		prefabName = toCopy->prefabName;
		prefabSpawnLocation = toCopy->prefabSpawnLocation;
		tagList = toCopy->GetTagList();
		SetParentID(parentID);
		if (ID != -1)
			SetID(ID);
		else
			SetID(GetNextGameObjectID());
		SetName(toCopy->GetName());
		SetActive(toCopy->IsActive());

		for (long childID : toCopy->childrenIDs)
		{
			std::shared_ptr<GameObject> actualChild = nullptr;

			// Find child in object pool provided
			if (childID != -1)
			{
				for (std::shared_ptr<GameObject> object : objectPool)
				{
					if (childID == object->GetID())
					{
						actualChild = object;
					}
				}
			}
				
			if (actualChild != nullptr)
			{
				std::shared_ptr<GameObject> childCopy = std::make_shared<GameObject>(actualChild, childObjectVector, objectPool, GetID());
				childObjectVector.push_back(childCopy);
				AddChild(childCopy->GetID());
			}
		}
		
		std::vector<std::shared_ptr<Component>> toCopyComponents = toCopy->GetComponents();
		for (std::shared_ptr<Component> component : toCopyComponents)
		{
			if (component->GetTypeString() == "Transform")
			{
				std::shared_ptr<Transform> newComponent = std::make_shared<Transform>(std::static_pointer_cast<Transform>(component), GetID());
				if (parentID != -1 && GetObjectById(parentID) != nullptr)
				{
					newComponent->SetOrigin(GetObjectById(parentID)->GetTransformComponent()->GetTruePosition());
				}
				components.push_back(newComponent);
			}
			else if (component->GetTypeString() == "Sprite")
			{
				std::shared_ptr<Sprite> newComponent = std::make_shared<Sprite>(std::static_pointer_cast<Sprite>(component), GetID());
				components.push_back(newComponent);
			}
			else if (component->GetTypeString() == "Animation")
			{
				std::shared_ptr<Animation> newComponent = std::make_shared<Animation>(std::static_pointer_cast<Animation>(component), GetID());
				components.push_back(newComponent);
			}
			else if (component->GetTypeString() == "Audio")
			{
				std::shared_ptr<Audio> newComponent = std::make_shared<Audio>(std::static_pointer_cast<Audio>(component), GetID());
				components.push_back(newComponent);
			}
			else if (component->GetTypeString() == "Button")
			{
				std::shared_ptr<Button> newComponent = std::make_shared<Button>(std::static_pointer_cast<Button>(component), GetID());
				components.push_back(newComponent);
			}
			else if (component->GetTypeString() == "BoxCollider")
			{
				std::shared_ptr<BoxCollider> newComponent = std::make_shared<BoxCollider>(std::static_pointer_cast<BoxCollider>(component), GetID());
				components.push_back(newComponent);
			}
			else if (component->GetTypeString() == "Camera")
			{
				std::shared_ptr<Camera> newComponent = std::make_shared<Camera>(std::static_pointer_cast<Camera>(component), GetID());
				components.push_back(newComponent);
			}
			else if (component->GetTypeString() == "Canvas")
			{
				std::shared_ptr<Canvas> newComponent = std::make_shared<Canvas>(std::static_pointer_cast<Canvas>(component), GetID());
				components.push_back(newComponent);
			}
			else if (component->GetTypeString() == "Script")
			{
				std::shared_ptr<ScriptComponent> newComponent = std::make_shared<ScriptComponent>(std::static_pointer_cast<ScriptComponent>(component), GetID());
				components.push_back(newComponent);
			}
			else if (component->GetTypeString() == "Text")
			{
				std::shared_ptr<Text> newComponent = std::make_shared<Text>(std::static_pointer_cast<Text>(component), GetID());
				components.push_back(newComponent);
			}
			else if (component->GetTypeString() == "CharacterController")
			{
				std::shared_ptr<CharacterController> newComponent = std::make_shared<CharacterController>(std::static_pointer_cast<CharacterController>(component), GetID());
				components.push_back(newComponent);
			}
			else if (component->GetTypeString() == "RigidBody")
			{
				std::shared_ptr<RigidBody> newComponent = std::make_shared<RigidBody>(std::static_pointer_cast<RigidBody>(component), GetID());
				components.push_back(newComponent);
			}
			else if (component->GetTypeString() == "BoxCollider")
			{
				std::shared_ptr<BoxCollider> newComponent = std::make_shared<BoxCollider>(std::static_pointer_cast<BoxCollider>(component), GetID());
				components.push_back(newComponent);
			}
			else if (component->GetTypeString() == "CircleCollider")
			{
				std::shared_ptr<CircleCollider> newComponent = std::make_shared<CircleCollider>(std::static_pointer_cast<CircleCollider>(component), GetID());
				components.push_back(newComponent);
			}
		}
	}

	GameObject::~GameObject()
	{
		// Reset all pointers
		for (int i = 0; i < components.size(); i++)
		{
			if (components[i] != nullptr)
			{
				components[i] = nullptr;
			}
		}
	}

	void GameObject::SetIsPrefab(bool _newIsPrefab)
	{
		_isPrefab = _newIsPrefab;
	}

	bool GameObject::IsPrefab()
	{
		return _isPrefab;
	}

	void GameObject::SetPrefabName(std::string newPrefabName)
	{
		prefabName = newPrefabName;
	}

	std::string GameObject::GetPrefabName()
	{
		return prefabName;
	}

	void GameObject::SetPrefabSpawnLocation(Vector2 newSpawnLocation)
	{
		prefabSpawnLocation = newSpawnLocation;
	}

	Vector2 GameObject::GetPrefabSpawnLocation()
	{
		return prefabSpawnLocation;
	}

	void GameObject::SetID(long newID)
	{
		ID = newID;
	}

	long GameObject::GetID()
	{
		return ID;
	}

	void GameObject::SetName(std::string newName)
	{
		name = newName;
	}

	std::string GameObject::GetName()
	{
		return name;
	}

	std::shared_ptr<TagList> GameObject::GetTagList()
	{
		return tagList;
	}

	void GameObject::SetTagList(std::shared_ptr<TagList> newTagList)
	{
		tagList = newTagList;
	}

	bool GameObject::HasTag(std::string tagName)
	{
		return tagList->HasTag(tagName);
	}

	void GameObject::SetTag(std::string tagName, bool _value)
	{
		tagList->SetTag(tagName, _value);
	}

	std::shared_ptr<Component> GameObject::AddComponent(Component::ComponentTypes type)
	{
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
		std::shared_ptr<RigidBody> rigidBodyComponent;
		std::shared_ptr<BoxCollider> boxColliderComponent;
		std::shared_ptr<CompositeCollider> compositeColliderComponent;
		std::shared_ptr<CircleCollider> circleColliderComponent;

		std::shared_ptr<GameObject> parent;

		// Get next Component ID from the scene
		std::shared_ptr<Scene> scene = GetLoadedScene();
		long nextID = scene->GetNextComponentID();

		switch (type)
		{
		case ComponentTypes::Transform:
			transformComponent = std::make_shared<Transform>(nextID, ID);
			scene->IncrementComponentID();
			components.push_back(transformComponent);

			// Set transforms origin to parents true position
			if (parentID != -1)
			{
				parent = GetObjectById(parentID);
				if (parent != nullptr && parent->HasComponent("Transform"))
				{
					std::shared_ptr <Transform> parentTransform = parent->GetTransformComponent();
					Vector2 parentTruePosition = parentTransform->GetTruePosition();
					transformComponent->SetOrigin(parentTruePosition);
				}
			}

			return transformComponent;
			break;

		case ComponentTypes::Sprite:
			spriteComponent = std::make_shared < Sprite>(nextID, ID);
			components.push_back(spriteComponent);
			scene->IncrementComponentID();
			return spriteComponent;
			break;

		case ComponentTypes::Camera:
			cameraComponent = std::make_shared < Camera>(nextID, ID);
			components.push_back(cameraComponent);
			scene->IncrementComponentID();
			return cameraComponent;
			break;

		case ComponentTypes::Script:
			scriptComponent = std::make_shared < ScriptComponent>(nextID, ID);
			components.push_back(scriptComponent);
			scene->IncrementComponentID();
			return scriptComponent;
			break;

		case ComponentTypes::Button:
			buttonComponent = uiManager->CreateButton(nextID, ID, 0);
			components.push_back(buttonComponent);
			scene->IncrementComponentID();
			return buttonComponent;
			break;

		case ComponentTypes::Canvas:
			canvasComponent = uiManager->CreateCanvas(nextID, ID, 0);
			components.push_back(canvasComponent);
			scene->IncrementComponentID();
			return canvasComponent;
			break;

		case ComponentTypes::Animation:
			animationComponent = std::make_shared<Animation>(nextID, ID);
			components.push_back(animationComponent);
			scene->IncrementComponentID();
			return animationComponent;
			break;

		case ComponentTypes::Audio:
			audioComponent = std::make_shared<Audio>(nextID, ID);
			components.push_back(audioComponent);
			scene->IncrementComponentID();
			return audioComponent;
			break;

		case ComponentTypes::Text:
			textComponent = std::make_shared<Text>(nextID, ID);
			components.push_back(textComponent);
			scene->IncrementComponentID();
			return textComponent;
			break;

		case ComponentTypes::CharacterController:
			characterControllerComponent = std::make_shared<CharacterController>(nextID, ID);
			components.push_back(characterControllerComponent);
			scene->IncrementComponentID();
			return characterControllerComponent;
			break;

		case ComponentTypes::RigidBody:
			rigidBodyComponent = std::make_shared<RigidBody>(nextID, ID);
			components.push_back(rigidBodyComponent);
			scene->IncrementComponentID();
			return rigidBodyComponent;
			break;


		case ComponentTypes::BoxCollider:
			boxColliderComponent = std::make_shared<BoxCollider>(nextID, ID);
			components.push_back(boxColliderComponent);
			scene->IncrementComponentID();
			return boxColliderComponent;
			break;

		case ComponentTypes::CircleCollider:
			circleColliderComponent = std::make_shared<CircleCollider>(nextID, ID);
			components.push_back(circleColliderComponent);
			scene->IncrementComponentID();
			return circleColliderComponent;
			break;

		case ComponentTypes::CompositeCollider:
			compositeColliderComponent = std::make_shared<CompositeCollider>(nextID, ID);
			components.push_back(compositeColliderComponent);
			scene->IncrementComponentID();
			return compositeColliderComponent;
			break;

		default:
			return nullptr;
			break;
		}
	}

	void GameObject::RemoveComponent(long componentID)
	{
		for (int i = 0; i < components.size(); i++)
		{
			if (components[i]->GetID() == componentID)
			{
				if (components.size() > 1)
				{
					for (int j = i; j < components.size(); j++)
					{
						if (j == components.size() - 1)
							components[j] = components[static_cast<std::vector<Component*, std::allocator<Component*>>::size_type>(j)];
						else
							components[j] = components[static_cast<std::vector<Component*, std::allocator<Component*>>::size_type>(j) + 1];
					}
					components.erase(components.begin() + components.size() - 1);
				}
				else
				{
					components.erase(components.begin());
				}				
				break;
			}
		}
	}

	void GameObject::DeleteComponents()
	{
		for (std::shared_ptr<Component> component : components)
		{
			component = nullptr;
		}

		components.clear();
	}

	std::shared_ptr<Transform> GameObject::AddTransformComponent()
	{
		return std::static_pointer_cast<Transform>(AddComponent(ComponentTypes::Transform));
	}

	std::shared_ptr<Sprite> GameObject::AddSpriteComponent()
	{
		return std::static_pointer_cast<Sprite>(AddComponent(ComponentTypes::Sprite));
	}

	std::shared_ptr<Camera> GameObject::AddCameraComponent()
	{
		return std::static_pointer_cast<Camera>(AddComponent(ComponentTypes::Camera));
	}

	std::shared_ptr<ScriptComponent> GameObject::AddScriptComponent()
	{
		return std::static_pointer_cast<ScriptComponent>(AddComponent(ComponentTypes::Script));
	}

	std::shared_ptr<Button> GameObject::AddButtonComponent()
	{
		return std::static_pointer_cast<Button>(AddComponent(ComponentTypes::Button));
	}

	std::shared_ptr<Canvas> GameObject::AddCanvasComponent()
	{
		return std::static_pointer_cast<Canvas>(AddComponent(ComponentTypes::Canvas));
	}

	std::shared_ptr<Animation> GameObject::AddAnimationComponent()
	{
		return std::static_pointer_cast<Animation>(AddComponent(ComponentTypes::Animation));
	}

	std::shared_ptr<Audio> GameObject::AddAudioComponent()
	{
		return std::static_pointer_cast<Audio>(AddComponent(ComponentTypes::Audio));
	}

	std::shared_ptr<Text> GameObject::AddTextComponent()
	{
		return std::static_pointer_cast<Text>(AddComponent(ComponentTypes::Text));
	}

	std::shared_ptr<BoxCollider> GameObject::AddBoxColliderComponent()
	{
		return std::static_pointer_cast<BoxCollider>(AddComponent(ComponentTypes::BoxCollider));
	}

	std::shared_ptr<CircleCollider> GameObject::AddCircleColliderComponent()
	{
		return std::static_pointer_cast<CircleCollider>(AddComponent(ComponentTypes::CircleCollider));
	}

	std::shared_ptr<CompositeCollider> GameObject::AddCompositeColliderComponent()
	{
		return std::static_pointer_cast<CompositeCollider>(AddComponent(ComponentTypes::CompositeCollider));
	}

	std::shared_ptr<RigidBody> GameObject::AddRigidBodyComponent()
	{
		return std::static_pointer_cast<RigidBody>(AddComponent(ComponentTypes::RigidBody));
	}

	std::shared_ptr<CharacterController> GameObject::AddCharacterControllerComponent()
	{
		return std::static_pointer_cast<CharacterController>(AddComponent(ComponentTypes::CharacterController));
	}


	std::shared_ptr<Component> GameObject::GetComponent(Component::ComponentTypes type)
	{
		for (int i = 0; i < components.size(); i++)
		{
			if (components[i]->GetType() == type)
			{
				return components[i];
			}
		}

		return nullptr;
	}

	bool GameObject::HasComponent(ComponentTypes type)
	{
		return GetComponent(type) != nullptr;
	}

	bool GameObject::HasComponent(std::string type)
	{
		for (std::shared_ptr<Component> component : components)
		{
			if (component->GetTypeString() == type)
				return true;
		}
		return false;
	}

	std::shared_ptr<Transform> GameObject::GetTransformComponent()
	{
		std::shared_ptr<Transform> transformComponent = std::static_pointer_cast<Transform>(GetComponent(ComponentTypes::Transform));
		return transformComponent;
	}

	std::shared_ptr<Sprite> GameObject::GetSpriteComponent()
	{
		std::shared_ptr<Sprite> spriteComponent = std::static_pointer_cast<Sprite>(GetComponent(ComponentTypes::Sprite));
		return spriteComponent;
	}

	std::shared_ptr<Camera> GameObject::GetCameraComponent()
	{
		std::shared_ptr<Camera> cameraComponent = std::static_pointer_cast<Camera>(GetComponent(ComponentTypes::Camera));
		return cameraComponent;
	}

	std::shared_ptr<Animation> GameObject::GetAnimationComponent()
	{
		std::shared_ptr<Animation> animationComponent = std::static_pointer_cast<Animation>(GetComponent(ComponentTypes::Animation));
		return animationComponent;
	}

	std::shared_ptr<Audio> GameObject::GetAudioComponent()
	{
		std::shared_ptr<Audio> audioComponent = std::static_pointer_cast<Audio>(GetComponent(ComponentTypes::Audio));
		return audioComponent;
	}

	std::shared_ptr<Button> GameObject::GetButtonComponent()
	{
		std::shared_ptr<Button> buttonComponent = std::static_pointer_cast<Button>(GetComponent(ComponentTypes::Button));
		return buttonComponent;
	}

	std::shared_ptr<Canvas> GameObject::GetCanvasComponent()
	{
		std::shared_ptr<Canvas> canvasComponent = std::static_pointer_cast<Canvas>(GetComponent(ComponentTypes::Canvas));
		return canvasComponent;
	}

	std::shared_ptr<ScriptComponent> GameObject::GetScriptComponent()
	{
		std::shared_ptr<ScriptComponent> scriptComponent = std::static_pointer_cast<ScriptComponent>(GetComponent(ComponentTypes::Script));
		return scriptComponent;
	}

	std::shared_ptr<Text> GameObject::GetTextComponent()
	{
		std::shared_ptr<Text> textComponent = std::static_pointer_cast<Text>(GetComponent(ComponentTypes::Text));
		return textComponent;
	}

	std::shared_ptr<CharacterController> GameObject::GetCharacterController()
	{
		std::shared_ptr<CharacterController> characterController = std::static_pointer_cast<CharacterController>(GetComponent(ComponentTypes::CharacterController));
		return characterController;
	}

	std::shared_ptr<RigidBody> GameObject::GetRigidBody()
	{
		std::shared_ptr<RigidBody> rigidBody = std::static_pointer_cast<RigidBody>(GetComponent(ComponentTypes::RigidBody));
		return rigidBody;
	}

	std::shared_ptr<BoxCollider> GameObject::GetBoxCollider()
	{
		std::shared_ptr<BoxCollider> boxCollider = std::static_pointer_cast<BoxCollider>(GetComponent(ComponentTypes::BoxCollider));
		return boxCollider;
	}

	std::vector<std::shared_ptr<BoxCollider>> GameObject::GetBoxColliders()
	{
		std::vector<std::shared_ptr<BoxCollider>> boxColliders;
		for (std::shared_ptr<Component> component : components)
			if (component->GetTypeString() == "BoxCollider")
				boxColliders.push_back(std::static_pointer_cast<BoxCollider>(component));

		return boxColliders;
	}

	std::shared_ptr<CircleCollider> GameObject::GetCircleCollider()
	{
		std::shared_ptr<CircleCollider> circleCollider = std::static_pointer_cast<CircleCollider>(GetComponent(ComponentTypes::CircleCollider));
		return circleCollider;
	}

	std::vector<std::shared_ptr<CircleCollider>> GameObject::GetCircleColliders()
	{
		std::vector<std::shared_ptr<CircleCollider>> circleColliders;
		for (std::shared_ptr<Component> component : components)
			if (component->GetTypeString() == "CircleCollider")
				circleColliders.push_back(std::static_pointer_cast<CircleCollider>(component));

		return circleColliders;
	}

	std::shared_ptr<CompositeCollider> GameObject::GetCompositeCollider()
	{
		std::shared_ptr<CompositeCollider> compositeCollider = std::static_pointer_cast<CompositeCollider>(GetComponent(ComponentTypes::CompositeCollider));
		return compositeCollider;
	}

	std::shared_ptr<GameScript> GameObject::GetGameScriptByName(std::string scriptName)
	{
		for (int i = 0; i < components.size(); i++)
		{
			if (components[i]->GetType() == ComponentTypes::Script)
			{
				std::shared_ptr<ScriptComponent> scriptComponent = std::static_pointer_cast<ScriptComponent>(components[i]);
				if (scriptComponent->GetAttachedScript() == scriptName)
					return scriptComponent->GetScriptInstance();
			}
		}

		return nullptr;
	}

	std::vector<std::shared_ptr<Component>> GameObject::GetComponentsOfType(Component::ComponentTypes type)
	{
		std::vector<std::shared_ptr<Component>> componentsOfType = std::vector<std::shared_ptr<Component>>();

		for (int i = 0; i < components.size(); i++)
		{
			if (components[i]->GetType() == type)
			{
				componentsOfType.push_back(components[i]);
				
			}
		}

		return componentsOfType;
	}

	std::vector<std::shared_ptr<Component>> &GameObject::GetComponents()
	{
		return components;
	}

	std::vector<std::shared_ptr<Component>> GameObject::GetScriptInstances()
	{
		std::vector<std::shared_ptr<Component>> scriptComponents = GetComponentsOfType(ComponentTypes::Script);
		std::vector<std::shared_ptr<Component>> gameScripts;

		for (std::shared_ptr<Component> script : scriptComponents)
			gameScripts.push_back(script);
		
		return gameScripts;
	}

	void GameObject::SetParentID(long newParentID)
	{
		parentID = newParentID;
	}

	long GameObject::GetParentID()
	{
		return parentID;
	}

	void GameObject::AddChild(long childID)
	{
		if (childID != -1)
		{
			childrenIDs.push_back(childID);
		}
	}

	void GameObject::RemoveChild(long childID)
	{
		for (int i = 0; i < childrenIDs.size(); i++)
		{
			if (childrenIDs[i] == childID)
			{
				childrenIDs.erase(childrenIDs.begin() + i);
			}
		}
	}

	std::shared_ptr<GameObject> GameObject::GetFirstChild()
	{
		return GetObjectById(childrenIDs[0]);
	}

	std::vector<long> GameObject::GetChildren()
	{
		return childrenIDs;
	}

	bool GameObject::HasChildren()
	{
		return childrenIDs.size() > 0;
	}

	void GameObject::SetActive(bool _active)
	{
		_isActive = _active;

		for (long child : GetChildren())
		{
			GetObjectById(child)->SetActive(_active);
		}
	}

	bool GameObject::IsActive()
	{
		return _isActive;
	}
	
	std::shared_ptr<GameObject> GameObject::GetParent()
	{
		return GetObjectById(parentID);
	}
}