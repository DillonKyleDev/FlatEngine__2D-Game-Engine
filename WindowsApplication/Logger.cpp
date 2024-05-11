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

	void Logger::LogSeparator()
	{
		std::string line = "------------------------------------------------------------------------------------\n";
		log->appendf(line.c_str());
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

	void Logger::DrawRectangle(Vector2 startingPoint, Vector2 endingPoint, Vector4 color, float thickness, ImDrawList* drawList)
	{
		drawList->AddRect(Vector2(startingPoint.x, startingPoint.y), Vector2(endingPoint.x, endingPoint.y), ImGui::GetColorU32(color), 0.0f, 0, thickness);
	}

	void Logger::DrawLine(Vector2 startingPoint, Vector2 endingPoint, Vector4 color, float thickness, ImDrawList* drawList)
	{
		drawList->AddLine(Vector2(startingPoint.x, startingPoint.y), Vector2(endingPoint.x, endingPoint.y), ImGui::GetColorU32(color), thickness);
	}

	void Logger::DrawPoint(Vector2 point, Vector4 color, ImDrawList* drawList)
	{
		drawList->AddLine(Vector2(point.x, point.y), Vector2(point.x + 1, point.y), ImGui::GetColorU32(color));
		drawList->AddLine(Vector2(point.x, point.y), Vector2(point.x, point.y - 1), ImGui::GetColorU32(color));
		drawList->AddLine(Vector2(point.x + 1, point.y + 1), Vector2(point.x + 1, point.y + 1), ImGui::GetColorU32(color));
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
			BeginWindow("Logger", _showLogger);

			if (RenderCheckbox("Clear buffer after every frame?", _clearBufferEveryFrame))
				FlatEngine::logger->ClearBuffer();

			static ImGuiTextBuffer* log = FlatEngine::logger->GetBuffer();
			static int lines = 0;

			ImGui::PushStyleColor(ImGuiCol_Text, logTextColor);
			ImGui::Text("Log buffer contents : % d bytes", log->size());
			ImGui::SameLine(0, 10);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 3);
			ImGui::PopStyleColor();
			if (RenderButton("Clear Buffer"))
				log->clear(); lines = 0;

			ImGui::Separator();
			ImGui::Separator();
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);

			Vector2 cursorPos = Vector2(ImGui::GetCursorScreenPos().x - 1, ImGui::GetCursorScreenPos().y - 1);
			Vector2 availSpace = ImGui::GetContentRegionAvail();

			// Draw Border around log
			ImGui::GetWindowDrawList()->AddRectFilled(cursorPos, Vector2(cursorPos.x + availSpace.x + 2, cursorPos.y + availSpace.y + 2), ImGui::GetColorU32(logOutlineColor));

			ImGui::PushStyleColor(ImGuiCol_ChildBg, logBgColor);
			ImGui::BeginChild("Log", Vector2(0, 0), childFlags);
			ImGui::PushStyleColor(ImGuiCol_Text, logTextColor);
			ImGui::TextUnformatted(log->begin(), log->end());
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::EndChild(); // Log
			
			EndWindow();


			// For keeping the log from filling up when logging continuous values
			if (_clearBufferEveryFrame)
			{
				FlatEngine::logger->ClearBuffer();
				FlatEngine::LogString("Log buffer is being cleared...");
			}
		}
	}
}