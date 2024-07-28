#pragma once
#include "Vector2.h"
#include "Vector4.h"
#include "Animation.h"

#include <sstream>
#include <map>
#include "imgui.h"
#include "SDL.h"

/*
######################################
######							######
######     Gui Declarations     ######
######                          ######
######################################
*/

namespace FlatEngine {
	class GameObject;
	class TagList;
	class Texture;
	class Project;
	class SceneManager;
	class Scene;
	class Process;
}

using GameObject = FlatEngine::GameObject;
using TagList = FlatEngine::TagList;
using Texture = FlatEngine::Texture;
using Animation = FlatEngine::Animation;
using Project = FlatEngine::Project;
using SceneManager = FlatEngine::SceneManager;
using Scene = FlatEngine::Scene;
using Process = FlatEngine::Process;

namespace FlatGui
{
	class WidgetsManager;

	// Global variables
	extern bool _editingValue;
	extern float childPadding;
	extern ImDrawList* drawList;

	// Managers
	extern WidgetsManager widgetsManager;

	//////////////////////
	//Global      Colors//
	//  //////VV///////  /
	//     UUUUUUUU     //
	///\-------------/////
	//////////////////////
	extern Vector4 transparentColor;
	extern Vector4 whiteColor;
	extern Vector4 outerWindowColor;
	extern Vector4 innerWindowColor;
	extern Vector4 singleItemColor;
	extern Vector4 windowTitleBg;
	//////////////////////
	// Components ////////
	//////////////////////
	extern Vector4 componentBorderColor;
	// Collider borders
	extern Vector4 colliderActiveColor;
	extern Vector4 colliderInactiveColor;
	extern Vector4 colliderCollidingColor;
	// Button Components
	extern Vector4 buttonComponentActiveColor;
	extern Vector4 buttonComponentInctiveColor;
	// Canvas
	extern Vector4 canvasBorderColor;
	// Camera
	extern Vector4 cameraBoxColor;
	// Log
	extern Vector4 logTextColor;
	extern Vector4 logBgColor;
	extern Vector4 logOutlineColor;
	//////////////////////
	/// Window  Colors ///
	//////////////////////
	extern Vector4 windowBgColor;
	extern Vector4 frameBgColor;
	extern Vector4 frameBgActiveColor;
	extern Vector4 frameBgHoveredColor;
	extern Vector4 dockingPreviewColor;
	extern Vector4 dockingPreviewEmptyColor;
	// tabs
	extern Vector4 tabColor;
	extern Vector4 tabHoveredColor;
	extern Vector4 tabUnfocusedColor;
	extern Vector4 tabUnfocusedActiveColor;
	extern Vector4 tabActiveColor;
	// title
	extern Vector4 titleBgColor;
	extern Vector4 titleBgActiveColor;
	extern Vector4 titleBgCollapsedColor;
	// Text
	extern Vector4 textSelectedBgColor;
	// Resizers
	extern Vector4 resizeGripColor;
	extern Vector4 resizeGripActiveColor;
	extern Vector4 resizeGripHoveredColor;
	// Misc (not sure what they're for)
	extern Vector4 popupBgColor;
	extern Vector4 navWindowHighlightColor;
	extern Vector4 navHighlightColor;
	extern Vector4 navWindowDimBgColor;
	extern Vector4 modalWindowDimBgColor;
	////////////////////
	// Custom Colors ///
	//////////////////// 
	// Input
	extern Vector4 inputColor;
	// Tables
	extern Vector4 noEditTableTextColor;
	extern Vector4 noEditTableRowFieldBgColor;
	extern Vector4 noEditTableRowValueBgColor;
	extern Vector4 tableCellLightColor;
	extern Vector4 tableCellDarkColor;
	// Trees
	extern Vector4 treeSelectableColor;
	extern Vector4 treeSelectableHoveredColor;
	extern Vector4 treeSelectableActiveColor;
	extern Vector4 treeSelectableSelectedColor;
	extern Vector4 hierarchyChildObjectColor;
	// Combos
	extern Vector4 comboBgColor;
	extern Vector4 comboHoveredColor;
	extern Vector4 comboSelectableColor;
	extern Vector4 comboArrowColor;
	extern Vector4 comboArrowHoveredColor;
	extern Vector4 comboSelectedColor;
	extern Vector4 comboHighlightedColor;
	// Buttons
	extern Vector4 buttonColor;
	extern Vector4 buttonHoveredColor;
	extern Vector4 buttonActiveColor;
	extern Vector4 imageButtonColor;
	extern Vector4 imageButtonDarkColor;
	extern Vector4 imageButtonHoveredColor;
	extern Vector4 imageButtonActiveColor;
	extern Vector4 imageButtonTintColor;
	// Sliders/Drags
	extern Vector4 sliderColor;
	extern Vector4 sliderHoveredColor;
	extern Vector4 sliderActiveColor;
	extern Vector4 dragColor;
	extern Vector4 dragHoveredColor;
	extern Vector4 dragActiveColor;
	// Checkboxes
	extern Vector4 checkboxBgColor;
	extern Vector4 checkboxCheckColor;
	extern Vector4 checkboxHoveredColor;
	extern Vector4 checkboxActiveColor;
	////////////////////
	//// End Colors ////
	////////////////////

