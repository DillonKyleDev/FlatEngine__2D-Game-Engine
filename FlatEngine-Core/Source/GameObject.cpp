#include "GameObject.h"
#include "FlatEngine.h"
#include "UIManager.h"
#include "TagList.h"
#include "Transform.h"
#include "Scene.h"
#include "Sprite.h"
#include "Camera.h"
#include "Text.h"
#include "Script.h"
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
#include "Scene.h"


namespace FlatEngine
{
	GameObject::GameObject(long newParentID, long myID)
	{
		_isValid = true;
		if (myID == -1)
			ID = GetNextGameObjectID();
		else
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
				GetLoadedScene()->AddTransform(newComponent, ID);

				if (parentID != -1 && GetObjectById(parentID)->IsValid())
				{
					newComponent.SetOrigin(GetObjectById(parentID)->GetTransform()->GetTruePosition());
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
				Script newComponent = Script(static_cast<Script*>(component), GetID());
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
		DeleteComponents();
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
		for (std::vector<Component*>::iterator iter = components.begin(); iter != components.end();)
		{
			if ((*iter)->GetID() == componentID)
			{
				// Remove from ECSManager
				GetLoadedScene()->RemoveComponent(*iter, ID);

				// Remove GameObject ptr to it
				components.erase(iter);		
				break;
			}
			iter++;
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
		if (nextID == -1)
			nextID = GetLoadedScene()->GetNextComponentID();

		Transform transform = Transform(nextID, ID);
		transform.SetActive(_active);
		transform.SetCollapsed(_collapsed);

		// Set transforms origin to parents true position
		if (parentID != -1)
		{
			GameObject *parent = GetObjectById(parentID);
			if (parent != nullptr && parent->HasComponent("Transform"))
			{
				Transform* parentTransform = parent->GetTransform();
				Vector2 parentTruePosition = parentTransform->GetTruePosition();
				transform.SetOrigin(parentTruePosition);
			}
		}

		Transform* transformPtr = GetLoadedScene()->AddTransform(transform, ID);
		components.push_back(transformPtr);
		return transformPtr;
	}

	Sprite* GameObject::AddSpriteComponent(long id, bool _active, bool _collapsed)
	{
		long nextID = id;
		if (nextID == -1)
			nextID = GetLoadedScene()->GetNextComponentID();
		Sprite sprite = Sprite(nextID, ID);
		sprite.SetActive(_active);
		sprite.SetCollapsed(_collapsed);
		
		Sprite* spritePtr = GetLoadedScene()->AddSprite(sprite, ID);
		components.push_back(spritePtr);
		return spritePtr;
	}

	Camera* GameObject::AddCameraComponent(long id, bool _active, bool _collapsed)
	{
		long nextID = id;
		if (nextID == -1)
			nextID = GetLoadedScene()->GetNextComponentID();
		Camera camera = Camera(nextID, ID);
		camera.SetActive(_active);
		camera.SetCollapsed(_collapsed);
		
		Camera* cameraPtr = GetLoadedScene()->AddCamera(camera, ID);
		components.push_back(cameraPtr);
		return cameraPtr;
	}

	Script* GameObject::AddScriptComponent(long id, bool _active, bool _collapsed)
	{
		long nextID = id;
		if (nextID == -1)
			nextID = GetLoadedScene()->GetNextComponentID();
		Script script = Script(nextID, ID);
		script.SetActive(_active);
		script.SetCollapsed(_collapsed);

		Script* scriptPtr = GetLoadedScene()->AddScript(script, ID);
		components.push_back(scriptPtr);
		return scriptPtr;
	}

	Button* GameObject::AddButtonComponent(long id, bool _active, bool _collapsed)
	{
		long nextID = id;
		if (nextID == -1)
			nextID = GetLoadedScene()->GetNextComponentID();
		Button button = F_UIManager.CreateButton(nextID, ID, 0);
		button.SetActive(_active);
		button.SetCollapsed(_collapsed);
		
		Button* buttonPtr = GetLoadedScene()->AddButton(button, ID);
		components.push_back(buttonPtr);
		return buttonPtr;
	}

	Canvas* GameObject::AddCanvasComponent(long id, bool _active, bool _collapsed)
	{
		long nextID = id;
		if (nextID == -1)
			nextID = GetLoadedScene()->GetNextComponentID();
		Canvas canvas = F_UIManager.CreateCanvas(nextID, ID, 0);
		canvas.SetActive(_active);
		canvas.SetCollapsed(_collapsed);

		Canvas* canvasPtr = GetLoadedScene()->AddCanvas(canvas, ID);
		components.push_back(canvasPtr);
		return canvasPtr;
	}

	Animation* GameObject::AddAnimationComponent(long id, bool _active, bool _collapsed)
	{
		long nextID = id;
		if (nextID == -1)
			nextID = GetLoadedScene()->GetNextComponentID();
		Animation animation = Animation(nextID, ID);
		animation.SetActive(_active);
		animation.SetCollapsed(_collapsed);
	
		Animation* animationPtr = GetLoadedScene()->AddAnimation(animation, ID);
		components.push_back(animationPtr);
		return animationPtr;
	}

	Audio* GameObject::AddAudioComponent(long id, bool _active, bool _collapsed)
	{
		long nextID = id;
		if (nextID == -1)
			nextID = GetLoadedScene()->GetNextComponentID();
		Audio audio = Audio(nextID, ID);
		audio.SetActive(_active);
		audio.SetCollapsed(_collapsed);

		Audio* audioPtr = GetLoadedScene()->AddAudio(audio, ID);
		components.push_back(audioPtr);
		return audioPtr;
	}

	Text* GameObject::AddTextComponent(long id, bool _active, bool _collapsed)
	{
		long nextID = id;
		if (nextID == -1)
			nextID = GetLoadedScene()->GetNextComponentID();
		Text text = Text(nextID, ID);
		text.SetActive(_active);
		text.SetCollapsed(_collapsed);
		
		Text* textPtr = GetLoadedScene()->AddText(text, ID);
		components.push_back(textPtr);
		return textPtr;
	}

	BoxCollider* GameObject::AddBoxColliderComponent(long id, bool _active, bool _collapsed)
	{
		Vector2 spriteDimensions = Vector2(2, 2);
		if (HasComponent("Sprite"))
		{
			Sprite* sprite = GetSprite();
			// Get sprite dimensions in terms of grid squares because that is the unit used by the engine
			spriteDimensions = Vector2(sprite->GetTextureWidth() / F_pixelsPerGridSpace, sprite->GetTextureHeight() / F_pixelsPerGridSpace);
		}

		long nextID = id;
		if (nextID == -1)
			nextID = GetLoadedScene()->GetNextComponentID();
		BoxCollider boxCollider = BoxCollider(nextID, ID);
		boxCollider.SetActive(_active);
		boxCollider.SetCollapsed(_collapsed);

		BoxCollider* colliderPtr = GetLoadedScene()->AddBoxCollider(boxCollider, ID);
		colliderPtr->SetActiveDimensions(spriteDimensions.x, spriteDimensions.y);
		components.push_back(colliderPtr);
		return colliderPtr;
	}

	CircleCollider* GameObject::AddCircleColliderComponent(long id, bool _active, bool _collapsed)
	{
		long nextID = id;
		if (nextID == -1)
			nextID = GetLoadedScene()->GetNextComponentID();
		CircleCollider circleCollider = CircleCollider(nextID, ID);
		circleCollider.SetActive(_active);
		circleCollider.SetCollapsed(_collapsed);

		CircleCollider* colliderPtr = GetLoadedScene()->AddCircleCollider(circleCollider, ID);
		components.push_back(colliderPtr);
		return colliderPtr;
	}

	CompositeCollider* GameObject::AddCompositeColliderComponent(long id, bool _active, bool _collapsed)
	{
		long nextID = id;
		if (nextID == -1)
			nextID = GetLoadedScene()->GetNextComponentID();
		CompositeCollider compositeCollider = CompositeCollider(nextID, ID);
		compositeCollider.SetActive(_active);
		compositeCollider.SetCollapsed(_collapsed);

		CompositeCollider* colliderPtr = GetLoadedScene()->AddCompositeCollider(compositeCollider, ID);
		components.push_back(colliderPtr);
		return colliderPtr;
	}

	RigidBody* GameObject::AddRigidBodyComponent(long id, bool _active, bool _collapsed)
	{
		long nextID = id;
		if (nextID == -1)
			nextID = GetLoadedScene()->GetNextComponentID();
		RigidBody rigidBody = RigidBody(nextID, ID);
		rigidBody.SetActive(_active);
		rigidBody.SetCollapsed(_collapsed);

		RigidBody* rigidBodyPtr = GetLoadedScene()->AddRigidBody(rigidBody, ID);
		components.push_back(rigidBodyPtr);
		return rigidBodyPtr;
	}

	CharacterController* GameObject::AddCharacterControllerComponent(long id, bool _active, bool _collapsed)
	{
		long nextID = id;
		if (nextID == -1)
			nextID = GetLoadedScene()->GetNextComponentID();
		CharacterController characterController = CharacterController(nextID, ID);
		characterController.SetActive(_active);
		characterController.SetCollapsed(_collapsed);

		CharacterController* characterControllerPtr = GetLoadedScene()->AddCharacterController(characterController, ID);
		components.push_back(characterControllerPtr);
		return characterControllerPtr;
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

	Transform* GameObject::GetTransform()
	{
		return GetLoadedScene()->GetTransformByOwner(ID);
	}
	Sprite* GameObject::GetSprite()
	{
		return GetLoadedScene()->GetSpriteByOwner(ID);
	}
	Camera* GameObject::GetCamera()
	{
		return GetLoadedScene()->GetCameraByOwner(ID);
	}
	Animation* GameObject::GetAnimation()
	{
		return GetLoadedScene()->GetAnimationByOwner(ID);
	}
	Audio* GameObject::GetAudio()
	{
		return GetLoadedScene()->GetAudioByOwner(ID);
	}
	Button* GameObject::GetButton()
	{
		return GetLoadedScene()->GetButtonByOwner(ID);
	}
	Canvas* GameObject::GetCanvas()
	{
		return GetLoadedScene()->GetCanvasByOwner(ID);
	}
	std::vector<Script*> GameObject::GetScripts()
	{
		return GetLoadedScene()->GetScriptsByOwner(ID);
	}
	Text* GameObject::GetText()
	{
		return GetLoadedScene()->GetTextByOwner(ID);
	}
	CharacterController* GameObject::GetCharacterController()
	{
		return GetLoadedScene()->GetCharacterControllerByOwner(ID);
	}
	RigidBody* GameObject::GetRigidBody()
	{
		return GetLoadedScene()->GetRigidBodyByOwner(ID);
	}
	std::vector<BoxCollider*> GameObject::GetBoxColliders()
	{
		return GetLoadedScene()->GetBoxCollidersByOwner(ID);
	}
	std::vector<CircleCollider*> GameObject::GetCircleColliders()
	{
		return GetLoadedScene()->GetCircleCollidersByOwner(ID);
	}
	CompositeCollider* GameObject::GetCompositeCollider()
	{
		return GetLoadedScene()->GetCompositeColliderByOwner(ID);
	}

	std::vector<Component*> GameObject::GetComponents()
	{
		return components;
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
	
	GameObject *GameObject::GetParent()
	{
		return GetObjectById(parentID);
	}
}