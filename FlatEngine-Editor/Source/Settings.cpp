#include "FlatEngine.h"
#include "FlatGui.h"
#include "Project.h"

#include "imgui.h"


namespace FlatGui 
{
	void RenderSettings()
	{
		ImGui::SetNextItemWidth(400);
		FlatEngine::BeginWindow("Settings", _showSettings);


		// LEFT SIDE
		FlatEngine::BeginResizeWindowChild("EngineSettings");
		FlatEngine::RenderSectionHeader("Engine Setting");
		// List settings grouping
		static std::string settingSelected = "State";
		FlatEngine::PushTreeList("EngineSettingSelect");
		FlatEngine::RenderTreeLeaf("State", settingSelected);
		FlatEngine::RenderTreeLeaf("Game", settingSelected);
		FlatEngine::PopTreeList();
		ImGui::EndChild();


		ImGui::SameLine();


		// RIGHT SIDE
		FlatEngine::BeginWindowChild("Edit Settings");
		if (FlatEngine::PushTable("SettingsTable", 2))
		{
			// List settings per grouping
			if (settingSelected == "State")
			{
				bool _autoSave = FlatEngine::F_LoadedProject.AutoSaveOn();
				if (FlatEngine::RenderCheckboxTableRow("##AutoSaveCheckbox", "Auto Save", _autoSave))
					FlatEngine::F_LoadedProject.SetAutoSave(_autoSave);
			}
			else if (settingSelected == "Game")
			{
				// Scene to load on start
				std::string startupScene = FL::F_LoadedProject.GetRuntimeScene();
				if (FL::RenderInputTableRow("Scene Loaded On Start", "Scene to load on game start", startupScene, true))
					FlatEngine::F_LoadedProject.SetRuntimeScene(startupScene);

				std::string finalBuildPath = FL::F_LoadedProject.GetBuildPath();
				if (FL::RenderInputTableRow("Final Build Path", "Final project build path", finalBuildPath))
					FlatEngine::F_LoadedProject.SetBuildPath(finalBuildPath);


				// Resolution
				Vector2 currentResolution = FlatEngine::F_LoadedProject.GetResolution();
				std::string currentResString = std::to_string((int)currentResolution.x) + " x " + std::to_string((int)currentResolution.y);
				std::vector<std::string> resolutions = { "800 x 600", "1920 x 1080", "1920 x 1200" };
				static int currentResolutionIndex = 0;
				for (int r = 0; r < resolutions.size(); r++)
				{
					if (resolutions[r] == currentResString)
						currentResolutionIndex = r;
				}
				FlatEngine::RenderSelectableTableRow("##ResolutionSelect", "Resolution", resolutions, currentResolutionIndex);

				if (resolutions.at(currentResolutionIndex) == "800 x 600")
					FlatEngine::F_LoadedProject.SetResolution(Vector2(800, 600));
				else if (resolutions.at(currentResolutionIndex) == "1920 x 1080")
					FlatEngine::F_LoadedProject.SetResolution(Vector2(1920, 1080));
				else if (resolutions.at(currentResolutionIndex) == "1920 x 1200")
					FlatEngine::F_LoadedProject.SetResolution(Vector2(1920, 1200));


				// Fullscreen
				bool _fullscreen = FlatEngine::F_LoadedProject.IsFullscreen();
				if (FlatEngine::RenderCheckboxTableRow("##FullscreenCheckbox", "Fullscreen", _fullscreen))
					FlatEngine::F_LoadedProject.SetFullscreen(_fullscreen);



				// Vsync
				bool _vsyncEnabled = FlatEngine::F_LoadedProject.IsVsyncEnabled();
				if (FlatEngine::RenderCheckboxTableRow("##VsyncCheckbox", "Vsync", _vsyncEnabled))
					FlatEngine::F_LoadedProject.SetVsyncEnabled(_vsyncEnabled);
			}

			FlatEngine::PopTable();
		}

		ImGui::EndChild();


		FlatEngine::EndWindow();
	}
}