#include "FlatEngine.h"
#include "implot.h"
#include "Process.h"

namespace FlatEngine { 
	
	void AddProfilerProcess(std::string name)
	{
		std::shared_ptr<FlatEngine::Process> process = std::make_shared<FlatEngine::Process>(name);
		profilerProcesses.push_back(process);
	}

	void AddProcessData(std::string processName, float data)
	{
		for (std::shared_ptr<Process> process : profilerProcesses)
		{
			if (process->GetProcessName() == processName)
				process->AddHangTimeData(data);
		}
	}

	void RemoveProfilerProcess(std::string toRemove)
	{
		std::vector<std::shared_ptr<Process>>::iterator iter = profilerProcesses.begin();

		while (iter != profilerProcesses.end())
		{
			if ((*iter)->GetProcessName() == toRemove)
			{
				profilerProcesses.erase(iter);
				return;
			}
			iter++;
		}
	}

	
	namespace FlatGui {

	void RenderProfiler()
	{
		BeginWindow("Profiler", _showProfiler);

		static ImGuiTableFlags flags = ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV |
			ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable;
		static bool anim = false;
		static int offset = 0;

		std::string ellapsedTime = "---";
		std::string averageFPS = "---";
		std::string deltaTime = "---";
		std::string framesCounted = "---";
		std::string sdlTicks = "---";

		if (FlatEngine::GameLoopStarted())
		{
			ellapsedTime = std::to_string(FlatEngine::GetEllapsedGameTime());
			averageFPS = std::to_string(FlatEngine::GetAverageFps());
			deltaTime = std::to_string(FlatEngine::GetDeltaTime());
			framesCounted = std::to_string(FlatEngine::gameLoop->GetFramesCounted());
			sdlTicks = std::to_string(FlatEngine::GetEngineTime());
		}

		// Render runtime data
		if (PushTable("##RunTimeData", 2))
		{
			RenderTextTableRow("##RUNTIME PROCESS", "RUNTIME PROCESS", "DATA");
			RenderTextTableRow("##EllapsedGameTimE", "Ellapsed Game Time (sec)", ellapsedTime.c_str());
			RenderTextTableRow("##AverageFPS", "Average FPS", averageFPS.c_str());
			RenderTextTableRow("##deltaTime", "deltaTime (ms)", deltaTime.c_str());
			RenderTextTableRow("##framesCounted", "Frames Counted", framesCounted.c_str());
			RenderTextTableRow("##sdlTicks", "SDL Ticks", sdlTicks.c_str());
			PopTable();
		}


		RenderCheckbox("Animate", anim);
		if (anim)
			offset = (offset + 1) % 100;


		std::vector<std::shared_ptr<Process>>::iterator it = profilerProcesses.begin();
		int processCounter = 1;

		if (ImGui::BeginTable("##table", 3, flags, Vector2(-1, 0))) 
		{
			ImGui::TableSetupColumn("Process Name", ImGuiTableColumnFlags_WidthFixed, 100.0f);
			ImGui::TableSetupColumn("Hang Time (ms)", ImGuiTableColumnFlags_WidthFixed, 100.0f);
			ImGui::TableSetupColumn("Hang Time Visualization");
			ImGui::TableHeadersRow();
			ImPlot::PushColormap(ImPlotColormap_Cool);

			if (profilerProcesses.size() > 0)
				while (it != profilerProcesses.end())
				{
					std::string processName = (*it)->GetProcessName();
					std::vector<float> hangTimeVector = (*it)->GetHangTimeData();
					std::deque<float> rawDataVector = (*it)->GetRawData();
					++it;

					float* dataArray;

					if (hangTimeVector.size() > 0)
					{
						dataArray = &hangTimeVector.front();
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						ImGui::Text(std::to_string(processCounter).c_str());
						ImGui::SameLine(0, 5);
						ImGui::Text(processName.c_str());
						ImGui::TableSetColumnIndex(1);
						ImGui::Text("%.0f ms", rawDataVector.front());
						ImGui::TableSetColumnIndex(2);
						ImGui::PushID(processCounter);
						Sparkline("##spark", dataArray, 100, 0, 10.0f, offset, ImPlot::GetColormapColor((int)rawDataVector.front()), Vector2(-1, 35));
						ImGui::PopID();
					}

					processCounter++;
				}

			ImPlot::PopColormap();
			ImGui::EndTable();
		}

		EndWindow();
	}

	void Sparkline(const char* id, const float* values, int count, float min_v, float max_v, int offset, const Vector4& col, const Vector2& size) {
		ImPlot::PushStyleVar(ImPlotStyleVar_PlotPadding, Vector2(0, 0));
		if (ImPlot::BeginPlot(id, size, ImPlotFlags_CanvasOnly)) {
			ImPlot::SetupAxes(nullptr, nullptr, ImPlotAxisFlags_NoDecorations, ImPlotAxisFlags_NoDecorations);
			ImPlot::SetupAxesLimits(0, count - 1, min_v, max_v, ImGuiCond_Always);
			ImPlot::SetNextLineStyle(col);
			ImPlot::SetNextFillStyle(col, 0.25);
			ImPlot::PlotLine(id, values, count, 1, 0, ImPlotLineFlags_Shaded, offset);
			ImPlot::EndPlot();
		}
		ImPlot::PopStyleVar();
	}
}
}