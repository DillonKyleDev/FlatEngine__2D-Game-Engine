#include "FlatGui.h"
#include "FlatEngine.h"

#include <string>


namespace FL = FlatEngine;

namespace FlatGui
{
	void RenderScriptEditor()
	{
		FL::BeginWindow("Script Editor", FG_b_showScriptEditor);
		// {

			FL::BeginWindowChild("Script Editor Child");
			// {

				ImGui::Text("Script Editor");
				std::string scriptContent = "function Start()";
				FL::RenderInput("##ScriptContent", "", scriptContent, false);
			
			// }
			FL::EndWindowChild(); // Script Editor child

		// }
		FL::EndWindow(); // Script Editor
	}
}