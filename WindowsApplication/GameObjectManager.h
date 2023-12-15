#pragma once
#include <vector>
#include <string>
#include "GameObject.h"

namespace GameObjectManager
{
	extern GameObject CreateGameObject(std::string name);
	extern void DeleteGameObject(GameObject gameObject);	
	extern void IncrementID();

	//Variables
	extern std::vector<GameObject> gameObjects;
	extern long currentID;
};