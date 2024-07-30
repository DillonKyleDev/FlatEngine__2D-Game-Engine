#include "Scene.h"
#include "FlatEngine.h"
#include "Application.h"
#include "GameObject.h"
#include "Transform.h"
#include "Sprite.h"
#include "Camera.h"
#include "ScriptComponent.h"
#include "GameScript.h"
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
		sceneObjects = std::vector<GameObject>();		
		animatorPreviewObjects = std::vector<GameObject*>();
		m_ECSManager = ECSManager();
		primaryCamera = nullptr;
		nextGameObjectID = 0;
		nextComponentID = 0;
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
		int index = sceneObjects.size();
		sceneObjects.push_back(sceneObject);
		return &sceneObjects[index];
	}

	std::vector<GameObject> &Scene::GetSceneObjects()
	{
		return sceneObjects;
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
		if (ID != -1)
		{
			for (int i = 0; i < sceneObjects.size(); i++)
			{
				if (ID == sceneObjects[i].GetID())
				{
					return &sceneObjects[i];
				}
			}
			for (int i = 0; i < animatorPreviewObjects.size(); i++)
			{
				if (ID == animatorPreviewObjects[i]->GetID())
				{
					return animatorPreviewObjects[i];
				}
			}
			return nullptr;
		}
		else return nullptr;
	}

	GameObject* Scene::GetObjectByName(std::string name)
	{
		for (int i = 0; i < sceneObjects.size(); i++)
		{
			if (name == sceneObjects[i].GetName())
			{
				return &sceneObjects[i];
			}
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

	GameObject* Scene::CreateGameObject(long parentID)
	{
		GameObject newObject = GameObject(parentID);
		newObject.AddTransformComponent();
		return AddSceneObject(newObject);
	}

	void Scene::DeleteGameObject(long sceneObjectID)
	{
		GameObject *objectToDelete = FlatEngine::GetObjectById(sceneObjectID);

		//if (GetFocusedGameObjectID() == sceneObjectID)
		//	SetFocusedGameObjectID(-1);

		// If this GameObject was the primary camera, unset it as the primaryCamera and set primaryCamera to nullptr
		if (primaryCamera != nullptr && primaryCamera->GetParentID() == objectToDelete->GetID())
		{
			primaryCamera->SetPrimaryCamera(false);
			primaryCamera = nullptr;
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
			if (component->GetTypeString() == "Script")
			{				 
				ScriptComponent* scriptComponent = static_cast<ScriptComponent*>(component);
				if (scriptComponent->GetScriptInstance())
				{
					long scriptID = scriptComponent->GetScriptInstance()->GetOwnerID();
					//FlatEngine::F_Application->GetGameLoop()->RemoveScript(scriptID); // TODO
				}
			}
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
		// Must remove the primaryCamera pointer from the loaded scene before deleting the GameObject.
		std::shared_ptr<Scene> loadedScene = FlatEngine::GetLoadedScene();
		Camera *primaryCamera = loadedScene->GetPrimaryCamera();
		long cameraObjectID = -1;
		if (primaryCamera != nullptr)
			cameraObjectID = FlatEngine::GetObjectById(primaryCamera->GetParentID())->GetID();

		// Check for children
		if (objectToDelete->HasChildren())
		{
			// Call this function again on this objects children
			for (int c = 0; c < objectToDelete->GetChildren().size(); c++)
			{
				GameObject *child = FlatEngine::GetObjectById(objectToDelete->GetChildren()[c]);
				Scene::DeleteChildrenAndSelf(child);
			}
		}
		// Then delete this GameObject
		
		for (std::vector<GameObject>::iterator iter = sceneObjects.begin(); iter != sceneObjects.end();)
		{
			if ((*iter).GetID() == objectToDelete->GetID())
			{
				// Remove the primaryCamera pointer from the loaded scene if it is attached to the deleting GameObject
				if (objectToDelete->GetID() == cameraObjectID)
					loadedScene->RemovePrimaryCamera();

				(*iter).DeleteComponents();
				sceneObjects.erase(iter);
			}
			iter++;
		}
	}

	void Scene::IncrementGameObjectID()
	{
		nextGameObjectID += 1;
	}

	long Scene::GetNextGameObjectID()
	{
		IncrementGameObjectID();
		return nextGameObjectID;
	}

	void  Scene::IncrementComponentID()
	{
		nextComponentID += 1;
	}

	long  Scene::GetNextComponentID()
	{
		return nextComponentID;
		IncrementComponentID();
	}

	void Scene::SetPrimaryCamera(Camera* camera)
	{
		if (camera != nullptr)
		{
			// Remove the old primaryCamera
			if (primaryCamera != nullptr)
			{
				primaryCamera->SetPrimaryCamera(false);
			}

			primaryCamera = camera;
			primaryCamera->SetPrimaryCamera(true);
		}
		else
		{
			FlatEngine::LogString("Scene::SetPrimaryCamera() - The Camera pointer you tried to set as the primary Camera was a nullptr.");
		}
	}

	void Scene::RemovePrimaryCamera()
	{
		primaryCamera = nullptr;
	}

	Camera *Scene::GetPrimaryCamera()
	{
		return primaryCamera;
	}

	std::vector<std::pair<Collider*, Collider*>> Scene::GetColliderPairs()
	{
		return m_ECSManager.GetColliderPairs();
	}

	void Scene::OnPrefabInstantiated(std::vector<GameObject> children)
	{
		m_ECSManager.UpdateColliderPairs();
		m_ECSManager.UpdateActiveRigidBodies();
		m_ECSManager.InitializeScriptObjects(children);
	}

	void Scene::UpdateColliderPairs()
	{
		m_ECSManager.UpdateColliderPairs();
	}

	void Scene::InitializeScriptObjects()
	{
		//m_ECSManager.InitializeScriptObjects
	}

	Transform* Scene::AddTransform(Transform transform, long ownerID)
	{
		return m_ECSManager.AddTransform(transform, ownerID);
	}

	Sprite* Scene::AddSprite(Sprite sprite, long ownerID)
	{
		return m_ECSManager.AddSprite(sprite, ownerID);
	}

	Camera* Scene::AddCamera(Camera camera, long ownerID)
	{
		return m_ECSManager.AddCamera(camera, ownerID);
	}

	ScriptComponent* Scene::AddScriptComponent(ScriptComponent script, long ownerID)
	{
		return m_ECSManager.AddScriptComponent(script, ownerID);
	}

	GameScript* Scene::AddScript(GameObject owner, ScriptComponent scriptComponent, GameScript scriptInstance)
	{
		return m_ECSManager.AddScript(owner, scriptComponent, scriptInstance);
	}

	Canvas* Scene::AddCanvas(Canvas canvas, long ownerID)
	{
		return m_ECSManager.AddCanvas(canvas, ownerID);
	}

	Audio* Scene::AddAudio(Audio audio, long ownerID)
	{
		return m_ECSManager.AddAudio(audio, ownerID);
	}

	Text* Scene::AddText(Text text, long ownerID)
	{
		return m_ECSManager.AddText(text, ownerID);
	}

	Collider* Scene::AddCollider(Collider collider, long ownerID)
	{
		return m_ECSManager.AddCollider(collider, ownerID);
	}

	CompositeCollider* Scene::AddCompositeCollider(CompositeCollider collider, long ownerID)
	{
		return m_ECSManager.AddCompositeCollider(collider, ownerID);
	}

	BoxCollider* Scene::AddBoxCollider(BoxCollider collider, long ownerID)
	{
		return m_ECSManager.AddBoxCollider(collider, ownerID);
	}

	CircleCollider* Scene::AddCircleCollider(CircleCollider collider, long ownerID)
	{
		return m_ECSManager.AddCircleCollider(collider, ownerID);
	}

	Animation* Scene::AddAnimation(Animation animation, long ownerID)
	{
		return m_ECSManager.AddAnimation(animation, ownerID);
	}

	Button* Scene::AddButton(Button button, long ownerID)
	{
		return m_ECSManager.AddButton(button, ownerID);
	}

	RigidBody* Scene::AddRigidBody(RigidBody rigidBody, long ownerID)
	{
		return m_ECSManager.AddRigidBody(rigidBody, ownerID);
	}

	CharacterController* Scene::AddCharacterController(CharacterController characterController, long ownerID)
	{
		return m_ECSManager.AddCharacterController(characterController, ownerID);
	}

	Transform* Scene::GetTransform(long ownerID)
	{
		return m_ECSManager.GetTransform(ownerID);
	}

	Sprite* Scene::GetSprite(long ownerID)
	{
		return m_ECSManager.GetSprite(ownerID);
	}

	Camera* Scene::GetCamera(long ownerID)
	{
		return m_ECSManager.GetCamera(ownerID);
	}

	ScriptComponent* Scene::GetScriptComponent(long ownerID)
	{
		return m_ECSManager.GetScriptComponent(ownerID);
	}

	GameScript* Scene::GetScript(long ownerID, std::string name)
	{
		return m_ECSManager.GetScript(ownerID, name);
	}

	Canvas* Scene::GetCanvas(long ownerID)
	{
		return m_ECSManager.GetCanvas(ownerID);
	}

	Audio* Scene::GetAudio(long ownerID)
	{
		return m_ECSManager.GetAudio(ownerID);
	}

	Text* Scene::GetText(long ownerID)
	{
		return m_ECSManager.GetText(ownerID);
	}

	Collider* Scene::GetCollider(long ownerID)
	{
		return m_ECSManager.GetCollider(ownerID);
	}

	CompositeCollider* Scene::GetCompositeCollider(long ownerID)
	{
		return m_ECSManager.GetCompositeCollider(ownerID);
	}

	BoxCollider* Scene::GetBoxCollider(long ownerID)
	{
		return m_ECSManager.GetBoxCollider(ownerID);
	}

	CircleCollider* Scene::GetCircleCollider(long ownerID)
	{
		return m_ECSManager.GetCircleCollider(ownerID);
	}

	Animation* Scene::GetAnimation(long ownerID)
	{
		return m_ECSManager.GetAnimation(ownerID);
	}

	Button* Scene::GetButton(long ownerID)
	{
		return m_ECSManager.GetButton(ownerID);
	}

	RigidBody* Scene::GetRigidBody(long ownerID)
	{
		return m_ECSManager.GetRigidBody(ownerID);
	}

	CharacterController* Scene::GetCharacterController(long ownerID)
	{
		return m_ECSManager.GetCharacterController(ownerID);
	}

	std::vector<std::pair<Transform, long>> Scene::GetTransforms()
	{
		return m_ECSManager.GetTransforms();
	}
	std::vector<std::pair<Sprite, long>> Scene::GetSprites()
	{
		return m_ECSManager.GetSprites();
	}
	std::vector<std::pair<Camera, long>> Scene::GetCameras()
	{
		return m_ECSManager.GetCameras();
	}
	std::vector<std::pair<ScriptComponent, long>> Scene::GetScriptComponents()
	{
		return m_ECSManager.GetScriptComponents();
	}
	std::vector<std::pair<GameScript, long>> Scene::GetScripts()
	{
		return m_ECSManager.GetScripts();
	}
	std::vector<std::pair<Button, long>> Scene::GetButtons()
	{
		return m_ECSManager.GetButtons();
	}
	std::vector<std::pair<Canvas, long>> Scene::GetCanvases()
	{
		return m_ECSManager.GetCanvases();
	}
	std::vector<std::pair<Animation, long>> Scene::GetAnimations()
	{
		return m_ECSManager.GetAnimations();
	}
	std::vector<std::pair<Audio, long>> Scene::GetAudios()
	{
		return m_ECSManager.GetAudios();
	}
	std::vector<std::pair<Text, long>> Scene::GetTexts()
	{
		return m_ECSManager.GetTexts();
	}
	std::vector<std::pair<Collider, long>> Scene::GetColliders()
	{
		return m_ECSManager.GetColliders();
	}
	std::vector<std::pair<CompositeCollider, long>> Scene::GetCompositeColliders()
	{
		return m_ECSManager.GetCompositeColliders();
	}
	std::vector<std::pair<BoxCollider, long>> Scene::GetBoxColliders()
	{
		return m_ECSManager.GetBoxColliders();
	}
	std::vector<std::pair<CircleCollider, long>> Scene::GetCircleColliders()
	{
		return m_ECSManager.GetCircleColliders();
	}
	std::vector<std::pair<RigidBody, long>> Scene::GetRigidBodies()
	{
		return m_ECSManager.GetRigidBodies();
	}
	std::vector<std::pair<CharacterController, long>> Scene::GetCharacterControllers()
	{
		return m_ECSManager.GetCharacterControllers();
	}
}