#include "Scene.h"
#include "FlatEngine.h"
#include "Application.h"
#include "GameObject.h"
#include "Transform.h"
#include "Sprite.h"
#include "Camera.h"
#include "Script.h"
#include "Button.h"
#include "Canvas.h"
#include "Animation.h"
#include "Audio.h"
#include "Text.h"
#include "Collider.h"
#include "CompositeCollider.h"
#include "BoxCollider.h"
#include "CircleCollider.h"
#include "RigidBody.h"
#include "CharacterController.h"
#include "GameLoop.h"
#include "ECSManager.h"

namespace FlatEngine
{
	Scene::Scene()
	{
		name = "New Scene";
		path = "";
		m_sceneObjects = std::map<long, GameObject>();		
		animatorPreviewObjects = std::vector<GameObject*>();
		m_ECSManager = ECSManager();
		m_primaryCamera = nullptr;
		m_nextGameObjectID = 0;
		m_nextComponentID = 0;
		m_freedComponentIDs = std::vector<long>();
		m_freedGameObjectIDs = std::vector<long>();
	}

	Scene::~Scene()
	{
	}

    void Scene::SetName(std::string newName)
    {
        name = newName;
    }

	std::string Scene::GetName()
	{
		return name;
	}

	void Scene::SetPath(std::string scenePath)
	{
		path = scenePath;
	}

	std::string Scene::GetPath()
	{
		return path;
	}

	GameObject* Scene::AddSceneObject(GameObject sceneObject)
	{
		long id = sceneObject.GetID();
		m_sceneObjects.emplace(id, sceneObject);
		KeepNextGameObjectIDUpToDate(id);

		if (sceneObject.HasComponent("BoxCollider"))
			UpdateColliderPairs();

		return &m_sceneObjects.at(id);
	}

	void Scene::KeepNextGameObjectIDUpToDate(long id)
	{
		if (id >= m_nextGameObjectID)
			m_nextGameObjectID = id + 1;
	}

	std::map<long, GameObject> &Scene::GetSceneObjects()
	{
		return m_sceneObjects;
	}

	void Scene::SetAnimatorPreviewObjects(std::vector<GameObject*> previewObjects)
	{
		animatorPreviewObjects = previewObjects;
	}

	std::vector<GameObject*> Scene::GetAnimatorPreviewObjects()
	{
		return animatorPreviewObjects;
	}

	GameObject* Scene::GetObjectById(long ID)
	{
		if (m_sceneObjects.count(ID) > 0)
			return &m_sceneObjects.at(ID);
		else return nullptr;
	}

	GameObject* Scene::GetObjectByName(std::string name)
	{
		for (std::map<long, GameObject>::iterator iter = m_sceneObjects.begin(); iter != m_sceneObjects.end();)
		{
			if (name == iter->second.GetName())
			{
				return &iter->second;
			}
			iter++;
		}
		for (GameObject *animPreviewObject : animatorPreviewObjects)
		{
			if (name == animPreviewObject->GetName())
			{
				return animPreviewObject;
			}
		}
		return nullptr;
	}

	GameObject* Scene::GetObjectByTag(std::string tag)
	{
		for (std::map<long, GameObject>::iterator iter = m_sceneObjects.begin(); iter != m_sceneObjects.end();)
		{
			if (iter->second.GetTagList().HasTag(tag))
			{
				return &iter->second;
			}
			iter++;
		}
		return nullptr;
	}

	GameObject* Scene::CreateGameObject(long parentID)
	{
		GameObject newObject = GameObject(parentID);
		newObject.AddTransformComponent();
		if (parentID != -1)
			m_sceneObjects.at(parentID).AddChild(newObject.GetID());
		return AddSceneObject(newObject);
	}

