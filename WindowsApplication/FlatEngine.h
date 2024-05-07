#pragma once
#include <stdio.h>
#include <string>
#include <sstream>
#include <vector>
#include "SDL.h"
#include <SDL_syswm.h>
#include <SDL_image.h>
#include <imgui.h>
#include "Window.h"

#include "Project.h"
#include "SceneManager.h"
#include "ScriptComponent.h"
#include "GameLoop.h"
#include "Sound.h."
#include "RigidBody.h"

#include "Animation.h"
#include "WidgetsManager.h"
#include "UIManager.h"
#include "scripts/GameManager.h"
//#include "Process.h" Might need this after deleting some other include
#include "MappingContext.h"
#include "json.hpp"

//ImGui - SDL Renderer
#pragma once
#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_sdlrenderer2.h"


namespace FlatEngine
{
	class GameObject;
	class UIManager;
	class Logger;
	class Scene;

	using ComponentTypes = Component::ComponentTypes;

	extern bool _isDebugMode;
	extern bool _closeProgram;

	// Controller Management
	extern std::vector<SDL_Joystick*> gamepads;
	extern int JOYSTICK_DEAD_ZONE;

	// #### FLAGENGINE #### //
	// 
	// Variables
	// 
	// Focused GameObject
	extern long FocusedGameObjectID;
	extern void SetFocusedGameObjectID(long ID);
	extern long GetFocusedGameObjectID();

	// Focused Animation
	extern std::shared_ptr<Animation::S_AnimationProperties> FocusedAnimation;
	extern std::shared_ptr<GameObject> objectForFocusedAnimation;
	extern std::shared_ptr<Animation::S_Property> selectedKeyFrameToEdit;
	extern int previewAnimationStartTime;
	extern int previewAnimationTime;
	extern bool _playPreviewAnimation;
	extern void SetFocusedAnimation(std::shared_ptr<Animation::S_AnimationProperties> animation);
	extern std::shared_ptr<Animation::S_AnimationProperties> GetFocusedAnimation();

	// Managers
	extern std::shared_ptr<GameManager> gameManager;
	extern FlatEngine::SceneManager *sceneManager;
	extern FlatEngine::Logger *logger;
	extern FlatEngine::GameLoop *gameLoop;
	extern std::shared_ptr<FlatEngine::FlatGui::WidgetsManager> widgetsManager;
	extern std::shared_ptr<FlatEngine::FlatGui::UIManager> uiManager;
	extern std::shared_ptr<Sound> soundController;
	extern std::vector<std::shared_ptr<RigidBody>> rigidBodies;
	extern std::vector<std::shared_ptr<BoxCollider>> boxColliders;

	// ImU32 colors
	extern ImU32 White;
	extern ImU32 Green;
	extern ImU32 Blue;
	extern ImU32 Red;
	extern ImU32 ActiveButtonColor;
	extern ImU32 InactiveButtonColor;
	extern ImU32 CanvasBorder;

	// Engine
	extern void Run(bool &_hasQuit);
	extern void CloseProgram();
	extern int GetEngineTime();
	extern json LoadFileData(std::string filepath);

	// Project Management
	extern std::shared_ptr<Project> loadedProject;
	extern void OpenProject(std::string path);
	extern void SaveProject(std::shared_ptr<Project> project, std::string path);

	// Controls Context Management
	extern std::vector<std::shared_ptr<MappingContext>> mappingContexts;
	extern void SaveMappingContext(std::string path, std::shared_ptr<MappingContext> context);
	extern void InitializeMappingContexts();
	extern std::shared_ptr<MappingContext> GetMappingContext(std::string contextName);

	// Scene Manager Prettification
	extern std::shared_ptr<Scene> GetLoadedScene();
	extern std::shared_ptr<Scene> CreateNewScene();
	std::string GetLoadedScenePath();
	extern long GetNextComponentID();
	extern long GetNextGameObjectID();
	extern void SaveScene(std::shared_ptr<Scene> scene, std::string filePath);
	extern void LoadScene(std::string name);

	// Scene Prettification
	extern std::vector<std::shared_ptr<GameObject>> GetSceneObjects();
	extern std::shared_ptr<GameObject> CreateGameObject(long parentID = -1);
	extern void DeleteGameObject(int sceneObjectID);
	extern std::shared_ptr<Component> GetObjectComponent(long objectID, ComponentTypes type);
	extern std::shared_ptr<GameObject> GetObjectById(long objectID);
	extern std::shared_ptr<GameObject> GetObjectByName(std::string name);

