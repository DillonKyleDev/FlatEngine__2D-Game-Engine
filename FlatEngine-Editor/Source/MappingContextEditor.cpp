#include "FlatEngine.h"
#include "FlatGui.h"
#include "MappingContext.h"

#include "imgui.h"


namespace FL = FlatEngine;

using MappingContext = FL::MappingContext;

namespace FlatGui 
{
	std::string fileName = "";
	bool b_openCreateContextModal = false;

	void RenderMappingContextEditor()
	{
		FL::BeginWindow("Mapping Context Editor", FG_b_showMappingContextEditor);
		// {

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
		
			if (FL::F_MappingContexts.size() > 0)
			{
				ImGui::PushStyleColor(ImGuiCol_ChildBg, FL::GetColor("innerWindow"));
				ImGui::BeginChild("Animation Preview", Vector2(0, 30), FL::F_headerFlags);
				ImGui::PopStyleColor();
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
				ImGui::Text("Select mapping context to edit:");
				ImGui::EndChild();
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
			}

			if (FL::F_MappingContexts.size() > 0)
			{
				MappingContext *currentContext = FL::GetMappingContext(FL::F_selectedMappingContextName);

				FL::PushComboStyles();
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 184);
				if (ImGui::BeginCombo("##contexts", FL::F_selectedMappingContextName.c_str()))
				{
					for (int i = 0; i < FL::F_MappingContexts.size(); i++)
					{

						bool b_isSelected = (FL::F_MappingContexts.at(i).GetName() == FL::F_selectedMappingContextName); // You can store your selection however you want, outside or inside your objects
						ImGui::PushStyleColor(ImGuiCol_FrameBg, FL::GetColor("outerWindow"));
						if (ImGui::Selectable(FL::F_MappingContexts.at(i).GetName().c_str(), b_isSelected))
						{
							FL::F_selectedMappingContextName = FL::F_MappingContexts.at(i).GetName();
						}
						if (b_isSelected)
						{
							ImGui::SetItemDefaultFocus();
						}
						ImGui::PopStyleColor();
					}
					ImGui::EndCombo();
				}
				FL::PopComboStyles();

				ImGui::SameLine();
			
				if (FL::F_MappingContexts.size() > 0)
				{
					if (FL::RenderButton("Save"))
					{
						SaveMappingContext(currentContext->GetPath(), *currentContext);
					}
				}
				ImGui::SameLine(0, 5);
				if (FL::RenderButton("Create New Context"))
				{
					b_openCreateContextModal = true;			
				}


				FL::RenderSeparator(5, 5);
				ImGui::PushStyleColor(ImGuiCol_ChildBg, FL::GetColor("innerWindow"));
				ImGui::BeginChild("Create New Input Action", Vector2(0, 30), FL::F_headerFlags);
				// {

					ImGui::PopStyleColor();
					FL::MoveScreenCursor(0, 5);
					FL::MoveScreenCursor(10, 0);
					ImGui::Text("Create new Input Action:");

				// }
				ImGui::EndChild();
				FL::MoveScreenCursor(0, 5);


				ImGui::Columns(2);


				// Column 1
				static int current_input = 0;
			
				FL::MoveScreenCursor(-3, 0);
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
				ImGui::Text("Input Source");

				FL::PushComboStyles();
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 3);
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
				if (ImGui::BeginCombo("##inputs", inputs[current_input]))
				{
					for (int n = 0; n < IM_ARRAYSIZE(inputs); n++)
					{
						bool b_isSelected = (inputs[current_input] == inputs[n]);
						if (ImGui::Selectable(inputs[n], b_isSelected))
						{
							current_input = n;
						}
						if (b_isSelected)
						{
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}
				FL::PopComboStyles();

				ImGui::NextColumn();

				// Column 2
				static std::string inputText = "Input Action Name";
				ImGui::Text("Input Action Name");
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, Vector2(8, 4));
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, Vector2(5, 5));
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 32);
				ImGui::PushStyleColor(ImGuiCol_FrameBg, FL::GetColor("input"));
				FL::RenderInput("##InputActionName", "", inputText, false, -1, ImGuiInputTextFlags_AutoSelectAll);
				ImGui::PopStyleColor();
				ImGui::PopStyleVar();
				ImGui::PopStyleVar();

				ImGui::SameLine();
				if (FL::RenderButton("Add"))
				{
					currentContext->AddKeyBinding(inputs[current_input], inputText);
				}

				// Go back to one column for Label
				ImGui::Columns(1);

				FL::MoveScreenCursor(0, 5);
				ImGui::Separator();
				FL::MoveScreenCursor(0, 5);
				ImGui::PushStyleColor(ImGuiCol_ChildBg, FL::GetColor("innerWindow"));
				ImGui::BeginChild("Existing Bindings", Vector2(0, 30), FL::F_headerFlags);
				// {

					ImGui::PopStyleColor();
					FL::MoveScreenCursor(0, 5);
					FL::MoveScreenCursor(10, 0);
					ImGui::Text("Existing Bindings:");

				// }
				ImGui::EndChild();

				FL::MoveScreenCursor(0, 5);


				// Go back to two columns
				ImGui::Columns(2);

				// First Column
				int inputIDCounter = 0;

				// Input Binding
				if (currentContext != nullptr)
				{
					for (std::pair<std::string, std::string> keyBinding : currentContext->GetKeyBindings())
					{
						if (keyBinding.second != "")
						{
							int selected_input = 0;
							for (int i = 0; i < IM_ARRAYSIZE(inputs); i++)
							{
								if (keyBinding.first == inputs[i])
								{
									selected_input = i;
								}
							}

							std::string comboId = "##selectedInput" + currentContext->GetName() + std::to_string(inputIDCounter);

							FL::PushComboStyles();
							FL::MoveScreenCursor(-3, 0);
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
										currentContext->AddInputAction(inputs[selected_input], tempInputAction);
									}
									if (is_selected)
									{
										ImGui::SetItemDefaultFocus();
									}
								}
								ImGui::EndCombo();
							}
							FL::PopComboStyles();

							inputIDCounter++;
						}
					}
				}

			
				ImGui::NextColumn();

				int inputActionIDCounter = 0;			
				if (currentContext != nullptr)
				{
					for (std::pair<std::string, std::string> keyBinding : currentContext->GetKeyBindings())
					{
						if (keyBinding.second != "")
						{						
							std::string inputActionName = keyBinding.second;
							std::string textLabelID = "##EditInputActionName" + keyBinding.first + std::to_string(inputActionIDCounter);

							ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, Vector2(8, 4));
							ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, Vector2(5, 5));
							ImGui::PushStyleColor(ImGuiCol_FrameBg, FL::GetColor("input"));
							ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 39);
							if (FL::RenderInput(textLabelID.c_str(), "", inputActionName))
							{
								currentContext->SetKeyBinding(keyBinding.first, inputActionName);
							}
							ImGui::PopStyleColor();
							ImGui::PopStyleVar();
							ImGui::PopStyleVar();
							inputActionIDCounter++;
						}
					}
				}

				ImGui::Columns(1);				
				FL::MoveScreenCursor(0, -5);
			}

			ImGui::PopStyleVar();

			// Create new Mapping Context modal
			if (FL::RenderInputModal("Create New Mapping Context", "Enter a name for the new Mapping Context", fileName, b_openCreateContextModal))
			{
				FL::CreateNewMappingContextFile(fileName);
				FL::F_selectedMappingContextName = fileName;
				FG_b_showMappingContextEditor = true;
			}

		// }
		FL::EndWindow();
	}
}