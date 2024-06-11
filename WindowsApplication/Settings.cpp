#include "FlatEngine.h"
#include "imgui.h"
#include "Project.h"


namespace FlatEngine { namespace FlatGui {

	void RenderSettings()
	{
		BeginWindow("Settings", _showSettings);


		// LEFT SIDE
		BeginResizeWindowChild("EngineSettings");
		RenderSectionHeader("Engine Setting");
		// List settings grouping
		static std::string settingSelected = "Collision Detection";
		PushTreeList("EngineSettingSelect");
		RenderTreeLeaf("Collision Detection", settingSelected);
		RenderTreeLeaf("Physics", settingSelected);
		RenderTreeLeaf("State", settingSelected);
		RenderTreeLeaf("Game", settingSelected);
		PopTreeList();
		ImGui::EndChild();


		ImGui::SameLine();


		// RIGHT SIDE
		BeginWindowChild("Edit Settings");
		if (PushTable("SettingsTable", 2))
		{
			// List settings per grouping
			if (settingSelected == "Collision Detection")
			{
				std::vector<std::string> collisionTypes = { "Shared Axis", "Separating Axis" };
				static int currentCollisionIndex = 0;
				for (int c = 0; c < collisionTypes.size(); c++)
				{
					if (collisionTypes[c] == loadedProject->GetCollisionDetection())
						currentCollisionIndex = c;
				}
				RenderSelectableTableRow("##CollisionDetectionSelect", "Collision Detection", collisionTypes, currentCollisionIndex);
				loadedProject->SetCollisionDetection(collisionTypes.at(currentCollisionIndex));
			}
			else if (settingSelected == "Physics")
			{
				std::vector<std::string> physicsTypes = { "Euler", "Verlet" };
				static int currentPhysicsIndex = 0;
				for (int p = 0; p < physicsTypes.size(); p++)
				{
					if (physicsTypes[p] == loadedProject->GetPhysicsSystem())
						currentPhysicsIndex = p;
				}
				RenderSelectableTableRow("##PhysicsSystemSelect", "Physics System", physicsTypes, currentPhysicsIndex);
				loadedProject->SetPhysicsSystem(physicsTypes.at(currentPhysicsIndex));
			}
			else if (settingSelected == "State")
			{
				bool _autoSave = loadedProject->AutoSaveOn();
				if (RenderCheckboxTableRow("##AutoSaveCheckbox", "Auto Save", _autoSave))
					loadedProject->SetAutoSave(_autoSave);
			}
			else if (settingSelected == "Game")
			{
				// Scene to load on start
				// TODO


				// Resolution
				Vector2 currentResolution = loadedProject->GetResolution();
				std::string currentResString = std::to_string((int)currentResolution.x) + " x " + std::to_string((int)currentResolution.y);
				std::vector<std::string> resolutions = { "800 x 600", "1920 x 1080", "1920 x 1200" };
				static int currentResolutionIndex = 0;
				for (int r = 0; r < resolutions.size(); r++)
				{
					if (resolutions[r] == currentResString)
						currentResolutionIndex = r;
				}
				RenderSelectableTableRow("##ResolutionSelect", "Resolution", resolutions, currentResolutionIndex);

				if (resolutions.at(currentResolutionIndex) == "800 x 600")
					loadedProject->SetResolution(Vector2(800, 600));
				else if (resolutions.at(currentResolutionIndex) == "1920 x 1080")
					loadedProject->SetResolution(Vector2(1920, 1080));
				else if (resolutions.at(currentResolutionIndex) == "1920 x 1200")
					loadedProject->SetResolution(Vector2(1920, 1200));


				// Fullscreen
				bool _fullscreen = loadedProject->IsFullscreen();
				if (RenderCheckboxTableRow("##FullscreenCheckbox", "Fullscreen", _fullscreen))
					loadedProject->SetFullscreen(_fullscreen);



				// Vsync
				bool _vsyncEnabled = loadedProject->IsVsyncEnabled();
				if (RenderCheckboxTableRow("##VsyncCheckbox", "Vsync", _vsyncEnabled))
					loadedProject->SetVsyncEnabled(_vsyncEnabled);
			}

			PopTable();
		}
	
		ImGui::EndChild();


		EndWindow();
	}
}}