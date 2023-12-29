#include "Scene.h"


namespace FlatEngine
{
	Scene::Scene()
	{
        this->name = "New Scene";
        this->sceneObjects = {};
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

	void Scene::AddSceneObject(GameObject sceneObject)
	{
		this->sceneObjects.push_back(sceneObject);
	}

	std::vector<GameObject> Scene::GetSceneObjects()
	{
		return this->sceneObjects;
	}


	GameObject Scene::CreateGameObject()
	{
		GameObject newObject;
		this->AddSceneObject(newObject);
		return newObject;
	}

	void Scene::DeleteGameObject(GameObject gameObject)
	{

	}

	void Scene::IncrementID()
	{
		this->currentID += 1;
	}

	std::vector<GameObject>& Scene::GetGameObjects()
	{
		return this->sceneObjects;
	}

	long Scene::GetCurrentID()
	{
		return this->currentID;
	}
}