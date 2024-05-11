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
		void DrawRectangle(Vector2 startingPoint, Vector2 endingPoint, Vector4 color, float thickness, ImDrawList* drawList);
		void DrawLine(Vector2 startingPoint, Vector2 endingPoint, Vector4 color, float thickness, ImDrawList* drawList);
		void DrawPoint(Vector2 point, Vector4 color, ImDrawList* drawlist);
		ImGuiTextBuffer *GetBuffer();
		void ClearBuffer();

	private:
		ImGuiTextBuffer *log;
	};
}

