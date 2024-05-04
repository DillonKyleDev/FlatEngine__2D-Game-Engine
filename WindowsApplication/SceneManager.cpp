#include "SceneManager.h"
#include "Scene.h"
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
#include "Audio.h"
#include "Text.h"
#include "CharacterController.h"
#include "BoxCollider.h"
#include "RigidBody.h"
#include "json.hpp"
using json = nlohmann::json;
using namespace nlohmann::literals;



namespace FlatEngine
{
	SceneManager::SceneManager()
	{
		this->loadedScene;
		animatorPreviewObjects = std::vector<std::shared_ptr<GameObject>>();
	}

	SceneManager::~SceneManager()
	{
	}

	std::shared_ptr<Scene> SceneManager::CreateNewScene()
	{
		// Remove loaded scene from memory
		this->loadedScene = nullptr;

		// Start up a new scene
		std::shared_ptr<Scene> freshScene(new Scene());
		this->loadedScene = freshScene;

		// Declare file and input stream
		std::ofstream file_obj;

		// Delete old contents of the file
		file_obj.open("NewScene.json", std::ofstream::out | std::ofstream::trunc);
		file_obj.close();

		// Opening file in append mode
		file_obj.open("NewScene.json", std::ios::app);

		// Array that will hold our gameObject json objects
		json sceneObjectsJsonArray;
		
		// Set sceneObjectsJsonArray to empty
		sceneObjectsJsonArray.push_back("NULL");

		// Create the GameObjects json object and add the empty array as the content
		json newFileObject = json::object({ {"Scene GameObjects", sceneObjectsJsonArray } });

		// Add the GameObjects object contents to the file
		file_obj << newFileObject.dump(4).c_str() << std::endl;

		// Close the file
		file_obj.close();

		return this->loadedScene;
	}

