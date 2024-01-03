#include "Logger.h"



namespace FlatEngine
{
	Logger::Logger()
	{
		this->log = new ImGuiTextBuffer();
	}

	Logger::~Logger()
	{

	}

	void Logger::LogString(std::string line)
	{
		line = line + "\n";
		this->log->appendf(line.c_str());
	}

	void Logger::LogFloat(float variable, std::string line)
	{
		line = line + std::to_string(variable) + "\n";
		this->log->appendf(line.c_str());
	}

	void Logger::LogInt(int variable, std::string line)
	{
		line = line + std::to_string(variable) + "\n";
		this->log->appendf(line.c_str());
	}

	void Logger::LogVector2(Vector2 vector, std::string line)
	{
		line = line + " Vector.x: " + std::to_string(vector.x) + " Vector.y: " + std::to_string(vector.y) + "\n";
		this->log->appendf(line.c_str());
	}

	void Logger::DrawRectangle(Vector2 startingPoint, Vector2 endingPoint)
	{
		ImDrawList *drawList = ImGui::GetForegroundDrawList();
		drawList->AddRect(ImVec2(startingPoint.x, startingPoint.y), ImVec2(endingPoint.x, endingPoint.y), 0x2D02EF8D, 0.0f, 0, 3.0f);
	}

	void Logger::DrawRectangle(ImVec2 startingPoint, ImVec2 endingPoint)
	{
		ImDrawList* drawList = ImGui::GetForegroundDrawList();
		drawList->AddRect(startingPoint, endingPoint, 0x2D02EF8D, 0.0f, 0, 3.0f);
	}

	ImGuiTextBuffer *Logger::GetBuffer()
	{
		return this->log;
	}

	void Logger::ClearBuffer()
	{
		this->log->clear();
	}
}