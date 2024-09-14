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
				"Keyboard_up",
				"Keyboard_down",
				"Keyboard_left",
				"Keyboard_right",
				"Keyboard_space",
				"Keyboard_leftCtrl",
				"Keyboard_leftShift",
				"Keyboard_leftAlt",
				"Keyboard_rightCtrl",
				"Keyboard_rightShift",
				"Keyboard_rightAlt",
				"Keyboard_capsLock",
				"Keyboard_numLock",
				"Keyboard_backspace",
				"Keyboard_escape",
				"Keyboard_tab",
				"Keyboard_printScreen",
				"Keyboard_insert",
				"Keyboard_home",
				"Keyboard_pageUp",
				"Keyboard_pageDown",
				"Keyboard_delete",
				"Keyboard_\\",
				"Keyboard_/",
				"Keyboard_;",
				"Keyboard_'",
				"Keyboard_[",
				"Keyboard_]",
				"Keyboard_<",
				"Keyboard_>",
				"Keyboard_`",
				 "Keyboard_1",
				"Keyboard_2",
				"Keyboard_3",
				"Keyboard_4",
				"Keyboard_5",
				"Keyboard_6",
				"Keyboard_7",
				"Keyboard_8",
				"Keyboard_9",
				"Keyboard_0",
				"Keyboard_-",
				"Keyboard_+",
				"Keyboard_f1",
				"Keyboard_f2",
				"Keyboard_f3",
				"Keyboard_f4",
				"Keyboard_f5",
				"Keyboard_f6",
				"Keyboard_f7",
				"Keyboard_f8",
				"Keyboard_f9",
				"Keyboard_f10",
				"Keyboard_f11",
				"Keyboard_f12",
				"Keyboard_numPad_1",
				"Keyboard_numPad_2",
				"Keyboard_numPad_3",
				"Keyboard_numPad_4",
				"Keyboard_numPad_5",
				"Keyboard_numPad_6",
				"Keyboard_numPad_7",
				"Keyboard_numPad_8",
				"Keyboard_numPad_9",
				"Keyboard_numPad_0",
				"Keyboard_numPad_*",
				"Keyboard_numPad_/",
				"Keyboard_numPad_+",
				"Keyboard_numPad_-",
				"Keyboard_numPad_.",
				"Keyboard_numPad_enter",
				"Keyboard_a",
				"Keyboard_b",
				"Keyboard_c",
				"Keyboard_d",
				"Keyboard_e",
				"Keyboard_f",
				"Keyboard_g",
				"Keyboard_h",
				"Keyboard_i",
				"Keyboard_j",
				"Keyboard_k",
				"Keyboard_l",
				"Keyboard_m",
				"Keyboard_n",
				"Keyboard_o",
				"Keyboard_p",
				"Keyboard_q",
				"Keyboard_r",
				"Keyboard_s",
				"Keyboard_t",
				"Keyboard_u",
				"Keyboard_v",
				"Keyboard_w",
				"Keyboard_x",
				"Keyboard_y",
				"Keyboard_z"
			};

			float widthAvailable = ImGui::GetContentRegionAvail().x;
			static int current_context = 0;
					
		
			if (FL::F_MappingContexts.size() > 0)
			{
				MappingContext* currentContext = FL::GetMappingContext(FL::F_selectedMappingContextName);


				ImGui::PushStyleColor(ImGuiCol_ChildBg, FL::GetColor("innerWindow"));
				ImGui::BeginChild("Context Selection", Vector2(0), FL::F_headerFlags);
				ImGui::PopStyleColor();
				// {
						
					FL::MoveScreenCursor(10, 5);
					ImGui::Text("Select mapping context to edit:");
					FL::MoveScreenCursor(0, 5);

					if (FL::F_MappingContexts.size() > 0)
					{
						FL::PushComboStyles();
						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 144);
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
						if (FL::RenderButton("New Context"))
						{
							b_openCreateContextModal = true;
						}
					}

				// }
				ImGui::EndChild(); // Context Selection
				
				
				FL::RenderSeparator(10, 10);


				ImGui::BeginDisabled(currentContext == nullptr);
				// {

					ImGui::PushStyleColor(ImGuiCol_ChildBg, FL::GetColor("innerWindow"));
					ImGui::BeginChild("Create New Input Action", Vector2(0), FL::F_headerFlags);
					ImGui::PopStyleColor();
					// {

						FL::MoveScreenCursor(10, 5);					
						ImGui::Text("Create new Input Action:");
						FL::MoveScreenCursor(0, 5);

						static int current_input = 0;
								
						ImGui::Text("Input Source");
						FL::PushComboStyles();										
						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x / 2);
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


						ImGui::SameLine();
						FL::MoveScreenCursor(0, -25);
						Vector2 cursorPos = ImGui::GetCursorScreenPos();
						ImGui::Text("Action Name:");
						static std::string inputText = "Input Action Name";
						ImGui::SetCursorScreenPos(Vector2(cursorPos.x, cursorPos.y + 25));					
						FL::RenderInput("##InputActionName", "", inputText, false, ImGui::GetContentRegionAvail().x - 40, ImGuiInputTextFlags_AutoSelectAll);


						ImGui::SameLine();
						if (FL::RenderButton("Add"))
						{
							currentContext->AddKeyBinding(inputs[current_input], inputText);
						}

					// }
					ImGui::EndChild(); // Create New Input Action


					FL::RenderSeparator(10, 10);


					ImGui::PushStyleColor(ImGuiCol_ChildBg, FL::GetColor("innerWindow"));
					ImGui::BeginChild("Existing Bindings", Vector2(0), FL::F_headerFlags);
					// {

						ImGui::PopStyleColor();
						FL::MoveScreenCursor(10, 5);					
						ImGui::Text("Existing Bindings:");
						FL::MoveScreenCursor(0, 5);

						int inputActionIDCounter = 0;
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

									std::string comboId = "##selectedInput" + currentContext->GetName() + std::to_string(inputActionIDCounter);

									FL::PushComboStyles();
									ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x / 2);
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
						

									ImGui::SameLine();


									std::string inputActionName = keyBinding.second;
									std::string textLabelID = "##EditInputActionName" + keyBinding.first + std::to_string(inputActionIDCounter);

									if (FL::RenderInput(textLabelID.c_str(), "", inputActionName))
									{
										currentContext->SetKeyBinding(keyBinding.first, inputActionName);
									}								
								}

								inputActionIDCounter++;
							}
						}

					// }
					ImGui::EndChild(); // Existing Bindings

				// }
				ImGui::EndDisabled();
			}
			

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