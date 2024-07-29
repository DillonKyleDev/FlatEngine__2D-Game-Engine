#include "GameObject.h"
#include "FlatEngine.h"
#include "UIManager.h"
#include "GameScript.h"
#include "TagList.h"
#include "Transform.h"
#include "Scene.h"
#include "Sprite.h"
#include "Camera.h"
#include "Text.h"
#include "ScriptComponent.h"
#include "Text.h"
#include "Animation.h"
#include "Audio.h"
#include "Button.h"
#include "Canvas.h"
#include "CharacterController.h"
#include "RigidBody.h"
#include "BoxCollider.h"
#include "CircleCollider.h"
#include "CompositeCollider.h"
#include "ECSManager.h"


namespace FlatEngine
{
	GameObject::GameObject(long newParentID, long myID)
	{
		_isValid = true;
		ID = myID;
		_isPrefab = false;
		prefabName = "";
		prefabSpawnLocation = Vector2(0, 0);
		m_tagList = TagList();
		parentID = newParentID;
		name = "GameObject(" + std::to_string(ID) + ")";
		components = std::vector<Component*>();
		_isActive = true;
		childrenIDs = std::vector<long>();
	}

	GameObject::GameObject(GameObject* toCopy)
	{
		if (toCopy == nullptr)
			_isValid = false;
	}