	void Scene::DeleteGameObject(long sceneObjectID)
	{
		GameObject *objectToDelete = FlatEngine::GetObjectById(sceneObjectID);

		// If this GameObject was the primary camera, unset it as the m_primaryCamera and set m_primaryCamera to nullptr
		if (m_primaryCamera != nullptr && m_primaryCamera->GetParentID() == objectToDelete->GetID())
		{
			m_primaryCamera->SetPrimaryCamera(false);
			m_primaryCamera = nullptr;
		}

		// Check for a parent and remove the child object reference
		long parentID = objectToDelete->GetParentID();
		if (parentID != -1)
		{
			GameObject *parent = FlatEngine::GetObjectById(parentID);
			parent->RemoveChild(sceneObjectID);
		}

		// To be updated right after GameObject and its components are deleted
		bool _hadRigidBody = false;
		bool _hadCollider = false;

		// Remove components from gameloop flow (activeScript instances, RigidBodies, Colliders)
		for (Component* component : objectToDelete->GetComponents())
		{
			if (component->GetTypeString() == "RigidBody")
			{
				//gameLoop->RemoveRigidBody(component->GetID());
				_hadRigidBody = true;
			}
			if (component->GetTypeString() == "CircleCollider" || component->GetTypeString() == "BoxCollider")
			{
				//gameLoop->RemoveCollider(component->GetID());
				_hadCollider = true;
			}
		}

		// Check for children and delete those as well
		Scene::DeleteChildrenAndSelf(objectToDelete);

		// Save the ID for use on the next created GameObject
		m_freedGameObjectIDs.push_back(sceneObjectID);

		// TODO
		// 
		//if (_hadRigidBody)
		//	F_Application->GetGameLoop()->UpdateActiveRigidBodies();
		//if (_hadCollider)
		//	F_Application->GetGameLoop()->UpdateActiveColliders();
	}

	// Recursive
	void Scene::DeleteChildrenAndSelf(GameObject *objectToDelete)
	{
		long id = objectToDelete->GetID();

		// Must remove the m_primaryCamera pointer from the loaded scene before deleting the GameObject.
		long cameraObjectID = -1;
		if (m_primaryCamera != nullptr)
			cameraObjectID = m_primaryCamera->GetParentID();

		// Check for children
		if (objectToDelete->HasChildren())
		{
			// Call this function again on this objects children
			for (int c = 0; c < objectToDelete->GetChildren().size(); c++)
			{
				LogString("Deleting children of: " + objectToDelete->GetName());
				GameObject *child = FlatEngine::GetObjectById(objectToDelete->GetChildren()[c]);
				LogString("Deleting child: " + child->GetName());
				Scene::DeleteChildrenAndSelf(child);
			}
		}
		
		// Remove the m_primaryCamera pointer from the loaded scene if it is attached to the deleting GameObject
		if (objectToDelete->GetID() == cameraObjectID)
			RemovePrimaryCamera();

		// Then delete this GameObject
		m_sceneObjects.erase(id);
	}

	void Scene::IncrementGameObjectID()
	{
		m_nextGameObjectID += 1;
	}

	long Scene::GetNextGameObjectID()
	{
		long id;
		if (m_freedGameObjectIDs.size() > 0)
		{
			id = m_freedGameObjectIDs.back();
			m_freedGameObjectIDs.pop_back();
		}
		else
		{
			id = m_nextGameObjectID;
			IncrementGameObjectID();
		}
		return id;
	}

	void  Scene::IncrementComponentID()
	{
		m_nextComponentID += 1;
	}

	long  Scene::GetNextComponentID()
	{
		long id;
		if (m_freedComponentIDs.size() > 0)
		{
			id = m_freedComponentIDs.back();
			m_freedComponentIDs.pop_back();
		}
		else
		{
			id = m_nextComponentID;
			IncrementComponentID();
		}
		return id;
	}

	void Scene::SetPrimaryCamera(Camera* camera)
	{
		if (camera != nullptr)
		{
			// Remove the old m_primaryCamera
			if (m_primaryCamera != nullptr)
			{
				m_primaryCamera->SetPrimaryCamera(false);
			}

			m_primaryCamera = camera;
			m_primaryCamera->SetPrimaryCamera(true);
		}
		else
		{
			FlatEngine::LogString("Scene::Setm_primaryCamera() - The Camera pointer you tried to set as the primary Camera was a nullptr.");
		}
	}

	void Scene::RemovePrimaryCamera()
	{
		m_primaryCamera = nullptr;
	}

	Camera *Scene::GetPrimaryCamera()
	{
		return m_primaryCamera;
	}

