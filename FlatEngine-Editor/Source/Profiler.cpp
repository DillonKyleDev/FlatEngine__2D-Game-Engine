#include "FlatGui.h"
#include "FlatEngine.h"
#include "GameLoop.h"
#include "ProfilerProcess.h"
#include "Collider.h"
#include "implot.h"
#include "Application.h"
#include "Scene.h"
#include <deque>

namespace FL = FlatEngine;

using Process = FL::Process;
using Collider = FL::Collider;
using Scene = FL::Scene;

namespace FlatGui 
{ 
	void RenderProfiler()
	{
		FL::BeginWindow("Profiler", FG_b_showProfiler);

		static ImGuiTableFlags flags = ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV |
			ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable;
		static bool anim = false;
		static int offset = 0;

		std::string ellapsedTime = "---";
		//std::string averageFPS = "---";
		//std::string deltaTime = "---";
		std::string framesCounted = "---";
		//std::string sdlTicks = "---";
		std::string numberOfColliderPairs = "---";

		if (FL::GameLoopStarted())
		{
			ellapsedTime = std::to_string(FL::GetEllapsedGameTimeInSec());
			//averageFPS = std::to_string(FL::GetAverageFps());
			//deltaTime = std::to_string(FL::GetDeltaTime());
			framesCounted = std::to_string(FL::F_Application->GetGameLoop()->GetFramesCounted());
			//sdlTicks = std::to_string(FL::GetEngineTime());
			numberOfColliderPairs = std::to_string(FL::GetLoadedScene()->GetColliderPairs().size());
		}

		// Render runtime data
		if (FL::PushTable("##RunTimeData", 2))
		{
			//FL::RenderTextTableRow("##RUNTIME PROCESS", "RUNTIME PROCESS", "DATA");
			FL::RenderTextTableRow("##EllapsedGameTimE", "Ellapsed Game Time (sec)", ellapsedTime.c_str());
			//FL::RenderTextTableRow("##AverageFPS", "Average FPS", averageFPS.c_str());
			//FL::RenderTextTableRow("##deltaTime", "deltaTime (ms)", deltaTime.c_str());
			FL::RenderTextTableRow("##framesCounted", "Frames Counted", framesCounted.c_str());
			//FL::RenderTextTableRow("##sdlTicks", "SDL Ticks", sdlTicks.c_str());
			FL::RenderTextTableRow("##colliderPairs", "Collider Pairs", numberOfColliderPairs.c_str());
			FL::PopTable();
		}

		static bool _showColliderPairs = true;
		FL::RenderCheckbox("Show Collider Pairs", _showColliderPairs);
		if (_showColliderPairs)
		{
			// Render runtime data
			if (FL::PushTable("##RunTimeData", 2))
			{
				FL::RenderTextTableRow("##ColliderPairs", "FIRST", "SECOND");

				for (std::pair<Collider*, Collider*> pair : FL::GetLoadedScene()->GetColliderPairs())
				{
					std::string col1String = pair.first->GetParent()->GetName();
					std::string col2String = pair.second->GetParent()->GetName();
					std::string TableRowID = "##ColliderPairID-" + std::to_string(pair.first->GetParent()->GetID()) + std::to_string(pair.second->GetParent()->GetID());					

					FL::RenderTextTableRow(TableRowID.c_str(), col1String.c_str(), col2String.c_str());
				}
				FL::PopTable();
			}
		}

		FL::RenderCheckbox("Animate", anim);
		if (anim)
			offset = (offset + 1) % 100;


		std::vector<Process>::iterator it = FL::F_ProfilerProcesses.begin();
		int processCounter = 1;

		if (ImGui::BeginTable("##table", 3, flags, Vector2(-1, 0))) 
		{
			ImGui::TableSetupColumn("Process Name", ImGuiTableColumnFlags_WidthFixed, 100.0f);
			ImGui::TableSetupColumn("Hang Time (ms)", ImGuiTableColumnFlags_WidthFixed, 100.0f);
			ImGui::TableSetupColumn("Hang Time Visualization");
			ImGui::TableHeadersRow();
			ImPlot::PushColormap(ImPlotColormap_Cool);

			if (FL::F_ProfilerProcesses.size() > 0)
				while (it != FL::F_ProfilerProcesses.end())
				{
					std::string processName = (*it).GetProcessName();
					std::vector<float> hangTimeVector = (*it).GetHangTimeData();
					std::deque<float> rawDataVector = (*it).GetRawData();
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

		FL::EndWindow();
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