	extern int maxSpriteLayers;
	extern float spriteScaleMultiplier;

	// Icons
	extern Texture transformArrow;
	extern Texture transformArrowAllWhite;
	extern Texture transformArrowXWhite;
	extern Texture transformArrowYWhite;
	extern Texture cameraIcon;
	extern Texture playIcon;
	extern Texture pauseIcon;
	extern Texture stopIcon;
	extern Texture nextFrameIcon;
	extern Texture expandIcon;
	extern Texture expandFlippedIcon;
	extern Texture trashIcon;
	extern Texture openFileIcon;
	extern Texture newFileIcon;
	extern Texture saveFileIcon;
	extern Texture saveAsFileIcon;
	extern Texture keyFrameIcon;
	extern Texture timelineScrubberIcon;
	extern Texture threeDotsIcon;
	extern Texture showIcon;
	extern Texture hideIcon;
	extern Texture prefabCubeIcon;
	extern Texture leftIcon;
	extern Texture rightIcon;
	extern Texture downIcon;
	extern Texture upIcon;
	extern Texture upLeftIcon;
	extern Texture upRightIcon;
	extern Texture downLeftIcon;
	extern Texture downRightIcon;
	extern Texture centerIcon;

	// Texture Colors
	extern Vector2 uv0;
	extern Vector2 uv1;
	extern Vector4 tint_col;
	extern Vector4 bg_col;

	// Flags
	extern ImGuiChildFlags childFlags;
	extern ImGuiChildFlags autoResizeChildFlags;
	extern ImGuiChildFlags resizeChildFlags;
	extern ImGuiChildFlags headerFlags;
	extern ImGuiTableFlags tableFlags;
	extern ImGuiTableFlags resizeableTableFlags;

	// Project Management
	extern std::shared_ptr<Project> loadedProject;
	extern void OpenProject(std::string path);
	extern void SaveProject(std::shared_ptr<Project> project, std::string path);

	extern std::shared_ptr<GameObject> GetPlayerObject();
	extern std::shared_ptr<GameObject> playerObject;

	// Focused GameObject
	extern long FocusedGameObjectID;
	extern void SetFocusedGameObjectID(long ID);
	extern long GetFocusedGameObjectID();

	// Focused Animation
	extern std::shared_ptr<Animation::S_AnimationProperties> FocusedAnimation;
	extern GameObject objectForFocusedAnimation;
	extern std::shared_ptr<Animation::S_Property> selectedKeyFrameToEdit;
	extern int previewAnimationStartTime;
	extern int previewAnimationTime;
	extern bool _playPreviewAnimation;
	extern void SetFocusedAnimation(std::shared_ptr<Animation::S_AnimationProperties> animation);
	extern std::shared_ptr<Animation::S_AnimationProperties> GetFocusedAnimation();
	// Global Animation Event Functions
	extern void DestroySelf(std::shared_ptr<FlatEngine::GameObject> thisObject);

