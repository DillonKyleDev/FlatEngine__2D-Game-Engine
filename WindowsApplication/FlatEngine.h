#pragma once
#include <stdio.h>
#include <string>
#include <sstream>
#include "SDL.h"
#include <SDL_syswm.h>
#include <imgui.h>
#include "Window.h"
#include "GameObject.h"
#include "SceneManager.h"
#include "Logger.h"
#include "Script.h"
#include "GameLoop.h"

//ImGui - SDL Renderer
#pragma once
#include "imgui.h"
#include "../WindowsApplication/backends/imgui_impl_sdl2.h"
#include "../WindowsApplication/backends/imgui_impl_sdlrenderer2.h"


namespace FlatEngine
{
	// #### FLAGENGINE #### //
	// 
	// Variables
	extern int FocusedGameObjectIndex;
	void SetFocusedGameObjectIndex(int index);
	int GetFocusedGameObjectIndex();
	extern FlatEngine::SceneManager *sceneManager;
	extern FlatEngine::Logger *logger;
	extern FlatEngine::GameLoop *gameLoop;

	// Engine
	extern void Run(bool &_hasQuit);

	// Logging Prettification
	extern void LogString(std::string line = "");
	extern void LogFloat(float var, std::string line = "");
	extern void LogInt(int var, std::string line = "");
	extern void LogVector2(Vector2 vector, std::string line = "");
	extern void DrawRectangle(Vector2 startingPoint, Vector2 endingPoint);
	extern void DrawRectangle(ImVec2 startingPoint, ImVec2 endingPoint);

	// Game Loop Prettification
	extern void StartGameLoop();
	extern void GameLoopUpdate();
	extern void PauseGameLoop();
	extern void StopGameLoop();
	extern int GetEllapsedGameTime();
	extern bool GameLoopStarted();
	extern bool GameLoopPaused();
	extern float GetAverageFps();




	// #### FLATGUI #### //
	namespace FlatGui
	{
		// General window variables
		extern bool _editingValue;
		extern float childPadding;
		extern ImVec4 outerWindowColor;
		extern ImVec4 innerWindowColor;
		extern ImVec4 singleItemColor;

		// Scene view
		extern int zoomLevels[];
		extern float zoomMultipliers[];
		extern int currentZoomLevel;
		extern float currentZoomMultiplier;
		extern float SCENE_VIEWPORT_WIDTH;
		extern float SCENE_VIEWPORT_HEIGHT;
		extern bool _firstSceneRenderPass;
		extern bool _sceneHasBeenSet;
		extern float gridStep;
		extern float xSceneCenter;
		extern float ySceneCenter;
		// Game view
		extern float GAME_VIEWPORT_WIDTH;
		extern 	float GAME_VIEWPORT_HEIGHT;
		extern float xGameCenter;
		extern float yGameCenter;
		extern bool _firstGameViewRenderPass;
		extern bool _gameViewHasBeenSet;

		extern void SetupImGui();
		extern void Render(bool& quit);
		extern void AddViewports();
		extern void RenderToolbar();
		extern void RenderHierarchy();
		extern void RenderInspector();
		extern void RenderGameView();
		extern void RenderGameObjects(ImVec2 scrolling, ImVec2 canvas_p0);
		extern void RenderSceneView();
		extern void RenderSceneGrid(ImVec2 scrolling, ImVec2 canvas_p0, ImVec2 canvas_p1, ImVec2 canvas_sz);
		extern void RenderSceneObjects(ImVec2 scrolling, ImVec2 canvas_p0);
		extern void IncreaseSceneZoom();
		extern void DecreaseSceneZoom();
		extern void RenderLog();
		extern void Cleanup();

		// Helper
		extern void AddImageToDrawList(SDL_Texture* texture, Vector2 position, ImVec2 centerPoint, float textureWidth, float textureHeight, Vector2 pivotPoint, ImVec2 scrolling, ImVec2 canvas_p0, Vector2 scale, bool _scalesWithZoom, float zoomMultiplier);
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