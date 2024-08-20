#include "FlatEngine.h"
#include <SDL.h>
#include <vector>


namespace FL = FlatEngine;

namespace FlatGui
{
	// Push back events on 
	std::vector<SDL_Event> keyboardEvents;

	void RenderScriptEditor()
	{
		FL::BeginWindow("Script Editor");

			FL::BeginWindowChild("Script Editor");

				ImGui::Text("Script Editor");
				std::string scriptContent = "function Start()";
				FL::RenderInput("##ScriptContent", "", scriptContent, false);



			FL::EndWindowChild();

		FL::EndWindow();
	}
}