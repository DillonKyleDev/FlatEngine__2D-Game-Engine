#include "Scene.h"
#include "FlatEngine.h"
#include "Application.h"
#include "Camera.h"
#include "GameObject.h"
#include "ScriptComponent.h"
#include "GameLoop.h"

namespace FlatEngine
{
	Scene::Scene()
	{
		name = "New Scene";
		path = "";
		sceneObjects = std::vector<GameObject>();		
		animatorPreviewObjects = std::vector<GameObject*>();
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
		sceneObjects.push_back(sceneObject);
		return &sceneObject;
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
		return AddSceneObject(&newObject);
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
					FlatEngine::F_Application->GetGameLoop()->RemoveScript(scriptID);
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

		if (_hadRigidBody)
			F_Application->GetGameLoop()->UpdateActiveRigidBodies();
		if (_hadCollider)
			F_Application->GetGameLoop()->UpdateActiveColliders();
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
}