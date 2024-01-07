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

	void Logger::DrawRectangle(Vector2 startingPoint, Vector2 endingPoint, ImU32 color, float thickness, ImDrawList* drawList)
	{
		drawList->AddRect(ImVec2(startingPoint.x, startingPoint.y), ImVec2(endingPoint.x, endingPoint.y), color, 0.0f, 0, thickness);
	}

	void Logger::DrawRectangle(ImVec2 startingPoint, ImVec2 endingPoint, ImU32 color, float thickness, ImDrawList* drawList)
	{
		drawList->AddRect(ImVec2(startingPoint.x, startingPoint.y), ImVec2(endingPoint.x, endingPoint.y), color, 0.0f, 0, thickness);
	}

	void Logger::DrawLine(Vector2 startingPoint, Vector2 endingPoint, ImU32 color, float thickness, ImDrawList* drawList)
	{
		drawList->AddLine(ImVec2(startingPoint.x, startingPoint.y), ImVec2(endingPoint.x, endingPoint.y), color, thickness);
	}

	void Logger::DrawLine(ImVec2 startingPoint, ImVec2 endingPoint, ImU32 color, float thickness, ImDrawList* drawList)
	{
		drawList->AddLine(ImVec2(startingPoint.x, startingPoint.y), ImVec2(endingPoint.x, endingPoint.y), color, thickness);
	}

	void Logger::DrawPoint(Vector2 point, ImU32 color, ImDrawList* drawList)
	{
		drawList->AddLine(ImVec2(point.x, point.y), ImVec2(point.x + 1, point.y), color);
		drawList->AddLine(ImVec2(point.x, point.y), ImVec2(point.x, point.y - 1), color);
		drawList->AddLine(ImVec2(point.x + 1, point.y + 1), ImVec2(point.x + 1, point.y + 1), color);
	}

	void Logger::DrawPoint(ImVec2 point, ImU32 color, ImDrawList* drawList)
	{
		drawList->AddLine(ImVec2(point.x, point.y), ImVec2(point.x + 1, point.y), color);
		drawList->AddLine(ImVec2(point.x, point.y), ImVec2(point.x, point.y - 1), color);
		drawList->AddLine(ImVec2(point.x + 1, point.y + 1), ImVec2(point.x + 1, point.y + 1), color);
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