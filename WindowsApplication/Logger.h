#pragma once
#include <string>
#include <imgui.h>



namespace FlatEngine
{
	class Logger
	{
	public:
		Logger();
		~Logger();

		void Log(std::string line);
		ImGuiTextBuffer *GetBuffer();
		void ClearBuffer();

	private:
		ImGuiTextBuffer *log;
	};
}

