#pragma once
#include <vector>
#include "GameObject.h"


namespace FlatEngine
{
	class Scene
	{
	public:
		Scene();
		~Scene();

		void SetName(std::string name);
		void AddSceneObject(GameObject sceneObject);
		void GetSceneObjects();
		std::string name;
		int num;


	private:
		
		std::vector<GameObject> sceneObjects;
	};
	
	extern void SaveScene(Scene scene);
	extern void LoadScene(std::string name);
}