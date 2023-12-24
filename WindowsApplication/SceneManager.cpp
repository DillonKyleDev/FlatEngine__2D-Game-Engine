#include "SceneManager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "json.hpp"
using json = nlohmann::json;
using namespace nlohmann::literals;


namespace FlatEngine
{
	SceneManager::SceneManager()
	{

	}

	SceneManager::~SceneManager()
	{

	}

	void SceneManager::SaveScene(Scene scene)
	{
		// Declare file and input stream
		std::ofstream file_obj;
		std::ifstream ifstream("SavedScenes.json");

		// Open file in in mode
		file_obj.open("SavedScenes.json", std::ios::in);

		// Variable to save the current file data into
		std::string fileContent = "";

		// Loop through the file line by line and save the data
		if (file_obj)
		{
			std::string line;
			while (!ifstream.eof()) {
				std::getline(ifstream, line);
				fileContent.append(line + "\n");
			}
		}

		// Close the file after reading
		file_obj.close();

		// Delete old contents of the file
		file_obj.open("SavedScenes.json", std::ofstream::out | std::ofstream::trunc);
		file_obj.close();

		// Opening file in append mode
		file_obj.open("SavedScenes.json", std::ios::app);

		// Here, this will be a loop that goes through each GameObject and inserts the
		// values into a json object with the important data. That will then be put into
		// the SceneObjects["Scene GameObjects"] array and then that will be pushed into
		// the empty newFileObject json object to be saved to the file.
		static json jsonObject = json::object(
		{
			{ "name", "Player" },
			{ "components",
				{
					{"type", "Transform"},
					{"xPos", 5.68},
					{"yPos", 23.40}
				}
			}
		});
		// += this current loop iterations GameObject saved object V
		//jsonObject.push_back(jsonObject);
		//jsonObject += json::object_t::value_type({ "GameObject", {{ "name", "Player1" }} });

		// Add json object full of GameObjects to the main array object
		json SceneObjects = json::array({jsonObject});

		// Recreate the GameObjects json object and add the array as the content
		static json newFileObject = json::object({});
		newFileObject += json::object_t::value_type({ "Scene GameObjects", SceneObjects });

		// Add the GameObjects object contents to the file
		file_obj << newFileObject.dump(4).c_str() << std::endl;

		// Close the file
		file_obj.close();
	}

	void SceneManager::LoadScene(std::string fileName)
	{

	}
}


// Alternative file writing method
//file_obj.write(fileContent.c_str(), fileContent.size());