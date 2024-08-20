#include "FlatEngine.h"
#include "FlatGui.h"
#include "GameLoop.h"

namespace FL = FlatEngine;

namespace FlatGui 
{
	void RenderToolbar()
	{
		bool _controlPanelWindow = false;
		FL::PushWindowStyles();
		ImGui::Begin("Gameloop Control Panel", &_controlPanelWindow, ImGuiDockNodeFlags_::ImGuiDockNodeFlags_NoUndocking);
		FL::PopWindowStyles();

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
		if (FL::GameLoopStarted())
		{
			ImGui::BeginDisabled();
			FL::RenderImageButton(playID.c_str(), FL::GetTexture("play"));
			ImGui::EndDisabled();
		}
		else if (FL::RenderImageButton(playID.c_str(), FL::GetTexture("play")))
			FL::StartGameLoop();

		ImGui::SameLine(0, 5);

		// Pause Icon
		if (!FL::GameLoopStarted())
		{
			ImGui::BeginDisabled();
			FL::RenderImageButton(pauseID.c_str(), FL::GetTexture("pause"));
			ImGui::EndDisabled();
		}
		else if (FL::RenderImageButton(pauseID.c_str(), FL::GetTexture("pause")))
			FL::PauseGameLoop();


		ImGui::SameLine(0, 5);

		// Stop Icon
		if (!FL::GameLoopStarted())
		{
			ImGui::BeginDisabled();
			FL::RenderImageButton(stopID.c_str(), FL::GetTexture("stop"));
			ImGui::EndDisabled();
		}
		else if (FL::RenderImageButton(stopID.c_str(), FL::GetTexture("stop")))
			FL::StopGameLoop();

		ImGui::SameLine(0, 5);

		// Next Frame Icon
		if (!FL::GameLoopPaused())
		{
			ImGui::BeginDisabled();
			FL::RenderImageButton(nextFrameID.c_str(), FL::GetTexture("nextFrame"));
			ImGui::EndDisabled();
		}
		else if (FL::RenderImageButton(nextFrameID.c_str(), FL::GetTexture("nextFrame")))
			FL::F_Application->GetGameLoop()->SetFrameSkipped(true);

		ImGui::PopStyleVar();

		ImGui::End();
	}
}