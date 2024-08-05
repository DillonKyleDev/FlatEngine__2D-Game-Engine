#include "FlatEngine.h"
#include "FlatGui.h"
#include "imgui.h"
#include "MappingContext.h"
#include <vector>

using MappingContext = FlatEngine::MappingContext;

namespace FlatGui 
{
	void RenderMappingContextEditor()
	{
		FlatEngine::BeginWindow("Mapping Context Editor", _showMappingContextEditor);
		
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
			"XInput_LeftJoystickX",
			"XInput_LeftJoystickY",
			"XInput_RightJoystickX",
			"XInput_RightJoystickY",
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
		
		ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, Vector2(0, -5));

		// Select Mapping Context to edit //

		// Section Label
		if (FlatEngine::F_MappingContexts.size() > 0)
		{
			ImGui::PushStyleColor(ImGuiCol_ChildBg, FlatEngine::F_innerWindowColor);
			ImGui::BeginChild("Animation Preview", Vector2(0, 30), FlatEngine::F_headerFlags);
			ImGui::PopStyleColor();
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
			ImGui::Text("Select mapping context to edit:");
			ImGui::EndChild();
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
		}

		if (FlatEngine::F_MappingContexts.size() > 0)
		{
			MappingContext currentContext = FlatEngine::F_MappingContexts.at(current_context);

			FlatEngine::PushComboStyles();
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 184);
			if (ImGui::BeginCombo("##contexts", currentContext.GetName().c_str()))
			{
				for (int n = 0; n < FlatEngine::F_MappingContexts.size(); n++)
				{
					bool is_selected = (FlatEngine::F_MappingContexts.at(current_context).GetName() == FlatEngine::F_MappingContexts.at(n).GetName()); // You can store your selection however you want, outside or inside your objects
					ImGui::PushStyleColor(ImGuiCol_FrameBg, FlatEngine::F_outerWindowColor);
					if (ImGui::Selectable(FlatEngine::F_MappingContexts.at(n).GetName().c_str(), is_selected))
						current_context = n;
					if (is_selected)
						ImGui::SetItemDefaultFocus();
					ImGui::PopStyleColor();
				}
				ImGui::EndCombo();
			}
			FlatEngine::PopComboStyles();

