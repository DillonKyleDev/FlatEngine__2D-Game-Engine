#pragma once
#include <vector>
#include <string>
#include "GameObject.h"


namespace FlatEngine 
{
	class GameObjectManager
	{
	public:
		GameObjectManager();
		~GameObjectManager();

		GameObject CreateGameObject();
		void DeleteGameObject(GameObject gameObject);
		void IncrementID();
		std::vector<GameObject> &GetGameObjects();
		long GetCurrentID();

	private:
		std::vector<GameObject> gameObjects;
		long currentID;
	};
}