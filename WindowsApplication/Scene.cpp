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


	void Scene::CreateGameObject()
	{
		GameObject *newObject = new GameObject();
		this->AddSceneObject(newObject);
	}

	void Scene::DeleteGameObject(int sceneObjectIndex)
	{
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
				FlatEngine::LogString("Primary camera removed");
			}

			this->primaryCamera = camera;
			this->primaryCamera->SetPrimaryCamera(true);
			FlatEngine::LogString("Primary camera set.");
		}
		else
		{
			FlatEngine::LogString("The Camera pointer you tried to set as the primary Camera was a nullptr.");
		}
	}

	void Scene::RemovePrimaryCamera()
	{
		this->primaryCamera = nullptr;
		FlatEngine::LogString("Primary camera removed");
	}

	Camera* Scene::GetPrimaryCamera()
	{
		return this->primaryCamera;
	}
}