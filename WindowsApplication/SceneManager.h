#pragma once
#include "Scene.h"

namespace FlatEngine
{
	class SceneManager
	{
	public:
		SceneManager();
		~SceneManager();

		std::shared_ptr<Scene> CreateNewScene();
		void SaveScene(std::shared_ptr<Scene> scene, std::string filename);
		void LoadScene(std::string name);
		std::shared_ptr<Scene> GetLoadedScene();

	private:
		std::shared_ptr<Scene> loadedScene;
	};
}

