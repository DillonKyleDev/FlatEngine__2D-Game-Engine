#include "Scene.h"
#include <iostream>
#include <fstream>
#include "json.hpp"
using json = nlohmann::json;


namespace FlatEngine
{
	void SaveScene(Scene scene)
	{
		//std::ifstream f("SavedScenes.json");

		std::string openBracket = "{";
		std::string closeBracket = "}";

		// Declare file
		std::ofstream file_obj;

		//json data = json::parse(f);
		

		json jsonObject = {
		  {"Test#1", 3.141},
		  {"happy", true},
		  {"name", "Niels"},
		  {"nothing", nullptr},
		  {"answer", {
			{"everything", 42}
		  }},
		  {"list", {1, 0, 2}},
		  {"object", {
			{"currency", "USD"},
			{"value", 42.99}
		  }}
		};

		std::string dumped = jsonObject.dump(4);

		// Opening file in						    append mode
		file_obj.open("SavedScenes.json", std::ios::app);

		// Open bracket
		file_obj.write(openBracket.c_str(), 1);


		file_obj.write(dumped.c_str(), dumped.size());

		// Close bracket
		file_obj.write(closeBracket.c_str(), 1);



        //std::string str = "Level_1";
        //scene.SetName(str);

        // Writing the object's data in file
        //file_obj.write(scene.name.c_str(), scene.name.size());
		
        //close the file
        //It's always a good practice to close the file after opening them
        file_obj.close();
	}

	void LoadScene(std::string name)
	{

	}

	Scene::Scene()
	{
        this->name = "New Scene";
        this->sceneObjects = {};
		int num = 3;
	}

	Scene::~Scene()
	{

	}

    void Scene::SetName(std::string name)
    {
        this->name = name;
    }

	void Scene::AddSceneObject(GameObject sceneObject)
	{

	}

	void Scene::GetSceneObjects()
	{

	}
}