	// Scene view
	extern float SCENE_VIEWPORT_WIDTH;
	extern float SCENE_VIEWPORT_HEIGHT;
	extern float DYNAMIC_VIEWPORT_WIDTH;
	extern float DYNAMIC_VIEWPORT_HEIGHT;
	extern bool _firstSceneRenderPass;
	extern bool _sceneHasBeenSet;
	extern Vector2 sceneViewGridStep;
	extern Vector2 sceneViewScrolling;
	extern Vector2 sceneViewCenter;
	extern bool _sceneViewLockedOnObject;
	extern GameObject *sceneViewLockedObject;

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
	extern bool _showSettings;

	// Game view
	extern float GAME_VIEWPORT_WIDTH;
	extern 	float GAME_VIEWPORT_HEIGHT;
	extern float xGameCenter;
	extern float yGameCenter;
	extern Vector2 gameViewCenter;
	extern Vector2 gameViewGridStep;

	// Profiler
	extern std::vector<std::shared_ptr<Process>> profilerProcesses;
	extern bool _logProfilerOutput;
	extern void AddProfilerProcess(std::string name);
	extern void AddProcessData(std::string processName, float data);
	extern void RemoveProfilerProcess(std::string toRemove);

	// Gui Rendering
	extern void SetupImGui();
	extern void CreateIcons();
	extern void Render(bool& quit);
	extern void RenderClear();
	extern void HandleEvents(bool& quit);
	extern void SetupProfilerProcesses();
	extern void RunOnceAfterInitialization();

	// Viewports
	extern void MainMenuBar();
	extern void AddViewports();
	extern void RenderToolbar();
	extern void RenderHierarchy();
	extern void RenderInspector();
	extern void Game_RenderView();
	extern void Game_RenderObjects(Vector2 canvas_p0, Vector2 canvas_sz);
	extern void Game_RenderObject(GameObject self, Vector2 canvas_p0, Vector2 canvas_sz, ImDrawList* draw_list, ImDrawListSplitter* drawSplitter, Vector2 cameraPosition, float cameraWidth, float cameraHeight);
	extern void Scene_RenderView();
	extern void RenderAnimator();
	extern void RenderAnimationPreview();
	extern void RenderAnimationTimelineGrid(Vector2& zeroPoint, Vector2 scrolling, Vector2 canvas_p0, Vector2 canvas_p1, Vector2 canvas_sz, float gridStep);
	extern void RenderKeyFrameEditor();
	extern void RenderLog();
	extern void RenderProfiler();
	extern void RenderMappingContextEditor();
	extern void RenderSettings();
	extern void Cleanup();
	extern void RenderGridView(Vector2& centerPoint, Vector2& scrolling, bool _weightedScroll, Vector2 canvas_p0, Vector2 canvas_p1, Vector2 canvas_sz, Vector2& step, Vector2 centerOffset);
	extern void RenderViewObjects(std::vector<GameObject> objects, Vector2 centerPoint, Vector2 canvas_p0, Vector2 canvas_sz, float step);
	extern void RenderViewObject(GameObject &self, Vector2 scrolling, Vector2 canvas_p0, Vector2 canvas_sz, float step, ImDrawList* draw_list, ImDrawListSplitter* drawSplitter);

	// Helper Functions //

