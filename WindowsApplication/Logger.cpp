#include "FlatEngine.h"
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


	namespace FlatGui {

		bool _clearBufferEveryFrame = true;
		bool _logProfilerOutput = false;

		void RenderLog()
		{
			ImGui::Begin("Debug Log");

			if (ImGui::Checkbox("Clear buffer after every frame", &_clearBufferEveryFrame))
				FlatEngine::logger->ClearBuffer();

			ImGuiChildFlags padding_child_flags = ImGuiChildFlags_::ImGuiChildFlags_AlwaysUseWindowPadding;

			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ChildBg, outerWindowColor);
			ImGui::BeginChild("Log Container", ImVec2(0, 0), padding_child_flags);
			ImGui::PopStyleColor();

			static ImGuiTextBuffer* log = FlatEngine::logger->GetBuffer();
			static int lines = 0;

			ImGui::Text("Log buffer contents : % d bytes", log->size());
			ImGui::SameLine(0, 10);
			if (ImGui::Button("Clear"))
			{
				log->clear(); lines = 0;
			}

			ImGui::PushStyleColor(ImGuiCol_ChildBg, innerWindowColor);
			ImGui::BeginChild("Log", ImVec2(0, 0), padding_child_flags);
			ImGui::TextUnformatted(log->begin(), log->end());
			ImGui::PopStyleColor();

			ImGui::EndChild(); // Log
			ImGui::EndChild(); // Log Container
			ImGui::End(); // Debug Log

			// For keeping the log from filling up when logging repeating values
			if (_clearBufferEveryFrame)
			{
				FlatEngine::logger->ClearBuffer();
				FlatEngine::LogString("Log buffer is being cleared...");
			}
		}
	}
}