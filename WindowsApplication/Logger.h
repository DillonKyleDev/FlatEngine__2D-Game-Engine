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
		void LogVector2(Vector2 vector, std::string line = "");
		void DrawRectangle(Vector2 startingPoint, Vector2 endingPoint, ImU32 color, float thickness, ImDrawList* drawList);
		void DrawRectangle(ImVec2 startingPoint, ImVec2 endingPoint, ImU32 color, float thickness, ImDrawList* drawList);
		void DrawLine(Vector2 startingPoint, Vector2 endingPoint, ImU32 color, float thickness, ImDrawList* drawList);
		void DrawLine(ImVec2 startingPoint, ImVec2 endingPoint, ImU32 color, float thickness, ImDrawList* drawList);
		void DrawPoint(ImVec2 point, ImU32 color, ImDrawList* drawlist);
		void DrawPoint(Vector2 point, ImU32 color, ImDrawList* drawlist);
		ImGuiTextBuffer *GetBuffer();
		void ClearBuffer();

	private:
		ImGuiTextBuffer *log;
	};
}