	// ImGui Wrappers
	extern void BeginWindow(std::string name);
	extern void BeginWindow(std::string name, bool& _isOpen);
	extern void EndWindow();
	extern void BeginWindowChild(std::string title);
	extern void BeginResizeWindowChild(std::string title);
	extern void EndWindowChild();
	extern void PushWindowStyles();
	extern void PopWindowStyles();
	extern void PushComboStyles();
	extern void PopComboStyles();
	extern void PushMenuStyles();
	extern void PopMenuStyles();
	extern void PushTableStyles();
	extern void PopTableStyles();
	extern bool PushTable(std::string id, int columns, ImGuiTableFlags flags = tableFlags);
	extern bool RenderFloatDragTableRow(std::string id, std::string fieldName, float& value, float increment, float min, float max);
	extern bool RenderTagListTableRow(std::string id, std::string fieldName, TagList& tagList);
	extern bool RenderIntDragTableRow(std::string id, std::string fieldName, int& value, float speed, int min, int max);
	extern bool RenderCheckboxTableRow(std::string id, std::string fieldName, bool& _value);
	extern void RenderSelectableTableRow(std::string id, std::string fieldName, std::vector<std::string> options, int& current_option);
	extern void RenderTextTableRow(std::string id, std::string fieldName, std::string value, std::string value2 = "");
	extern void PopTable();
	extern bool RenderInput(std::string id, std::string label, std::string& value, bool _canOpenFiles = false, ImGuiInputTextFlags flags = 0);
	extern bool RenderButton(std::string text, Vector2 size = Vector2(0, 0), float rounding = 1, Vector4 color = buttonColor, Vector4 hoverColor = buttonHoveredColor, Vector4 activeColor = buttonActiveColor);
	extern bool RenderImageButton(std::string id, SDL_Texture* texture, Vector2 size = Vector2(16, 16), float rounding = 1, Vector4 bgColor = imageButtonColor, Vector4 tint = imageButtonTintColor, Vector4 hoverColor = imageButtonHoveredColor, Vector4 activeColor = imageButtonActiveColor);
	extern bool RenderDragFloat(std::string text, float width, float& value, float increment, float min, float max, ImGuiSliderFlags flags = 0);
	extern bool RenderDragInt(std::string text, float width, int& value, float speed, int min, int max, ImGuiSliderFlags flags = 0);
	extern bool RenderCheckbox(std::string text, bool& _toCheck);
	extern void RenderSectionHeader(std::string headerText, float height = 0);
	extern bool RenderInvisibleButton(std::string id, Vector2 startingPoint, Vector2 size, bool _allowOverlap = true, bool _showRect = false);
	extern void RenderSelectable(std::string id, std::vector<std::string> options, int& current_option);
	extern void PushTreeList(std::string id);
	extern void RenderTreeLeaf(std::string name, std::string& node_clicked);
	extern void PopTreeList();
	// SDL
	extern Vector2 AddImageToDrawList(SDL_Texture* texture, Vector2 position, Vector2 centerPoint, float textureWidth, float textureHeight, Vector2 pivotPoint, Vector2 scale, bool _scalesWithZoom, float zoomMultiplier, ImDrawList* draw_list, float rotation = 0, ImU32 addColor = (((ImU32)(255) << 24) | ((ImU32)(255) << 16) | ((ImU32)(255) << 8) | ((ImU32)(255) << 0)));
	// Color converter
	extern ImU32 Vector4ToImU32(Vector4 color);
	// SceneView
	extern void RenderSceneViewTooltip();
	// Hierarchy
	extern void AddObjectWithChild(GameObject currentObject, const char* charName, int& node_clicked, long& queuedForDelete, float indent);
	extern void AddObjectWithoutChild(GameObject currentObject, const char* charName, int& node_clicked, long& queuedForDelete, float indent);
	extern void ResetHierarchyExpanderTracker();
	// Profiler
	void Sparkline(const char* id, const float* values, int count, float min_v, float max_v, int offset, const Vector4& col, const Vector2& size);
	// Casts
	extern std::vector<std::shared_ptr<GameObject>> RayCast(Vector2 origin, Vector2 direction, float distance);
	extern std::vector<std::shared_ptr<GameObject>> SphereCast(Vector2 origin, Vector2 direction, float distance);
}