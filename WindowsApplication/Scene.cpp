#include "Scene.h"
#include "FlatEngine.h"


namespace FlatEngine
{
	Scene::Scene()
	{
        this->name = "New Scene";
        this->sceneObjects = std::vector< std::shared_ptr<GameObject>>();
		this->primaryCamera;
	}

	Scene::~Scene()
	{
	}

    void Scene::SetName(std::string name)
    {
        this->name = name;
    }

	std::string Scene::GetName()
	{
		return this->name;
	}

	void Scene::AddSceneObject(std::shared_ptr<GameObject> sceneObject)
	{
		this->sceneObjects.push_back(sceneObject);
	}

	std::vector<std::shared_ptr<GameObject>> Scene::GetSceneObjects()
	{
		return this->sceneObjects;
	}


	std::shared_ptr<GameObject> Scene::GetObjectById(long ID)
	{
		ID = ID;
		for (int i = 0; i < this->sceneObjects.size(); i++)
		{
			if (ID == sceneObjects[i]->GetID())
			{
				return sceneObjects[i];
			}
		}
		return nullptr;
	}


	std::shared_ptr<GameObject> Scene::CreateGameObject(long parentID)
	{
		std::shared_ptr<GameObject> newObject(new GameObject(parentID));
		this->AddSceneObject(newObject);
		return newObject;
	}

	void Scene::DeleteGameObject(long sceneObjectID)
	{
		std::shared_ptr<GameObject> objectToDelete = FlatEngine::GetObjectById(sceneObjectID);

		// If this GameObject was the primary camera, unset it as the primaryCamera and set this->primaryCamera to nullptr
		if (this->primaryCamera != nullptr && this->primaryCamera->GetParentID() == objectToDelete->GetID())
		{
			this->primaryCamera->SetPrimaryCamera(false);
			this->primaryCamera = nullptr;
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
		long cameraObjectID = FlatEngine::GetObjectById(primaryCamera->GetParentID())->GetID();

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
		for (int i = 0; i < this->sceneObjects.size(); i++)
		{
			if (this->sceneObjects[i]->GetID() == objectToDelete->GetID())
			{
				// Remove the primaryCamera pointer from the loaded scene if it is attached to the deleting GameObject
				if (objectToDelete->GetID() == cameraObjectID)
					loadedScene->RemovePrimaryCamera();

				this->sceneObjects.erase(this->sceneObjects.begin() + i);
			}
		}
	}


	void Scene::IncrementGameObjectID()
	{
		this->nextGameObjectID += 1;
	}

	long Scene::GetNextGameObjectID()
	{
		return this->nextGameObjectID;
	}

	void  Scene::IncrementComponentID()
	{
		this->nextComponentID += 1;
	}

	long  Scene::GetNextComponentID()
	{
		return this->nextComponentID;
	}

	void Scene::SetPrimaryCamera(std::shared_ptr<Camera> camera)
	{
		if (camera != nullptr)
		{
			// Remove the old primaryCamera
			if (this->primaryCamera != nullptr)
			{
				this->primaryCamera->SetPrimaryCamera(false);
			}

			this->primaryCamera = camera;
			this->primaryCamera->SetPrimaryCamera(true);
		}
		else
		{
			FlatEngine::LogString("Scene::SetPrimaryCamera() - The Camera pointer you tried to set as the primary Camera was a nullptr.");
		}
	}

	void Scene::RemovePrimaryCamera()
	{
		this->primaryCamera = nullptr;
	}

	std::shared_ptr<Camera> Scene::GetPrimaryCamera()
	{
		return this->primaryCamera;
	}
}