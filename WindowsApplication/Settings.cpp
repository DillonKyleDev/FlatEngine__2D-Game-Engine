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
		PushTable("SettingsTable", 2);

		// List settings per grouping
		if (settingSelected == "Collision Detection")
		{
			std::vector<std::string> collisionTypes = { "Simple Box", "Separating Axis (Rotational)" };
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
			RenderCheckboxTableRow("##AutoSaveCheckbox", "Auto Save", _autoSave);
			loadedProject->SetAutoSave(_autoSave);
		}
		else if (settingSelected == "Game")
		{
			// Scene to load on start

		}

		PopTable();
		ImGui::EndChild();


		EndWindow();
	}
}}