	// Logging Prettification
	extern void LogString(std::string line = "");
	extern void LogSeparator();
	extern void LogFloat(float var, std::string line = "");
	extern void LogInt(int var, std::string line = "");
	extern void LogVector2(Vector2 vector, std::string line = "");
	extern void DrawRectangle(Vector2 startingPoint, Vector2 endingPoint, ImVec2 canvas_p0, ImVec2 canvas_sz, ImU32 color, float thickness, ImDrawList* drawList);
	extern void DrawRectangle(ImVec2 startingPoint, ImVec2 endingPoint, ImVec2 canvas_p0, ImVec2 canvas_sz, ImU32 color, float thickness, ImDrawList* drawList);
	extern void DrawLine(Vector2 startingPoint, Vector2 endingPoint, ImU32 color, float thickness, ImDrawList* drawList);
	extern void DrawLine(ImVec2 startingPoint, ImVec2 endingPoint, ImU32 color, float thickness, ImDrawList* drawList);
	extern void DrawPoint(Vector2 point, ImU32 color, ImDrawList* drawList);
	extern void DrawPoint(ImVec2 point, ImU32 color, ImDrawList* drawList);

	// Profiler
	extern std::vector<std::shared_ptr<Process>> profilerProcesses;
	extern bool _logProfilerOutput;
	extern void AddProfilerProcess(std::shared_ptr<Process> process);
	extern void AddProcessData(std::string processName, float data);
	extern void RemoveProfilerProcess(std::string toRemove);

	// Game Loop Prettification
	extern void StartGameLoop();
	extern void GameLoopUpdate();
	extern void PauseGameLoop();
	extern void StopGameLoop();
	extern int GetEllapsedGameTime();
	extern bool GameLoopStarted();
	extern bool GameLoopPaused();
	extern float GetAverageFps();
	extern float GetDeltaTime();

	// Helper Functions
	extern bool AreCollidingWorld(ImVec4 ObjectA, ImVec4 ObjectB);
	extern bool AreCollidingViewport(ImVec4 ObjectA, ImVec4 ObjectB);
	extern 	bool AreColliding(ImVec4 ObjectA, ImVec4 ObjectB);
	extern Vector2 Lerp(Vector2 startPos, Vector2 endPos, float ease);




	// #### FLATGUI #### //
	//
	namespace FlatGui
	{
		// Global variables
		extern bool _editingValue;
		extern float childPadding;

