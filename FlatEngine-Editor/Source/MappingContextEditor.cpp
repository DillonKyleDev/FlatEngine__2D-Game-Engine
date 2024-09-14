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
			
			std::vector<std::string> inputStrings;

			for (std::pair<long, std::string> inputKeycode : FL::F_MappedKeyboardCodes)
			{
				inputStrings.push_back(inputKeycode.second);
			}
			for (std::pair<long, std::string> inputKeycode : FL::F_MappedXInputButtonCodes)
			{
				inputStrings.push_back(inputKeycode.second);
			}
			for (std::pair<long, std::string> inputKeycode : FL::F_MappedXInputDPadCodes)
			{
				inputStrings.push_back(inputKeycode.second);
			}
			for (std::pair<long, std::string> inputKeycode : FL::F_MappedXInputAnalogCodes)
			{
				inputStrings.push_back(inputKeycode.second);
			}



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
						if (ImGui::BeginCombo("##inputs", inputStrings[current_input].c_str()))
						{
							for (int n = 0; n < inputStrings.size(); n++)
							{
								bool b_isSelected = (inputStrings[current_input] == inputStrings[n]);
								if (ImGui::Selectable(inputStrings[n].c_str(), b_isSelected))
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
							currentContext->AddKeyBinding(inputStrings[current_input].c_str(), inputText);
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
									for (int i = 0; i < inputStrings.size(); i++)
									{
										if (keyBinding.first == inputStrings[i])
										{
											selected_input = i;
										}
									}

									std::string comboId = "##selectedInput" + currentContext->GetName() + std::to_string(inputActionIDCounter);

									FL::PushComboStyles();
									ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x / 2);
									if (ImGui::BeginCombo(comboId.c_str(), inputStrings[selected_input].c_str()))
									{
										for (int n = 0; n < inputStrings.size(); n++)
										{
											bool is_selected = (inputStrings[selected_input] == inputStrings[n]);

											if (ImGui::Selectable(inputStrings[n].c_str(), is_selected))
											{
												std::string tempInputAction = keyBinding.second;
												selected_input = n;
												currentContext->RemoveKeyBinding(keyBinding.first);
												currentContext->AddKeyBinding(inputStrings[selected_input], tempInputAction);
												currentContext->AddInputAction(inputStrings[selected_input], tempInputAction);
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