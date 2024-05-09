#pragma once
#include <string>
#include <imgui.h>
#include "Vector2.h"


namespace FlatEngine
{
	class Logger
	{
	public:
		Logger();
		~Logger();

		void LogString(std::string line);
		void LogSeparator();
		void LogFloat(float variable, std::string line = "");
		void LogInt(int variable, std::string line = "");
		void LogVector2(ImVec2 vector, std::string line = "");
		void DrawRectangle(ImVec2 startingPoint, ImVec2 endingPoint, ImVec4 color, float thickness, ImDrawList* drawList);
		void DrawLine(ImVec2 startingPoint, ImVec2 endingPoint, ImVec4 color, float thickness, ImDrawList* drawList);
		void DrawPoint(ImVec2 point, ImVec4 color, ImDrawList* drawlist);
		ImGuiTextBuffer *GetBuffer();
		void ClearBuffer();

	private:
		ImGuiTextBuffer *log;
	};
}