		// Colors
		extern ImVec4 transparentColor;
		extern ImVec4 whiteColor;
		extern ImVec4 outerWindowColor;
		extern ImVec4 innerWindowColor;
		extern ImVec4 singleItemColor;
		extern ImVec4 windowTitleBg;
		// Components
		//
 		extern ImVec4 componentBorderColor;
		// Collider borders
		extern ImVec4 boxColliderActiveColor;
		extern ImVec4 boxColliderInactiveColor;
		extern ImVec4 boxColliderCollidingColor;
		extern ImVec4 circleColliderActiveColor;
		extern ImVec4 circleColliderInactiveColor;
		// Button Components
		extern ImVec4 buttonComponentActiveColor;
		extern ImVec4 buttonComponentInctiveColor;
		// Log
		extern ImVec4 logTextColor;
		extern ImVec4 logBgColor;
		extern ImVec4 logOutlineColor;
		// Windows
		extern ImVec4 windowBgColor;
		extern ImVec4 frameBgColor;
		extern ImVec4 frameBgActiveColor;
		extern ImVec4 frameBgHoveredColor;
		extern ImVec4 dockingPreviewColor;
		extern ImVec4 dockingPreviewEmptyColor;
		// tabs
		extern ImVec4 tabColor;
		extern ImVec4 tabHoveredColor;
		extern ImVec4 tabUnfocusedColor;
		extern ImVec4 tabUnfocusedActiveColor;
		extern ImVec4 tabActiveColor;
		// title
		extern ImVec4 titleBgColor;
		extern ImVec4 titleBgActiveColor;
		extern ImVec4 titleBgCollapsedColor;
		// Text
		extern ImVec4 textSelectedBgColor;
		// Resizers
		extern ImVec4 resizeGripColor;
		extern ImVec4 resizeGripActiveColor;
		extern ImVec4 resizeGripHoveredColor;
		// Misc (not sure what they're for)
		extern ImVec4 popupBgColor;
		extern ImVec4 navWindowHighlightColor;
		extern ImVec4 navHighlightColor;
		extern ImVec4 navWindowDimBgColor; 
		extern ImVec4 modalWindowDimBgColor;
		// Input
		extern ImVec4 inputColor;
		// Tables
		extern ImVec4 noEditTableTextColor;
		extern ImVec4 noEditTableRowFieldBgColor;
		extern ImVec4 noEditTableRowValueBgColor;
		extern ImVec4 tableCellLightColor;
		extern ImVec4 tableCellDarkColor;
		// Trees
		extern ImVec4 treeSelectableColor;
		extern ImVec4 treeSelectableHoveredColor;
		extern ImVec4 treeSelectableActiveColor;
		extern ImVec4 treeSelectableSelectedColor;
		extern ImVec4 hierarchyChildObjectColor;
		// Combos
		extern ImVec4 comboBgColor;
		extern ImVec4 comboHoveredColor;
		extern ImVec4 comboSelectableColor;
		extern ImVec4 comboArrowColor;
		extern ImVec4 comboArrowHoveredColor;
		extern ImVec4 comboSelectedColor;
		extern ImVec4 comboHighlightedColor;
		// Buttons
		extern ImVec4 buttonColor;
		extern ImVec4 buttonHoveredColor;
		extern ImVec4 buttonActiveColor;
		extern ImVec4 imageButtonColor;
		extern ImVec4 imageButtonHoveredColor;
		extern ImVec4 imageButtonActiveColor;
		extern ImVec4 imageButtonTintColor;
		// Sliders/Drags
		extern ImVec4 sliderColor;
		extern ImVec4 sliderHoveredColor;
		extern ImVec4 sliderActiveColor;
		extern ImVec4 dragColor;
		extern ImVec4 dragHoveredColor;
		extern ImVec4 dragActiveColor;
		// Checkboxes
		extern ImVec4 checkboxBgColor;
		extern ImVec4 checkboxCheckColor;
		extern ImVec4 checkboxHoveredColor;
		extern ImVec4 checkboxActiveColor;


		extern int maxSpriteLayers;
		extern float spriteScaleMultiplier;

		// Icons
		extern std::shared_ptr<Texture> transformArrow;
		extern std::shared_ptr<Texture> cameraIcon;

		extern std::shared_ptr<Texture> playIcon;
		extern std::shared_ptr<Texture> pauseIcon;
		extern std::shared_ptr<Texture> stopIcon;
		extern std::shared_ptr<Texture> expandIcon;
		extern std::shared_ptr<Texture> expandFlippedIcon;
		extern std::shared_ptr<Texture> trashIcon;
		extern std::shared_ptr<Texture> openFileIcon;
		extern std::shared_ptr<Texture> newFileIcon;
		extern std::shared_ptr<Texture> saveFileIcon;
		extern std::shared_ptr<Texture> saveAsFileIcon;
		extern std::shared_ptr<Texture> keyFrameIcon;
		extern std::shared_ptr<Texture> timelineScrubberIcon;
		extern std::shared_ptr<Texture> threeDotsIcon;
		extern std::shared_ptr<Texture> showIcon;
		extern std::shared_ptr<Texture> hideIcon;

		extern SDL_Texture* transformArrowTexture;
		extern SDL_Texture* cameraTexture;

		extern SDL_Texture* playTexture;
		extern SDL_Texture* pauseTexture;
		extern SDL_Texture* stopTexture;		
		extern SDL_Texture* expandTexture;
		extern SDL_Texture* expandFlippedTexture;
		extern SDL_Texture* trashTexture;
		extern SDL_Texture* openFileTexture;
		extern SDL_Texture* newFileTexture;
		extern SDL_Texture* saveFileTexture;
		extern SDL_Texture* saveAsFileTexture;
		extern SDL_Texture* keyFrameTexture;
		extern SDL_Texture* timelineScrubberTexture;
		extern SDL_Texture* threeDotsTexture;
		extern SDL_Texture* showTexture;
		extern SDL_Texture* hideTexture;
		// Texture Colors
		extern ImVec2 uv0;
		extern ImVec2 uv1;
		extern ImVec4 tint_col;
		extern ImVec4 bg_col;

