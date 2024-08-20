#pragma once
#include "Application.h"
#include "Animation.h"

#include <stdio.h>
#include <string>
#include <map>
#include <sstream>
#include <vector>
#include "SDL.h"
#include <SDL_syswm.h>
#include <SDL_image.h>
#include "json.hpp"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

#define SOL_ALL_SAFETIES_ON 1
#include <sol.hpp>


/*
######################################
######							######
######         Engine           ######
######                          ######
######################################
*/

class GameManager;

namespace FlatEngine
{
	class Vector2;
	class Vector4;
	class AssetManager;
	class GameLoop;
	class Project;
	class Scene;
	class Process;
	class Logger;
	class Process;
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

	extern std::string F_DirectoriesLuaFilepath; // If you change the location of Directories.lua, be sure to update this value in FlatEngine.cpp
	extern std::shared_ptr<Application> F_Application;
	extern sol::state F_Lua;
	extern std::vector<std::string> F_luaScriptNames;
	extern AssetManager F_AssetManager;
	extern std::vector<std::string> F_selectedFiles;

	extern bool _isDebugMode;
	extern bool _closeProgram;

	// Flags
	extern ImGuiChildFlags F_childFlags;
	extern ImGuiChildFlags F_autoResizeChildFlags;
	extern ImGuiChildFlags F_resizeChildFlags;
	extern ImGuiChildFlags F_headerFlags;
	extern ImGuiTableFlags F_tableFlags;
	extern ImGuiTableFlags F_resizeableTableFlags;
	extern ImGuiInputTextFlags F_inputFlags;

	extern float F_childPadding;


	// Controller Management
	extern std::vector<SDL_Joystick*> gamepads;
	extern int JOYSTICK_DEAD_ZONE;

	// Managers
	extern SceneManager F_SceneManager;
	extern UIManager F_UIManager;
	extern Logger F_Logger;
	extern std::vector<Process> F_ProfilerProcesses;
	extern std::shared_ptr<FlatEngine::FlatGui::WidgetsManager> widgetsManager;
	extern Sound F_SoundController;
	extern TTF_Font* F_fontCinzel;
	extern std::vector<MappingContext> F_MappingContexts;
	extern std::shared_ptr<PrefabManager> prefabManager;

	extern int F_maxSpriteLayers;
	extern float F_spriteScaleMultiplier;

	// Game view
	extern float F_GAME_VIEWPORT_WIDTH;
	extern 	float F_GAME_VIEWPORT_HEIGHT;
	extern float F_xGameCenter;
	extern float F_yGameCenter;
	extern Vector2 F_gameViewCenter;
	extern Vector2 F_gameViewGridStep;


	extern bool LoadFonts();
	extern void FreeFonts();
	extern std::string GetDir(std::string dirName);
	extern std::string GetFilePath(std::string fileName);
	extern std::shared_ptr<Texture> GetTextureObject(std::string textureName);
	extern SDL_Texture* GetTexture(std::string textureName);
	extern Vector4 GetColor(std::string colorName);
	extern Uint32 GetColor32(std::string colorName);

	extern void SetupImGui();
	extern void QuitImGui();
	extern void HandleEvents(bool& quit);
	extern void HandleContextEvents(FlatEngine::MappingContext& context, SDL_Event event, std::vector<std::string>& firedKeys);

	// Lua / Sol
	extern void InitLua();
	extern void RegisterLuaFunctions();
	extern void RegisterLuaTypes();
	extern void RunLuaFuncOnAllScripts(std::string functionName);
	extern void RunAwakeAndStart();
	extern void LuaTesting(GameObject& toSend);
	extern void RetrieveLuaScriptNames();
	extern void CreateNewLuaScript(std::string filename);

	// Profiler
	extern void AddProfilerProcess(std::string name);
	extern void AddProcessData(std::string processName, float data);
	extern void RemoveProfilerProcess(std::string toRemove);

	// Project Management
	extern Project F_LoadedProject;
	extern void SetLoadedProject(Project loadedProject);
	extern Project& GetLoadedProject();

	// Player Management
	extern GameObject* GetPlayerObject();
	extern GameObject* F_PlayerObject;

	// Scene Management
	extern Scene* GetLoadedScene();
	extern Scene* CreateNewScene();
	extern void SaveScene(std::shared_ptr<Scene> scene, std::string filepath);
	extern void SaveCurrentScene();
	extern void LoadScene(std::string filepath);
	std::string GetLoadedScenePath();
	extern std::map<long, GameObject> &GetSceneObjects();
	extern GameObject* CreateGameObject(long parentID = -1);
	extern void DeleteGameObject(int sceneObjectID);
	extern Component* GetObjectComponent(long objectID, ComponentTypes type);
	extern GameObject* GetObjectById(long objectID);
	extern GameObject* GetObjectByName(std::string name);
	extern GameObject* GetObjectByTag(std::string tag);

