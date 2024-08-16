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
class Vector4;

namespace FlatEngine
{
	extern std::shared_ptr<Application> F_Application;
	extern sol::state F_Lua;
	extern std::vector<std::string> F_luaScriptNames;

	extern bool _isDebugMode;
	extern bool _closeProgram;

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


	//////////////////////
	//Global      Colors//
	//  //////VV///////  /
	//     UUUUUUUU     //
	///\-------------/////
	//////////////////////
	extern Vector4 F_transparentColor;
	extern Vector4 F_whiteColor;
	//////////////////////
	// Components ////////
	//////////////////////
	extern Vector4 F_componentBorderColor;
	// Collider borders
	extern Vector4 F_colliderActiveColor;
	extern Vector4 F_colliderInactiveColor;
	extern Vector4 F_colliderCollidingColor;
	// Button Components
	extern Vector4 F_buttonComponentActiveColor;
	extern Vector4 F_buttonComponentInctiveColor;
	extern Vector4 F_buttonComponentInctiveColor;
	// Canvas
	extern Vector4 F_canvasBorderColor;
	// Camera
	extern Vector4 F_cameraBoxColor;
	// Log
	extern Vector4 F_logTextColor;
	extern Vector4 F_logBgColor;
	extern Vector4 F_logOutlineColor;
	//////////////////////
	/// Window  Colors ///
	//////////////////////
	extern Vector4 F_selectProjectBgColor;
	extern Vector4 F_outerWindowColor;
	extern Vector4 F_innerWindowColor;
	extern Vector4 F_singleItemColor;
	extern Vector4 F_windowTitleBg;
	extern Vector4 F_windowBgColor;
	extern Vector4 F_frameBgColor;
	extern Vector4 F_frameBgActiveColor;
	extern Vector4 F_frameBgHoveredColor;
	extern Vector4 F_dockingPreviewColor;
	extern Vector4 F_dockingPreviewEmptyColor;
	// tabs
	extern Vector4 F_tabColor;
	extern Vector4 F_tabHoveredColor;
	extern Vector4 F_tabUnfocusedColor;
	extern Vector4 F_tabUnfocusedActiveColor;
	extern Vector4 F_tabActiveColor;
	// title
	extern Vector4 F_titleBgColor;
	extern Vector4 F_titleBgActiveColor;
	extern Vector4 F_titleBgCollapsedColor;
	// Text
	extern Vector4 F_textSelectedBgColor;
	// Resizers
	extern Vector4 F_resizeGripColor;
	extern Vector4 F_resizeGripActiveColor;
	extern Vector4 F_resizeGripHoveredColor;
	// Misc (not sure what they're for)
	extern Vector4 F_popupBgColor;
	extern Vector4 F_navWindowHighlightColor;
	extern Vector4 F_navHighlightColor;
	extern Vector4 F_navWindowDimBgColor;
	extern Vector4 F_modalWindowDimBgColor;
	////////////////////
	// Custom Colors ///
	//////////////////// 
	// Input
	extern Vector4 F_inputColor;
	// Tables
	extern Vector4 F_noEditTableTextColor;
	extern Vector4 F_noEditTableRowFieldBgColor;
	extern Vector4 F_noEditTableRowValueBgColor;
	extern Vector4 F_tableCellLightColor;
	extern Vector4 F_tableCellDarkColor;
	// Trees
	extern Vector4 F_treeSelectableColor;
	extern Vector4 F_treeSelectableHoveredColor;
	extern Vector4 F_treeSelectableActiveColor;
	extern Vector4 F_treeSelectableSelectedColor;
	extern Vector4 F_hierarchyChildObjectColor;
	// Combos
	extern Vector4 F_comboBgColor;
	extern Vector4 F_comboHoveredColor;
	extern Vector4 F_comboSelectableColor;
	extern Vector4 F_comboArrowColor;
	extern Vector4 F_comboArrowHoveredColor;
	extern Vector4 F_comboSelectedColor;
	extern Vector4 F_comboHighlightedColor;
	// Buttons
	extern Vector4 F_buttonColor;
	extern Vector4 F_buttonHoveredColor;
	extern Vector4 F_buttonActiveColor;
	extern Vector4 F_imageButtonColor;
	extern Vector4 F_imageButtonDarkColor;
	extern Vector4 F_imageButtonHoveredColor;
	extern Vector4 F_imageButtonActiveColor;
	extern Vector4 F_imageButtonTintColor;
	// Sliders/Drags
	extern Vector4 F_sliderColor;
	extern Vector4 F_sliderHoveredColor;
	extern Vector4 F_sliderActiveColor;
	extern Vector4 F_dragColor;
	extern Vector4 F_dragHoveredColor;
	extern Vector4 F_dragActiveColor;
	// Checkboxes
	extern Vector4 F_checkboxBgColor;
	extern Vector4 F_checkboxCheckColor;
	extern Vector4 F_checkboxHoveredColor;
	extern Vector4 F_checkboxActiveColor;
	////////////////////
	//// End Colors ////
	////////////////////

