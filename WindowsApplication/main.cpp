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
		SDL_GL_SetSwapInterval(0); // vsync disabled -- 1 to activate
		FlatEngine::logger->LogString("SDL initialized... - Video - Audio - Joystick -");

		//Set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0"))
		{
			printf("Warning: Linear texture filtering not enabled!");
		}

		char t[] = "FlatEngine";
		char* title = &(t[0]);

		//Initialize Window::window
		if (Window::Init(title, 1600, 800))
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


						if (FlatEngine::FlatGui::_showProfiler)
						{
							// Add Profiler Processes
							// 						
							FlatEngine::AddProfilerProcess("Render");
							FlatEngine::AddProfilerProcess("Render Present");
							// RenderMainMenuBar()						
							//AddProfilerProcess("Render Main Menu Bar");
							// RenderToolbar()							
							//AddProfilerProcess("Render Toolbar");

							// RenderHierarchy()
							if (FlatEngine::FlatGui::_showHierarchy)							
								FlatEngine::AddProfilerProcess("Render Hierarchy");
							else
								FlatEngine::RemoveProfilerProcess("Render Hierarchy");

							// RenderInspector()
							if (FlatEngine::FlatGui::_showInspector)					
								FlatEngine::AddProfilerProcess("Render Inspector");
							else
								FlatEngine::RemoveProfilerProcess("Render Inspector");

							// Game_RenderView
							if (FlatEngine::FlatGui::_showGameView)			
								FlatEngine::AddProfilerProcess("Render Game View");
							else
								FlatEngine::RemoveProfilerProcess("Render Game View");

							// Scene_RenderView
							if (FlatEngine::FlatGui::_showSceneView)
								FlatEngine::AddProfilerProcess("Render Scene View");
							else
								FlatEngine::RemoveProfilerProcess("Render Scene View");

							// RenderAnimator
							if (FlatEngine::FlatGui::_showAnimator)					
								FlatEngine::AddProfilerProcess("Render Animator");
							else
								FlatEngine::RemoveProfilerProcess("Render Animator");

							// RenderAnimationPreview
							if (FlatEngine::FlatGui::_showAnimationPreview)
								FlatEngine::AddProfilerProcess("Render Animation Preview");
							else
								FlatEngine::RemoveProfilerProcess("Render Animation Preview");

							// RenderKeyFrameEditor
							if (FlatEngine::FlatGui::_showKeyFrameEditor)			
								FlatEngine::AddProfilerProcess("Render Key Frame Editor");
							else
								FlatEngine::RemoveProfilerProcess("Render Key Frame Editor");

							// RenderLog
							if (FlatEngine::FlatGui::_showLogger)						
								FlatEngine::AddProfilerProcess("Render Log");
							else
								FlatEngine::RemoveProfilerProcess("Render Log");

							// RenderProfiler
							if (FlatEngine::FlatGui::_showProfiler)							
								FlatEngine::AddProfilerProcess("Render Profiler");
							else
								FlatEngine::RemoveProfilerProcess("Render Profiler");

							// RenderMappingContextEditor
							if (FlatEngine::FlatGui::_showMappingContextEditor)						
								FlatEngine::AddProfilerProcess("Render Mapping Context Editor");
							else
								FlatEngine::RemoveProfilerProcess("Render Mapping Context Editor");

							// RenderSettings
							if (FlatEngine::FlatGui::_showSettings)					
								FlatEngine::AddProfilerProcess("Render Settings");
							else
								FlatEngine::RemoveProfilerProcess("Render Settings");
						}
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
	FlatEngine::RemoveProfilerProcess("Render Present");
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