	std::vector<std::pair<Collider*, Collider*>> Scene::GetColliderPairs()
	{
		return m_ECSManager.GetColliderPairs();
	}

	void Scene::OnPrefabInstantiated(std::vector<GameObject> children)
	{
		m_ECSManager.UpdateColliderPairs();
		m_ECSManager.UpdateActiveRigidBodies();
	}

	void Scene::UpdateColliderPairs()
	{
		m_ECSManager.UpdateColliderPairs();
	}

	void Scene::InitializeScriptObjects()
	{
		//m_ECSManager.InitializeScriptObjects
	}

	void Scene::KeepNextComponentIDUpToDate(long id)
	{
		if (id >= m_nextComponentID)
			m_nextComponentID = id + 1;
	}

	Transform* Scene::AddTransform(Transform transform, long ownerID)
	{
		KeepNextComponentIDUpToDate(transform.GetID());
		return m_ECSManager.AddTransform(transform, ownerID);
	}

	Sprite* Scene::AddSprite(Sprite sprite, long ownerID)
	{
		KeepNextComponentIDUpToDate(sprite.GetID());
		return m_ECSManager.AddSprite(sprite, ownerID);
	}

	Camera* Scene::AddCamera(Camera camera, long ownerID)
	{
		KeepNextComponentIDUpToDate(camera.GetID());
		return m_ECSManager.AddCamera(camera, ownerID);
	}

	Script* Scene::AddScript(Script script, long ownerID)
	{
		KeepNextComponentIDUpToDate(script.GetID());
		return m_ECSManager.AddScript(script, ownerID);
	}

	Canvas* Scene::AddCanvas(Canvas canvas, long ownerID)
	{
		KeepNextComponentIDUpToDate(canvas.GetID());
		return m_ECSManager.AddCanvas(canvas, ownerID);
	}

	Audio* Scene::AddAudio(Audio audio, long ownerID)
	{
		KeepNextComponentIDUpToDate(audio.GetID());
		return m_ECSManager.AddAudio(audio, ownerID);
	}

	Text* Scene::AddText(Text text, long ownerID)
	{
		KeepNextComponentIDUpToDate(text.GetID());
		return m_ECSManager.AddText(text, ownerID);
	}

	CompositeCollider* Scene::AddCompositeCollider(CompositeCollider collider, long ownerID)
	{
		KeepNextComponentIDUpToDate(collider.GetID());
		return m_ECSManager.AddCompositeCollider(collider, ownerID);
	}

	BoxCollider* Scene::AddBoxCollider(BoxCollider collider, long ownerID)
	{
		KeepNextComponentIDUpToDate(collider.GetID());
		return m_ECSManager.AddBoxCollider(collider, ownerID);
	}

	CircleCollider* Scene::AddCircleCollider(CircleCollider collider, long ownerID)
	{
		KeepNextComponentIDUpToDate(collider.GetID());
		return m_ECSManager.AddCircleCollider(collider, ownerID);
	}

	Animation* Scene::AddAnimation(Animation animation, long ownerID)
	{
		KeepNextComponentIDUpToDate(animation.GetID());
		return m_ECSManager.AddAnimation(animation, ownerID);
	}

	Button* Scene::AddButton(Button button, long ownerID)
	{
		KeepNextComponentIDUpToDate(button.GetID());
		return m_ECSManager.AddButton(button, ownerID);
	}

	RigidBody* Scene::AddRigidBody(RigidBody rigidBody, long ownerID)
	{
		KeepNextComponentIDUpToDate(rigidBody.GetID());
		return m_ECSManager.AddRigidBody(rigidBody, ownerID);
	}

	CharacterController* Scene::AddCharacterController(CharacterController characterController, long ownerID)
	{
		KeepNextComponentIDUpToDate(characterController.GetID());
		return m_ECSManager.AddCharacterController(characterController, ownerID);
	}

	void Scene::RemoveComponent(Component* component, long ownerID)
	{
		long id = component->GetID();
		if (m_ECSManager.RemoveComponent(component, ownerID))
			m_freedComponentIDs.push_back(id);
	}

	Transform* Scene::GetTransformByOwner(long ownerID)
	{
		return m_ECSManager.GetTransformByOwner(ownerID);
	}

