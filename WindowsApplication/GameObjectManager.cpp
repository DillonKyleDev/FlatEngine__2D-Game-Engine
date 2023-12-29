#include "GameObjectManager.h"
#include "FlatEngine.h"


namespace FlatEngine {

	GameObjectManager::GameObjectManager()
	{
		std::vector<GameObject> gameObjects;
		this->currentID = 0;
	}

	GameObjectManager::~GameObjectManager()
	{

	}

	GameObject GameObjectManager::CreateGameObject()
	{
		GameObject newObject;
		this->gameObjects.push_back(newObject);
		FlatEngine::sceneManager.GetLoadedScene().AddSceneObject(newObject);
		return newObject;
	}

	void GameObjectManager::DeleteGameObject(GameObject gameObject)
	{

	}

	void GameObjectManager::IncrementID()
	{
		this->currentID += 1;
	}

	std::vector<GameObject> &GameObjectManager::GetGameObjects()
	{
		return this->gameObjects;
	}

	long GameObjectManager::GetCurrentID()
	{
		return this->currentID;
	}
}