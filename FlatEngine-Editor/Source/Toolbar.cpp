#include "FlatEngine.h"
#include "FlatGui.h"
#include "GameLoop.h"


namespace FlatGui 
{
	void RenderToolbar()
	{
		bool _controlPanelWindow = false;
		FlatEngine::PushWindowStyles();
		ImGui::Begin("Gameloop Control Panel", &_controlPanelWindow, ImGuiDockNodeFlags_::ImGuiDockNodeFlags_NoUndocking);
		FlatEngine::PopWindowStyles();

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
		if (FlatEngine::GameLoopStarted())
		{
			ImGui::BeginDisabled();
			FlatEngine::RenderImageButton(playID.c_str(), FlatEngine::F_playIcon.GetTexture(), ImVec2(16, 16), 0, FlatEngine::F_imageButtonColor, FlatEngine::F_whiteColor);
			ImGui::EndDisabled();
		}
		else if (FlatEngine::RenderImageButton(playID.c_str(), FlatEngine::F_playIcon.GetTexture(), ImVec2(16, 16), 0, FlatEngine::F_imageButtonColor, FlatEngine::F_whiteColor))
			FlatEngine::StartGameLoop();

		ImGui::SameLine(0, 5);

		// Pause Icon
		if (!FlatEngine::GameLoopStarted())
		{
			ImGui::BeginDisabled();
			FlatEngine::RenderImageButton(pauseID.c_str(), FlatEngine::F_pauseIcon.GetTexture(), ImVec2(16, 16), 0, FlatEngine::F_imageButtonColor, FlatEngine::F_whiteColor);
			ImGui::EndDisabled();
		}
		else if (FlatEngine::RenderImageButton(pauseID.c_str(), FlatEngine::F_pauseIcon.GetTexture(), ImVec2(16, 16), 0, FlatEngine::F_imageButtonColor, FlatEngine::F_whiteColor))
			FlatEngine::PauseGameLoop();


		ImGui::SameLine(0, 5);

		// Stop Icon
		if (!FlatEngine::GameLoopStarted())
		{
			ImGui::BeginDisabled();
			FlatEngine::RenderImageButton(stopID.c_str(), FlatEngine::F_stopIcon.GetTexture(), ImVec2(16, 16), 0, FlatEngine::F_imageButtonColor, FlatEngine::F_whiteColor);
			ImGui::EndDisabled();
		}
		else if (FlatEngine::RenderImageButton(stopID.c_str(), FlatEngine::F_stopIcon.GetTexture(), ImVec2(16, 16), 0, FlatEngine::F_imageButtonColor, FlatEngine::F_whiteColor))
			FlatEngine::StopGameLoop();

		ImGui::SameLine(0, 5);

		// Next Frame Icon
		if (!FlatEngine::GameLoopPaused())
		{
			ImGui::BeginDisabled();
			FlatEngine::RenderImageButton(nextFrameID.c_str(), FlatEngine::F_nextFrameIcon.GetTexture(), ImVec2(16, 16), 0, FlatEngine::F_imageButtonColor, FlatEngine::F_whiteColor);
			ImGui::EndDisabled();
		}
		else if (FlatEngine::RenderImageButton(nextFrameID.c_str(), FlatEngine::F_nextFrameIcon.GetTexture(), ImVec2(16, 16), 0, FlatEngine::F_imageButtonColor, FlatEngine::F_whiteColor))
			FlatEngine::F_Application->GetGameLoop()->SetFrameSkipped(true);

		ImGui::PopStyleVar();

		ImGui::End();
	}
}