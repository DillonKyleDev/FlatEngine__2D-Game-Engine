#include "Scene.h"
#include "FlatEngine.h"
#include "Camera.h"


namespace FlatEngine
{
	Scene::Scene()
	{
		name = "New Scene";
		path = "";
		sceneObjects = std::vector<std::shared_ptr<GameObject>>();		
		animatorPreviewObjects = std::vector<std::shared_ptr<GameObject>>();
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

	void Scene::AddSceneObject(std::shared_ptr<GameObject> sceneObject)
	{
		sceneObjects.push_back(sceneObject);
	}

	std::vector<std::shared_ptr<GameObject>> Scene::GetSceneObjects()
	{
		return sceneObjects;
	}

	void Scene::SetAnimatorPreviewObjects(std::vector<std::shared_ptr<GameObject>> previewObjects)
	{
		animatorPreviewObjects = previewObjects;
	}

	std::vector<std::shared_ptr<GameObject>> &Scene::GetAnimatorPreviewObjects()
	{
		return animatorPreviewObjects;
	}

	std::shared_ptr<GameObject> Scene::GetObjectById(long ID)
	{
		if (ID != -1)
		{
			for (std::shared_ptr<GameObject> sceneObject : sceneObjects)
			{
				if (ID == sceneObject->GetID())
				{
					return sceneObject;
				}
			}
			for (std::shared_ptr<GameObject> animPreviewObject : animatorPreviewObjects)
			{
				if (ID == animPreviewObject->GetID())
				{
					return animPreviewObject;
				}
			}
			return nullptr;
		}
		else return nullptr;
	}

	std::shared_ptr<GameObject> Scene::GetObjectByName(std::string name)
	{
		for (int i = 0; i < sceneObjects.size(); i++)
		{
			if (name == sceneObjects[i]->GetName())
			{
				return sceneObjects[i];
			}
		}
		for (std::shared_ptr<GameObject> animPreviewObject : animatorPreviewObjects)
		{
			if (name == animPreviewObject->GetName())
			{
				return animPreviewObject;
			}
		}
		return std::shared_ptr<GameObject>(nullptr);
	}

	std::shared_ptr<GameObject> Scene::CreateGameObject(long parentID)
	{
		std::shared_ptr<GameObject> newObject = std::make_shared<GameObject>();
		AddSceneObject(newObject);
		return newObject;
	}

	void Scene::DeleteGameObject(long sceneObjectID)
	{
		std::shared_ptr<GameObject> objectToDelete = FlatEngine::GetObjectById(sceneObjectID);

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
			std::shared_ptr<GameObject> parent = FlatEngine::GetObjectById(parentID);
			parent->RemoveChild(sceneObjectID);
		}
		// Check for children and delete those as well
		Scene::DeleteChildrenAndSelf(objectToDelete);
	}

	// Recursive
	void Scene::DeleteChildrenAndSelf(std::shared_ptr<GameObject> objectToDelete)
	{
		// Must remove the primaryCamera pointer from the loaded scene before deleting the GameObject.
		std::shared_ptr<Scene> loadedScene = FlatEngine::GetLoadedScene();
		std::shared_ptr<Camera> primaryCamera = loadedScene->GetPrimaryCamera();
		long cameraObjectID = -1;
		if (primaryCamera != nullptr)
			cameraObjectID = FlatEngine::GetObjectById(primaryCamera->GetParentID())->GetID();

		// Check for children
		if (objectToDelete->HasChildren())
		{
			// Call this function again on this objects children
			for (int c = 0; c < objectToDelete->GetChildren().size(); c++)
			{
				std::shared_ptr<GameObject> child = FlatEngine::GetObjectById(objectToDelete->GetChildren()[c]);
				Scene::DeleteChildrenAndSelf(child);
			}
		}
		// Then delete this GameObject
		for (int i = 0; i < sceneObjects.size(); i++)
		{
			if (sceneObjects[i]->GetID() == objectToDelete->GetID())
			{
				// Remove the primaryCamera pointer from the loaded scene if it is attached to the deleting GameObject
				if (objectToDelete->GetID() == cameraObjectID)
					loadedScene->RemovePrimaryCamera();

				sceneObjects.erase(sceneObjects.begin() + i);
			}
		}
	}

	void Scene::IncrementGameObjectID()
	{
		nextGameObjectID += 1;
	}

	long Scene::GetNextGameObjectID()
	{
		for (std::shared_ptr<GameObject> sceneObject : sceneObjects)
		{
			if (sceneObject->GetID() == nextGameObjectID)
			{
				IncrementGameObjectID();
				GetNextGameObjectID();
			}
		}
		for (std::shared_ptr<GameObject> animationObject : animatorPreviewObjects)
		{
			if (animationObject->GetID() == nextGameObjectID)
			{
				IncrementGameObjectID();
				GetNextGameObjectID();
			}
		}

		return nextGameObjectID;
	}

	void  Scene::IncrementComponentID()
	{
		nextComponentID += 1;
	}

	long  Scene::GetNextComponentID()
	{
		return nextComponentID;
	}

	void Scene::SetPrimaryCamera(std::shared_ptr<Camera> camera)
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

	std::shared_ptr<Camera> Scene::GetPrimaryCamera()
	{
		return primaryCamera;
	}
}