	void SceneManager::SaveScene(std::shared_ptr<Scene> scene, std::string filename)
	{
		loadedScenePath = filename;

		// Declare file and input stream
		std::ofstream file_obj;
		std::ifstream ifstream(filename);

		// Delete old contents of the file
		file_obj.open(filename, std::ofstream::out | std::ofstream::trunc);
		file_obj.close();

		// Opening file in append mode
		file_obj.open(filename, std::ios::app);
 
		// Array that will hold our gameObject json objects
		json sceneObjectsJsonArray;

		std::vector<std::shared_ptr<GameObject>> sceneObjects = scene->GetSceneObjects();
		if (sceneObjects.size() > 0)
		{
			for (int i = 0; i < sceneObjects.size(); i++)
			{
				// Declare components array json object for components
				json componentsArray = json::array();

				// Get the object we'll be working with and it's components
				std::shared_ptr<GameObject> currentObject = sceneObjects[i];
				std::vector<std::shared_ptr<Component>> components = currentObject->GetComponents();

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
					{"_isActive", currentObject->IsActive()},
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

	void SceneManager::SaveCurrentScene()
	{
		SaveScene(loadedScene, loadedScenePath);
	}

	void SceneManager::LoadScene(std::string filename)
	{
		loadedScenePath = filename;

		// Remove loaded scene from memory
		loadedScene = nullptr;

		// Start up a new scene
		std::shared_ptr<Scene> freshScene = std::make_shared<Scene>();
		loadedScene = freshScene;
		freshScene->SetName(FlatEngine::FlatGui::GetFilenameFromPath(filename, false));
		freshScene->SetPath(filename);

		// Declare file and input stream
		std::ofstream file_obj;
		std::ifstream ifstream(filename);

		// Open file in in mode
		file_obj.open(filename, std::ios::in);

		// Variable to save the current file data into
		std::string fileContent = "";

		// Loop through the file line by line and save the data
		if (file_obj.good())
		{
			std::string line;
			while (!ifstream.eof()) {
				std::getline(ifstream, line);
				fileContent.append(line + "\n");
			}
		}

		// Close the file after reading
		file_obj.close();

		if (file_obj.good())
		{

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
					bool _isActive = true;
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
					// Check _isActive key exists
					if (currentObjectJson.contains("_isActive"))
						_isActive = currentObjectJson["_isActive"];
					else
						FlatEngine::LogInt(i, "SceneManager::Load() - Saved scene json does not contain a value for '_isActive' in object: ");
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
							loadedChildrenIDs.push_back(currentObjectJson["children"][c]);
						}
					}
					else
						FlatEngine::LogInt(i, "SceneManager::Load() - Saved scene json does not contain a value for 'children' in object: ");


					// Create new GameObject to load the data into
					std::shared_ptr<GameObject> loadedObject = std::make_shared<GameObject>(loadedParentID, loadedID);
					loadedObject->SetName(loadedName);
					loadedObject->SetActive(_isActive);

					// Add children
					for (int c = 0; c < currentObjectJson["children"].size(); c++)
					{
						loadedObject->AddChild(loadedChildrenIDs[c]);
						// VVV I don't think this is right but it fixed a duplicate ID bug temporarily VVV
						freshScene->IncrementGameObjectID();
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
							bool _isActive = true;
							float xScale = 1;
							float yScale = 1;
							float xPos = 0;
							float yPos = 0;
							float rotation = 0;

							std::shared_ptr<Transform> newTransform = std::static_pointer_cast<Transform>(loadedObject->AddComponent(ComponentTypes::Transform));

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
							// Load _isActive
							if (currentObjectJson["components"][j].contains("_isActive"))
								_isActive = currentObjectJson["components"][j]["_isActive"];
							else
								FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for '_isActive' in object: ");


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
							newTransform->SetActive(_isActive);
							newTransform->SetPosition(Vector2(xPos, yPos));
							newTransform->SetScale(Vector2(xScale, yScale));
							newTransform->SetRotation(rotation);
						}
						else if (type == "Sprite")
						{
							std::shared_ptr<Sprite> newSprite = std::static_pointer_cast<Sprite>(loadedObject->AddComponent(ComponentTypes::Sprite));

							// Default values
							long id = -1;
							bool _isCollapsed = true;
							bool _isActive = true;
							std::string path = "";
							float xOffset = 0;
							float yOffset = 0;
							int renderOrder = 0;

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
							// Load _isActive
							if (currentObjectJson["components"][j].contains("_isActive"))
								_isActive = currentObjectJson["components"][j]["_isActive"];
							else
								FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for '_isActive' in object: ");


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

							if (currentObjectJson["components"][j].contains("renderOrder"))
								renderOrder = currentObjectJson["components"][j]["renderOrder"];
							else
								FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'renderOrder' in object: ");

							newSprite->SetID(id);
							newSprite->SetCollapsed(_isCollapsed);
							newSprite->SetActive(_isActive);
							newSprite->SetTexture(path);
							newSprite->SetOffset(Vector2(xOffset, yOffset));
							newSprite->SetRenderOrder(renderOrder);
						}
						else if (type == "Camera")
						{
							std::shared_ptr<Camera> newCamera = std::static_pointer_cast<Camera>(loadedObject->AddComponent(ComponentTypes::Camera));

							// Default values
							long id = -1;
							bool _isCollapsed = true;
							bool _isActive = true;
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
							// Load _isActive
							if (currentObjectJson["components"][j].contains("_isActive"))
								_isActive = currentObjectJson["components"][j]["_isActive"];
							else
								FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for '_isActive' in object: ");

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
							newCamera->SetActive(_isActive);
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
							std::shared_ptr<ScriptComponent> newScript = std::static_pointer_cast<ScriptComponent>(loadedObject->AddComponent(ComponentTypes::Script));

							// Default values
							long id = -1;
							bool _isCollapsed = true;
							bool _isActive = false;
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
							// Load _isActive
							if (currentObjectJson["components"][j].contains("_isActive"))
								_isActive = currentObjectJson["components"][j]["_isActive"];
							else
								FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for '_isActive' in object: ");


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
							newScript->SetActive(_isActive);
							newScript->SetAttachedScript(attachedScript);
						}
						else if (type == "Button")
						{
							std::shared_ptr<Button> newButton = std::static_pointer_cast<Button>(loadedObject->AddComponent(ComponentTypes::Button));

							// Default values
							long id = -1;
							bool _isCollapsed = true;
							bool _isActive = true;
							float activeWidth = 5;
							float activeHeight = 3;
							Vector2 activeOffset = { 0, 0 };
							int activeLayer = 0;
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
							// Load _isActive
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
							// Active Layer
							if (currentObjectJson["components"][j].contains("activeLayer"))
								activeLayer = currentObjectJson["components"][j]["activeLayer"];
							else
								FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'activeLayer' in object: ");

							newButton->SetID(id);
							newButton->SetCollapsed(_isCollapsed);
							newButton->SetActive(_isActive);
							newButton->SetActiveDimensions(activeWidth, activeHeight);
							newButton->SetActiveOffset(activeOffset);
							newButton->SetActiveLayer(activeLayer);
						}
						else if (type == "Canvas")
						{
							std::shared_ptr<Canvas> newCanvas = std::static_pointer_cast<Canvas>(loadedObject->AddComponent(ComponentTypes::Canvas));

							// Default values
							long id = -1;
							bool _isCollapsed = true;
							bool _isActive = true;
							float canvasWidth = 5;
							float canvasHeight = 3;
							int layerNumber = -1;
							bool _blocksLayers = true;


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
							// Load _isActive
							if (currentObjectJson["components"][j].contains("_isActive"))
								_isActive = currentObjectJson["components"][j]["_isActive"];
							else
								FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for '_isActive' in object: ");



							if (currentObjectJson["components"][j].contains("width"))
								canvasWidth = currentObjectJson["components"][j]["width"];
							else
								FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'width' in object: ");

							if (currentObjectJson["components"][j].contains("height"))
								canvasHeight = currentObjectJson["components"][j]["height"];
							else
								FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'height' in object: ");

							if (currentObjectJson["components"][j].contains("layerNumber"))
								layerNumber = currentObjectJson["components"][j]["layerNumber"];
							else
								FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'layerNumber' in object: ");

							if (currentObjectJson["components"][j].contains("_blocksLayers"))
								_blocksLayers = currentObjectJson["components"][j]["_blocksLayers"];
							else
								FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for '_blocksLayers' in object: ");

							// Assign values to the new Canvas
							newCanvas->SetID(id);
							newCanvas->SetCollapsed(_isCollapsed);
							newCanvas->SetActive(_isActive);
							newCanvas->SetDimensions(canvasWidth, canvasHeight);
							newCanvas->SetLayerNumber(layerNumber);
							newCanvas->SetBlocksLayers(_blocksLayers);
						}
						else if (type == "Animation")
						{
							std::shared_ptr<Animation> newAnimation = std::static_pointer_cast<Animation>(loadedObject->AddComponent(ComponentTypes::Animation));

							// Default values
							long id = -1;
							bool _isCollapsed = false;
							bool _isActive = true;
							std::string path = "";

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
							// Load _isActive
							if (currentObjectJson["components"][j].contains("_isActive"))
								_isActive = currentObjectJson["components"][j]["_isActive"];
							else
								FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for '_isActive' in object: ");



							// Path
							if (currentObjectJson["components"][j].contains("path"))
								path = currentObjectJson["components"][j]["path"];
							else
								FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'path' in object: ");

							// Assign values to the new Canvas
							newAnimation->SetID(id);
							newAnimation->SetCollapsed(_isCollapsed);
							newAnimation->SetActive(_isActive);
							newAnimation->Stop();
							newAnimation->SetAnimationPath(path);
						}
						else if (type == "Audio")
						{
							std::shared_ptr<Audio> newAudio = std::static_pointer_cast<Audio>(loadedObject->AddComponent(ComponentTypes::Audio));

							// Default values
							long id = -1;
							bool _isCollapsed = false;
							bool _isActive = true;
							bool _isMusic = false;
							std::string path = "";


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
							// Load _isActive
							if (currentObjectJson["components"][j].contains("_isActive"))
								_isActive = currentObjectJson["components"][j]["_isActive"];
							else
								FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for '_isActive' in object: ");



							if (currentObjectJson["components"][j].contains("_isMusic"))
								_isMusic = currentObjectJson["components"][j]["_isMusic"];
							else
								FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for '_isMusic' in object: ");

							if (currentObjectJson["components"][j].contains("path"))
								path = currentObjectJson["components"][j]["path"];
							else
								FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'path' in object: ");

							// Assign values to the new Canvas
							newAudio->SetID(id);
							newAudio->SetCollapsed(_isCollapsed);
							newAudio->SetActive(_isActive);
							newAudio->SetPath(path);
							newAudio->SetIsMusic(_isMusic);
							if (_isMusic)
								newAudio->LoadMusic(path);
							else
								newAudio->LoadEffect(path);
						}
						else if (type == "Text")
						{
							std::shared_ptr<Text> newText = std::static_pointer_cast<Text>(loadedObject->AddComponent(ComponentTypes::Text));

							// Default values
							long id = -1;
							bool _isCollapsed = true;
							bool _isActive = true;
							std::string fontPath = "";
							int fontSize = 40;
							std::string text = "Text Component";
							Uint8 f_red = 1;
							Uint8 f_green = 1;
							Uint8 f_blue = 1;
							Uint8 f_alpha = 1;
							Vector2 offset = { 0, 0 };
							int renderOrder = 0;

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
							// Load _isActive
							if (currentObjectJson["components"][j].contains("_isActive"))
								_isActive = currentObjectJson["components"][j]["_isActive"];
							else
								FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for '_isActive' in object: ");

							
							
							// Text Properties
							if (currentObjectJson["components"][j].contains("fontPath"))
								fontPath = currentObjectJson["components"][j]["fontPath"];
							else
								FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'fontPath' in object: ");
							if (currentObjectJson["components"][j].contains("fontSize"))
								fontSize = currentObjectJson["components"][j]["fontSize"];
							else
								FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'fontSize' in object: ");
							if (currentObjectJson["components"][j].contains("text"))
								text = currentObjectJson["components"][j]["text"];
							else
								FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'text' in object: ");
							if (currentObjectJson["components"][j].contains("renderOrder"))
								renderOrder = currentObjectJson["components"][j]["renderOrder"];
							else
								FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'renderOrder' in object: ");
							// Font RGBA values
							if (currentObjectJson["components"][j].contains("f_red"))
								f_red = currentObjectJson["components"][j]["f_red"];
							else
								FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'f_red' in object: ");
							if (currentObjectJson["components"][j].contains("f_green"))
								f_green = currentObjectJson["components"][j]["f_green"];
							else
								FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'f_green' in object: ");
							if (currentObjectJson["components"][j].contains("f_blue"))
								f_blue = currentObjectJson["components"][j]["f_blue"];
							else
								FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'f_blue' in object: ");
							// Offset
							if (currentObjectJson["components"][j].contains("xOffset"))
								offset.x = currentObjectJson["components"][j]["xOffset"];
							else
								FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'xOffset' in object: ");
							if (currentObjectJson["components"][j].contains("yOffset"))
								offset.y = currentObjectJson["components"][j]["yOffset"];
							else
								FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'yOffset' in object: ");

							SDL_Color fontColor;
							fontColor.r = f_red;
							fontColor.g = f_green;
							fontColor.b = f_blue;

							newText->SetID(id);
							newText->SetCollapsed(_isCollapsed);
							newText->SetActive(_isActive);
							newText->SetFontPath(fontPath);
							newText->SetFontSize(fontSize);
							newText->SetColor(fontColor);
							newText->SetText(text);
							newText->SetOffset(offset);
							newText->SetRenderOrder(renderOrder);
							newText->LoadText();
						}
						else if (type == "CharacterController")
						{
							std::shared_ptr<CharacterController> newCharacterController = std::static_pointer_cast<CharacterController>(loadedObject->AddComponent(ComponentTypes::CharacterController));

							// Default values
							long id = -1;
							bool _isCollapsed = true;
							bool _isActive = true;
							float walkSpeed = 1;
							float runSpeed = 2;
							float gravity = 1;
							bool _isMoving = false;
							float velocity = 0;

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
							// Load _isActive
							if (currentObjectJson["components"][j].contains("_isActive"))
								_isActive = currentObjectJson["components"][j]["_isActive"];
							else
								FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for '_isActive' in object: ");

							
							
							
							// CharacterController Properties
							if (currentObjectJson["components"][j].contains("walkSpeed"))
								walkSpeed = currentObjectJson["components"][j]["walkSpeed"];
							else
								FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'walkSpeed' in object: ");
							if (currentObjectJson["components"][j].contains("runSpeed"))
								runSpeed = currentObjectJson["components"][j]["runSpeed"];
							else
								FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'runSpeed' in object: ");
							if (currentObjectJson["components"][j].contains("gravity"))
								gravity = currentObjectJson["components"][j]["gravity"];
							else
								FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'gravity' in object: ");
							if (currentObjectJson["components"][j].contains("_isMoving"))
								_isMoving = currentObjectJson["components"][j]["_isMoving"];
							else
								FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for '_isMoving' in object: ");
							// Font RGBA values
							if (currentObjectJson["components"][j].contains("velocity"))
								velocity = currentObjectJson["components"][j]["velocity"];
							else
								FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'velocity' in object: ");
						
							newCharacterController->SetID(id);
							newCharacterController->SetCollapsed(_isCollapsed);
							newCharacterController->SetActive(_isActive);
							newCharacterController->SetWalkSpeed(walkSpeed);
							newCharacterController->SetRunSpeed(runSpeed);
							newCharacterController->SetGravity(gravity);
							newCharacterController->SetMoving(_isMoving);
							newCharacterController->SetVelocity(velocity);
						}
						else if (type == "BoxCollider")
						{
							std::shared_ptr<BoxCollider> newBoxCollider = std::static_pointer_cast<BoxCollider>(loadedObject->AddComponent(ComponentTypes::BoxCollider));

							// Default values
							long id = -1;
							bool _isCollapsed = true;
							bool _isActive = true;
							float activeWidth = 10;
							float activeHeight = 10;
							Vector2 activeOffset = Vector2(0,0);
							int activeLayer = 0;
							std::string connectedScript = "";

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
							// Load _isActive
							if (currentObjectJson["components"][j].contains("_isActive"))
								_isActive = currentObjectJson["components"][j]["_isActive"];
							else
								FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for '_isActive' in object: ");

							
							
							// BoxCollider Properties
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
							if (currentObjectJson["components"][j].contains("activeLayer"))
								activeLayer = currentObjectJson["components"][j]["activeLayer"];
							else
								FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'activeLayer' in object: ");
							if (currentObjectJson["components"][j].contains("connectedScript"))
								connectedScript = currentObjectJson["components"][j]["connectedScript"];
							else
								FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'connectedScript' in object: ");


							newBoxCollider->SetID(id);
							newBoxCollider->SetCollapsed(_isCollapsed);
							newBoxCollider->SetActive(_isActive);
							newBoxCollider->SetActiveDimensions(activeWidth, activeHeight);
							newBoxCollider->SetActiveOffset(activeOffset);
							newBoxCollider->SetActiveLayer(activeLayer);
							newBoxCollider->SetConnectedScript(connectedScript);
						}
						else if (type == "RigidBody")
						{
							std::shared_ptr<RigidBody> newRigidBody = std::static_pointer_cast<RigidBody>(loadedObject->AddComponent(ComponentTypes::RigidBody));

							// Default values
							long id = -1;
							bool _isCollapsed = true;
							bool _isActive = true;
							float mass = 1;
							float angularDrag = 1;
							float gravity = 1;							
							bool _isContinious = false;
							bool _isKinematic = false;

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
							// Load _isActive
							if (currentObjectJson["components"][j].contains("_isActive"))
								_isActive = currentObjectJson["components"][j]["_isActive"];
							else
								FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for '_isActive' in object: ");

							
							
							// CharacterController Properties
							if (currentObjectJson["components"][j].contains("mass"))
								mass = currentObjectJson["components"][j]["mass"];
							else
								FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'mass' in object: ");
							if (currentObjectJson["components"][j].contains("angularDrag"))
								angularDrag = currentObjectJson["components"][j]["angularDrag"];
							else
								FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'angularDrag' in object: ");
							if (currentObjectJson["components"][j].contains("gravity"))
								gravity = currentObjectJson["components"][j]["gravity"];
							else
								FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for 'gravity' in object: ");
							if (currentObjectJson["components"][j].contains("_isContinious"))
								_isContinious = currentObjectJson["components"][j]["_isContinious"];
							else
								FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for '_isContinious' in object: ");
							if (currentObjectJson["components"][j].contains("_isKinematic"))
								_isKinematic = currentObjectJson["components"][j]["_isKinematic"];
							else
								FlatEngine::LogInt(j, "SceneManager::Load() - Saved scene json does not contain a value for '_isKinematic' in object: ");


							newRigidBody->SetID(id);
							newRigidBody->SetCollapsed(_isCollapsed);
							newRigidBody->SetActive(_isActive);
							newRigidBody->SetMass(mass);
							newRigidBody->SetAngularDrag(angularDrag);
							newRigidBody->SetGravity(gravity);
							newRigidBody->SetIsContinuous(_isContinious);
							newRigidBody->SetIsKinematic(_isKinematic);
						}
					}

					// Add created GameObject to our freshScene
					freshScene->AddSceneObject(loadedObject);
				}
			}
		}

		// Assign our freshScene to the SceneManagers currently loadedScene member variable
		this->loadedScene = freshScene;
	}

	std::shared_ptr<Scene> SceneManager::GetLoadedScene()
	{
		return this->loadedScene;
	}
	
	std::string SceneManager::GetLoadedScenePath()
	{
		return loadedScenePath;
	}
	
	void SceneManager::SaveAnimationPreviewObjects()
	{
		if (loadedScene->GetAnimatorPreviewObjects().size() > 0)
			animatorPreviewObjects = loadedScene->GetAnimatorPreviewObjects();
	}
	
	void SceneManager::LoadAnimationPreviewObjects()
	{
		loadedScene->SetAnimatorPreviewObjects(animatorPreviewObjects);
	}
}


// Alternative file writing method
//file_obj.write(fileContent.c_str(), fileContent.size());