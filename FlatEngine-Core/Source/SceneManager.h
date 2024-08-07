#pragma once
#include <string>
#include <memory>
#include "Scene.h"
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
		bool LoadScene(std::string filepath);
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

