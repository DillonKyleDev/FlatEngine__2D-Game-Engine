#include "FlatEngine.h"
#include "implot.h"
#include "Process.h"

namespace FlatEngine { 
	
	void AddProfilerProcess(std::shared_ptr<Process> process)
	{
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

	void RemoveProfilerProcess(std::string  toRemove)
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
		ImGui::Begin("Profiler");

		ImGuiChildFlags padding_child_flags = ImGuiChildFlags_::ImGuiChildFlags_AlwaysUseWindowPadding;

		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ChildBg, outerWindowColor);
		ImGui::BeginChild("Profiler Container", ImVec2(0, 0), padding_child_flags);
		ImGui::PopStyleColor();

		static ImGuiTableFlags flags = ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV |
			ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable;
		static bool anim = true;
		static int offset = 0;

		std::string ellapsedTime = "---";
		std::string averageFPS = "---";
		std::string deltaTime = "---";

		if (FlatEngine::GameLoopStarted())
		{
			ellapsedTime = std::to_string(FlatEngine::GetEllapsedGameTime() / 1000);
			averageFPS = std::to_string(FlatEngine::GetAverageFps());
			deltaTime = std::to_string(FlatEngine::GetDeltaTime());
		}

		if (ImGui::BeginTable("##RuntimeData", 2, flags, ImVec2(-1, 0))) 
		{

			ImGui::TableSetupColumn("RUNTIME PROCESS");
			ImGui::TableSetupColumn("DATA");
			ImGui::TableHeadersRow();

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Ellapsed Game Time (sec)");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text(ellapsedTime.c_str());
			ImGui::PushID("EllapsedTime");
			ImGui::PopID();

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Average FPS");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text(averageFPS.c_str());
			ImGui::PushID("AverageFPS");
			ImGui::PopID();

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("deltaTime (ms)");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text(deltaTime.c_str());
			ImGui::PushID("deltaTime");
			ImGui::PopID();

			ImGui::EndTable();
		}


		ImGui::Checkbox("Animate", &anim);
		if (anim)
			offset = (offset + 1) % 100;


		std::vector<std::shared_ptr<Process>>::iterator it = profilerProcesses.begin();
		int processCounter = 1;

		if (ImGui::BeginTable("##table", 3, flags, ImVec2(-1, 0))) 
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
						ImGui::Text("%.3f ms", rawDataVector.front());
						ImGui::TableSetColumnIndex(2);
						ImGui::PushID(processCounter);
						Sparkline("##spark", dataArray, 100, 0, 10.0f, offset, ImPlot::GetColormapColor(rawDataVector.front()), ImVec2(-1, 35));
						ImGui::PopID();
					}

					processCounter++;
				}

			ImPlot::PopColormap();
			ImGui::EndTable();
		}

		ImGui::EndChild(); // Profiler Container
		ImGui::End(); // Profiler
	}

	void Sparkline(const char* id, const float* values, int count, float min_v, float max_v, int offset, const ImVec4& col, const ImVec2& size) {
		ImPlot::PushStyleVar(ImPlotStyleVar_PlotPadding, ImVec2(0, 0));
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