#include "FlatEngine.h"
#include "imgui.h"
#include "MappingContext.h"


namespace FlatEngine { namespace FlatGui {

	void RenderMappingContextEditor()
	{
		ImGui::Begin("Mapping Context Editor");
		ImGui::BeginChild("Mapping Context Editor");
		
		const char* inputs[] = { 
			// XInput
			"XInput_A", 
			"XInput_B", 
			"XInput_X", 
			"XInput_Y", 
			"XInput_LB", 
			"XInput_RB", 
			"XInput_ScreenShot", 
			"XInput_Start", 
			"XInput_LS", 
			"XInput_RS", 
			"XInput_Home", 
			"XInput_Tray",
			"XInput_DPadUp",
			"XInput_DPadDown",
			"XInput_DPadLeft",
			"XInput_DPadRight",
			"XInput_LeftJoystick",
			"XInput_RightJoystick",
			"XInput_LT",
			"XInput_RT",
			// Keyboard + Mouse
			"SDLK_UP",
			"SDLK_DOWN",
			"SDLK_LEFT",
			"SDLK_RIGHT",
			// Letters
			"SDLK_a",
			"SDLK_b",
			"SDLK_c",
			"SDLK_d",
			"SDLK_e",
			"SDLK_f",
			"SDLK_g",
			"SDLK_h",
			"SDLK_i",
			"SDLK_j",
			"SDLK_k",
			"SDLK_l",
			"SDLK_m",
			"SDLK_n",
			"SDLK_o",
			"SDLK_p",
			"SDLK_q",
			"SDLK_r",
			"SDLK_s",
			"SDLK_t",
			"SDLK_u",
			"SDLK_v",
			"SDLK_w",
			"SDLK_x",
			"SDLK_y",
			"SDLK_z",
		};

		float widthAvailable = ImGui::GetContentRegionAvail().x;
		static int current_context = 0;
		
		ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0, -5));

		// Select Mapping Context to edit //
		
		// Save and create new mapping contexts
		if (mappingContexts.size() > 0)
		{
			if (ImGui::Button("Save"))
			{
				std::shared_ptr<MappingContext> currentContext = mappingContexts.at(current_context);
				SaveMappingContext(currentContext->GetPath(), currentContext);
			}
		}
		ImGui::SameLine(0, 5);
		if (ImGui::Button("Create New Context"))
		{
			std::string path = OpenSaveFileExplorer();
			std::string name = path.substr(path.find_last_of("/\\") + 1);
			std::shared_ptr<MappingContext> newContext = std::make_shared<MappingContext>();
			newContext->SetPath(path);
			newContext->SetName(name);
			SaveMappingContext(path, newContext);
			InitializeMappingContexts();
		}
		ImGui::Separator();