	extern long GetNextComponentID();
	extern long GetNextGameObjectID();

	// SDL
	extern Vector2 AddImageToDrawList(SDL_Texture* texture, Vector2 position, Vector2 centerPoint, float textureWidth, float textureHeight, Vector2 pivotPoint, Vector2 scale, bool _scalesWithZoom, float zoomMultiplier, ImDrawList* draw_list, float rotation = 0, ImU32 addColor = (((ImU32)(255) << 24) | ((ImU32)(255) << 16) | ((ImU32)(255) << 8) | ((ImU32)(255) << 0)));

	// Engine
	extern bool Init(int windowWidth, int windowHeight);
	extern void CloseProgram();
	extern Uint32 GetEngineTime();
	extern void ManageControllers();

	// Controls Context Management
	extern void SaveMappingContext(std::string path, MappingContext context);
	extern void InitializeMappingContexts();
	extern MappingContext* GetMappingContext(std::string contextName);

	// Prefabs
	extern void CreatePrefab(std::string path, GameObject &gameObject);
	extern void InitializePrefabs();
	extern GameObject Instantiate(std::string prefabName, Vector2 position, long parentID = -1, long ID = -1);
	extern std::map<std::string, std::vector<GameObject>> GetPrefabs();

	// Logging Prettification
	extern void LogString(std::string line = "", std::string from = "[C++]");
	extern void LogFloat(float var, std::string line = "", std::string from = "[C++]");
	extern void LogInt(int var, std::string line = "", std::string from = "[C++]");
	extern void LogVector2(Vector2 vector, std::string line = "", std::string from = "[C++]");
	extern void LogSeparator();
	extern void DrawRectangle(Vector2 startingPoint, Vector2 endingPoint, Vector2 canvas_p0, Vector2 canvas_sz, Vector4 color, float thickness, ImDrawList* drawList);
	extern void DrawLine(Vector2 startingPoint, Vector2 endingPoint, Vector4 color, float thickness, ImDrawList* drawList);
	extern void DrawRectangleFromLines(Vector2* corners, Vector4 color, float thickness, ImDrawList* drawList);
	extern void DrawCircle(Vector2 center, float radius, Vector4 color, ImDrawList* drawList, float thickness = 1, int segments = 0);
	extern void DrawPoint(Vector2 point, Vector4 color, ImDrawList* drawList);
	extern void DebugRectangle(Vector2 startingPoint, Vector2 endPoint, Vector4 color, float thickness, ImDrawList* drawList);

	// Game View
	extern void Game_RenderView();
	extern void Game_RenderObjects(Vector2 canvas_p0, Vector2 canvas_sz);
	extern void Game_RenderObject(GameObject self, Vector2 canvas_p0, Vector2 canvas_sz, ImDrawList* draw_list, ImDrawListSplitter* drawSplitter, Vector2 cameraPosition, float cameraWidth, float cameraHeight);

	// Game Loop Prettification
	extern void StartGameLoop();
	extern void GameLoopUpdate();
	extern void PauseGameLoop();
	extern void PauseGame();
	extern void StopGameLoop();
	extern float GetEllapsedGameTimeInSec();
	extern long GetEllapsedGameTimeInMs();
	extern bool GameLoopStarted();
	extern bool GameLoopPaused();
	extern float GetAverageFps();
	extern float GetDeltaTime();

	//////////////////////
	// Helper Functions //
	//////////////////////

	// Json parsing
	extern json CreateJsonFromObject(GameObject gameObject);
	extern GameObject CreateObjectFromJson(json objectJson);
	extern float CheckJsonFloat(json obj, std::string checkFor, std::string loadedName);
	extern int CheckJsonInt(json obj, std::string checkFor, std::string loadedName);
	extern long CheckJsonLong(json obj, std::string checkFor, std::string loadedName);
	extern bool CheckJsonBool(json obj, std::string checkFor, std::string loadedName);
	extern bool JsonContains(json obj, std::string checkFor, std::string loadedName);
	extern std::string CheckJsonString(json obj, std::string checkFor, std::string loadedName);

	// ImGui Wrappers
	extern void BeginImGuiRender();
	extern void EndImGuiRender();
	extern void SetNextViewportToFillWindow();

