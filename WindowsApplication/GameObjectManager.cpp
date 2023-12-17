#include "GameObjectManager.h"


namespace GameObjectManager {

	//Variables
	std::vector<GameObject> GameObjectManager::gameObjects;
	long currentID = 0;

	GameObject CreateGameObject()
	{
		GameObject newObject;
		GameObjectManager::gameObjects.push_back(newObject);
		return newObject;
	}


	void DeleteGameObject(GameObject gameObject)
	{

	}


	void IncrementID()
	{
		currentID += 1;
	}
}

