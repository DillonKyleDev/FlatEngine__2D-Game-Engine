#include "FlatEngine.h"

namespace FlatEngine { namespace FlatGui {
	void RenderToolbar()
	{
		bool _controlPanelWindow = false;
		ImGui::Begin("Gameloop Control Panel", &_controlPanelWindow, ImGuiDockNodeFlags_::ImGuiDockNodeFlags_None);

		std::string playID = "##PlayGameloopIcon";
		std::string pauseID = "##PauseGameloopIcon";
		std::string stopID = "##StopGameloopIcon";
		ImVec4 gameloopControl_tint_col = ImVec4(1.0, 1.0, 1.0, 1.0f);
		ImVec4 gameloopControl_bg_col = ImVec4(.50f, .50f, .5f, 1.0f);

		ImGui::PushStyleVar(ImGuiStyleVar_DisabledAlpha, 0.4f);

		auto windowWidth = ImGui::GetWindowSize().x;

		ImGui::SetCursorPosX((windowWidth - 100) * 0.5f);

		if (gameLoop->IsStarted() && !gameLoop->IsPaused())
		{
			ImGui::BeginDisabled();
			ImGui::ImageButton(playID.c_str(), playTexture, ImVec2(16, 16), uv0, uv1, bg_col, tint_col);
			ImGui::EndDisabled();
		}
		else
		{
			if (ImGui::ImageButton(playID.c_str(), playTexture, ImVec2(16, 16), uv0, uv1, bg_col, tint_col))
				StartGameLoop();
			if (ImGui::IsItemHovered())
				ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_Hand);
		}

		ImGui::SameLine(0, 5);

		if (!gameLoop->IsStarted())
		{
			ImGui::BeginDisabled();
			ImGui::ImageButton(pauseID.c_str(), pauseTexture, ImVec2(16, 16), uv0, uv1, bg_col, tint_col);
			ImGui::EndDisabled();
		}
		else
		{
			if (ImGui::ImageButton(pauseID.c_str(), pauseTexture, ImVec2(16, 16), uv0, uv1, bg_col, tint_col))
				PauseGameLoop();
			if (ImGui::IsItemHovered())
				ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_Hand);
		}


		ImGui::SameLine(0, 5);

		if (!gameLoop->IsStarted())
		{
			ImGui::BeginDisabled();
			ImGui::ImageButton(stopID.c_str(), stopTexture, ImVec2(16, 16), uv0, uv1, bg_col, tint_col);
			ImGui::EndDisabled();
		}
		else
		{
			if (ImGui::ImageButton(stopID.c_str(), stopTexture, ImVec2(16, 16), uv0, uv1, bg_col, tint_col))
				StopGameLoop();
			if (ImGui::IsItemHovered())
				ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_Hand);
		}

		ImGui::PopStyleVar();

		ImGui::End();
	}
}
}