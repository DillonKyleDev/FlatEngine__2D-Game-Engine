#include "GameObjectManager.h"

GameObject GameObjectManager::CreateGameObject(std::string name)
{
	GameObject newObject;
	newObject.SetName(name);
	GameObjectManager::gameObjects.push_back(newObject);
	return newObject;
}


void GameObjectManager::DeleteGameObject(GameObject gameObject)
{

}


void GameObjectManager::IncrementID()
{
	GameObjectManager::currentID += 1;
}


//Variables
std::vector<GameObject> GameObjectManager::gameObjects;
long GameObjectManager::currentID = 0;

