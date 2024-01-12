#pragma once
#include <stdio.h>
#include <string>
#include <sstream>
#include <vector>
#include "SDL.h"
#include <SDL_syswm.h>
#include <imgui.h>
#include "Window.h"
#include "GameObject.h"
#include "SceneManager.h"
#include "Logger.h"
#include "ScriptComponent.h"
#include "GameLoop.h"
#include "Canvas.h"
#include "WidgetsManager.h"


//ImGui - SDL Renderer
#pragma once
#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_sdlrenderer2.h"


namespace FlatEngine
{
	using ComponentTypes = Component::ComponentTypes;

	extern bool _isDebugMode;

	// #### FLAGENGINE #### //
	// 
	// Variables
	extern long FocusedGameObjectID;
	extern void SetFocusedGameObjectID(long ID);
	extern long GetFocusedGameObjectID();

	extern FlatEngine::SceneManager *sceneManager;
	extern FlatEngine::Logger *logger;
	extern FlatEngine::GameLoop *gameLoop;
	extern std::shared_ptr<FlatEngine::FlatGui::WidgetsManager> widgetsManager;

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

	// Scene Manager Prettification
	extern std::shared_ptr<Scene> GetLoadedScene();
	extern std::shared_ptr<Scene> CreateNewScene();
	extern void SaveScene(std::shared_ptr<Scene> scene);
	extern void LoadScene(std::string name);

	// Scene Prettification
	extern std::vector<std::shared_ptr<GameObject>> GetSceneObjects();
	extern std::shared_ptr<GameObject> CreateGameObject(long parentID = -1);
	extern void DeleteGameObject(int sceneObjectID);
	extern std::shared_ptr<Component> GetObjectComponent(long objectID, ComponentTypes type);
	extern std::shared_ptr<GameObject> GetObjectById(long objectID);

	// Logging Prettification
	extern void LogString(std::string line = "");
	extern void LogFloat(float var, std::string line = "");
	extern void LogInt(int var, std::string line = "");
	extern void LogVector2(Vector2 vector, std::string line = "");
	extern void DrawRectangle(Vector2 startingPoint, Vector2 endingPoint, ImVec2 canvas_p0, ImVec2 canvas_sz, ImU32 color, float thickness, ImDrawList* drawList);
	extern void DrawRectangle(ImVec2 startingPoint, ImVec2 endingPoint, ImVec2 canvas_p0, ImVec2 canvas_sz, ImU32 color, float thickness, ImDrawList* drawList);
	extern void DrawLine(Vector2 startingPoint, Vector2 endingPoint, ImU32 color, float thickness, ImDrawList* drawList);
	extern void DrawLine(ImVec2 startingPoint, ImVec2 endingPoint, ImU32 color, float thickness, ImDrawList* drawList);
	extern void DrawPoint(Vector2 point, ImU32 color, ImDrawList* drawList);
	extern void DrawPoint(ImVec2 point, ImU32 color, ImDrawList* drawList);

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
	//
	extern bool AreColliding(ImVec4 ObjectA, ImVec4 ObjectB);




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

		// Icons
		extern std::unique_ptr<Texture> expandIcon;
		extern std::unique_ptr<Texture> expandFlippedIcon;
		extern std::unique_ptr<Texture> trashIcon;
		extern std::unique_ptr<Texture> transformArrow;
		extern std::unique_ptr<Texture> cameraTexture;

		// Scene view
		extern float SCENE_VIEWPORT_WIDTH;
		extern float SCENE_VIEWPORT_HEIGHT;
		extern float DYNAMIC_VIEWPORT_WIDTH;
		extern float DYNAMIC_VIEWPORT_HEIGHT;
		extern bool _firstSceneRenderPass;
		extern bool _sceneHasBeenSet;
		extern float gridStep;

		// Settings
		extern int iconTransparency;
		extern bool _clearBufferEveryFrame;

		// Game view
		extern float GAME_VIEWPORT_WIDTH;
		extern 	float GAME_VIEWPORT_HEIGHT;
		extern float xGameCenter;
		extern float yGameCenter;
		extern ImVec2 worldCenterPoint;

		// Gui Rendering
		extern void SetupImGui();
		extern void Render(bool& quit);
		extern void AddViewports();
		extern void RenderToolbar();
		extern void RenderHierarchy();
		extern void RenderInspector();
		extern void Game_RenderView();
		extern void Game_RenderObjects(ImVec2 canvas_p0, ImVec2 canvas_sz);
		extern void Scene_RenderView();
		extern void Scene_RenderGrid(ImVec2 scrolling, ImVec2 canvas_p0, ImVec2 canvas_p1, ImVec2 canvas_sz, float gridStep);
		extern void Scene_RenderObjects(ImVec2 scrolling, ImVec2 canvas_p0, ImVec2 canvas_sz);
		extern void RenderLog();
		extern void Cleanup();

		// Helper Functions
		//
		extern void AddImageToDrawList(SDL_Texture* texture, Vector2 position, ImVec2 centerPoint, float textureWidth, float textureHeight, Vector2 pivotPoint, Vector2 scale, bool _scalesWithZoom, float zoomMultiplier, ImDrawList *draw_list, ImU32 addColor = (((ImU32)(255) << 24) | ((ImU32)(255) << 16) | ((ImU32)(255) << 8) | ((ImU32)(255) << 0)));
		// Just add - canvas_p0 to get Window coordinates
		extern float WorldToViewport(float centerPoint, float worldPosition, float zoomFactor, bool _isYCoord = false);
		extern ImVec2 ViewportToWorld(ImVec2 viewportPosition);
		// Recursive - For Rendering Hierarchy Objects and their children
		extern void AddObjectWithChild(std::shared_ptr<GameObject> currentObject, const char* charName, int& node_clicked, long &queuedForDelete);
		extern void AddObjectWithoutChild(std::shared_ptr<GameObject> currentObject, const char* charName, int& node_clicked, long &queuedForDelete);
		// Recursive - For adding GameObjects and their children to the scene/game view
		void Scene_RenderSelfThenChildren(std::shared_ptr<GameObject> self, Vector2 parentOffset, ImVec2 scrolling, ImVec2 canvas_p0, ImVec2 canvas_sz, ImDrawList* draw_list, ImDrawListSplitter* drawSplitter);
		void Game_RenderSelfThenChildren(std::shared_ptr<GameObject> self, Vector2 parentOffset, ImVec2 worldCenterPoint, ImVec2 canvas_p0, ImVec2 canvas_sz, ImDrawList* draw_list, ImDrawListSplitter* drawSplitter, ImVec2 cameraPosition, float cameraWidth, float cameraHeight, float cameraZoom);
		ImVec2 Game_GetTotalCameraOffset(std::shared_ptr<Camera> primaryCamera);
		// Recursive - For getting gameObjects total offset
		void Game_GetParentOffset(std::shared_ptr<GameObject> child, Vector2& offset);
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