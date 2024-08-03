#pragma once
#include "Application.h"
#include <stdio.h>
#include <string>
#include <sstream>
#include <vector>
#include "SDL.h"
#include <SDL_syswm.h>
#include <SDL_image.h>
#include "json.hpp"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

#include "Animation.h"

/*
######################################
######							######
######         Engine           ######
######                          ######
######################################
*/

class GameManager;
class Vector4;

namespace FlatEngine
{
	extern std::shared_ptr<Application> F_Application;

	class GameLoop;
	class Project;
	class Scene;
	class Process;
	class Logger;
	class SceneManager;
	class PrefabManager;
	class MappingContext;
	class Texture;
	class GameObject;
	class TagList;
	class Camera;
	class Collider;
	class ScriptComponent;
	class Sound;
	class RigidBody;
	class CharacterController;
	class UIManager;
	class ECSManager;

	namespace FlatGui {
		class WidgetsManager;
	};

	using ComponentTypes = Component::ComponentTypes;

	extern bool _isDebugMode;
	extern bool _closeProgram;

	// Controller Management
	extern std::vector<SDL_Joystick*> gamepads;
	extern int JOYSTICK_DEAD_ZONE;

	// Managers
	extern SceneManager F_SceneManager;
	extern UIManager F_UIManager;
	extern Logger F_Logger;
	extern std::shared_ptr<FlatEngine::FlatGui::WidgetsManager> widgetsManager;
	extern Sound F_SoundController;
	extern TTF_Font* F_fontCinzel;

	extern bool LoadFonts();
	extern void FreeFonts();

	// Scene Management
	extern std::shared_ptr<Scene> GetLoadedScene();
	extern std::shared_ptr<Scene> CreateNewScene();
	extern void SaveScene(std::shared_ptr<Scene> scene, std::string filePath);
	extern void LoadScene(std::string name);
	std::string GetLoadedScenePath();
	extern std::vector<GameObject> &GetSceneObjects();
	extern GameObject* CreateGameObject(long parentID = -1);
	extern void DeleteGameObject(int sceneObjectID);
	extern Component* GetObjectComponent(long objectID, ComponentTypes type);
	extern GameObject* GetObjectById(long objectID);
	extern GameObject* GetObjectByName(std::string name);

	extern long GetNextComponentID();
	extern long GetNextGameObjectID();

	// Engine
	extern bool Init();
	extern void CloseProgram();
	extern Uint32 GetEngineTime();
	extern json LoadFileData(std::string filepath);
	extern void ManageControllers();

	// Controls Context Management
	extern std::vector<std::shared_ptr<MappingContext>> mappingContexts;
	extern void SaveMappingContext(std::string path, std::shared_ptr<MappingContext> context);
	extern void InitializeMappingContexts();
	extern std::shared_ptr<MappingContext> GetMappingContext(std::string contextName);

	// Prefabs
	extern std::shared_ptr<PrefabManager> prefabManager;
	extern void CreatePrefab(std::string path, GameObject &gameObject);
	extern void InitializePrefabs();
	extern GameObject Instantiate(std::string prefabName, Vector2 position, long parentID = -1, long ID = -1);
	extern std::map<std::string, std::vector<GameObject>> GetPrefabs();

	// Logging Prettification
	extern void LogString(std::string line = "");
	extern void LogSeparator();
	extern void LogFloat(float var, std::string line = "");
	extern void LogInt(int var, std::string line = "");
	extern void LogVector2(Vector2 vector, std::string line = "");
	extern void DrawRectangle(Vector2 startingPoint, Vector2 endingPoint, Vector2 canvas_p0, Vector2 canvas_sz, Vector4 color, float thickness, ImDrawList* drawList);
	extern void DrawLine(Vector2 startingPoint, Vector2 endingPoint, Vector4 color, float thickness, ImDrawList* drawList);
	extern void DrawRectangleFromLines(Vector2* corners, Vector4 color, float thickness, ImDrawList* drawList);
	extern void DrawCircle(Vector2 center, float radius, Vector4 color, ImDrawList* drawList, float thickness = 1, int segments = 0);
	extern void DrawPoint(Vector2 point, Vector4 color, ImDrawList* drawList);
	extern void DebugRectangle(Vector2 startingPoint, Vector2 endPoint, Vector4 color, float thickness, ImDrawList* drawList);

	// Game Loop Prettification
	extern void StartGameLoop();
	extern void GameLoopUpdate();
	extern void PauseGameLoop();
	extern void PauseGame();
	extern void StopGameLoop();
	extern long GetEllapsedGameTimeInSec();
	extern double GetEllapsedGameTimeInMs();
	extern bool GameLoopStarted();
	extern bool GameLoopPaused();
	extern float GetAverageFps();
	extern float GetDeltaTime();

	// Helper Functions
	extern bool AreCollidingViewport(Vector4 ObjectA, Vector4 ObjectB);
	extern Vector2 Lerp(Vector2 startPos, Vector2 endPos, float ease);

	// File managing
	extern std::string OpenSaveFileExplorer();
	extern std::string OpenLoadFileExplorer();
	extern std::string GetFilenameFromPath(std::string path, bool _keepExtension = false);
	extern std::string GetCurrentDir();

	// Animation Manager
	extern void CreateNewAnimationFile(std::string path);
	extern void SaveAnimationFile(std::shared_ptr<Animation::S_AnimationProperties> propertiesObject, std::string path);
	extern std::shared_ptr<Animation::S_AnimationProperties> LoadAnimationFile(std::string path);
};