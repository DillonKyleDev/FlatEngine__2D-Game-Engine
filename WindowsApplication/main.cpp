#pragma once
#include <SDL.h>
#include "Window.h"
#include "FlatEngine.h"
#include "TextureManager.h"
#include "SceneManager.h"
#include <SDL_mixer.h>
#include "Logger.h"
#include "Process.h"

//Main loop flag
bool _hasQuit = false;

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	SDL_SetHint(SDL_HINT_JOYSTICK_THREAD, "1");
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		FlatEngine::logger->LogString("SDL initialized... - Video - Audio - Joystick -");

		//Set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0"))
		{
			printf("Warning: Linear texture filtering not enabled!");
		}

		char t[] = "FlatEngine";
		char* title = &(t[0]);

		//Initialize Window::window
		if (Window::Init(title, 1920, 1080))
		{
			FlatEngine::LogString("Window initialized...");

			//Initialize SDL_image for png loading
			int imgFlags = IMG_INIT_PNG;
			if (!(IMG_Init(imgFlags) & imgFlags))
			{
				printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
				success = false;
			}
			else
			{
				FlatEngine::LogString("SDL_image initialized...");
				//Initialize SDL_ttf for text rendering
				if (TTF_Init() == -1)
				{
					printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
					success = false;
				}
				else
				{
					//Initialize SDL_mixer
					if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
					{
						printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
						success = false;
					}
					else
					{
						FlatEngine::LogString("SDL_mixer initialized...");
						FlatEngine::LogString("Ready...");
						FlatEngine::LogSeparator();
						FlatEngine::LogString("Begin Logging...");
						FlatEngine::LogSeparator();

						// Add Profiler Processes
						// 
						// Render
						std::shared_ptr<FlatEngine::Process> Render = std::make_shared<FlatEngine::Process>("Render");
						AddProfilerProcess(Render);
						// RenderMainMenuBar
						std::shared_ptr<FlatEngine::Process> RenderMainMenuBar = std::make_shared<FlatEngine::Process>("RenderMainMenuBar");
						AddProfilerProcess(RenderMainMenuBar);
						// RenderToolbar
						std::shared_ptr<FlatEngine::Process> RenderToolbar = std::make_shared<FlatEngine::Process>("RenderToolbar");
						AddProfilerProcess(RenderToolbar);
						// RenderHierarchy
						std::shared_ptr<FlatEngine::Process> RenderHierarchy = std::make_shared<FlatEngine::Process>("RenderHierarchy");
						AddProfilerProcess(RenderHierarchy);
						// RenderInspector
						std::shared_ptr<FlatEngine::Process> RenderInspector = std::make_shared<FlatEngine::Process>("RenderInspector");
						AddProfilerProcess(RenderInspector);
						// Game_RenderView
						std::shared_ptr<FlatEngine::Process> Game_RenderView = std::make_shared<FlatEngine::Process>("Game_RenderView");
						AddProfilerProcess(Game_RenderView);	


						// Game_RenderView_Camera
						std::shared_ptr<FlatEngine::Process> Game_RenderView_Camera = std::make_shared<FlatEngine::Process>("##Game_RenderView_Camera");
						AddProfilerProcess(Game_RenderView_Camera);
						// Game_RenderView_RenderObject
						std::shared_ptr<FlatEngine::Process> Game_RenderView_RenderObject = std::make_shared<FlatEngine::Process>("##Game_RenderView_RenderObject");
						AddProfilerProcess(Game_RenderView_RenderObject);



						// Scene_RenderView
						std::shared_ptr<FlatEngine::Process> Scene_RenderView = std::make_shared<FlatEngine::Process>("Scene_RenderView");
						AddProfilerProcess(Scene_RenderView);
						// RenderAnimator
						std::shared_ptr<FlatEngine::Process> RenderAnimator = std::make_shared<FlatEngine::Process>("RenderAnimator");
						AddProfilerProcess(RenderAnimator);
						// RenderAnimationPreview
						std::shared_ptr<FlatEngine::Process> RenderAnimationPreview = std::make_shared<FlatEngine::Process>("RenderAnimationPreview");
						AddProfilerProcess(RenderAnimationPreview);
						// RenderKeyFrameEditor
						std::shared_ptr<FlatEngine::Process> RenderKeyFrameEditor = std::make_shared<FlatEngine::Process>("RenderKeyFrameEditor");
						AddProfilerProcess(RenderKeyFrameEditor);
						// RenderLog
						std::shared_ptr<FlatEngine::Process> RenderLog = std::make_shared<FlatEngine::Process>("RenderLog");
						AddProfilerProcess(RenderLog);
						// RenderProfiler
						std::shared_ptr<FlatEngine::Process> RenderProfiler = std::make_shared<FlatEngine::Process>("RenderProfiler");
						AddProfilerProcess(RenderProfiler);
						// RenderMappingContextEditor
						std::shared_ptr<FlatEngine::Process> RenderMappingContextEditor = std::make_shared<FlatEngine::Process>("RenderMappingContextEditor");
						AddProfilerProcess(RenderMappingContextEditor);
						// RenderSettings
						std::shared_ptr<FlatEngine::Process> RenderSettings = std::make_shared<FlatEngine::Process>("RenderSettings");
						AddProfilerProcess(RenderSettings);
					}
				}
			}
		}
	}

	return success;
}


bool loadMedia()
{
	return TextureManager::LoadTextures();
};


void close()
{
	//Quit TextureManager
	TextureManager::Cleanup();

	//Quit ImGui
	FlatEngine::FlatGui::Cleanup();

	//Quit SDL subsystems
	Mix_Quit();
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();

	// Remove Profiler Processes
	FlatEngine::RemoveProfilerProcess("Render");
	FlatEngine::RemoveProfilerProcess("RenderMainMenuBar");
	FlatEngine::RemoveProfilerProcess("RenderToolbar");
	FlatEngine::RemoveProfilerProcess("RenderHierarchy");
	FlatEngine::RemoveProfilerProcess("RenderInspector");
	FlatEngine::RemoveProfilerProcess("Game_RenderView");

	FlatEngine::RemoveProfilerProcess("##Game_RenderView_Camera");
	FlatEngine::RemoveProfilerProcess("##Game_RenderView_RenderObject");

	FlatEngine::RemoveProfilerProcess("Scene_RenderView");
	FlatEngine::RemoveProfilerProcess("RenderAnimator");
	FlatEngine::RemoveProfilerProcess("RenderAnimationPreview");
	FlatEngine::RemoveProfilerProcess("RenderKeyFrameEditor");
	FlatEngine::RemoveProfilerProcess("RenderLog");
	FlatEngine::RemoveProfilerProcess("RenderProfiler");
	FlatEngine::RemoveProfilerProcess("RenderMappingContextEditor");
	FlatEngine::RemoveProfilerProcess("RenderSettings");
}


int main(int argc, char* args[])
{
	//Start up SDL and create window
	if (!init())
	{
		printf("Failed to initialize!\n");
	}
	else
	{
		FlatEngine::FlatGui::SetupImGui();
		// Create icons
		FlatEngine::FlatGui::CreateIcons();

		//Load media
		if (!loadMedia())
		{
			printf("Failed to load media!\n");
		}
		else
		{
			//While application is running
			while (!_hasQuit)
			{
				FlatEngine::Run(_hasQuit);
			}
		}
	}

	close();

	return 0;
}