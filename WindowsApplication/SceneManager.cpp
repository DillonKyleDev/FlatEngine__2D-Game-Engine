#include "SceneManager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <array>
#include <SDL.h>
#include "FlatEngine.h"
#include "Transform.h"
#include "Sprite.h"
#include "Camera.h"
#include "ScriptComponent.h"
#include "json.hpp"
using json = nlohmann::json;
using namespace nlohmann::literals;



namespace FlatEngine
{
	SceneManager::SceneManager()
	{
		this->loadedScene = new Scene();
	}

	SceneManager::~SceneManager()
	{

	}

	void SceneManager::SaveScene(Scene *scene)
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
 
		// Array that will hold our gameObject json objects
		json sceneObjectsJsonArray;

		std::vector<GameObject*> sceneObjects = scene->GetSceneObjects();
		if (sceneObjects.size() > 0)
		{
			for (int i = 0; i < sceneObjects.size(); i++)
			{
				// Declare components array json object for components
				json componentsArray = json::array();

				// Get the object we'll be working with and it's components
				GameObject* currentObject = sceneObjects[i];
				std::vector<FlatEngine::Component*> components = currentObject->GetComponents();

				for (int j = 0; j < components.size(); j++)
				{
					std::string typeString = components[j]->GetTypeString();

					if (typeString != "Null")
					{
						std::string data = components[j]->GetData();
						componentsArray.push_back(json::parse(data));
					}
				}

				// Declare children array json object for children
				json childrenArray = json::array();

				for (int c = 0; c < currentObject->GetChildren().size(); c++)
				{
					childrenArray.push_back(currentObject->GetChildren()[c]);
				}

				// Get object name
				std::string objectName = currentObject->GetName();

				// Create Game Object Json data object
				json gameObjectJson = json::object({ 
					{"name", objectName}, 
					{"id", currentObject->GetID()},
					{"parent", currentObject->GetParentID()},
					{"children", childrenArray}, 
					{"components", componentsArray} 
				});

				// Adding elements to objects in this way doesn't seem to save correctly. ^ Use the way above ^
				//gameObjectJson += json::object_t::value_type("name", objectName);
				//gameObjectJson += json::object_t::value_type("components", componentsArray);

				// Finally, add the gameObject json to the sceneObjectsJsonArray
				sceneObjectsJsonArray.push_back(gameObjectJson);
			}
		}
		else
		{
			sceneObjectsJsonArray.push_back("NULL");
		}


		// Recreate the GameObjects json object and add the array as the content
		json newFileObject = json::object({ {"Scene GameObjects", sceneObjectsJsonArray } });

		// Add the GameObjects object contents to the file
		file_obj << newFileObject.dump(4).c_str() << std::endl;