			ImGui::SameLine();
			// Save and create new mapping contexts
			if (FlatEngine::F_MappingContexts.size() > 0)
			{
				if (FlatEngine::RenderButton("Save"))
				{
					MappingContext currentContext = FlatEngine::F_MappingContexts.at(current_context);
					SaveMappingContext(currentContext.GetPath(), currentContext);
				}
			}
			ImGui::SameLine(0, 5);
			if (FlatEngine::RenderButton("Create New Context"))
			{
				std::string path = FlatEngine::OpenSaveFileExplorer();
				std::string name = path.substr(path.find_last_of("/\\") + 1);
				MappingContext newContext = MappingContext();
				newContext.SetPath(path);
				newContext.SetName(name);
				SaveMappingContext(path, newContext);
				FlatEngine::InitializeMappingContexts();
			}

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);

			// Create New Input Action //
			//
			// Section Label
			ImGui::Separator();
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
			ImGui::PushStyleColor(ImGuiCol_ChildBg, FlatEngine::F_innerWindowColor);
			ImGui::BeginChild("Create New Input Action", Vector2(0, 30), FlatEngine::F_headerFlags);
			ImGui::PopStyleColor();
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
			ImGui::Text("Create new Input Action:");
			ImGui::EndChild();
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);

			ImGui::Columns(2);

			// Column 1
			static int current_input = 0;

			// Label

			ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 3);
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
			ImGui::Text("Input Source");

			// Select New Input
			FlatEngine::PushComboStyles();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 3);
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
			if (ImGui::BeginCombo("##inputs", inputs[current_input]))
			{
				for (int n = 0; n < IM_ARRAYSIZE(inputs); n++)
				{
					bool is_selected = (inputs[current_input] == inputs[n]);
					if (ImGui::Selectable(inputs[n], is_selected))
						current_input = n;
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
			FlatEngine::PopComboStyles();

			ImGui::NextColumn();

			// Column 2
			// Name editing
			static std::string placeHolder = "Input Action Name";
			std::string nameLabel = "Input Action Name";
			char inputActionName[1024];
			strcpy_s(inputActionName, placeHolder.c_str());
			ImGuiInputTextFlags flags = ImGuiInputTextFlags_::ImGuiInputTextFlags_AutoSelectAll;

			// Edit field
			ImGui::Text(nameLabel.c_str());

			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 32);
			ImGui::PushStyleColor(ImGuiCol_FrameBg, FlatEngine::F_inputColor);
			if (ImGui::InputText("##InputActionName", inputActionName, IM_ARRAYSIZE(inputActionName), flags))
			{
				placeHolder = inputActionName;
			}
			ImGui::PopStyleColor();

			ImGui::SameLine();
			if (FlatEngine::RenderButton("Add"))
			{
				currentContext.AddKeyBinding(inputs[current_input], inputActionName);
				//currentContext->AddInputAction(inputs[current_input], inputActionName);
			}

			// Go back to one column for Label
			ImGui::Columns(1);

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
			ImGui::Separator();
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
			ImGui::PushStyleColor(ImGuiCol_ChildBg, FlatEngine::F_innerWindowColor);
			ImGui::BeginChild("Existing Bindings", Vector2(0, 30), FlatEngine::F_headerFlags);
			ImGui::PopStyleColor();
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
			ImGui::Text("Existing Bindings:");
			ImGui::EndChild();
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);


			// Go back to two columns
			ImGui::Columns(2);

			// First Column
			int inputIDCounter = 0;

			// Input Binding
			for (std::pair<std::string, std::string> keyBinding : currentContext.GetKeyBindings())
			{
				if (keyBinding.second != "")
				{
					int selected_input = 0;
					for (int i = 0; i < IM_ARRAYSIZE(inputs); i++)
						if (keyBinding.first == inputs[i])
							selected_input = i;

					std::string comboId = "##selectedInput" + currentContext.GetName() + std::to_string(inputIDCounter);

					FlatEngine::PushComboStyles();
					ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 3);
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
								currentContext.RemoveKeyBinding(keyBinding.first);
								currentContext.AddKeyBinding(inputs[selected_input], tempInputAction);
								currentContext.AddInputAction(inputs[selected_input], tempInputAction);
							}
							if (is_selected)					
								ImGui::SetItemDefaultFocus();
						}
						ImGui::EndCombo();
					}
					FlatEngine::PopComboStyles();

					inputIDCounter++;
				}
			}


			// Column 2
			ImGui::NextColumn();

			int inputActionIDCounter = 0;
			// Input Action name
			for (std::pair<std::string, std::string> keyBinding : currentContext.GetKeyBindings())
			{
				if (keyBinding.second != "")
				{
					// Name editing
					std::string tempInputActionName = "";
					char inputActionName[1024];
					strcpy_s(inputActionName, keyBinding.second.c_str());
					//ImGuiInputTextFlags flags = ImGuiInputTextFlags_::ImGuiInputTextFlags_AutoSelectAll;

					std::string textLabelID = "##InputActionName" + keyBinding.second + std::to_string(inputActionIDCounter);

					ImGui::PushStyleColor(ImGuiCol_FrameBg, FlatEngine::F_inputColor);
					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 39);
					if (ImGui::InputText(textLabelID.c_str(), inputActionName, IM_ARRAYSIZE(inputActionName), 0))
					{
						currentContext.SetKeyBinding(keyBinding.first, inputActionName);
					}
					if (ImGui::IsItemEdited())
					{
						// BROKEN Figure it out later
						// LogString(inputActionName);
						// if (inputActionName != keyBinding.second.c_str())
						//	 tempInputActionName = inputActionName;
					}
					ImGui::SameLine();
					if (FlatEngine::RenderButton("Save"))
					{
						//currentContext->SetKeyBinding(keyBinding.first, tempInputActionName);
					}

					ImGui::PopStyleColor();
					inputActionIDCounter++;
				}
			}

			ImGui::Columns(1);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
		}

		ImGui::PopStyleVar();

		FlatEngine::EndWindow();
	}

}