		// Section Label
		if (mappingContexts.size() > 0)
		{
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
			ImGui::GetFont()->Scale *= 1.3;
			ImGui::PushFont(ImGui::GetFont());
			ImGui::Text("Select mapping context to edit:");
			ImGui::GetFont()->Scale /= 1.3;
			ImGui::PopFont();
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
			ImGui::Separator();
			ImGui::Separator();
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
		}

		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		if (mappingContexts.size() > 0)
		{
			std::shared_ptr<MappingContext> currentContext = mappingContexts.at(current_context);

			if (ImGui::BeginCombo("##contexts", currentContext->GetName().c_str()))
			{
				for (int n = 0; n < mappingContexts.size(); n++)
				{
					bool is_selected = (mappingContexts.at(current_context)->GetName() == mappingContexts.at(n)->GetName()); // You can store your selection however you want, outside or inside your objects
					if (ImGui::Selectable(mappingContexts.at(n)->GetName().c_str(), is_selected))
						current_context = n;
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

			// Create New Input Action //

			// Section Label
			ImGui::Separator();
		
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
			ImGui::GetFont()->Scale *= 1.3;
			ImGui::PushFont(ImGui::GetFont());
			ImGui::Text("Create new Input Action:");
			ImGui::GetFont()->Scale /= 1.3;
			ImGui::PopFont();
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
			ImGui::Separator();
			ImGui::Separator();

			ImGui::Columns(2);
			ImGui::SetColumnWidth(0, widthAvailable / 2);

			// Column 1
			static int current_input = 0;

			// Label
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
			ImGui::Text("Input Source");

			// Select New Input
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
			if (ImGui::BeginCombo("##inputs", inputs[current_input]))
			{
				for (int n = 0; n < IM_ARRAYSIZE(inputs); n++)
				{
					bool is_selected = (inputs[current_input] == inputs[n]); // You can store your selection however you want, outside or inside your objects
					if (ImGui::Selectable(inputs[n], is_selected))
						current_input = n;
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

			ImGui::NextColumn();
			ImGui::SetColumnWidth(1, widthAvailable / 2);

			// Column 2
			// Name editing
			static std::string placeHolder = "Input Action Name";
			std::string nameLabel = "Input Action Name";
			char inputActionName[1024];
			strcpy_s(inputActionName, placeHolder.c_str());
			ImGuiInputTextFlags flags = ImGuiInputTextFlags_::ImGuiInputTextFlags_AutoSelectAll;

			// Edit field
			ImGui::Text(nameLabel.c_str());

			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 35);
			if (ImGui::InputText("##InputActionName", inputActionName, IM_ARRAYSIZE(inputActionName), flags))
			{
				placeHolder = inputActionName;
			}

			ImGui::SameLine();
			if (ImGui::Button("Add"))
			{
				currentContext->AddKeyBinding(inputs[current_input], inputActionName);
				//currentContext->AddInputAction(inputs[current_input], inputActionName);
			}

			// Go back to one column for Label
			ImGui::Columns(1);

			// Currently created Input Actions on this context
			ImGui::Separator();
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
			ImGui::GetFont()->Scale *= 1.3;
			ImGui::PushFont(ImGui::GetFont());
			ImGui::Text("Existing Bindings");
			ImGui::GetFont()->Scale /= 1.3;
			ImGui::PopFont();
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
			ImGui::Separator();
			ImGui::Separator();


			// Go back to two columns
			ImGui::Columns(2);
			ImGui::SetColumnWidth(0, widthAvailable / 2);

			// Column 1

			// Label
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

			int inputIDCounter = 0;

			// Input Binding
			for (std::pair<std::string, std::string> keyBinding : currentContext->GetKeyBindings())
			{
				if (keyBinding.second != "")
				{
					int selected_input = 0;
					for (int i = 0; i < IM_ARRAYSIZE(inputs); i++)
						if (keyBinding.first == inputs[i])
							selected_input = i;

					std::string comboId = "##selectedInput" + currentContext->GetName() + std::to_string(inputIDCounter);
					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
					if (ImGui::BeginCombo(comboId.c_str(), inputs[selected_input]))
					{
						for (int n = 0; n < IM_ARRAYSIZE(inputs); n++)
						{
							bool is_selected = (inputs[selected_input] == inputs[n]);
							if (ImGui::Selectable(inputs[n], is_selected))
							{
								std::string tempInputAction = keyBinding.second;
								selected_input = n;
								currentContext->RemoveKeyBinding(keyBinding.first);
								currentContext->AddKeyBinding(inputs[selected_input], tempInputAction);
							}
							if (is_selected)
								ImGui::SetItemDefaultFocus();
						}
						ImGui::EndCombo();
					}
					inputIDCounter++;
				}
			}


			// Column 2
			ImGui::NextColumn();
			ImGui::SetColumnWidth(0, widthAvailable / 2);

			int inputActionIDCounter = 0;
			// Input Action name
			for (std::pair<std::string, std::string> keyBinding : currentContext->GetKeyBindings())
			{
				if (keyBinding.second != "")
				{
					// Name editing
					std::string tempInputActionName = "";
					char inputActionName[1024];
					strcpy_s(inputActionName, keyBinding.second.c_str());
					//ImGuiInputTextFlags flags = ImGuiInputTextFlags_::ImGuiInputTextFlags_AutoSelectAll;

					std::string textLabelID = "##InputActionName" + keyBinding.second + std::to_string(inputActionIDCounter);

					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 40);
					if (ImGui::InputText(textLabelID.c_str(), inputActionName, IM_ARRAYSIZE(inputActionName), 0))
					{
						currentContext->SetKeyBinding(keyBinding.first, inputActionName);
					}
					if (ImGui::IsItemEdited())
					{
						// BROKEN Figure it out later
						/*		LogString(inputActionName);
						if (inputActionName != keyBinding.second.c_str())
							tempInputActionName = inputActionName;*/
					}
					ImGui::SameLine();
					if (ImGui::Button("Save"))
					{
						//currentContext->SetKeyBinding(keyBinding.first, tempInputActionName);
					}


					inputActionIDCounter++;
				}
			}

			ImGui::Text("");
		}


		ImGui::PopStyleVar();

		ImGui::EndChild();
		ImGui::End();
	}

} 
}