	extern void BeginWindow(std::string name, Vector4 bgColor = GetColor("windowBg"));
	extern void BeginWindow(std::string name, bool& _isOpen, Vector4 bgColor = GetColor("windowBg"));
	extern void BeginWindow(std::string name, bool& b_isOpen, ImGuiWindowFlags flags, Vector4 bgColor = GetColor("windowBg"));
	extern void EndWindow();
	extern void BeginWindowChild(std::string title, Vector4 bgColor = GetColor("outerWindow"));
	extern void BeginResizeWindowChild(std::string title, Vector4 bgColor = GetColor("outerWindow"));
	extern void EndWindowChild();
	extern void PushWindowStyles(Vector4 bgColor = GetColor("windowBg"));
	extern void PopWindowStyles();
	extern void PushComboStyles();
	extern void PopComboStyles();
	extern void PushMenuStyles();
	extern void PopMenuStyles();
	extern void PushTableStyles();
	extern void PopTableStyles();
	extern bool PushTable(std::string id, int columns, ImGuiTableFlags flags = F_tableFlags);
	extern bool RenderFloatDragTableRow(std::string id, std::string fieldName, float& value, float increment, float min, float max);
	extern bool RenderTagListTableRow(std::string id, std::string fieldName, TagList& tagList);
	extern bool RenderIntDragTableRow(std::string id, std::string fieldName, int& value, float speed, int min, int max);
	extern bool RenderCheckboxTableRow(std::string id, std::string fieldName, bool& _value);
	extern void RenderSelectableTableRow(std::string id, std::string fieldName, std::vector<std::string> options, int& current_option);
	extern void RenderTextTableRow(std::string id, std::string fieldName, std::string value, std::string value2 = "");
	extern void PopTable();
	extern bool RenderInput(std::string id, std::string label, std::string& value, bool _canOpenFiles = false, float inputWidth = -1, ImGuiInputTextFlags flags = 0);
	extern bool RenderButton(std::string text, Vector2 size = Vector2(0, 0), float rounding = 1, Vector4 color = GetColor("button"), Vector4 hoverColor = GetColor("buttonHovered"), Vector4 activeColor = GetColor("buttonActive"));
	extern bool RenderImageButton(std::string id, SDL_Texture* texture, Vector2 size = Vector2(16, 16), float rounding = 1, Vector4 bgColor = GetColor("imageButton"), Vector4 tint = GetColor("imageButtonTint"), Vector4 hoverColor = GetColor("imageButtonHovered"), Vector4 activeColor = GetColor("imageButtonActive"));
	extern bool RenderDragFloat(std::string text, float width, float& value, float increment, float min, float max, ImGuiSliderFlags flags = 0);
	extern bool RenderDragInt(std::string text, float width, int& value, float speed, int min, int max, ImGuiSliderFlags flags = 0);
	extern bool RenderCheckbox(std::string text, bool& _toCheck);
	extern void RenderSectionHeader(std::string headerText, float height = 0);
	extern bool RenderInvisibleButton(std::string id, Vector2 startingPoint, Vector2 size, bool _allowOverlap = true, bool _showRect = false);
	extern bool RenderSelectable(std::string id, std::vector<std::string> options, int& current_option);
	extern void PushTreeList(std::string id);
	extern void RenderTreeLeaf(std::string name, std::string& node_clicked);
	extern void PopTreeList();
	extern void BeginToolTip(std::string title);
	extern void EndToolTip();
	extern void RenderToolTipText(std::string label, std::string text);
	extern void RenderToolTipFloat(std::string label, float data);
	extern void RenderToolTipLong(std::string label, long data);
	extern void RenderToolTipLongVector(std::string label, std::vector<long> data);
	extern bool RenderInputModal(std::string label, std::string description, std::string& inputValue);


	extern bool AreCollidingViewport(Vector4 ObjectA, Vector4 ObjectB);
	extern Vector2 Lerp(Vector2 startPos, Vector2 endPos, float ease);

	// File managing
	extern std::string OpenSaveFileExplorer();
	extern std::string OpenLoadFileExplorer();
	extern std::string GetFilenameFromPath(std::string path, bool _keepExtension = false);
	extern std::string MakePathRelative(std::string filepath);
	extern std::string GetCurrentDir();
	extern bool DoesFileExist(std::string filepath);
	extern bool FilepathHasExtension(std::string filepath, std::string extension);
	extern json LoadFileData(std::string filepath);

	// Animation Manager
	extern void CreateNewAnimationFile(std::string path);
	extern void SaveAnimationFile(std::shared_ptr<Animation::S_AnimationProperties> propertiesObject, std::string path);
	extern std::shared_ptr<Animation::S_AnimationProperties> LoadAnimationFile(std::string path);
};