#include "Scene.h"
#include "FlatEngine.h"


namespace FlatEngine
{
	Scene::Scene()
	{
        this->name = "New Scene";
        this->sceneObjects = {};
		this->primaryCamera = nullptr;
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

	void Scene::AddSceneObject(GameObject *sceneObject)
	{
		this->sceneObjects.push_back(sceneObject);
	}

	std::vector<GameObject*> Scene::GetSceneObjects()
	{
		return this->sceneObjects;
	}


	GameObject* Scene::GetObjectById(long ID)
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


	GameObject* Scene::CreateGameObject(long parentID)
	{
		GameObject *newObject = new GameObject(parentID);
		this->AddSceneObject(newObject);
		return newObject;
	}

	void Scene::DeleteGameObject(int sceneObjectID)
	{
		// If this GameObject was the primary camera, unset it as the primaryCamera and set this->primaryCamera to nullptr
		if (this->primaryCamera != nullptr && this->primaryCamera->GetParentID() == this->sceneObjects[sceneObjectID]->GetID())
		{
			this->primaryCamera->SetPrimaryCamera(false);
			this->primaryCamera = nullptr;
		}
		this->sceneObjects.erase(this->sceneObjects.begin() + sceneObjectID);
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

	void Scene::SetPrimaryCamera(Camera* camera)
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

	Camera* Scene::GetPrimaryCamera()
	{
		return this->primaryCamera;
	}
}