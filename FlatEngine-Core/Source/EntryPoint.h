#pragma once
#include "Application.h"
#include "FlatEngine.h"
#include "TextureManager.h"

namespace FlatEngine
{
	extern std::shared_ptr<Application> CreateApplication(int argc, char** argv);
	bool g_ApplicationRunning = true;
	bool g_HasStarted = false;

	int Main(int argc, char** argv)
	{
		// This loop allows us to restart our application instead of just closing it
		while (g_ApplicationRunning && !g_HasStarted)
		{
			g_HasStarted = true;
			// Initialize FlatEngine. Start up SDL and create window
			if (!FlatEngine::Init())
			{
				printf("Failed to initialize.\n");
			}
			else
			{
				//Load media
				if (!F_TextureManager.LoadTextures())
				{
					printf("Failed to load media.\n");
				}
				else
				{
					FlatEngine::F_Application = CreateApplication(argc, argv);
					F_Application->OnInit();
					F_Application->Run();
				}
			}
			CloseProgram();
		}

		return 1;
	}
}