	// Icons
	extern std::string F_ResourceFailedToLoadImagePath;	
	extern Texture F_selectProjectImage;	
	extern Texture F_projectHubBgImage;
	extern Texture F_transformArrow;
	extern Texture F_transformArrowAllWhite;
	extern Texture F_transformArrowXWhite;
	extern Texture F_transformArrowYWhite;
	extern Texture F_cameraIcon;
	extern Texture F_playIcon;
	extern Texture F_pauseIcon;
	extern Texture F_stopIcon;
	extern Texture F_nextFrameIcon;
	extern Texture F_expandIcon;
	extern Texture F_expandFlippedIcon;
	extern Texture F_trashIcon;
	extern Texture F_openFileIcon;
	extern Texture F_newFileIcon;
	extern Texture F_saveFileIcon;
	extern Texture F_saveAsFileIcon;
	extern Texture F_keyFrameIcon;
	extern Texture F_timelineScrubberIcon;
	extern Texture F_threeDotsIcon;
	extern Texture F_showIcon;
	extern Texture F_hideIcon;
	extern Texture F_prefabCubeIcon;
	extern Texture F_leftIcon;
	extern Texture F_rightIcon;
	extern Texture F_downIcon;
	extern Texture F_upIcon;
	extern Texture F_upLeftIcon;
	extern Texture F_upRightIcon;
	extern Texture F_downLeftIcon;
	extern Texture F_downRightIcon;
	extern Texture F_centerIcon;

	// Texture Colors
	extern Vector2 F_uv0;
	extern Vector2 F_uv1;
	extern Vector4 F_tint_col;
	extern Vector4 F_bg_col;

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

	extern void SetupImGui();
	extern void QuitImGui();
	extern void CreateIcons();
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

	extern void BeginWindow(std::string name, Vector4 bgColor = F_windowBgColor);
	extern void BeginWindow(std::string name, bool& _isOpen, Vector4 bgColor = F_windowBgColor);
	extern void BeginWindow(std::string name, bool& b_isOpen, ImGuiWindowFlags flags, Vector4 bgColor = F_windowBgColor);
	extern void EndWindow();
	extern void BeginWindowChild(std::string title, Vector4 bgColor = F_outerWindowColor);
	extern void BeginResizeWindowChild(std::string title, Vector4 bgColor = F_outerWindowColor);
	extern void EndWindowChild();
	extern void PushWindowStyles(Vector4 bgColor = F_windowBgColor);
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
	extern bool RenderButton(std::string text, Vector2 size = Vector2(0, 0), float rounding = 1, Vector4 color = F_buttonColor, Vector4 hoverColor = F_buttonHoveredColor, Vector4 activeColor = F_buttonActiveColor);
	extern bool RenderImageButton(std::string id, SDL_Texture* texture, Vector2 size = Vector2(16, 16), float rounding = 1, Vector4 bgColor = F_imageButtonColor, Vector4 tint = F_imageButtonTintColor, Vector4 hoverColor = F_imageButtonHoveredColor, Vector4 activeColor = F_imageButtonActiveColor);
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
	extern std::string GetCurrentDir();
	extern bool DoesFileExist(std::string filepath);
	extern bool FilepathHasExtension(std::string filepath, std::string extension);
	extern json LoadFileData(std::string filepath);

	// Animation Manager
	extern void CreateNewAnimationFile(std::string path);
	extern void SaveAnimationFile(std::shared_ptr<Animation::S_AnimationProperties> propertiesObject, std::string path);
	extern std::shared_ptr<Animation::S_AnimationProperties> LoadAnimationFile(std::string path);
};