		// Flags
		extern ImGuiChildFlags childFlags;
		extern ImGuiChildFlags headerFlags;
		extern ImGuiTableFlags tableFlags;

		// Scene view
		extern float SCENE_VIEWPORT_WIDTH;
		extern float SCENE_VIEWPORT_HEIGHT;
		extern float DYNAMIC_VIEWPORT_WIDTH;
		extern float DYNAMIC_VIEWPORT_HEIGHT;
		extern bool _firstSceneRenderPass;
		extern bool _sceneHasBeenSet;
		extern ImVec2 sceneViewGridStep;
		extern ImVec2 sceneViewScrolling;
		extern ImVec2 sceneViewCenter;

		extern void RunOnceAfterInitialization();

		// Animation Manager
		extern void CreateNewAnimationFile(std::string path);
		extern void SaveAnimationFile(std::shared_ptr<Animation::S_AnimationProperties> propertiesObject, std::string path);
		extern std::shared_ptr<Animation::S_AnimationProperties> LoadAnimationFile(std::string path);

		// Profiler
		void Sparkline(const char* id, const float* values, int count, float min_v, float max_v, int offset, const ImVec4& col, const ImVec2& size);

		// Settings
		extern int iconTransparency;
		extern bool _clearBufferEveryFrame;

		// Hierarchy
		extern std::map<long, bool> leafExpandedTracker;

		// Window Visibility
		extern bool _showDemoWindow;
		extern bool _showSceneView;
		extern bool _showGameView;
		extern bool _showHierarchy;
		extern bool _showInspector;
		extern bool _showAnimator;
		extern bool _showAnimationPreview;
		extern bool _showKeyFrameEditor;
		extern bool _showLogger;
		extern bool _showProfiler;
		extern bool _showMappingContextEditor;

		// Game view
		extern float GAME_VIEWPORT_WIDTH;
		extern 	float GAME_VIEWPORT_HEIGHT;
		extern float xGameCenter;
		extern float yGameCenter;
		extern ImVec2 worldCenterPoint;
		extern ImVec2 gameViewCenter;
		extern ImVec2 gameViewGridStep;

		// Gui Rendering
		extern void SetupImGui();
		extern void CreateIcons();
		extern void Render(bool& quit);
		extern void HandleEvents(bool& quit);
		extern void MainMenuBar();
		extern std::string OpenSaveFileExplorer();
		extern std::string OpenLoadFileExplorer();
		extern std::string GetFilenameFromPath(std::string path, bool _keepExtension = false);
		extern void AddViewports();
		extern void RenderToolbar();
		extern void RenderHierarchy();
		extern void RenderInspector();
		extern void Game_RenderView();
		extern void Game_RenderObjects(ImVec2 canvas_p0, ImVec2 canvas_sz);
		extern void Scene_RenderView();
		extern void RenderAnimator();
		extern void RenderAnimationPreview();
		extern void RenderAnimationTimelineGrid(ImVec2& zeroPoint, ImVec2 scrolling, ImVec2 canvas_p0, ImVec2 canvas_p1, ImVec2 canvas_sz, float gridStep);
		extern void RenderKeyFrameEditor();
		extern void RenderLog();
		extern void RenderProfiler();
		extern void RenderMappingContextEditor();
		extern void Cleanup();

		extern void RenderGridView(ImVec2& centerPoint, ImVec2 &scrolling, bool _weightedScroll, ImVec2 canvas_p0, ImVec2 canvas_p1, ImVec2 canvas_sz, ImVec2 &step, ImVec2 centerOffset);
		extern void RenderViewObjects(std::vector<std::shared_ptr<GameObject>> objects, ImVec2 centerPoint, ImVec2 canvas_p0, ImVec2 canvas_sz, float step);
		extern void RenderSelfThenChildren(std::shared_ptr<GameObject> self, Vector2 parentOffset, Vector2 parentScale, ImVec2 scrolling, ImVec2 canvas_p0, ImVec2 canvas_sz, float step, ImDrawList* draw_list, ImDrawListSplitter* drawSplitter);

