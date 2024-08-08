#include "FlatEngine.h"
#include "FlatGui.h"
#include "Project.h"

#include "imgui.h"


namespace FlatGui {

	void RenderSettings()
	{
		FlatEngine::BeginWindow("Settings", _showSettings);


		// LEFT SIDE
		FlatEngine::BeginResizeWindowChild("EngineSettings");
		FlatEngine::RenderSectionHeader("Engine Setting");
		// List settings grouping
		static std::string settingSelected = "Collision Detection";
		FlatEngine::PushTreeList("EngineSettingSelect");
		FlatEngine::RenderTreeLeaf("Collision Detection", settingSelected);
		FlatEngine::RenderTreeLeaf("Physics", settingSelected);
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
			if (settingSelected == "Collision Detection")
			{
				std::vector<std::string> collisionTypes = { "Shared Axis", "Separating Axis" };
				static int currentCollisionIndex = 0;
				for (int c = 0; c < collisionTypes.size(); c++)
				{
					if (collisionTypes[c] == FlatEngine::F_LoadedProject.GetCollisionDetection())
						currentCollisionIndex = c;
				}
				FlatEngine::RenderSelectableTableRow("##CollisionDetectionSelect", "Collision Detection", collisionTypes, currentCollisionIndex);
				FlatEngine::F_LoadedProject.SetCollisionDetection(collisionTypes.at(currentCollisionIndex));
			}
			else if (settingSelected == "Physics")
			{
				std::vector<std::string> physicsTypes = { "Euler", "Verlet" };
				static int currentPhysicsIndex = 0;
				for (int p = 0; p < physicsTypes.size(); p++)
				{
					if (physicsTypes[p] == FlatEngine::F_LoadedProject.GetPhysicsSystem())
						currentPhysicsIndex = p;
				}
				FlatEngine::RenderSelectableTableRow("##PhysicsSystemSelect", "Physics System", physicsTypes, currentPhysicsIndex);
				FlatEngine::F_LoadedProject.SetPhysicsSystem(physicsTypes.at(currentPhysicsIndex));
			}
			else if (settingSelected == "State")
			{
				bool _autoSave = FlatEngine::F_LoadedProject.AutoSaveOn();
				if (FlatEngine::RenderCheckboxTableRow("##AutoSaveCheckbox", "Auto Save", _autoSave))
					FlatEngine::F_LoadedProject.SetAutoSave(_autoSave);
			}
			else if (settingSelected == "Game")
			{
				// Scene to load on start
				// TODO


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