	Sprite* Scene::GetSpriteByOwner(long ownerID)
	{
		return m_ECSManager.GetSpriteByOwner(ownerID);
	}

	Camera* Scene::GetCameraByOwner(long ownerID)
	{
		return m_ECSManager.GetCameraByOwner(ownerID);
	}

	std::vector<Script*> Scene::GetScriptsByOwner(long ownerID)
	{
		return m_ECSManager.GetScriptsByOwner(ownerID);
	}

	Canvas* Scene::GetCanvasByOwner(long ownerID)
	{
		return m_ECSManager.GetCanvasByOwner(ownerID);
	}

	Audio* Scene::GetAudioByOwner(long ownerID)
	{
		return m_ECSManager.GetAudioByOwner(ownerID);
	}

	Text* Scene::GetTextByOwner(long ownerID)
	{
		return m_ECSManager.GetTextByOwner(ownerID);
	}

	CompositeCollider* Scene::GetCompositeColliderByOwner(long ownerID)
	{
		return m_ECSManager.GetCompositeColliderByOwner(ownerID);
	}

	std::vector<BoxCollider*> Scene::GetBoxCollidersByOwner(long ownerID)
	{
		return m_ECSManager.GetBoxCollidersByOwner(ownerID);
	}

	std::vector<CircleCollider*> Scene::GetCircleCollidersByOwner(long ownerID)
	{
		return m_ECSManager.GetCircleCollidersByOwner(ownerID);
	}

	Animation* Scene::GetAnimationByOwner(long ownerID)
	{
		return m_ECSManager.GetAnimationByOwner(ownerID);
	}

	Button* Scene::GetButtonByOwner(long ownerID)
	{
		return m_ECSManager.GetButtonByOwner(ownerID);
	}

	RigidBody* Scene::GetRigidBodyByOwner(long ownerID)
	{
		return m_ECSManager.GetRigidBodyByOwner(ownerID);
	}

	CharacterController* Scene::GetCharacterControllerByOwner(long ownerID)
	{
		return m_ECSManager.GetCharacterControllerByOwner(ownerID);
	}

	std::map<long, Transform> &Scene::GetTransforms()
	{
		return m_ECSManager.GetTransforms();
	}
	std::map<long, Sprite>& Scene::GetSprites()
	{
		return m_ECSManager.GetSprites();
	}
	std::map<long, Camera>& Scene::GetCameras()
	{
		return m_ECSManager.GetCameras();
	}
	std::map<long, std::map<long, Script>>& Scene::GetScripts()
	{
		return m_ECSManager.GetScripts();
	}
	std::map<long, std::vector<std::string>> &Scene::GetLuaScriptsByOwner()
	{
		return m_ECSManager.GetLuaScriptsByOwner();
	}
	std::map<long, Button>& Scene::GetButtons()
	{
		return m_ECSManager.GetButtons();
	}
	std::map<long, Canvas>& Scene::GetCanvases()
	{
		return m_ECSManager.GetCanvases();
	}
	std::map<long, Animation>& Scene::GetAnimations()
	{
		return m_ECSManager.GetAnimations();
	}
	std::map<long, Audio>& Scene::GetAudios()
	{
		return m_ECSManager.GetAudios();
	}
	std::map<long, Text>& Scene::GetTexts()
	{
		return m_ECSManager.GetTexts();
	}
	std::vector<Collider*> Scene::GetColliders()
	{
		return m_ECSManager.GetColliders();
	}
	std::map<long, CompositeCollider>& Scene::GetCompositeColliders()
	{
		return m_ECSManager.GetCompositeColliders();
	}
	std::map<long, std::map<long, BoxCollider>>& Scene::GetBoxColliders()
	{
		return m_ECSManager.GetBoxColliders();
	}
	std::map<long, std::map<long, CircleCollider>>& Scene::GetCircleColliders()
	{
		return m_ECSManager.GetCircleColliders();
	}
	std::map<long, RigidBody>& Scene::GetRigidBodies()
	{
		return m_ECSManager.GetRigidBodies();
	}
	std::map<long, CharacterController>& Scene::GetCharacterControllers()
	{
		return m_ECSManager.GetCharacterControllers();
	}
}