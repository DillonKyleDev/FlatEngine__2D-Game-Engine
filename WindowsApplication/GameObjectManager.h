#pragma once
#include <vector>
#include <string>
#include "GameObject.h"


namespace GameObjectManager
{
	extern GameObject CreateGameObject();
	extern void DeleteGameObject(GameObject gameObject);	
	extern void IncrementID();

	//Variables
	extern std::vector<GameObject> gameObjects;
	extern long currentID;
};