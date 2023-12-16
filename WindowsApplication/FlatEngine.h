#pragma once
#include <SDL.h>
#include <SDL_syswm.h>
#include <imgui.h>
#include "Window.h"
#include "GameObjectManager.h"

//ImGui - SDL + DirextX12
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_dx12.h"
#include <d3d12.h>
#include <dxgi1_4.h>
#include <stdio.h>
#include <string>
#include <sstream>

#ifdef _DEBUG
#define DX12_ENABLE_DEBUG_LAYER
#endif

#ifdef DX12_ENABLE_DEBUG_LAYER
#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")
#endif

namespace FlatEngine
{

	namespace FlatGui
	{
		void SetupImGui();
		void HandleEvents(SDL_Event event, bool &quit);
		void Render(bool& quit);
		void RenderHierarchy();
		void Cleanup();
	}
};