		// Helper Functions
		//
		// 
		// ImGui Wrappers
		extern void PushWindowStyles();
		extern void PopWindowStyles();
		extern void PushComboStyles();
		extern void PopComboStyles();
		extern void PushMenuStyles();
		extern void PopMenuStyles();
		extern void PushTableStyles();
		extern void PopTableStyles();
		extern void PushTable(std::string id, int columns, ImGuiTableFlags flags = tableFlags);
		extern bool RenderFloatDragTableRow(std::string id, std::string fieldName, float &value, float increment, float min, float max);
		extern bool RenderIntDragTableRow(std::string id, std::string fieldName, int& value, float increment, float min, float max);
		extern void RenderTextTableRow(std::string id, std::string fieldName, std::string value);
		extern void PopTable();
		extern bool RenderInput(std::string id, std::string label, std::string &value, bool _canOpenFiles = false, ImGuiInputTextFlags flags = 0);
		extern bool RenderButton(std::string text, ImVec2 size = ImVec2(0,0), float rounding = 1, ImVec4 color = buttonColor, ImVec4 hoverColor = buttonHoveredColor, ImVec4 activeColor = buttonActiveColor);
		extern bool RenderImageButton(std::string id, SDL_Texture *texture, ImVec2 size = ImVec2(16, 16), float rounding = 1, ImVec4 bgColor = imageButtonColor, ImVec4 tint = imageButtonTintColor, ImVec4 hoverColor = imageButtonHoveredColor, ImVec4 activeColor = imageButtonActiveColor);
		extern bool RenderSlider(std::string text, float width, float& value, float increment, float min, float max);
		extern bool RenderDragFloat(std::string text, float width, float& value, float increment, float min, float max, ImGuiSliderFlags flags = 0);
		extern bool RenderDragInt(std::string text, float width, int& value, float increment, int min, int max, ImGuiSliderFlags flags = 0);
		extern bool RenderCheckbox(std::string text, bool &_toCheck);
		extern void RenderSectionHeader(std::string headerText, float height = 0);

		extern ImU32 ImVec4ToImU32(ImVec4 color);
		
		// Hierarchy
		extern void ResetHierarchyExpanderTracker();

		extern ImVec2 AddImageToDrawList(SDL_Texture* texture, Vector2 position, ImVec2 centerPoint, float textureWidth, float textureHeight, Vector2 pivotPoint, Vector2 scale, bool _scalesWithZoom, float zoomMultiplier, ImDrawList *draw_list, ImU32 addColor = (((ImU32)(255) << 24) | ((ImU32)(255) << 16) | ((ImU32)(255) << 8) | ((ImU32)(255) << 0)));
		// Just add - canvas_p0 to get Window coordinates
		extern float WorldToViewport(float centerPoint, float worldPosition, float zoomFactor, bool _isYCoord = false);
		extern ImVec2 ViewportToWorld(ImVec2 viewportPosition);
		// Recursive - For Rendering Hierarchy Objects and their children
		extern void AddObjectWithChild(std::shared_ptr<GameObject> currentObject, const char* charName, int& node_clicked, long &queuedForDelete, float indent);
		extern void AddObjectWithoutChild(std::shared_ptr<GameObject> currentObject, const char* charName, int& node_clicked, long &queuedForDelete, float indent);
		// Recursive - For adding GameObjects and their children to the scene/game view
		void Game_RenderSelfThenChildren(std::shared_ptr<GameObject> self, Vector2 parentOffset, Vector2 parentScale, ImVec2 worldCenterPoint, ImVec2 canvas_p0, ImVec2 canvas_sz, ImDrawList* draw_list, ImDrawListSplitter* drawSplitter, ImVec2 cameraPosition, float cameraWidth, float cameraHeight, float cameraZoom);
		ImVec2 Game_GetTotalCameraOffset(std::shared_ptr<Camera> primaryCamera);
		// Recursive - For getting gameObjects total offset
		void Game_GetTotalOffsetAndScale(std::shared_ptr<GameObject> child, Vector2& offset, Vector2& scale);
	}
};



//DirectX12 stuff
//#include "imgui_impl_dx12.h"
//#include <d3d12.h>
//#include <dxgi1_4.h>
//
//#ifdef _DEBUG
//#define DX12_ENABLE_DEBUG_LAYER
//#endif
//
//#ifdef DX12_ENABLE_DEBUG_LAYER
//#include <dxgidebug.h>
//#pragma comment(lib, "dxguid.lib")
//#endif