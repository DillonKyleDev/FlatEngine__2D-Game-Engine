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
		void SaveScene(Scene* scene, std::string filename);
		void SaveCurrentScene();
		void LoadScene(std::string name);
		Scene* GetLoadedScene();
		std::string GetLoadedScenePath();
		void SaveAnimationPreviewObjects();
		void LoadAnimationPreviewObjects();

		static json CreateJsonFromObject(GameObject gameObject);
		static GameObject CreateObjectFromJson(json objectJson);
		static float CheckJsonFloat(json obj, std::string checkFor, std::string loadedName);
		static int CheckJsonInt(json obj, std::string checkFor, std::string loadedName);
		static long CheckJsonLong(json obj, std::string checkFor, std::string loadedName);
		static bool CheckJsonBool(json obj, std::string checkFor, std::string loadedName);
		static bool JsonContains(json obj, std::string checkFor, std::string loadedName);
		static std::string CheckJsonString(json obj, std::string checkFor, std::string loadedName);

	private:
		Scene m_loadedScene;
		std::string m_loadedScenePath;
		std::vector<GameObject*> m_animatorPreviewObjects;
	};
}

