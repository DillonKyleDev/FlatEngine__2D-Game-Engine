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
		void LogFloat(float variable, std::string line = "");
		void LogInt(int variable, std::string line = "");
		void LogVector2(Vector2 vector, std::string line = "");
		void DrawRectangle(Vector2 startingPoint, Vector2 endingPoint);
		void DrawRectangle(ImVec2 startingPoint, ImVec2 endingPoint);
		ImGuiTextBuffer *GetBuffer();
		void ClearBuffer();

	private:
		ImGuiTextBuffer *log;
	};
}

