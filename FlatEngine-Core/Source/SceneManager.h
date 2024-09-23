#pragma once
#include "Scene.h"

#include <string>
#include <vector>
#include "json.hpp"

using json = nlohmann::json;
using namespace nlohmann::literals;


namespace FlatEngine
{
	class GameObject;

	class SceneManager
	{
	public:
		SceneManager();
		~SceneManager();

		Scene* CreateNewScene();
		void SaveScene(Scene* scene, std::string filepath);
		void SaveCurrentScene();
		bool LoadScene(std::string filepath, bool b_isTemp = false);		
		Scene* GetLoadedScene();
		std::string GetLoadedScenePath();
		void SaveAnimationPreviewObjects();
		void LoadAnimationPreviewObjects();

	private:
		Scene m_loadedScene;
		std::string m_loadedScenePath;
		std::vector<GameObject*> m_animatorPreviewObjects;
	};
}

