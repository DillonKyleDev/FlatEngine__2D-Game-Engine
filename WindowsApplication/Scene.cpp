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


	void Scene::CreateGameObject()
	{
		GameObject *newObject = new GameObject();
		this->AddSceneObject(newObject);
	}

	void Scene::DeleteGameObject(int sceneObjectIndex)
	{
		// If this GameObject was the primary camera, unset it as the primaryCamera and set this->primaryCamera to nullptr
		if (this->primaryCamera != nullptr && this->primaryCamera->GetParentID() == this->sceneObjects[sceneObjectIndex]->GetID())
		{
			this->primaryCamera->SetPrimaryCamera(false);
			this->primaryCamera = nullptr;
		}
		this->sceneObjects.erase(this->sceneObjects.begin() + sceneObjectIndex);
	}

	void Scene::IncrementID()
	{
		this->currentID += 1;
	}

	long Scene::GetCurrentID()
	{
		return this->currentID;
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