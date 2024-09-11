#include "FlatGui.h"
#include "FlatEngine.h"
#include "Logger.h"
#include "Project.h"

namespace FL = FlatEngine;

namespace FlatGui 
{
	bool FG_b_clearBufferEveryFrame = false;

	void RenderLog()
	{
		FL::BeginWindow("Logger", FG_b_showLogger);

		// Border around object
		auto windowPos = ImGui::GetWindowPos();
		auto windowSize = ImGui::GetWindowSize();  // This is the size of the current box, perfect for getting the exact dimensions for a border
		ImGui::GetWindowDrawList()->AddRect({ windowPos.x + 2, windowPos.y + 2 }, { windowSize.x + windowPos.x - 2, windowPos.y + windowSize.y - 2 }, FL::GetColor32("componentBorder"), 0);


		if (FL::RenderCheckbox("Clear buffer after every frame?", FG_b_clearBufferEveryFrame))
		{
			FL::F_Logger.ClearBuffer();
			SaveProject(FL::F_LoadedProject, FL::F_LoadedProject.GetPath());
		}

		ImGuiTextBuffer log = FL::F_Logger.GetBuffer();
		static int lines = 0;

		if (FL::RenderButton("Clear"))
		{
			FL::F_Logger.ClearBuffer();
			lines = 0;
		}
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Text, FL::GetColor("logText"));
		ImGui::Text("Log buffer contents : % d bytes", log.size());
		ImGui::SameLine(0, 10);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 3);
		ImGui::PopStyleColor();

		ImGui::Separator();
		ImGui::Separator();
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);

		Vector2 cursorPos = Vector2(ImGui::GetCursorScreenPos().x - 1, ImGui::GetCursorScreenPos().y - 1);
		Vector2 availSpace = ImGui::GetContentRegionAvail();

		// Draw Border around log
		ImGui::GetWindowDrawList()->AddRectFilled(cursorPos, Vector2(cursorPos.x + availSpace.x + 2, cursorPos.y + availSpace.y + 2), FL::GetColor32("logOutline"));

		ImGui::PushStyleColor(ImGuiCol_ChildBg, FL::GetColor("logBg"));
		ImGui::BeginChild("Log", Vector2(0, 0), FL::F_childFlags);
		ImGui::PushStyleColor(ImGuiCol_Text, FL::GetColor("logText"));
		ImGui::TextUnformatted(log.begin(), log.end());
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::EndChild(); // Log

		FL::EndWindow();


		// For keeping the log from filling up when logging continuous values
		if (FG_b_clearBufferEveryFrame)
		{
			FL::F_Logger.ClearBuffer();
			FL::LogString("Log buffer is being cleared...");
		}
	}
}