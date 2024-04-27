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

	extern std::shared_ptr<GameManager> gameManager;

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

	extern FlatEngine::SceneManager *sceneManager;
	extern FlatEngine::Logger *logger;
	extern FlatEngine::GameLoop *gameLoop;
	extern std::shared_ptr<FlatEngine::FlatGui::WidgetsManager> widgetsManager;
	extern std::shared_ptr<FlatEngine::FlatGui::UIManager> uiManager;

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

	// Audio Managing
	extern std::shared_ptr<Sound> soundController;

	// Project Management
	extern std::shared_ptr<Project> loadedProject;
	extern void OpenProject(std::string path);
	extern void SaveProject(std::shared_ptr<Project> project, std::string path);

	// Context Management
	extern std::vector<std::shared_ptr<MappingContext>> mappingContexts;
	extern void SaveMappingContext(std::string path, std::shared_ptr<MappingContext> context);
	extern void InitializeMappingContexts();
	extern void ClearIAContextBindings();
	extern std::shared_ptr<MappingContext> GetMappingContext(std::string contextName);

	// Scene Manager Prettification
	extern std::shared_ptr<Scene> GetLoadedScene();
	extern std::shared_ptr<Scene> CreateNewScene();
	std::string GetLoadedScenePath();
	extern long GetNextComponentID();
	extern long GetNextGameObjectID();
	extern void SaveScene(std::shared_ptr<Scene> scene, std::string filename);
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
	extern Vector2 Lerp(Vector2 startPos, Vector2 endPos, float ease);




	// #### FLATGUI #### //
	//
	namespace FlatGui
	{
		// General window variables
		extern bool _editingValue;
		extern float childPadding;
		extern ImVec4 outerWindowColor;
		extern ImVec4 innerWindowColor;
		extern ImVec4 singleItemColor;

		extern int maxSpriteLayers;
		extern float spriteScaleMultiplier;

		// Icons
		extern std::shared_ptr<Texture> playIcon;
		extern std::shared_ptr<Texture> pauseIcon;
		extern std::shared_ptr<Texture> stopIcon;
		extern std::shared_ptr<Texture> expandIcon;
		extern std::shared_ptr<Texture> expandFlippedIcon;
		extern std::shared_ptr<Texture> trashIcon;
		extern std::shared_ptr<Texture> openFileIcon;
		extern std::shared_ptr<Texture> newFileIcon;
		extern std::shared_ptr<Texture> saveFileIcon;
		extern std::shared_ptr<Texture> transformArrow;
		extern std::shared_ptr<Texture> cameraTexture;
		extern std::shared_ptr<Texture> keyFrameIcon;
		extern std::shared_ptr<Texture> timelineScrubberIcon;
		extern SDL_Texture* playTexture;
		extern SDL_Texture* pauseTexture;
		extern SDL_Texture* stopTexture;		
		extern SDL_Texture* expandTexture;
		extern SDL_Texture* expandFlippedTexture;
		extern SDL_Texture* trashTexture;
		extern SDL_Texture* openFileTexture;
		extern SDL_Texture* newFileTexture;
		extern SDL_Texture* saveFileTexture;
		extern SDL_Texture* keyFrameTexture;
		extern SDL_Texture* timelineScrubberTexture;

		extern ImVec2 uv0;
		extern ImVec2 uv1;
		extern ImVec4 tint_col;
		extern ImVec4 bg_col;

		// Scene view
		extern float SCENE_VIEWPORT_WIDTH;
		extern float SCENE_VIEWPORT_HEIGHT;
		extern float DYNAMIC_VIEWPORT_WIDTH;
		extern float DYNAMIC_VIEWPORT_HEIGHT;
		extern bool _firstSceneRenderPass;
		extern bool _sceneHasBeenSet;
		extern float gridStep;
		extern ImVec2 sceneViewScrolling;

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

		// Window Visibility
		extern bool _showSceneView;
		extern bool _showGameView;
		extern bool _showHierarchy;
		extern bool _showInspector;
		extern bool _showAnimator;
		extern bool _showAnimationPreview;
		extern bool _showKeyFrameEditor;
		extern bool _showLogger;
		extern bool _showProfiler;

		// Game view
		extern float GAME_VIEWPORT_WIDTH;
		extern 	float GAME_VIEWPORT_HEIGHT;
		extern float xGameCenter;
		extern float yGameCenter;
		extern ImVec2 worldCenterPoint;

		// Gui Rendering
		extern void SetupImGui();
		extern void CreateIcons();
		extern void Render(bool& quit);
		extern void HandleEvents(bool& quit);
		extern void MainMenuBar();
		extern std::string OpenSaveFileExplorer();
		extern std::string OpenLoadFileExplorer();
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
		extern void Cleanup();

		extern void RenderGridView(ImVec2& centerPoint, ImVec2 scrolling, ImVec2 canvas_p0, ImVec2 canvas_p1, ImVec2 canvas_sz, ImVec2 step, ImVec2 centerOffset);
		extern void RenderViewObjects(std::vector<std::shared_ptr<GameObject>> objects, ImVec2 centerPoint, ImVec2 canvas_p0, ImVec2 canvas_sz);
		extern void RenderSelfThenChildren(std::shared_ptr<GameObject> self, Vector2 parentOffset, Vector2 parentScale, ImVec2 scrolling, ImVec2 canvas_p0, ImVec2 canvas_sz, ImDrawList* draw_list, ImDrawListSplitter* drawSplitter);

		// Helper Functions
		//
		extern ImVec2 AddImageToDrawList(SDL_Texture* texture, Vector2 position, ImVec2 centerPoint, float textureWidth, float textureHeight, Vector2 pivotPoint, Vector2 scale, bool _scalesWithZoom, float zoomMultiplier, ImDrawList *draw_list, ImU32 addColor = (((ImU32)(255) << 24) | ((ImU32)(255) << 16) | ((ImU32)(255) << 8) | ((ImU32)(255) << 0)));
		// Just add - canvas_p0 to get Window coordinates
		extern float WorldToViewport(float centerPoint, float worldPosition, float zoomFactor, bool _isYCoord = false);
		extern ImVec2 ViewportToWorld(ImVec2 viewportPosition);
		// Recursive - For Rendering Hierarchy Objects and their children
		extern void AddObjectWithChild(std::shared_ptr<GameObject> currentObject, const char* charName, int& node_clicked, long &queuedForDelete);
		extern void AddObjectWithoutChild(std::shared_ptr<GameObject> currentObject, const char* charName, int& node_clicked, long &queuedForDelete);
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