		// Close the file
		file_obj.close();
	}

	void SceneManager::LoadScene(std::string fileName)
	{
		delete this->loadedScene;
		this->loadedScene = nullptr;

		Scene *freshScene = new Scene();
		this->loadedScene = freshScene;

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

		// Go from string to json object
		json fileContentJson = json::parse(fileContent);

		if (fileContentJson["Scene GameObjects"][0] != "NULL")
		{
			auto firstObjectName = fileContentJson["Scene GameObjects"];
			
			//Getting data from the json 
			//std::string name = firstObjectName[0]["name"];

			// Loop through the saved GameObjects in the JSON file
			for (int i = 0; i < fileContentJson["Scene GameObjects"].size(); i++)
			{
				// Default values
				std::string loadedName = "Name";
				long loadedID = -1;
				long loadedParentID = -1;
				std::vector<long> loadedChildrenIDs = std::vector<long>();

				// Get data from the loaded object
				json currentObjectJson = fileContentJson["Scene GameObjects"][i];
				// Check name key exists
				if (currentObjectJson.contains("name"))
					loadedName = currentObjectJson["name"];
				else
					FlatEngine::LogInt(i, "SceneManager::Load() - Saved scene json does not contain a value for 'name' in object: ");
				// Check ID key exists
				if (currentObjectJson.contains("id"))
					loadedID = currentObjectJson["id"];
				else
					FlatEngine::LogInt(i, "SceneManager::Load() - Saved scene json does not contain a value for 'id' in object: ");
				// Check parent key exists
				if (currentObjectJson.contains("parent"))
					loadedParentID = currentObjectJson["parent"];
				else
					FlatEngine::LogInt(i, "SceneManager::Load() - Saved scene json does not contain a value for 'parent' in object: ");
				// Check children key exists
				if (currentObjectJson.contains("children"))
				{
					// Add children
					for (int c = 0; c < currentObjectJson["children"].size(); c++)
					{
						FlatEngine::LogFloat(c, "Current Child Number: ");
						loadedChildrenIDs.push_back(currentObjectJson["children"][c]);
					}
				}
				else
					FlatEngine::LogInt(i, "SceneManager::Load() - Saved scene json does not contain a value for 'children' in object: ");
				

				// Create new GameObject to load the data into
				GameObject* loadedObject = new GameObject();
				loadedObject->SetName(loadedName);
				loadedObject->SetID(loadedID);
				loadedObject->SetParentID(loadedParentID);

				// Add children
				for (int c = 0; c < currentObjectJson["children"].size(); c++)
				{
					FlatEngine::LogFloat(c, "Current Child Number: ");
					loadedObject->AddChild(loadedChildrenIDs[c]);
				}

				// Loop through the components in this GameObjects json
				for (int j = 0; j < currentObjectJson["components"].size(); j++)
				{
					std::string type = "";
					if (currentObjectJson["components"][j].contains("type"))
						type = currentObjectJson["components"][j]["type"];
					else
						FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'type' in object: ");

					//Add each loaded component to the newly created GameObject
					if (type == "Transform")
					{
						// Default values
						long id = -1;
						bool _isCollapsed = true;
						float xScale = 1;
						float yScale = 1;
						float xPos = 0;
						float yPos = 0;
						float rotation = 0;

						FlatEngine::Transform* newTransform = static_cast<FlatEngine::Transform*>(loadedObject->AddComponent(Component::ComponentTypes::Transform));

						// Load ID
						if (currentObjectJson["components"][j].contains("id"))
							id = currentObjectJson["components"][j]["id"];
						else
							FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'id' in object: ");
						// Load _isCollapsed
						if (currentObjectJson["components"][j].contains("_isCollapsed"))
							_isCollapsed = currentObjectJson["components"][j]["_isCollapsed"];
						else
							FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for '_isCollapsed' in object: ");

						// position
						if (currentObjectJson["components"][j].contains("xPos"))
							xPos = currentObjectJson["components"][j]["xPos"];
						else
							FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'xPos' in object: ");

						if (currentObjectJson["components"][j].contains("yPos"))
							yPos = currentObjectJson["components"][j]["yPos"];
						else
							FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'yPos' in object: ");

						// scale
						if (currentObjectJson["components"][j].contains("xScale"))
							xScale = currentObjectJson["components"][j]["xScale"];
						else
							FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'xScale' in object: ");

						if (currentObjectJson["components"][j].contains("yScale"))
							yScale = currentObjectJson["components"][j]["yScale"];
						else
							FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'yScale' in object: ");

						// rotation
						if (currentObjectJson["components"][j].contains("rotation"))
							rotation = currentObjectJson["components"][j]["rotation"];
						else
							FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'rotation' in object: ");
						
						newTransform->SetID(id);
						newTransform->SetCollapsed(_isCollapsed);
						newTransform->SetPosition(Vector2(xPos, yPos));
						newTransform->SetScale(Vector2(xScale, yScale));
						newTransform->SetRotation(rotation);
					}
					else if (type == "Sprite")
					{
						FlatEngine::Sprite* newSprite = static_cast<FlatEngine::Sprite*>(loadedObject->AddComponent(Component::ComponentTypes::Sprite));

						// Default values
						long id = -1;
						bool _isCollapsed = true;
						std::string path = "";
						float xOffset = 0;
						float yOffset = 0;

						// Load ID
						if (currentObjectJson["components"][j].contains("id"))
							id = currentObjectJson["components"][j]["id"];
						else
							FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'id' in object: ");
						// Load _isCollapsed
						if (currentObjectJson["components"][j].contains("_isCollapsed"))
							_isCollapsed = currentObjectJson["components"][j]["_isCollapsed"];
						else
							FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for '_isCollapsed' in object: ");

						if (currentObjectJson["components"][j].contains("texture"))
							path = currentObjectJson["components"][j]["texture"];
						else
							FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'texture' in object: ");
						if (currentObjectJson["components"][j].contains("xOffset"))
							xOffset = currentObjectJson["components"][j]["xOffset"];
						else
							FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'xOffset' in object: ");
						if (currentObjectJson["components"][j].contains("yOffset"))
							yOffset = currentObjectJson["components"][j]["yOffset"];
						else
							FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'yOffset' in object: ");

						newSprite->SetID(id);
						newSprite->SetCollapsed(_isCollapsed);
						newSprite->SetTexture(path);
						newSprite->SetOffset(Vector2(xOffset, yOffset));
					}
					else if (type == "Camera")
					{
						FlatEngine::Camera* newCamera = static_cast<FlatEngine::Camera*>(loadedObject->AddComponent(Component::ComponentTypes::Camera));

						// Default values
						long id = -1;
						bool _isCollapsed = true;
						float width = 400;
						float height = 300;
						bool _isPrimaryCamera = false;
						float zoom = 1;
						float f_red = 1;
						float f_green = 1;
						float f_blue = 1;
						float f_alpha = 1;

						// Load ID
						if (currentObjectJson["components"][j].contains("id"))
							id = currentObjectJson["components"][j]["id"];
						else
							FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'id' in object: ");
						// Load _isCollapsed
						if (currentObjectJson["components"][j].contains("_isCollapsed"))
							_isCollapsed = currentObjectJson["components"][j]["_isCollapsed"];
						else
							FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for '_isCollapsed' in object: ");

						// Width and height
						if (currentObjectJson["components"][j].contains("width"))
							width = currentObjectJson["components"][j]["width"];
						else
							FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'width' in object: ");
						if (currentObjectJson["components"][j].contains("height"))
							height = currentObjectJson["components"][j]["height"];
						else
							FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'height' in object: ");
						// Is primaryCamera
						if (currentObjectJson["components"][j].contains("_isPrimaryCamera"))
							_isPrimaryCamera = currentObjectJson["components"][j]["_isPrimaryCamera"];
						else
							FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for '_isPrimaryCamera' in object: ");
						// Zoom
						if (currentObjectJson["components"][j].contains("zoom"))
							zoom = currentObjectJson["components"][j]["zoom"];
						else
							FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'zoom' in object: ");
						// Frustrum RGBA values
						if (currentObjectJson["components"][j].contains("frustrumRed"))
							f_red = currentObjectJson["components"][j]["frustrumRed"];
						else
							FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'frustrumRed' in object: ");
						if (currentObjectJson["components"][j].contains("frustrumGreen"))
							f_green = currentObjectJson["components"][j]["frustrumGreen"];
						else
							FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'frustrumGreen' in object: ");
						if (currentObjectJson["components"][j].contains("frustrumBlue"))
							f_blue = currentObjectJson["components"][j]["frustrumBlue"];
						else
							FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'frustrumBlue' in object: ");
						if (currentObjectJson["components"][j].contains("frustrumAlpha"))
							f_alpha = currentObjectJson["components"][j]["frustrumAlpha"];
						else
							FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'frustrumAlpha' in object: ");

						ImVec4 frustrumColor = ImVec4(f_red, f_green, f_blue, f_alpha);

						newCamera->SetID(id);
						newCamera->SetCollapsed(_isCollapsed);
						newCamera->SetDimensions(width, height);
						newCamera->SetPrimaryCamera(_isPrimaryCamera);
						newCamera->SetZoom(zoom);
						newCamera->SetFrustrumColor(frustrumColor);

						// If this camera is the primary camera, set it in the Scene as the primaryCamera
						if (_isPrimaryCamera)
							freshScene->SetPrimaryCamera(newCamera);
					}
					else if (type == "Script")
					{
						FlatEngine::ScriptComponent* newScript = static_cast<FlatEngine::ScriptComponent*>(loadedObject->AddComponent(Component::ComponentTypes::Script));

						// Default values
						long id = -1;
						bool _isCollapsed = true;
						std::string attachedScript = "";
						bool _isActive = false;

						// Load ID
						if (currentObjectJson["components"][j].contains("id"))
							id = currentObjectJson["components"][j]["id"];
						else
							FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'id' in object: ");
						// Load _isCollapsed
						if (currentObjectJson["components"][j].contains("_isCollapsed"))
							_isCollapsed = currentObjectJson["components"][j]["_isCollapsed"];
						else
							FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for '_isCollapsed' in object: ");

						if (currentObjectJson["components"][j].contains("attachedScript"))
							attachedScript = currentObjectJson["components"][j]["attachedScript"];
						else
							FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'attachedScript' in object: ");

						if (currentObjectJson["components"][j].contains("_isActive"))
							_isActive = currentObjectJson["components"][j]["_isActive"];
						else
							FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for '_isActive' in object: ");

						newScript->SetID(id);
						newScript->SetCollapsed(_isCollapsed);
						newScript->SetAttachedScript(attachedScript);
						newScript->SetActive(_isActive);
					}
					else if (type == "Button")
					{
						FlatEngine::Button* newButton = static_cast<FlatEngine::Button*>(loadedObject->AddComponent(ComponentTypes::Button));

						// Default values
						long id = -1;
						bool _isCollapsed = true;
						bool _isActive = true;
						float activeWidth = 5;
						float activeHeight = 3;
						Vector2 activeOffset = { 0, 0 };
						std::string attachedScript = "";

						// Load ID
						if (currentObjectJson["components"][j].contains("id"))
							id = currentObjectJson["components"][j]["id"];
						else
							FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'id' in object: ");
						// Load _isCollapsed
						if (currentObjectJson["components"][j].contains("_isCollapsed"))
							_isCollapsed = currentObjectJson["components"][j]["_isCollapsed"];
						else
							FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for '_isCollapsed' in object: ");

						if (currentObjectJson["components"][j].contains("_isActive"))
							_isActive = currentObjectJson["components"][j]["_isActive"];
						else
							FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for '_isActive' in object: ");

						if (currentObjectJson["components"][j].contains("activeWidth"))
							activeWidth = currentObjectJson["components"][j]["activeWidth"];
						else
							FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'activeWidth' in object: ");

						if (currentObjectJson["components"][j].contains("activeHeight"))
							activeHeight = currentObjectJson["components"][j]["activeHeight"];
						else
							FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'activeHeight' in object: ");

						if (currentObjectJson["components"][j].contains("activeOffsetX"))
							activeOffset.x = currentObjectJson["components"][j]["activeOffsetX"];
						else
							FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'activeOffsetX' in object: ");

						if (currentObjectJson["components"][j].contains("activeOffsetY"))
							activeOffset.y = currentObjectJson["components"][j]["activeOffsetY"];
						else
							FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'activeOffsetY' in object: ");
						
						newButton->SetID(id);
						newButton->SetCollapsed(_isCollapsed);
						newButton->SetActive(_isActive);
						newButton->SetActiveDimensions(activeWidth, activeHeight);
						newButton->SetActiveOffset(activeOffset);						
					}
				}
				
				// Add created GameObject to our freshScene
				freshScene->AddSceneObject(loadedObject);
			}
		}

		// Assign our freshScene to the SceneManagers currently loadedScene member variable
		this->loadedScene = freshScene;
	}

	Scene *SceneManager::GetLoadedScene()
	{
		return this->loadedScene;
	}
}


// Alternative file writing method
//file_obj.write(fileContent.c_str(), fileContent.size());