	// Copy Constructor
	GameObject::GameObject(GameObject* toCopy, std::vector<GameObject>& childObjectVector, std::vector<GameObject> objectPool, long parentID, long myID)
	{
		_isValid = toCopy->_isValid;
		_isPrefab = toCopy->_isPrefab;
		prefabName = toCopy->prefabName;
		prefabSpawnLocation = toCopy->prefabSpawnLocation;
		m_tagList = toCopy->GetTagList();
		SetParentID(parentID);
		if (myID != -1)
			SetID(myID);
		else
			SetID(GetNextGameObjectID());
		SetName(toCopy->GetName());
		SetActive(toCopy->IsActive());

		for (long childID : toCopy->childrenIDs)
		{
			GameObject* actualChild = nullptr;

			// Find child in object pool provided
			if (childID != -1)
			{
				for (GameObject object : objectPool)
				{
					if (childID == object.GetID())
					{
						actualChild = &object;
					}
				}
			}
				
			if (actualChild != nullptr)
			{
				GameObject childCopy = GameObject(actualChild, childObjectVector, objectPool, GetID());
				childObjectVector.push_back(&childCopy);
				AddChild(childCopy.GetID());
			}
		}
		
		std::vector<Component*> toCopyComponents = toCopy->GetComponents();
		for (Component* component : toCopyComponents)
		{
			if (component->GetTypeString() == "Transform")
			{
				Transform newComponent = Transform(static_cast<Transform*>(component), GetID());

				F_ECSManager.m_Transforms.push_back(newComponent);
				F_ECSManager.m_transformMap.emplace(this->ID, (long)F_ECSManager.m_Transforms.size() - 1);


				if (parentID != -1 && GetObjectById(parentID)->IsValid())
				{
					newComponent.SetOrigin(GetObjectById(parentID)->GetTransformComponent()->GetTruePosition());
				}
				newComponent.SetOrigin(Vector2(0,0));
				//components.push_back(&newComponent);
			}
			else if (component->GetTypeString() == "Sprite")
			{
				Sprite newComponent = Sprite(static_cast<Sprite*>(component), GetID());
				//components.push_back(&newComponent);
			}
			else if (component->GetTypeString() == "Animation")
			{
				Animation newComponent = Animation(static_cast<Animation*>(component), GetID());
				//components.push_back(&newComponent);
			}
			else if (component->GetTypeString() == "Audio")
			{
				Audio newComponent = Audio(static_cast<Audio*>(component), GetID());
				//components.push_back(&newComponent);
			}
			else if (component->GetTypeString() == "Button")
			{
				Button newComponent = Button(static_cast<Button*>(component), GetID());
				//components.push_back(&newComponent);
			}
			else if (component->GetTypeString() == "BoxCollider")
			{
				BoxCollider newComponent = BoxCollider(static_cast<BoxCollider*>(component), GetID());
				//components.push_back(&newComponent);
			}
			else if (component->GetTypeString() == "Camera")
			{
				Camera newComponent = Camera(static_cast<Camera*>(component), GetID());
				//components.push_back(&newComponent);
			}
			else if (component->GetTypeString() == "Canvas")
			{
				Canvas newComponent = Canvas(static_cast<Canvas*>(component), GetID());
				//components.push_back(&newComponent);
			}
			else if (component->GetTypeString() == "Script")
			{
				ScriptComponent newComponent = ScriptComponent(static_cast<ScriptComponent*>(component), GetID());
				//components.push_back(&newComponent);
			}
			else if (component->GetTypeString() == "Text")
			{
				Text newComponent = Text(static_cast<Text*>(component), GetID());
				//components.push_back(&newComponent);
			}
			else if (component->GetTypeString() == "CharacterController")
			{
				CharacterController newComponent = CharacterController(static_cast<CharacterController*>(component), GetID());
				//components.push_back(&newComponent);
			}
			else if (component->GetTypeString() == "RigidBody")
			{
				RigidBody newComponent = RigidBody(static_cast<RigidBody*>(component), GetID());
				//components.push_back(&newComponent);
			}
			else if (component->GetTypeString() == "BoxCollider")
			{
				BoxCollider newComponent = BoxCollider(static_cast<BoxCollider*>(component), GetID());
				//components.push_back(&newComponent);
			}
			else if (component->GetTypeString() == "CircleCollider")
			{
				CircleCollider newComponent = CircleCollider(static_cast<CircleCollider*>(component), GetID());
				//components.push_back(&newComponent);
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

	bool GameObject::operator==(const GameObject toCompare)
	{
		return ID == toCompare.ID;
	}

	bool GameObject::IsValid()
	{
		return _isValid;
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

	TagList& GameObject::GetTagList()
	{
		return m_tagList;
	}

	void GameObject::SetTagList(TagList newTagList)
	{
		m_tagList = newTagList;
	}

	bool GameObject::HasTag(std::string tagName)
	{
		return m_tagList.HasTag(tagName);
	}

	void GameObject::SetTag(std::string tagName, bool _value)
	{
		m_tagList.SetTag(tagName, _value);
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
		for (Component* component : components)
		{
			component = nullptr;
		}

		components.clear();
	}

	Transform* GameObject::AddTransformComponent(long id, bool _active, bool _collapsed)
	{
		long nextID = id;
		if (nextID != -1)
			nextID = GetLoadedScene()->GetNextComponentID();

		Transform transform = Transform(nextID, ID);
		transform.SetActive(_active);
		transform.SetCollapsed(_collapsed);
		F_ECSManager.m_Transforms.push_back(transform);
		F_ECSManager.m_transformMap.emplace(this->ID, (long)F_ECSManager.m_Transforms.size() - 1);

		// Set transforms origin to parents true position
		if (parentID != -1)
		{
			GameObject *parent = GetObjectById(parentID);
			if (parent != nullptr && parent->HasComponent("Transform"))
			{
				Transform* parentTransform = parent->GetTransformComponent();
				Vector2 parentTruePosition = parentTransform->GetTruePosition();
				transform.SetOrigin(parentTruePosition);
			}
		}

		components.push_back(&F_ECSManager.m_Transforms.at(F_ECSManager.m_transformMap.at(this->ID)));
		return &F_ECSManager.m_Transforms.at(F_ECSManager.m_transformMap.at(this->ID));
	}

	Sprite* GameObject::AddSpriteComponent(long id, bool _active, bool _collapsed)
	{
		long nextID = id;
		if (nextID != -1)
			nextID = GetLoadedScene()->GetNextComponentID();
		Sprite sprite = Sprite(nextID, ID);
		sprite.SetActive(_active);
		sprite.SetCollapsed(_collapsed);
		
		F_ECSManager.m_Sprites.push_back(sprite);
		F_ECSManager.m_spriteMap.emplace(this->ID, (long)F_ECSManager.m_Sprites.size() - 1);
		components.push_back(&F_ECSManager.m_Sprites.at(F_ECSManager.m_spriteMap.at(this->ID)));
		return &F_ECSManager.m_Sprites.at(F_ECSManager.m_spriteMap.at(this->ID));
	}

	Camera* GameObject::AddCameraComponent(long id, bool _active, bool _collapsed)
	{
		long nextID = id;
		if (nextID != -1)
			nextID = GetLoadedScene()->GetNextComponentID();
		Camera camera = Camera(nextID, ID);
		camera.SetActive(_active);
		camera.SetCollapsed(_collapsed);
		
		F_ECSManager.m_Cameras.push_back(camera);
		F_ECSManager.m_cameraMap.emplace(this->ID, (long)F_ECSManager.m_Cameras.size() - 1);
		components.push_back(&F_ECSManager.m_Cameras.at(F_ECSManager.m_cameraMap.at(this->ID)));
		return &F_ECSManager.m_Cameras.at(F_ECSManager.m_cameraMap.at(this->ID));
	}

	ScriptComponent* GameObject::AddScriptComponent(long id, bool _active, bool _collapsed)
	{
		long nextID = id;
		if (nextID != -1)
			nextID = GetLoadedScene()->GetNextComponentID();
		ScriptComponent script = ScriptComponent(nextID, ID);
		script.SetActive(_active);
		script.SetCollapsed(_collapsed);
		
		// if object doesn't already have a mapped location in the m_scriptMap, add it
		//std::vector<long> indices = F_ECSManager.m_scriptMap.at(this->ID);
		//indices.push_back()

		F_ECSManager.m_Scripts.push_back(script);
		std::vector<long> ids;
		ids.push_back((long)F_ECSManager.m_Scripts.size() - 1);
		F_ECSManager.m_scriptMap.emplace(this->ID, ids);
		long index = F_ECSManager.m_scriptMap.at(this->ID)[0];
		components.push_back(&F_ECSManager.m_Scripts.at(F_ECSManager.m_scriptMap.at(this->ID)[0]));
		return &F_ECSManager.m_Scripts.at(index);
	}

	Button* GameObject::AddButtonComponent(long id, bool _active, bool _collapsed)
	{
		long nextID = id;
		if (nextID != -1)
			nextID = GetLoadedScene()->GetNextComponentID();
		Button button = F_UIManager.CreateButton(nextID, ID, 0);
		button.SetActive(_active);
		button.SetCollapsed(_collapsed);
		
		F_ECSManager.m_Buttons.push_back(button);
		F_ECSManager.m_buttonMap.emplace(this->ID, (long)F_ECSManager.m_Buttons.size() - 1);
		components.push_back(&F_ECSManager.m_Buttons.at(F_ECSManager.m_buttonMap.at(this->ID)));
		return &F_ECSManager.m_Buttons.at(F_ECSManager.m_buttonMap.at(this->ID));
	}

	Canvas* GameObject::AddCanvasComponent(long id, bool _active, bool _collapsed)
	{
		long nextID = id;
		if (nextID != -1)
			nextID = GetLoadedScene()->GetNextComponentID();
		Canvas canvas = F_UIManager.CreateCanvas(nextID, ID, 0);
		canvas.SetActive(_active);
		canvas.SetCollapsed(_collapsed);
	
		F_ECSManager.m_Canvases.push_back(canvas);
		F_ECSManager.m_canvasMap.emplace(this->ID, (long)F_ECSManager.m_Canvases.size() - 1);
		components.push_back(&F_ECSManager.m_Canvases.at(F_ECSManager.m_canvasMap.at(this->ID)));
		return &F_ECSManager.m_Canvases.at(F_ECSManager.m_canvasMap.at(this->ID));
	}

	Animation* GameObject::AddAnimationComponent(long id, bool _active, bool _collapsed)
	{
		long nextID = id;
		if (nextID != -1)
			nextID = GetLoadedScene()->GetNextComponentID();
		Animation animation = Animation(nextID, ID);
		animation.SetActive(_active);
		animation.SetCollapsed(_collapsed);
	
		F_ECSManager.m_Animations.push_back(animation);
		F_ECSManager.m_animationMap.emplace(this->ID, (long)F_ECSManager.m_Animations.size() - 1);
		components.push_back(&F_ECSManager.m_Animations.at(F_ECSManager.m_animationMap.at(this->ID)));
		return &F_ECSManager.m_Animations.at(F_ECSManager.m_animationMap.at(this->ID));
	}

	Audio* GameObject::AddAudioComponent(long id, bool _active, bool _collapsed)
	{
		long nextID = id;
		if (nextID != -1)
			nextID = GetLoadedScene()->GetNextComponentID();
		Audio audio = Audio(nextID, ID);
		audio.SetActive(_active);
		audio.SetCollapsed(_collapsed);

		F_ECSManager.m_Audios.push_back(audio);
		F_ECSManager.m_audioMap.emplace(this->ID, (long)F_ECSManager.m_Audios.size() - 1);
		components.push_back(&F_ECSManager.m_Audios.at(F_ECSManager.m_audioMap.at(this->ID)));
		return &F_ECSManager.m_Audios.at(F_ECSManager.m_audioMap.at(this->ID));
	}

	Text* GameObject::AddTextComponent(long id, bool _active, bool _collapsed)
	{
		long nextID = id;
		if (nextID != -1)
			nextID = GetLoadedScene()->GetNextComponentID();
		Text text = Text(nextID, ID);
		text.SetActive(_active);
		text.SetCollapsed(_collapsed);

		F_ECSManager.m_Texts.push_back(text);
		F_ECSManager.m_textMap.emplace(this->ID, (long)F_ECSManager.m_Texts.size() - 1);
		components.push_back(&F_ECSManager.m_Texts.at(F_ECSManager.m_textMap.at(this->ID)));
		return &F_ECSManager.m_Texts.at(F_ECSManager.m_textMap.at(this->ID));
	}

	BoxCollider* GameObject::AddBoxColliderComponent(long id, bool _active, bool _collapsed)
	{
		long nextID = id;
		if (nextID != -1)
			nextID = GetLoadedScene()->GetNextComponentID();
		BoxCollider boxCollider = BoxCollider(nextID, ID);
		boxCollider.SetActive(_active);
		boxCollider.SetCollapsed(_collapsed);

		F_ECSManager.m_BoxColliders.push_back(boxCollider);
		F_ECSManager.m_boxColliderMap.emplace(this->ID, (long)F_ECSManager.m_BoxColliders.size() - 1);
		components.push_back(&F_ECSManager.m_BoxColliders.at(F_ECSManager.m_boxColliderMap.at(this->ID)));
		return &F_ECSManager.m_BoxColliders.at(F_ECSManager.m_boxColliderMap.at(this->ID));
	}

	CircleCollider* GameObject::AddCircleColliderComponent(long id, bool _active, bool _collapsed)
	{
		long nextID = id;
		if (nextID != -1)
			nextID = GetLoadedScene()->GetNextComponentID();
		CircleCollider circleCollider = CircleCollider(nextID, ID);
		circleCollider.SetActive(_active);
		circleCollider.SetCollapsed(_collapsed);

		F_ECSManager.m_CircleColliders.push_back(circleCollider);
		F_ECSManager.m_circleColliderMap.emplace(this->ID, (long)F_ECSManager.m_CircleColliders.size() - 1);
		components.push_back(&F_ECSManager.m_CircleColliders.at(F_ECSManager.m_circleColliderMap.at(this->ID)));
		return &F_ECSManager.m_CircleColliders.at(F_ECSManager.m_circleColliderMap.at(this->ID));
	}

	CompositeCollider* GameObject::AddCompositeColliderComponent(long id, bool _active, bool _collapsed)
	{
		long nextID = id;
		if (nextID != -1)
			nextID = GetLoadedScene()->GetNextComponentID();
		CompositeCollider compositeCollider = CompositeCollider(nextID, ID);
		compositeCollider.SetActive(_active);
		compositeCollider.SetCollapsed(_collapsed);

		F_ECSManager.m_CompositeColliders.push_back(compositeCollider);
		F_ECSManager.m_compositeColliderMap.emplace(this->ID, (long)F_ECSManager.m_CompositeColliders.size() - 1);
		components.push_back(&F_ECSManager.m_CompositeColliders.at(F_ECSManager.m_compositeColliderMap.at(this->ID)));
		return &F_ECSManager.m_CompositeColliders.at(F_ECSManager.m_compositeColliderMap.at(this->ID));
	}

	RigidBody* GameObject::AddRigidBodyComponent(long id, bool _active, bool _collapsed)
	{
		long nextID = id;
		if (nextID != -1)
			nextID = GetLoadedScene()->GetNextComponentID();
		RigidBody rigidBody = RigidBody(nextID, ID);
		rigidBody.SetActive(_active);
		rigidBody.SetCollapsed(_collapsed);

		F_ECSManager.m_RigidBodies.push_back(rigidBody);
		F_ECSManager.m_rigidBodyMap.emplace(this->ID, (long)F_ECSManager.m_RigidBodies.size() - 1);
		components.push_back(&F_ECSManager.m_RigidBodies.at(F_ECSManager.m_rigidBodyMap.at(this->ID)));
		return &F_ECSManager.m_RigidBodies.at(F_ECSManager.m_rigidBodyMap.at(this->ID));
	}

	CharacterController* GameObject::AddCharacterControllerComponent(long id, bool _active, bool _collapsed)
	{
		long nextID = id;
		if (nextID != -1)
			nextID = GetLoadedScene()->GetNextComponentID();
		CharacterController characterController = CharacterController(nextID, ID);
		characterController.SetActive(_active);
		characterController.SetCollapsed(_collapsed);

		F_ECSManager.m_CharacterControllers.push_back(characterController);
		F_ECSManager.m_characterControllerMap.emplace(this->ID, (long)F_ECSManager.m_CharacterControllers.size() - 1);
		components.push_back(&F_ECSManager.m_CharacterControllers.at(F_ECSManager.m_characterControllerMap.at(this->ID)));
		return &F_ECSManager.m_CharacterControllers.at(F_ECSManager.m_characterControllerMap.at(this->ID));
	}


	Component* GameObject::GetComponent(Component::ComponentTypes type)
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
		for (Component* component : components)
		{
			if (component->GetTypeString() == type)
				return true;
		}
		return false;
	}

	Transform* GameObject::GetTransformComponent()
	{
		//Transform> transformComponent = static_cast<Transform>(GetComponent(ComponentTypes::Transform));
		//return transformComponent;
		//return &FlatEngine::F_ECSManager.m_Transforms.at(FlatEngine::F_ECSManager.m_transformMap.at(ID));

		if (FlatEngine::F_ECSManager.m_transformMap.count(ID) && FlatEngine::F_ECSManager.m_Transforms.size() >= FlatEngine::F_ECSManager.m_transformMap.at(ID))
			return &FlatEngine::F_ECSManager.m_Transforms.at(FlatEngine::F_ECSManager.m_transformMap.at(ID));
		else return nullptr;
	}

	Sprite* GameObject::GetSpriteComponent()
	{
		//Sprite spriteComponent = static_cast<Sprite>(GetComponent(ComponentTypes::Sprite));
		//return spriteComponent;
		//return &FlatEngine::F_ECSManager.m_Sprites.at(FlatEngine::F_ECSManager.m_spriteMap.at(ID));

		if (FlatEngine::F_ECSManager.m_spriteMap.count(ID) && FlatEngine::F_ECSManager.m_Sprites.size() >= FlatEngine::F_ECSManager.m_spriteMap.at(ID))
			return &FlatEngine::F_ECSManager.m_Sprites.at(FlatEngine::F_ECSManager.m_spriteMap.at(ID));
		else return nullptr;
	}

	Camera* GameObject::GetCameraComponent()
	{
		//Camera cameraComponent = static_cast<Camera>(GetComponent(ComponentTypes::Camera));
		//return cameraComponent;
		//return &FlatEngine::F_ECSManager.m_Cameras.at(FlatEngine::F_ECSManager.m_cameraMap.at(ID));

		if (FlatEngine::F_ECSManager.m_cameraMap.count(ID) && FlatEngine::F_ECSManager.m_Cameras.size() >= FlatEngine::F_ECSManager.m_cameraMap.at(ID))
			return &FlatEngine::F_ECSManager.m_Cameras.at(FlatEngine::F_ECSManager.m_cameraMap.at(ID));
		else return nullptr;
	}

	Animation* GameObject::GetAnimationComponent()
	{
		//Animation animationComponent = static_cast<Animation>(GetComponent(ComponentTypes::Animation));
		//return animationComponent;

		if (FlatEngine::F_ECSManager.m_animationMap.count(ID) && FlatEngine::F_ECSManager.m_Animations.size() >= FlatEngine::F_ECSManager.m_animationMap.at(ID))
			return &FlatEngine::F_ECSManager.m_Animations.at(FlatEngine::F_ECSManager.m_animationMap.at(ID));
		else return nullptr;
	}

	Audio* GameObject::GetAudioComponent()
	{
		//Audio audioComponent = static_cast<Audio>(GetComponent(ComponentTypes::Audio));
		//return audioComponent;
		//return &FlatEngine::F_ECSManager.m_Audios.at(FlatEngine::F_ECSManager.m_audioMap.at(ID));

		if (FlatEngine::F_ECSManager.m_audioMap.count(ID) && FlatEngine::F_ECSManager.m_Audios.size() >= FlatEngine::F_ECSManager.m_audioMap.at(ID))
			return &FlatEngine::F_ECSManager.m_Audios.at(FlatEngine::F_ECSManager.m_audioMap.at(ID));
		else return nullptr;
	}

	Button* GameObject::GetButtonComponent()
	{
		//Button buttonComponent = static_cast<Button>(GetComponent(ComponentTypes::Button));
		//return buttonComponent;
		//return &FlatEngine::F_ECSManager.m_Buttons.at(FlatEngine::F_ECSManager.m_buttonMap.at(ID));

		if (FlatEngine::F_ECSManager.m_buttonMap.count(ID) && FlatEngine::F_ECSManager.m_Buttons.size() >= FlatEngine::F_ECSManager.m_buttonMap.at(ID))
			return &FlatEngine::F_ECSManager.m_Buttons.at(FlatEngine::F_ECSManager.m_buttonMap.at(ID));
		else return nullptr;
	}

	Canvas* GameObject::GetCanvasComponent()
	{
		//Canvas canvasComponent = static_cast<Canvas>(GetComponent(ComponentTypes::Canvas));
		//return canvasComponent;
		//return &FlatEngine::F_ECSManager.m_Canvases.at(FlatEngine::F_ECSManager.m_canvasMap.at(ID));

		if (FlatEngine::F_ECSManager.m_canvasMap.count(ID) && FlatEngine::F_ECSManager.m_Canvases.size() >= FlatEngine::F_ECSManager.m_canvasMap.at(ID))
			return &FlatEngine::F_ECSManager.m_Canvases.at(FlatEngine::F_ECSManager.m_canvasMap.at(ID));
		else return nullptr;
	}

	ScriptComponent* GameObject::GetScriptComponent()
	{
		//ScriptComponent scriptComponent = static_cast<ScriptComponent>(GetComponent(ComponentTypes::Script));
		//return scriptComponent;
		//return &FlatEngine::F_ECSManager.m_Scripts.at(FlatEngine::F_ECSManager.m_scriptMap.at(ID)[0]);

		if (FlatEngine::F_ECSManager.m_scriptMap.count(ID) && FlatEngine::F_ECSManager.m_Scripts.size() >= FlatEngine::F_ECSManager.m_scriptMap.at(ID)[0])
			return &FlatEngine::F_ECSManager.m_Scripts.at(FlatEngine::F_ECSManager.m_scriptMap.at(ID)[0]);
		else return nullptr;
	}

	std::vector<ScriptComponent*> GameObject::GetScriptComponents()
	{
		std::vector<ScriptComponent*> scripts = std::vector<ScriptComponent*>();

		if (FlatEngine::F_ECSManager.m_scriptMap.count(ID) && FlatEngine::F_ECSManager.m_Scripts.size() >= FlatEngine::F_ECSManager.m_scriptMap.at(ID)[0])
		{
			for (int i = 0; i < FlatEngine::F_ECSManager.m_scriptMap.at(ID).size(); i++)
			{
				scripts.push_back(&FlatEngine::F_ECSManager.m_Scripts.at(FlatEngine::F_ECSManager.m_scriptMap.at(ID)[i]));
			}
		}
		return scripts;
	}

	Text* GameObject::GetTextComponent()
	{
		//Text textComponent = static_cast<Text>(GetComponent(ComponentTypes::Text));
		//return textComponent;
		//return &FlatEngine::F_ECSManager.m_Texts.at(FlatEngine::F_ECSManager.m_textMap.at(ID));

		if (FlatEngine::F_ECSManager.m_textMap.count(ID) && FlatEngine::F_ECSManager.m_Texts.size() >= FlatEngine::F_ECSManager.m_textMap.at(ID))
			return &FlatEngine::F_ECSManager.m_Texts.at(FlatEngine::F_ECSManager.m_textMap.at(ID));
		else return nullptr;
	}

	CharacterController* GameObject::GetCharacterController()
	{
		//CharacterController characterController = static_cast<CharacterController>(GetComponent(ComponentTypes::CharacterController));
		//return characterController;
		//return &FlatEngine::F_ECSManager.m_CharacterControllers.at(FlatEngine::F_ECSManager.m_characterControllerMap.at(ID));

		if (FlatEngine::F_ECSManager.m_characterControllerMap.count(ID) && FlatEngine::F_ECSManager.m_CharacterControllers.size() >= FlatEngine::F_ECSManager.m_characterControllerMap.at(ID))
			return &FlatEngine::F_ECSManager.m_CharacterControllers.at(FlatEngine::F_ECSManager.m_characterControllerMap.at(ID));
		else return nullptr;
	}

	RigidBody* GameObject::GetRigidBody()
	{
		//RigidBody rigidBody = static_cast<RigidBody>(GetComponent(ComponentTypes::RigidBody));
		//return rigidBody;
		//return &FlatEngine::F_ECSManager.m_RigidBodies.at(FlatEngine::F_ECSManager.m_rigidBodyMap.at(ID));

		if (FlatEngine::F_ECSManager.m_rigidBodyMap.count(ID) && FlatEngine::F_ECSManager.m_RigidBodies.size() >= FlatEngine::F_ECSManager.m_rigidBodyMap.at(ID))
			return &FlatEngine::F_ECSManager.m_RigidBodies.at(FlatEngine::F_ECSManager.m_rigidBodyMap.at(ID));
		else return nullptr;
	}

	BoxCollider* GameObject::GetBoxCollider()
	{
		//BoxCollider boxCollider = static_cast<BoxCollider>(GetComponent(ComponentTypes::BoxCollider));
		//return boxCollider;
		//return &FlatEngine::F_ECSManager.m_BoxColliders.at(FlatEngine::F_ECSManager.m_boxColliderMap.at(ID));

		if (FlatEngine::F_ECSManager.m_boxColliderMap.count(ID) && FlatEngine::F_ECSManager.m_BoxColliders.size() >= FlatEngine::F_ECSManager.m_boxColliderMap.at(ID))
			return &FlatEngine::F_ECSManager.m_BoxColliders.at(FlatEngine::F_ECSManager.m_boxColliderMap.at(ID));
		else return nullptr;
	}

	std::vector<BoxCollider*> GameObject::GetBoxColliders()
	{
		std::vector<BoxCollider*> boxColliders;
		for (Component* component : components)
			if (component->GetTypeString() == "BoxCollider")
				boxColliders.push_back(static_cast<BoxCollider*>(component));

		return boxColliders;
	}

	CircleCollider* GameObject::GetCircleCollider()
	{
		//CircleCollider circleCollider = static_cast<CircleCollider>(GetComponent(ComponentTypes::CircleCollider));
		//return circleCollider;
		return nullptr;
	}

	std::vector<CircleCollider*> GameObject::GetCircleColliders()
	{
		std::vector<CircleCollider*> circleColliders;
		for (Component* component : components)
			if (component->GetTypeString() == "CircleCollider")
				circleColliders.push_back(static_cast<CircleCollider*>(component));

		return circleColliders;
	}

	CompositeCollider* GameObject::GetCompositeCollider()
	{
		//CompositeCollider> compositeCollider = static_cast<CompositeCollider>(GetComponent(ComponentTypes::CompositeCollider));
		//return compositeCollider;
		return nullptr;
	}

	GameScript* GameObject::GetGameScriptByName(std::string scriptName)
	{
		for (int i = 0; i < components.size(); i++)
		{
			if (components[i]->GetType() == ComponentTypes::Script)
			{
				ScriptComponent* scriptComponent = static_cast<ScriptComponent*>(components[i]);
				if (scriptComponent->GetAttachedScript() == scriptName)
					return scriptComponent->GetScriptInstance();
			}
		}

		return nullptr;
	}

	std::vector<Component*> &GameObject::GetComponents()
	{
		return components;
	}

	std::vector<GameScript*> GameObject::GetScriptInstances()
	{
		std::vector<ScriptComponent*> scriptComponents = GetScriptComponents();
		std::vector<GameScript*> gameScripts;

		for (ScriptComponent *script : scriptComponents)
			gameScripts.push_back(script->GetScriptInstance());
		
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

	GameObject GameObject::GetFirstChild()
	{
		return GetObjectById(childrenIDs[0]);
	}

	GameObject GameObject::FindChildByName(std::string name)
	{
		for (int i = 0; i < childrenIDs.size(); i++)
		{
			if (GetObjectById(childrenIDs[i])->GetName() == name)
			{
				return GetObjectById(childrenIDs[i]);
			}
		}

		GameObject nullObject = GameObject(nullptr);
		return nullObject;
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
	
	GameObject GameObject::GetParent()
	{
		return GetObjectById(parentID);
	}
}