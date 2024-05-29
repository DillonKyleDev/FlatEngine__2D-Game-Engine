#include "FlatEngine.h"

namespace FlatEngine { namespace FlatGui {
	void RenderToolbar()
	{
		bool _controlPanelWindow = false;
		PushWindowStyles();
		ImGui::Begin("Gameloop Control Panel", &_controlPanelWindow, ImGuiDockNodeFlags_::ImGuiDockNodeFlags_NoUndocking);
		PopWindowStyles();

		std::string playID = "##PlayGameloopIcon";
		std::string pauseID = "##PauseGameloopIcon";
		std::string stopID = "##StopGameloopIcon";
		std::string nextFrameID = "##AdvanceGameloopIcon";
		ImVec4 gameloopControl_tint_col = ImVec4(1.0, 1.0, 1.0, 1.0f);
		ImVec4 gameloopControl_bg_col = ImVec4(.50f, .50f, .5f, 1.0f);

		ImGui::PushStyleVar(ImGuiStyleVar_DisabledAlpha, 0.4f);

		auto windowWidth = ImGui::GetWindowSize().x;

		ImGui::SetCursorPosX((windowWidth - 100) * 0.5f);

		// Play Icon
		if (gameLoop->IsStarted())
		{
			ImGui::BeginDisabled();
			RenderImageButton(playID.c_str(), playTexture, ImVec2(16, 16), 0, imageButtonColor, whiteColor);
			ImGui::EndDisabled();
		}
		else if (RenderImageButton(playID.c_str(), playTexture, ImVec2(16, 16), 0, imageButtonColor, whiteColor))
			StartGameLoop();

		ImGui::SameLine(0, 5);

		// Pause Icon
		if (!gameLoop->IsStarted())
		{
			ImGui::BeginDisabled();
			RenderImageButton(pauseID.c_str(), pauseTexture, ImVec2(16, 16), 0, imageButtonColor, whiteColor);
			ImGui::EndDisabled();
		}
		else if (RenderImageButton(pauseID.c_str(), pauseTexture, ImVec2(16, 16), 0, imageButtonColor, whiteColor))
			PauseGameLoop();


		ImGui::SameLine(0, 5);

		// Stop Icon
		if (!gameLoop->IsStarted())
		{
			ImGui::BeginDisabled();
			RenderImageButton(stopID.c_str(), stopTexture, ImVec2(16, 16), 0, imageButtonColor, whiteColor);
			ImGui::EndDisabled();
		}
		else if (RenderImageButton(stopID.c_str(), stopTexture, ImVec2(16, 16), 0, imageButtonColor, whiteColor))
			StopGameLoop();

		ImGui::SameLine(0, 5);

		// Next Frame Icon
		if (!gameLoop->IsPaused())
		{
			ImGui::BeginDisabled();
			RenderImageButton(nextFrameID.c_str(), nextFrameTexture, ImVec2(16, 16), 0, imageButtonColor, whiteColor);
			ImGui::EndDisabled();
		}
		else if (RenderImageButton(nextFrameID.c_str(), nextFrameTexture, ImVec2(16, 16), 0, imageButtonColor, whiteColor))
			FlatEngine::gameLoop->SetFrameSkipped(true);

		ImGui::PopStyleVar();

		ImGui::End();
	}
}
}