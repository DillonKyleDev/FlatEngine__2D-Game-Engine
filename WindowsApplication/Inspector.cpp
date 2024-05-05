#include "FlatEngine.h"
#include "Scene.h"
#include "Sprite.h"
#include "Camera.h"
#include "Text.h"
#include "Audio.h"
#include "CharacterController.h"
#include "BoxCollider.h"
//#include "CircleCollider.h"
#include "RigidBody.h"

namespace FlatEngine { namespace FlatGui {

	void RenderInspector()
	{
		PushWindowStyles();
		ImGui::Begin("Inspector");
		PopWindowStyles();

		long focusedObjectID = FlatEngine::GetFocusedGameObjectID();

		ImGui::PushStyleColor(ImGuiCol_ChildBg, outerWindowColor);
		ImGuiChildFlags padding_child_flags = ImGuiChildFlags_::ImGuiChildFlags_AlwaysUseWindowPadding;
		ImGui::BeginChild("Inspector Background", ImVec2(0, 0), padding_child_flags);

		if (focusedObjectID != -1)
		{
			// Get focused GameObject
			std::shared_ptr<GameObject> focusedObject = FlatEngine::GetObjectById(focusedObjectID);

			// For checking if SceneObject has these components yet
			std::shared_ptr<Component> transformComponent = focusedObject->GetComponent(ComponentTypes::Transform);
			std::shared_ptr<Component> spriteComponent = focusedObject->GetComponent(ComponentTypes::Sprite);
			std::shared_ptr<Component> cameraComponent = focusedObject->GetComponent(ComponentTypes::Camera);
			std::shared_ptr<Component> canvasComponent = focusedObject->GetComponent(ComponentTypes::Canvas);
			std::shared_ptr<Component> animationComponent = focusedObject->GetComponent(ComponentTypes::Animation);
			std::shared_ptr<Component> characterController = focusedObject->GetComponent(ComponentTypes::CharacterController);
			std::shared_ptr<Component> rigidBody = focusedObject->GetComponent(ComponentTypes::RigidBody);
			std::shared_ptr<Component> boxCollider = focusedObject->GetComponent(ComponentTypes::BoxCollider);
			std::shared_ptr<Component> circleCollider = focusedObject->GetComponent(ComponentTypes::CircleCollider);

			// Lambda
			auto L_ShowAddComponentsWindow = [&]()
			{
				PushMenuStyles();

				// Add all the component types you can add to this GameObject
				//
				if (transformComponent == nullptr)
				{
					if (ImGui::MenuItem("Transform"))
					{
						focusedObject->AddComponent(ComponentTypes::Transform);
						ImGui::CloseCurrentPopup();
					}
				}

				if (spriteComponent == nullptr)
				{
					if (ImGui::MenuItem("Sprite"))
					{
						focusedObject->AddComponent(ComponentTypes::Sprite);
						ImGui::CloseCurrentPopup();
					}
				}

				if (ImGui::MenuItem("Button"))
				{
					focusedObject->AddComponent(ComponentTypes::Button);
					ImGui::CloseCurrentPopup();
				}

				if (cameraComponent == nullptr)
				{
					if (ImGui::MenuItem("Camera"))
					{
						focusedObject->AddComponent(ComponentTypes::Camera);
						ImGui::CloseCurrentPopup();
					}
				}

				if (canvasComponent == nullptr)
				{
					if (ImGui::MenuItem("Canvas"))
					{
						focusedObject->AddComponent(ComponentTypes::Canvas);
						ImGui::CloseCurrentPopup();
					}
				}

				if (animationComponent == nullptr)
				{
					if (ImGui::MenuItem("Animation"))
					{
						focusedObject->AddComponent(ComponentTypes::Animation);
						ImGui::CloseCurrentPopup();
					}
				}

				if (ImGui::MenuItem("Audio"))
				{
					focusedObject->AddComponent(ComponentTypes::Audio);
					ImGui::CloseCurrentPopup();
				}

				if (ImGui::MenuItem("Text"))
				{
					focusedObject->AddComponent(ComponentTypes::Text);
					ImGui::CloseCurrentPopup();
				}

				if (ImGui::MenuItem("Script"))
				{
					focusedObject->AddComponent(ComponentTypes::Script);
					ImGui::CloseCurrentPopup();
				}

				if (characterController == nullptr)
				{
					if (ImGui::MenuItem("CharacterController"))
					{
						focusedObject->AddComponent(ComponentTypes::CharacterController);
						ImGui::CloseCurrentPopup();
					}
				}

				if (rigidBody == nullptr)
				{
					if (ImGui::MenuItem("RigidBody"))
					{
						focusedObject->AddComponent(ComponentTypes::RigidBody);
						ImGui::CloseCurrentPopup();
					}
				}

				if (ImGui::MenuItem("BoxCollider"))
				{
					focusedObject->AddComponent(ComponentTypes::BoxCollider);
					ImGui::CloseCurrentPopup();
				}

				if (ImGui::MenuItem("CircleCollider"))
				{
					focusedObject->AddComponent(ComponentTypes::CircleCollider);
					ImGui::CloseCurrentPopup();
				}

				PopMenuStyles();
			};

			// Name editing
			std::string nameLabel = "Name: ";
			char newName[1024];
			strcpy_s(newName, focusedObject->GetName().c_str());
			ImGuiInputTextFlags flags = ImGuiInputTextFlags_::ImGuiInputTextFlags_AutoSelectAll;

			// Edit field
			ImGui::Text(nameLabel.c_str());
			ImGui::SameLine(0, 5);

			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
			ImGui::PushStyleColor(ImGuiCol_FrameBg, inputColor);
			if (ImGui::InputText("##GameObject Name", newName, IM_ARRAYSIZE(newName), flags))
				focusedObject->SetName(newName);
			ImGui::PopStyleColor();

			bool _objectActive = focusedObject->IsActive();

			if (RenderCheckbox("Active", _objectActive))
				focusedObject->SetActive(_objectActive);
			ImGui::SameLine(ImGui::GetContentRegionAvail().x - 20, 5);
			RenderImageButton("##InspectorMoreButton", threeDotsTexture, ImVec2(16, 16), 1, transparentColor);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 4);

			PushMenuStyles();
			if (ImGui::BeginPopupContextItem("##InspectorMoreContext", ImGuiPopupFlags_MouseButtonLeft)) // <-- use last item id as popup id
			{
				if (ImGui::BeginMenu("Add Component"))
				{
					L_ShowAddComponentsWindow();
					ImGui::EndMenu();
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Delete GameObject"))
				{
					SetFocusedGameObjectID(-1);
					DeleteGameObject(focusedObject->GetID());
					ImGui::CloseCurrentPopup();
				}
	
				ImGui::EndPopup();
			}
			PopMenuStyles();
			
			ImGui::Separator();
			ImGui::Separator();

			// Components section
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 3);
			ImVec2 screenCursor = ImGui::GetCursorScreenPos();
			ImGui::GetWindowDrawList()->AddRectFilled(screenCursor, ImVec2(screenCursor.x + ImGui::GetContentRegionAvail().x, screenCursor.y + 30), ImGui::GetColorU32(innerWindowColor));
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 8, ImGui::GetCursorPosY() + 8));
			ImGui::Text("Components");
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 6);
			ImGui::Separator();
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 4);



			// Lambda
			auto L_IsActiveCheckbox = [](bool& _isActive)
			{
				ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 3, ImGui::GetCursorPosY() + 1));
				bool _checked = RenderCheckbox("Active", _isActive);
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3);
				ImGui::Separator();
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 4);

				return _checked;
			};



			std::vector<std::shared_ptr<Component>> components = focusedObject->GetComponents();

			if (components.size() > 0)
			{
				

				long queuedForDelete = -1;
				// Open File Icon
				float openFileWidth = (float)openFileIcon->getWidth();
				float openFileHeight = (float)openFileIcon->getHeight();

				// Flags for child padding and dimensions
				ImGuiChildFlags child_flags = ImGuiChildFlags_::ImGuiChildFlags_AlwaysUseWindowPadding | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_::ImGuiChildFlags_AlwaysAutoResize;

				for (int i = 0; i < components.size(); i++)
				{
					if (components[i] != nullptr)
					{
						// Is Collapsed
						bool _isCollapsed = components[i]->IsCollapsed();

						// Component Name
						std::string componentType = components[i]->GetTypeString();
						// Get Component ID in to keep the child unique
						std::string componentID = componentType + std::to_string(components[i]->GetID());

						// Begin Component Child
						ImGui::PushStyleColor(ImGuiCol_ChildBg, innerWindowColor);
						ImGui::PushStyleColor(ImGuiCol_Border, componentBorderColor);
						ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1.0f);
						
						ImGui::BeginChild(componentID.c_str(), ImVec2(0, 0), child_flags);

						ImGui::PopStyleColor();
						ImGui::PopStyleColor();
						ImGui::PopStyleVar();

						auto wPos = ImGui::GetWindowPos();
						auto wSize = ImGui::GetWindowSize();
						ImGui::GetWindowDrawList()->AddRect({ wPos.x + 2, wPos.y + 2 }, {wPos.x + wSize.x - 2, wPos.y + wSize.y - 2}, ImColor(componentBorderColor.x, componentBorderColor.y, componentBorderColor.z, componentBorderColor.w));

						// Component Name
						ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 5, ImGui::GetCursorPosY() + 5));
						ImGui::Text(componentType.c_str());

						// Same Line
						ImGui::SameLine(ImGui::GetContentRegionAvail().x - (childPadding + 42), 5);

						// Pushes	
						ImGui::PushItemWidth(-1.0f);
						
						ImGui::PushStyleColor(ImGuiCol_Border, componentBorderColor);
						ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1.0f, 1.0f));
						ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 1, 1, 0));

						std::string expandID = "##expandIcon-" + i;
						std::string trashcanID = "##trashIcon-" + i;
						std::string openFileID = "##openFileIcon-" + i;

						ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() - 3));
						// Trash Can Icon for removing Component from Focused Object
						if (RenderImageButton(trashcanID.c_str(), trashTexture))
							queuedForDelete = components[i]->GetID();

						ImGui::SameLine(0, 5);

						ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() - 3));
						// Draw Expand Icon for expanding/collapsing current component information
						if (_isCollapsed)
						{
							if (RenderImageButton(expandID.c_str(), expandTexture))
								components[i]->SetCollapsed(!_isCollapsed);
						}
						else
							if (RenderImageButton(expandID.c_str(), expandFlippedTexture))
								components[i]->SetCollapsed(!_isCollapsed);


						if (!_isCollapsed)
						{
							ImGui::Separator();
							ImGui::Separator();
						}
						else {
							ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() - 15));
							ImGui::Text("");
						}


						// Pops
						ImGui::PopStyleColor();
						ImGui::PopStyleVar();
						ImGui::PopItemWidth();
						ImGui::PopStyleColor();
					


						// Render Component
						//
						if (!_isCollapsed)
						{
							// Component Data - Give it background color and padding
							// Push
							ImGui::PushStyleColor(ImGuiCol_ChildBg, singleItemColor);
							ImGui::BeginChild(componentType.c_str(), ImVec2(0, 0), child_flags);

							// Make full width Push
							ImGui::PushItemWidth(-1.0f);

							if (componentType == "Transform")
							{
								// Position, scale, and rotation of transform
								std::shared_ptr<FlatEngine::Transform> transform = std::static_pointer_cast<FlatEngine::Transform>(components[i]);
								Vector2 position = transform->GetPosition();
								float xPos = position.x;
								float yPos = position.y;
								Vector2 scale = transform->GetScale();
								float scaleX = scale.x;
								float scaleY = scale.y;
								float rotation = transform->GetRotation();
								bool _isActive = transform->IsActive();
								float columnWidth = ImGui::GetContentRegionAvail().x / 2 - 10;

								// Active Checkbox
								if (L_IsActiveCheckbox(_isActive))
									transform->SetActive(_isActive);

								PushTableStyles();
								if (ImGui::BeginTable("##TransformProperties", 2, tableFlags))
								{					
									ImGui::TableSetupColumn("##PROPERTY", 0, columnWidth);
									ImGui::TableSetupColumn("##VALUE", 0, columnWidth);
									ImGui::TableNextRow();								

									// Position X
									ImGui::TableSetColumnIndex(0);
									ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
									ImGui::Text("X Position");
									ImGui::TableSetColumnIndex(1);
									RenderDragFloat("##xPosDrag", 0, xPos, 0.5f, -FLT_MAX, -FLT_MAX);
									ImGui::PushID("xPosition");
									ImGui::PopID();

									// Position Y
									ImGui::TableNextRow();
									ImGui::TableSetColumnIndex(0);
									ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
									ImGui::Text("Y Position");
									ImGui::TableSetColumnIndex(1);
									RenderDragFloat("##yPosDrag", 0, yPos, 0.5f, -FLT_MAX, -FLT_MAX);
									ImGui::PushID("yPosition");
									ImGui::PopID();

									transform->SetPosition(Vector2(xPos, yPos));

									// Rotation
									ImGui::TableNextRow();
									ImGui::TableSetColumnIndex(0);
									ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
									ImGui::Text("Rotation");
									ImGui::TableSetColumnIndex(1);
									RenderDragFloat("##rotationDrag", 0, rotation, 1.0f, -360, 360);
									transform->SetRotation(rotation);
									ImGui::PushID("rotationDrag");
									ImGui::PopID();

									// Scale x
									ImGui::TableNextRow();
									ImGui::TableSetColumnIndex(0);
									ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
									ImGui::Text("X Scale");
									ImGui::TableSetColumnIndex(1);
									RenderDragFloat("##xScaleDrag", 0, scaleX, 0.5f, -FLT_MAX, -FLT_MAX);
									ImGui::PushID("xScale");
									ImGui::PopID();
									
									// Scale y
									ImGui::TableNextRow();
									ImGui::TableSetColumnIndex(0);
									ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
									ImGui::Text("Y Scale");
									ImGui::TableSetColumnIndex(1);
									RenderDragFloat("##yScaleDrag", 0, scaleY, 0.5f, -FLT_MAX, -FLT_MAX);
									ImGui::PushID("yScale");
									ImGui::PopID();

									transform->SetScale(Vector2(scaleX, scaleY));

									ImGui::EndTable();
									PopTableStyles();
								}
							}
							else if (componentType == "Sprite")
							{
								// Sprite path and texture dimension variables
								std::shared_ptr<FlatEngine::Sprite> sprite = std::static_pointer_cast<FlatEngine::Sprite>(components[i]);
								std::string path = sprite->GetPath();
								char newPath[1024];
								strcpy_s(newPath, path.c_str());
								float textureWidth = sprite->GetTextureWidth();
								float textureHeight = sprite->GetTextureHeight();
								int renderOrder = sprite->GetRenderOrder();
								bool _isActive = sprite->IsActive();

								// Active Checkbox
								if (L_IsActiveCheckbox(_isActive))
									sprite->SetActive(_isActive);

								// Sprite Path Strings
								std::string pathString = "Path: ";
								std::string textureWidthString = std::to_string(textureWidth);
								std::string textureHeightString = std::to_string(textureHeight);

								// Render Sprite Path
								ImGui::Text(pathString.c_str());
								ImGui::SameLine(0, 5);

								// Load From File
								if (RenderImageButton(openFileID.c_str(), openFileTexture))
								{
									std::string assetPath = OpenLoadFileExplorer();
									strcpy_s(newPath, assetPath.c_str());
									sprite->SetTexture(newPath);
								}
								ImGui::SameLine(0, 5);

								// Sprite Path Edit
								ImGui::PushStyleColor(ImGuiCol_FrameBg, inputColor);
								if (ImGui::InputText("##spritePath", newPath, IM_ARRAYSIZE(newPath), flags))
									sprite->SetTexture(newPath);
								ImGui::PopStyleColor();

								// Sprite offset variables
								Vector2 offset = sprite->GetOffset();
								float xOffset = offset.x;
								float yOffset = offset.y;
								float columnWidth = ImGui::GetContentRegionAvail().x / 2 - 10;

								PushTableStyles();
								if (ImGui::BeginTable("##SpriteProperties", 2, tableFlags))
								{
									ImGui::TableSetupColumn("##PROPERTY", 0, columnWidth);
									ImGui::TableSetupColumn("##VALUE", 0, columnWidth);
									
									// X Offset
									ImGui::TableNextRow();
									ImGui::TableSetColumnIndex(0);
									ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
									ImGui::Text("X Offset");
									ImGui::TableSetColumnIndex(1);
									RenderDragFloat("##xOffsetDrag", 0, xOffset, 0.1f, -FLT_MAX, -FLT_MAX);
									ImGui::PushID("xOffset");
									ImGui::PopID();

									// Y Offset
									ImGui::TableNextRow();
									ImGui::TableSetColumnIndex(0);
									ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
									ImGui::Text("Y Offset");
									ImGui::TableSetColumnIndex(1);
									RenderDragFloat("##yOffsetDrag", 0, yOffset, 0.1f, -FLT_MAX, -FLT_MAX);
									ImGui::PushID("yOffset");
									ImGui::PopID();

									// Assign the new slider values to the sprites pivotPoint
									sprite->SetOffset(Vector2(xOffset, yOffset));

									// Render Order
									ImGui::TableNextRow();
									ImGui::TableSetColumnIndex(0);
									ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
									ImGui::Text("Render order");
									ImGui::TableSetColumnIndex(1);
									RenderDragInt("##renderOrder", 0, renderOrder, 1, 0, maxSpriteLayers);
									sprite->SetRenderOrder(renderOrder);
									ImGui::PushID("renderOrder");
									ImGui::PopID();

									// Texture Width									
									ImGui::TableNextRow();
									ImGui::TableSetColumnIndex(0);
									// Set table cell bg color
									ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(uneditableTableRowDarkColor));
									ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
									// Push uneditableTableTextColor text color
									ImGui::PushStyleColor(ImGuiCol_Text, uneditableTableTextColor);
									ImGui::Text("Texture width");
									ImGui::TableSetColumnIndex(1);

									ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(uneditableTableRowFieldColor));
									ImGui::Text(textureWidthString.c_str());
									ImGui::PushID("textureWidth");
									ImGui::PopID();

									// Texture Height
									ImGui::TableNextRow();
									ImGui::TableSetColumnIndex(0);

									ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(uneditableTableRowLightColor));
									ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
									ImGui::Text("Texture height");
									ImGui::TableSetColumnIndex(1);
									ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(uneditableTableRowFieldColor));
									// Pop uneditableTableTextColor text color
									ImGui::Text(textureHeightString.c_str());
									ImGui::PopStyleColor();
									ImGui::PushID("textureHeight");
									ImGui::PopID();

									ImGui::EndTable();
								}
								PopTableStyles();
							}
							else if (componentType == "Camera")
							{
								// Camera position and aspect ratio
								std::shared_ptr<FlatEngine::Camera> camera = std::static_pointer_cast<FlatEngine::Camera>(components[i]);
								float width = camera->GetWidth();
								float height = camera->GetHeight();
								bool _isPrimary = camera->IsPrimary();
								float zoom = camera->GetZoom();
								ImVec4 frustrumColor = camera->GetFrustrumColor();
								bool _isActive = camera->IsActive();

								// Active Checkbox
								if (L_IsActiveCheckbox(_isActive))
									camera->SetActive(_isActive);

								// Push Item Width Setting
								ImGui::PushItemWidth(ImGui::GetContentRegionMax().x / 2 - 5);

								// Drags for camera width and height editing
								ImGui::Text("Camera width:");
								ImGui::SameLine(ImGui::GetContentRegionMax().x / 2 + 5, 0);
								ImGui::Text("Camera height:");
								RenderDragFloat("##CameraWidth", 0, width, 0.5f, 0, -FLT_MAX);
								ImGui::SameLine(0, 5);
								RenderDragFloat("##CameraHeight", 0, height, 0.5f, 0, -FLT_MAX);

								// Pop Width Setting
								ImGui::PopItemWidth();

								// Assign the new slider values
								camera->SetDimensions(width, height);

								// Zoom slider
								ImGui::Text("Camera zoom:");
								ImGui::SliderFloat("##Zoom", &zoom, 1.0f, 100.0f, "%.3f");
								camera->SetZoom(zoom);

								// Frustrum color picker
								ImVec4 color = ImVec4(frustrumColor.x / 255.0f, frustrumColor.y / 255.0f, frustrumColor.z / 255.0f, frustrumColor.w / 255.0f);

								ImGui::ColorEdit4("##FrustrumColor", (float*)&color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
								ImGui::SameLine(0, 5);
								ImGui::Text("Frustrum color");

								// Set frustrum color
								camera->SetFrustrumColor(ImVec4(color.x * 255.0f, color.y * 255.0f, color.z * 255.0f, color.w * 255.0f));

								// _isPrimaryCamera checkbox
								// Before allowing this camera to be set as primary, we need to ensure it has a transform component
								if (focusedObject->GetComponent(ComponentTypes::Transform) != nullptr)
								{									
									if (RenderCheckbox("Is Primary Camera", _isPrimary))
									{
										if (_isPrimary)
											FlatEngine::GetLoadedScene()->SetPrimaryCamera(camera);
										else
											FlatEngine::GetLoadedScene()->RemovePrimaryCamera();
									}									
								}
								else
								{
									bool temp = false;
									if (RenderCheckbox("Is Primary Camera", temp))
										FlatEngine::LogString("FlatGui::RenderInspector() - Attempt to set Camera component as primary failed: No Transform component found...");
									temp = false;
									ImGui::TextWrapped("*A Camera Component must be coupled with a Transform Component to be set as the primary camera.*");
								}

								camera->SetPrimaryCamera(_isPrimary);
							}
							else if (componentType == "Script")
							{
								std::shared_ptr<FlatEngine::ScriptComponent> script = std::static_pointer_cast<FlatEngine::ScriptComponent>(components[i]);
								std::string path = script->GetAttachedScript();
								bool _isActive = script->IsActive();

								// Active Checkbox
								if (L_IsActiveCheckbox(_isActive))
									script->SetActive(_isActive);

								// For path editing
								char newPath[1024];
								strcpy_s(newPath, path.c_str());
								std::string pathString = "Name: ";
								ImGui::Text(pathString.c_str());
								ImGui::SameLine(0, 5);
								ImGuiSliderFlags flags = ImGuiSliderFlags_::ImGuiSliderFlags_None;
								std::string inputId = "##scriptName_" + std::to_string(script->GetID());

								ImGui::PushStyleColor(ImGuiCol_FrameBg, inputColor);
								if (ImGui::InputText(inputId.c_str(), newPath, IM_ARRAYSIZE(newPath), flags))
									script->SetAttachedScript(newPath);
								ImGui::PopStyleColor();
							}
							else if (componentType == "Button")
							{
								std::shared_ptr<FlatEngine::Button> button = std::static_pointer_cast<FlatEngine::Button>(components[i]);
								bool _isActive = button->IsActive();
								float activeWidth = button->GetActiveWidth();
								float activeHeight = button->GetActiveHeight();
								Vector2 activeOffset = button->GetActiveOffset();
								int activeLayer = button->GetActiveLayer();
								float columnWidth = ImGui::GetContentRegionAvail().x / 2 - 10;

								// Active Checkbox
								if (L_IsActiveCheckbox(_isActive))
									button->SetActive(_isActive);

								PushTableStyles();
								if (ImGui::BeginTable("##TransformProperties", 2, tableFlags))
								{
									ImGui::TableSetupColumn("##PROPERTY", 0, columnWidth);
									ImGui::TableSetupColumn("##VALUE", 0, columnWidth);

									// Active Width
									
									ImGui::TableNextRow();
									ImGui::TableSetColumnIndex(0);
									ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
									ImGui::Text("Active width");
									ImGui::TableSetColumnIndex(1);
									RenderDragFloat("##activeWidth", 0, activeWidth, 0.5f, 0, 1000);
									ImGui::PushID("activeWidth");
									ImGui::PopID();

									// Active Height
									ImGui::TableNextRow();
									ImGui::TableSetColumnIndex(0);
									ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
									ImGui::Text("Active height");
									ImGui::TableSetColumnIndex(1);
									RenderDragFloat("##activeHeight", 0, activeHeight, 0.5f, 0, 1000);
									ImGui::PushID("activeHeight");
									ImGui::PopID();

									button->SetActiveDimensions(activeWidth, activeHeight);

									// X Offset
									ImGui::TableNextRow();
									ImGui::TableSetColumnIndex(0);
									ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
									ImGui::Text("X Offset");
									ImGui::TableSetColumnIndex(1);
									RenderDragFloat("##activeoffsetx", 0, activeOffset.x, 0.1f, -FLT_MAX, -FLT_MAX);
									ImGui::PushID("activeXOffset");
									ImGui::PopID();

									// Y Offset
									ImGui::TableNextRow();
									ImGui::TableSetColumnIndex(0);
									ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
									ImGui::Text("Y Offset");
									ImGui::TableSetColumnIndex(1);
									RenderDragFloat("##activeoffsety", 0, activeOffset.y, 0.1f, -FLT_MAX, -FLT_MAX);
									ImGui::PushID("activeYOffset");
									ImGui::PopID();

									button->SetActiveOffset(activeOffset);

									ImGui::EndTable();
								}
								PopTableStyles();
							}
							else if (componentType == "Canvas")
							{
								std::shared_ptr<FlatEngine::Canvas> canvas = std::static_pointer_cast<FlatEngine::Canvas>(components[i]);

								// Retrieve Canvas values
								float canvasWidth = canvas->GetWidth();
								float canvasHeight = canvas->GetHeight();
								int layerNumber = canvas->GetLayerNumber();
								bool _blocksLayers = canvas->GetBlocksLayers();
								std::vector<std::shared_ptr<Button>> canvasButtons = canvas->GetButtons();
								bool _isActive = canvas->IsActive();

								// Active Checkbox
								if (L_IsActiveCheckbox(_isActive))
									canvas->SetActive(_isActive);

								// _BlocksLayers Checkbox
								RenderCheckbox("Blocks Layers:", _blocksLayers);
								canvas->SetBlocksLayers(_blocksLayers);

								// Text
								ImGui::Text("Canvas Layer: ");

								// Set Layer Number Slider
								ImGui::SliderInt("##layerNumber", &layerNumber, 0, 20, "%d");
								// Set cursor type
								if (ImGui::IsItemHovered())
									ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_Hand);

								// Set Canvas Layer to the new value
								canvas->SetLayerNumber(layerNumber);

								// Text
								ImGui::Text("Canvas width: ");
								ImGui::SameLine(ImGui::GetContentRegionMax().x / 2, 5);
								ImGui::Text("Canvas height: ");

								RenderDragFloat("##canvasWidth", ImGui::GetContentRegionMax().x / 2 - 5, canvasWidth, 0.5f, 0.1f, -FLT_MAX);
								ImGui::SameLine(0, 5);
								RenderDragFloat("##canvasHeight", ImGui::GetContentRegionMax().x / 2 - 5, canvasHeight, 0.5f, 0.1f, -FLT_MAX);

								// Assign new values to Canvas
								canvas->SetDimensions(canvasWidth, canvasHeight);
							}
							else if (componentType == "Animation")
							{
								std::shared_ptr<FlatEngine::Animation> animation = std::static_pointer_cast<FlatEngine::Animation>(components[i]);

								std::string path = animation->GetAnimationPath();
								char newPath[1024];
								strcpy_s(newPath, path.c_str());
								bool _isActive = animation->IsActive();

								// Active Checkbox
								if (L_IsActiveCheckbox(_isActive))
									animation->SetActive(_isActive);

								// Sprite Animation Path String
								std::string pathString = "Path: ";
								std::string buttonID = openFileID + "animation";
								// Render Animation Path
								ImGui::Text(pathString.c_str());
								ImGui::SameLine(0, 5);

								if (RenderImageButton(buttonID.c_str(), openFileTexture))
								{
									std::string assetPath = OpenLoadFileExplorer();
									strcpy_s(newPath, assetPath.c_str());
									animation->SetAnimationPath(newPath);
								}

								ImGui::SameLine(0, 5);

								ImGui::PushStyleColor(ImGuiCol_FrameBg, inputColor);
								if (ImGui::InputText("##animationPath", newPath, IM_ARRAYSIZE(newPath), flags))
									animation->SetAnimationPath(newPath);
								ImGui::PopStyleColor();

								if (RenderButton("Play Animation"))
									animation->Play(GetEllapsedGameTime());

								if (animation->GetAnimationPath() != "")
								{
									ImGui::SameLine(0, 5);
									if (RenderButton("Edit Animation"))
									{
										_showAnimator = true;
										_showAnimationPreview = true;

										SetFocusedAnimation(LoadAnimationFile(animation->GetAnimationPath()));
										loadedProject->SetLoadedPreviewAnimationPath(animation->GetAnimationPath());
									}
								}
							}
							else if (componentType == "Audio")
							{
								std::shared_ptr<FlatEngine::Audio> audio = std::static_pointer_cast<FlatEngine::Audio>(components[i]);

								// Retrieve Audio values
								std::string path = audio->GetPath();
								bool _isMusic = audio->IsMusic();
								bool _isActive = audio->IsActive();

								// Active Checkbox
								if (L_IsActiveCheckbox(_isActive))
									audio->SetActive(_isActive);

								// For path editing
								char newPath[1024];
								strcpy_s(newPath, path.c_str());
								std::string pathString = "Path: ";
								ImGui::Text(pathString.c_str());
								ImGui::SameLine(0, 5);

								// On Path editing, reload the effect
								ImGuiSliderFlags flags = ImGuiSliderFlags_::ImGuiSliderFlags_None;
								std::string inputId = "##audioPath_" + std::to_string(audio->GetID());

								ImGui::PushStyleColor(ImGuiCol_FrameBg, inputColor);
								if (ImGui::InputText(inputId.c_str(), newPath, IM_ARRAYSIZE(newPath), flags))
								{
									if (_isMusic)
										audio->LoadMusic(newPath);
									else
										audio->LoadEffect(newPath);

									audio->SetPath(newPath);
								}
								ImGui::PopStyleColor();

								// _isMusic checkbox, also reload the effect as music or chunk
								std::string checkboxId = "Is Music##" + std::to_string(audio->GetID());
								if (RenderCheckbox(checkboxId, _isMusic))
								{
									if (_isMusic)
										audio->LoadMusic(newPath);
									else
										audio->LoadEffect(newPath);

									audio->SetIsMusic(_isMusic);
								}

								// Set cursor type
								if (ImGui::IsItemHovered())
									ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_Hand);

								// Play Audio
								if (RenderButton("Play"))
									audio->Play();
								ImGui::SameLine(0, 5);
								// Pause Audio
								if (RenderButton("Pause"))
									audio->Pause();
								ImGui::SameLine(0, 5);
								// Stop Audio
								if (RenderButton("Stop"))
									audio->Stop();
							}
							else if (componentType == "Text")
							{
								// Sprite path and texture dimension variables
								std::shared_ptr<FlatEngine::Text> text = std::static_pointer_cast<FlatEngine::Text>(components[i]);
								bool _isActive = text->IsActive();

								// Active Checkbox
								if (L_IsActiveCheckbox(_isActive))
									text->SetActive(_isActive);

								// Text String Variables
								std::string textText = text->GetText();
								char newTextString[1024];
								strcpy_s(newTextString, textText.c_str());
								// Text Strings
								std::string textString = "Text: ";
								// Render Text String
								ImGui::Text(textString.c_str());
								ImGui::SameLine(0, 5);

								ImGui::PushStyleColor(ImGuiCol_FrameBg, inputColor);
								if (ImGui::InputText("##textString", newTextString, IM_ARRAYSIZE(newTextString), flags))
								{
									text->SetText(newTextString);
									text->LoadText();
								}
								ImGui::PopStyleColor();


								// Other variables
								std::shared_ptr<Texture> texture = text->GetTexture();
								float textureWidth = (float)texture->getWidth();
								float textureHeight = (float)texture->getHeight();
								int renderOrder = text->GetRenderOrder();
								SDL_Color color = text->GetColor();

								// Font Path Variables
								std::string fontPath = text->GetFontPath();
								char newPath[1024];
								strcpy_s(newPath, fontPath.c_str());
								// Font Path Strings
								std::string pathString = "Font path: ";
								// Render Font Path
								ImGui::Text(pathString.c_str());
								ImGui::SameLine(0, 5);

								ImGui::PushStyleColor(ImGuiCol_FrameBg, inputColor);
								if (ImGui::InputText("##fontPath", newPath, IM_ARRAYSIZE(newPath), flags))
									text->SetFontPath(newPath);
								ImGui::PopStyleColor();

								// Text Width and Height
								std::string textureWidthString = "Text width: " + std::to_string(textureWidth);
								std::string textureHeightString = "Text height: " + std::to_string(textureHeight);
								ImGui::Text(textureWidthString.c_str());
								ImGui::Text(textureHeightString.c_str());

								// Push Item Width Setting
								ImGui::PushItemWidth(ImGui::GetContentRegionMax().x / 2 - 5);

								// Text offset variables
								Vector2 offset = text->GetOffset();
								float xOffset = offset.x;
								float yOffset = offset.y;
								ImGuiSliderFlags flags = ImGuiSliderFlags_::ImGuiSliderFlags_None;

								// Render Drags for offset of texture editing
								ImGui::Text("xOffset:");
								ImGui::SameLine(ImGui::GetContentRegionMax().x / 2 + 5, 0);
								ImGui::Text("yOffset:");
								RenderDragFloat("##xOffset", 0, xOffset, 0.5f, -FLT_MAX, -FLT_MAX);
								ImGui::SameLine(0, 5);
								RenderDragFloat("##yOffset", 0, yOffset, 0.5f, -FLT_MAX, -FLT_MAX);
								// Set new offset values
								text->SetOffset(Vector2(xOffset, yOffset));

								// Pop Width Setting
								ImGui::PopItemWidth();

								// Render Order
								ImGui::Text("Render order: ");
								if (ImGui::SliderInt("Text Render Order", &renderOrder, 0, maxSpriteLayers, "%d"))
									text->SetRenderOrder(renderOrder);

								// Push Item Width Setting
								ImGui::PushItemWidth(ImGui::GetContentRegionMax().x / 2 - 5);

								// Pop Width Setting
								ImGui::PopItemWidth();
							}
							else if (componentType == "CharacterController")
							{
								// Get Character Controller
								std::shared_ptr<CharacterController> characterController = std::static_pointer_cast<CharacterController>(components[i]);
								bool _isActive = characterController->IsActive();
								float walkSpeed = characterController->GetWalkSpeed();
								float runSpeed = characterController->GetRunSpeed();
								float gravity = characterController->GetGravity();
								bool _isMoving = characterController->IsMoving();
								float velocity = characterController->GetVelocity();								
								float columnWidth = ImGui::GetContentRegionAvail().x / 2 - 10;

								// Active Checkbox
								if (L_IsActiveCheckbox(_isActive))
									characterController->SetActive(_isActive);

								PushTableStyles();
								if (ImGui::BeginTable("##CharacterControllerProps", 2, tableFlags, ImVec2(-1, 0)))
								{

									ImGui::TableSetupColumn("##PROPERTY", 0, columnWidth);
									ImGui::TableSetupColumn("##DATA", 0, columnWidth);

									ImGui::TableNextRow();

									ImGui::TableSetColumnIndex(0);
									ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
									ImGui::Text("Walk Speed");
									ImGui::TableSetColumnIndex(1);
									if (RenderDragFloat("##walkSpeedSlider", ImGui::GetContentRegionAvail().x, walkSpeed, 0.01f, 0.0f, 20))
										characterController->SetWalkSpeed(walkSpeed);
									ImGui::PushID("WalkSpeed");
									ImGui::PopID();

									ImGui::TableNextRow();
									ImGui::TableSetColumnIndex(0);
									ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
									ImGui::Text("Run Speed");
									ImGui::TableSetColumnIndex(1);									
									if (RenderDragFloat("##runSpeedSlider", ImGui::GetContentRegionAvail().x, runSpeed, 0.01f, 0.0f, 20))
										characterController->SetRunSpeed(runSpeed);
									ImGui::PushID("RunSpeed");
									ImGui::PopID();

									ImGui::TableNextRow();
									ImGui::TableSetColumnIndex(0);
									ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
									ImGui::Text("Gravity");
									ImGui::TableSetColumnIndex(1);
									if (RenderDragFloat("##gravitySlider", ImGui::GetContentRegionAvail().x, gravity, 0.05f, 0.0f, 100))
										characterController->SetGravity(gravity);
									ImGui::PushID("Gravity");
									ImGui::PopID();

									ImGui::TableNextRow();
									ImGui::TableSetColumnIndex(0);
						
									ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(uneditableTableRowLightColor));
									ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
									// Push uneditableTableTextColor text color
									ImGui::PushStyleColor(ImGuiCol_Text, uneditableTableTextColor);
									ImGui::Text("Velocity");
									ImGui::TableSetColumnIndex(1);
									ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(uneditableTableRowFieldColor));									
									ImGui::Text(std::to_string(velocity).c_str());
									ImGui::PushID("Velocity");
									ImGui::PopID();

									ImGui::TableNextRow();
									ImGui::TableSetColumnIndex(0);
						
									ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(uneditableTableRowDarkColor));									
									ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
									ImGui::Text("Is Moving");
									ImGui::TableSetColumnIndex(1);
						
									ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(uneditableTableRowFieldColor));									
									std::string isMoving = "false";
									if (characterController->IsMoving())
										isMoving = "true";
									ImGui::Text(isMoving.c_str());
									ImGui::PushID("IsMoving");
									// Pop uneditableTableTextColor text color
									ImGui::PopStyleColor();
									ImGui::PopID();

									ImGui::EndTable();
								}
								PopTableStyles();
							}
							else if (componentType == "BoxCollider")
							{
								// Get Character Controller
								std::shared_ptr<BoxCollider> boxCollider = std::static_pointer_cast<BoxCollider>(components[i]);
								bool _isActive = boxCollider->IsActive();
								bool _isColliding = boxCollider->IsColliding();
								float activeWidth = boxCollider->GetActiveWidth();
								float activeHeight = boxCollider->GetActiveHeight();
								ImVec4 activeEdges = boxCollider->GetActiveEdges();
								Vector2 activeOffset = boxCollider->GetActiveOffset();
								bool _isContinious = boxCollider->IsContinuous();
								int activeLayer = boxCollider->GetActiveLayer();														
								float columnWidth = ImGui::GetContentRegionAvail().x / 2 - 10;

								// Active Checkbox
								if (L_IsActiveCheckbox(_isActive))
									boxCollider->SetActive(_isActive);

								PushTableStyles();
								if (ImGui::BeginTable("##BoxColliderProps", 2, tableFlags, ImVec2(-1, 0)))
								{

									ImGui::TableSetupColumn("##PROPERTY", 0, columnWidth);
									ImGui::TableSetupColumn("##DATA", 0, columnWidth);

									ImGui::TableNextRow();

									// Active Width
									ImGui::TableSetColumnIndex(0);
									ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
									ImGui::Text("Width");
									ImGui::TableSetColumnIndex(1);
									RenderDragFloat("##activeBoxColliderWidth", ImGui::GetContentRegionAvail().x, activeWidth, 0.01f, 0.0f, 20);
									ImGui::PushID("activeWidth");
									ImGui::PopID();

									// Active Height
									ImGui::TableNextRow();
									ImGui::TableSetColumnIndex(0);
									ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
									ImGui::Text("Height");
									ImGui::TableSetColumnIndex(1);
									RenderDragFloat("##activeHeight", ImGui::GetContentRegionAvail().x, activeHeight, 0.01f, 0.0f, 20);
									ImGui::PushID("activeBoxColliderHeight");
									ImGui::PopID();

									boxCollider->SetActiveDimensions(activeWidth, activeHeight);

									// Active Offset X
									ImGui::TableNextRow();
									ImGui::TableSetColumnIndex(0);
									ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
									ImGui::Text("X Offset");
									ImGui::TableSetColumnIndex(1);
									RenderDragFloat("##activeOffsetBoxColliderX", ImGui::GetContentRegionAvail().x, activeOffset.x, 0.05f, 0.0f, 100);
									ImGui::PushID("activeOffsetBoxColliderX");
									ImGui::PopID();

									// Active Offset Y
									ImGui::TableNextRow();
									ImGui::TableSetColumnIndex(0);
									ImGui::Text("Y Offset");
									ImGui::TableSetColumnIndex(1);
									ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
									RenderDragFloat("##activeOffsetBoxColliderY", ImGui::GetContentRegionAvail().x, activeOffset.x, 0.05f, 0.0f, 100);								
									ImGui::PushID("activeOffsetBoxColliderY");
									ImGui::PopID();

									// Active Layer
									ImGui::TableNextRow();
									ImGui::TableSetColumnIndex(0);
									ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
									ImGui::Text("Active Layer");
									ImGui::TableSetColumnIndex(1);
									RenderDragInt("##activeLayer", 0, activeLayer, 1, 0, 100);
									boxCollider->SetActiveLayer(activeLayer);
									ImGui::PushID("renderOrder");
									ImGui::PopID();

									ImGui::TableNextRow();
									ImGui::TableSetColumnIndex(0);
									// Push uneditableTableTextColor text color
									ImGui::PushStyleColor(ImGuiCol_Text, uneditableTableTextColor);
									ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(uneditableTableRowDarkColor));
									ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
									ImGui::Text("Is Colliding");
									ImGui::TableSetColumnIndex(1);

									ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(uneditableTableRowFieldColor));
									std::string isCollidingString = "false";
									if (_isColliding)
										isCollidingString = "true";
									ImGui::Text(isCollidingString.c_str());
									// Pop uneditableTableTextColor text color
									ImGui::PopStyleColor();
									ImGui::PushID("BoxIsColliding");
									ImGui::PopID();

									ImGui::EndTable();
								}
								PopTableStyles();

								ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
								// Continuous Checkbox
								RenderCheckbox(" Is Continuous", _isContinious);
								boxCollider->SetIsContinuous(_isContinious);

								ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
							}
							else if (componentType == "RigidBody")
							{
								// Get Character Controller
								std::shared_ptr<RigidBody> rigidBody = std::static_pointer_cast<RigidBody>(components[i]);
								bool _isActive = rigidBody->IsActive();
								float mass = rigidBody->GetMass();
								float gravity = rigidBody->GetGravity();
								float angularDrag = rigidBody->GetAngularDrag();
								Vector2 velocity = rigidBody->GetVelocity();
								bool _isContinious = rigidBody->IsContinuous();
								bool _isKinematic = rigidBody->IsKinematic();
								bool _isStatic = rigidBody->IsStatic();
								bool _isGrounded = rigidBody->IsGrounded();
								bool _isMoving = rigidBody->IsMoving();
								float columnWidth = ImGui::GetContentRegionAvail().x / 2 - 10;

								// Active Checkbox
								if (L_IsActiveCheckbox(_isActive))
									rigidBody->SetActive(_isActive);

								PushTableStyles();
								if (ImGui::BeginTable("##RigidBodyProps", 2, tableFlags, ImVec2(-1, 0)))
								{

									ImGui::TableSetupColumn("##PROPERTY", 0, columnWidth);
									ImGui::TableSetupColumn("##DATA", 0, columnWidth);

									ImGui::TableNextRow();
									ImGui::TableSetColumnIndex(0);
									ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
									ImGui::Text("Mass");
									ImGui::TableSetColumnIndex(1);
									if (RenderDragFloat("##massSlider", ImGui::GetContentRegionAvail().x, mass, 0.01f, 0.0f, 20))
										rigidBody->SetMass(mass);
									ImGui::PushID("Mass");
									ImGui::PopID();

									ImGui::TableNextRow();
									ImGui::TableSetColumnIndex(0);
									ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
									ImGui::Text("Gravity Scale");
									ImGui::TableSetColumnIndex(1);
									if (RenderDragFloat("##gravityScaleSlider", ImGui::GetContentRegionAvail().x, gravity, 0.01f, 0.0f, 20))
										rigidBody->SetGravity(gravity);
									ImGui::PushID("RunSpeed");
									ImGui::PopID();

									ImGui::TableNextRow();
									ImGui::TableSetColumnIndex(0);
									ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(uneditableTableRowLightColor));
									ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
									// Push uneditableTableTextColor text color
									ImGui::PushStyleColor(ImGuiCol_Text, uneditableTableTextColor);
									ImGui::Text("Velocity X");
									ImGui::TableSetColumnIndex(1);
									ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(uneditableTableRowFieldColor));
									ImGui::Text(std::to_string(velocity.x).c_str());
									ImGui::PushID("VelocityX");
									ImGui::PopID();

									ImGui::TableNextRow();
									ImGui::TableSetColumnIndex(0);
									ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(uneditableTableRowLightColor));
									ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
									ImGui::Text("Velocity Y");
									ImGui::TableSetColumnIndex(1);
									ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(uneditableTableRowFieldColor));
									ImGui::Text(std::to_string(velocity.y).c_str());
									ImGui::PushID("VelocityY");
									ImGui::PopID();

									ImGui::TableNextRow();
									ImGui::TableSetColumnIndex(0);
									ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(uneditableTableRowDarkColor));
									ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
									ImGui::Text("Is Moving");
									ImGui::TableSetColumnIndex(1);
									ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(uneditableTableRowFieldColor));
									std::string isMovingString = "false";
									if (_isMoving)
										isMovingString = "true";
									ImGui::Text(isMovingString.c_str());
									ImGui::PushID("RigidBodyMoving");
									ImGui::PopID();

									ImGui::TableNextRow();
									ImGui::TableSetColumnIndex(0);
									ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(uneditableTableRowLightColor));
									ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
									ImGui::Text("Is Grounded");
									ImGui::TableSetColumnIndex(1);
									ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(uneditableTableRowFieldColor));
									std::string isGroundedString = "false";
									if (_isGrounded)
										isGroundedString = "true";
									ImGui::Text(isGroundedString.c_str());
									// Pop uneditableTableTextColor text color
									ImGui::PopStyleColor();
									ImGui::PushID("RigidBodyGrounded");
									ImGui::PopID();

									ImGui::EndTable();								
								}
								ImGui::PopStyleColor();
								ImGui::PopStyleColor();

								ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
								// Continuous Checkbox
								RenderCheckbox(" Is Continuous", _isContinious);
								rigidBody->SetIsContinuous(_isContinious);

								ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
								// Kinematic Checkbox
								RenderCheckbox(" Is Kinematic", _isKinematic);
								rigidBody->SetIsKinematic(_isKinematic);

								ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
								// Static Checkbox
								RenderCheckbox(" Is Static", _isStatic);
								rigidBody->SetIsKinematic(_isKinematic);
							}
							
							// Pops
							ImGui::PopItemWidth();
							ImGui::PopStyleColor();
							
							// Add some space to the bottom of each component
							ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);

							ImGui::EndChild();
						}
					}

					ImGui::EndChild();
				}

				if (queuedForDelete != -1)
				{
					focusedObject->RemoveComponent(queuedForDelete);
					queuedForDelete = -1;
				}
			}

			// Render the Adding Components button
			RenderButton("Add Component", ImVec2(ImGui::GetContentRegionAvail().x, 0));
			if (ImGui::BeginPopupContextItem("##AddComponent", ImGuiPopupFlags_MouseButtonLeft)) // <-- use last item id as popup id
			{
				L_ShowAddComponentsWindow();
				ImGui::EndPopup();
			}

			ImGui::Text("");
		}
		ImGui::PopStyleColor();
		ImGui::EndChild();

		ImGui::End();
	}
}
}