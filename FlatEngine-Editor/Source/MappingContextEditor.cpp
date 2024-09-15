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
			
			float widthAvailable = ImGui::GetContentRegionAvail().x;
			static int currentContext = 0;
					
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

						static int currentInput = 0;
						static std::string inputText = "Input Action Name";

						ImGui::Text("Input Source");
						FL::PushComboStyles();										
						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x / 2);
						if (ImGui::BeginCombo("##CreateInputActionInputSelector", FL::F_KeyBindingsAvailable[currentInput].c_str()))
						{
							for (int n = 0; n < FL::F_KeyBindingsAvailable.size(); n++)
							{
								bool b_isSelected = (FL::F_KeyBindingsAvailable[currentInput] == FL::F_KeyBindingsAvailable[n]);
								if (ImGui::Selectable(FL::F_KeyBindingsAvailable[n].c_str(), b_isSelected))
								{
									currentInput = n;									
								}							
							}
							ImGui::EndCombo();
						}
						FL::PopComboStyles();


						ImGui::SameLine();
						FL::MoveScreenCursor(0, -25);
						Vector2 cursorPos = ImGui::GetCursorScreenPos();
						ImGui::Text("Action Name:");
						ImGui::SetCursorScreenPos(Vector2(cursorPos.x, cursorPos.y + 25));					
						FL::RenderInput("##InputActionName", "", inputText, false, ImGui::GetContentRegionAvail().x - 40, ImGuiInputTextFlags_AutoSelectAll);


						ImGui::SameLine();
						if (FL::RenderButton("Add"))
						{
							currentContext->AddKeyBinding(FL::F_KeyBindingsAvailable[currentInput].c_str(), inputText);
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
							for (std::pair<std::string, std::shared_ptr<FL::InputMapping>> inputAction : currentContext->GetInputActions())
							{
								int selectedInput = 0;
								for (int i = 0; i < FL::F_KeyBindingsAvailable.size(); i++)
								{
									if (inputAction.second->keyCode == FL::F_KeyBindingsAvailable[i])
									{
										selectedInput = i;
									}
								}

								std::string comboId = "##EditInputActionKeyCode" + currentContext->GetName() + std::to_string(inputActionIDCounter);
								FL::PushComboStyles();
								ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x / 2);
								if (ImGui::BeginCombo(comboId.c_str(), FL::F_KeyBindingsAvailable[selectedInput].c_str()))
								{
									for (int n = 0; n < FL::F_KeyBindingsAvailable.size(); n++)
									{
										bool b_isSelected = (FL::F_KeyBindingsAvailable[selectedInput] == FL::F_KeyBindingsAvailable[n]);

										if (ImGui::Selectable(FL::F_KeyBindingsAvailable[n].c_str(), b_isSelected))
										{											
											selectedInput = n;
											currentContext->AddKeyBinding(FL::F_KeyBindingsAvailable[selectedInput], inputAction.first);
										}										
									}
									ImGui::EndCombo();
								}
								FL::PopComboStyles();
						

								ImGui::SameLine();


								std::string inputActionName = inputAction.first;
								std::string textLabelID = "##EditInputActionName" + inputAction.second->keyCode + std::to_string(inputActionIDCounter);

								if (FL::RenderInput(textLabelID.c_str(), "", inputActionName))
								{
									currentContext->AddKeyBinding(inputAction.second->keyCode, inputActionName);
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