#include "FlatEngine.h"
#include "TagList.h"
#include "Scene.h"
#include "Sprite.h"
#include "Camera.h"
#include "Text.h"
#include "Audio.h"
#include "CharacterController.h"
#include "BoxCollider.h"
#include "CircleCollider.h"
#include "RigidBody.h"

namespace FlatEngine { namespace FlatGui {

	void RenderInspector()
	{
		BeginWindow("Inspector");

		long focusedObjectID = FlatEngine::GetFocusedGameObjectID();

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

			ImGui::SetCursorScreenPos(Vector2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y + 2));

			// GameObject Active Checkbox
			if (RenderCheckbox("Active", _objectActive))
				focusedObject->SetActive(_objectActive);
			ImGui::SameLine(ImGui::GetContentRegionAvail().x - 62, 5);

			// GameObject TagList Dropdown
			std::shared_ptr<TagList> tagList = focusedObject->GetTagList();			
			RenderButton("Tags");
			PushMenuStyles();
			if (ImGui::BeginPopupContextItem("TagsPopup", ImGuiPopupFlags_MouseButtonLeft)) // <-- use last item id as popup id
			{
				if (ImGui::MenuItem("Player", NULL, tagList->HasTag("Player")))
					tagList->ToggleTag("Player");
				if (ImGui::MenuItem("Enemy", NULL,  tagList->HasTag("Enemy")))
					tagList->ToggleTag("Enemy");
				if (ImGui::MenuItem("Npc", NULL,  tagList->HasTag("Npc")))
					tagList->ToggleTag("Npc");
				if (ImGui::MenuItem("OnlyForPlayer", NULL,  tagList->HasTag("OnlyForPlayer")))
					tagList->ToggleTag("OnlyForPlayer");
				if (ImGui::MenuItem("OnlyForEnemy", NULL,  tagList->HasTag("OnlyForEnemy")))
					tagList->ToggleTag("OnlyForEnemy");
				if (ImGui::MenuItem("OnlyForNpc", NULL,  tagList->HasTag("OnlyForNpc")))
					tagList->ToggleTag("OnlyForNpc");
				if (ImGui::MenuItem("IgnorePlayer", NULL,  tagList->HasTag("IgnorePlayer")))
					tagList->ToggleTag("IgnorePlayer");
				if (ImGui::MenuItem("IgnoreEnemy", NULL,  tagList->HasTag("IgnoreEnemy")))
					tagList->ToggleTag("IgnoreEnemy");
				if (ImGui::MenuItem("IgnoreNpc", NULL,  tagList->HasTag("IgnoreNpc")))
					tagList->ToggleTag("IgnoreNpc");
				if (ImGui::MenuItem("Projectile", NULL,  tagList->HasTag("Projectile")))
					tagList->ToggleTag("Projectile");
				if (ImGui::MenuItem("Terrain", NULL,  tagList->HasTag("Terrain")))
					tagList->ToggleTag("Terrain");
				if (ImGui::MenuItem("InteractableItem", NULL,  tagList->HasTag("InteractableItem")))
					tagList->ToggleTag("InteractableItem");
				if (ImGui::MenuItem("InteractableObject", NULL,  tagList->HasTag("InteractableObject")))
					tagList->ToggleTag("InteractableObject");
				if (ImGui::MenuItem("Item", NULL,  tagList->HasTag("Item")))
					tagList->ToggleTag("Item");
				ImGui::EndPopup();
			}
			PopMenuStyles();


			// Three Dots More Options Button
			ImGui::SameLine(0, 5);
			RenderImageButton("##InspectorMoreButton", threeDotsTexture, Vector2(16, 16), 1, transparentColor);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 4);
			// Render the actual menu on click
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
			


			// Lambda
			auto L_IsActiveCheckbox = [](bool& _isActive)
			{
				ImGui::SetCursorPos(Vector2(ImGui::GetCursorPosX() + 3, ImGui::GetCursorPosY() + 1));
				bool _checked = RenderCheckbox("Active", _isActive);
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3);
				ImGui::Separator();
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 4);

				return _checked;
			};


			// Components Section
			RenderSectionHeader("Components");


			std::vector<std::shared_ptr<Component>> components = focusedObject->GetComponents();

			// For scrolling components section with background
			ImGui::PushStyleColor(ImGuiCol_ChildBg, logBgColor);
			ImGui::BeginChild("ComponentsSectionBg", Vector2(0,ImGui::GetContentRegionAvail().y - 30), childFlags);
			ImGui::PopStyleColor();

			if (components.size() > 0)
			{
				long queuedForDelete = -1;

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
						//ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f);
						
						ImGui::BeginChild(componentID.c_str(), Vector2(0, 0), autoResizeChildFlags);

						ImGui::PopStyleColor();
						ImGui::PopStyleColor();
						//ImGui::PopStyleVar();

						// Border around each component
						auto wPos = ImGui::GetWindowPos();
						auto wSize = ImGui::GetWindowSize();
						ImGui::GetWindowDrawList()->AddRect({ wPos.x + 2, wPos.y + 2 }, {wPos.x + wSize.x - 2, wPos.y + wSize.y - 2}, ImColor(componentBorderColor.x, componentBorderColor.y, componentBorderColor.z, componentBorderColor.w));

						// Component Name
						ImGui::SetCursorPos(Vector2(ImGui::GetCursorPosX() + 5, ImGui::GetCursorPosY() + 5));
						ImGui::Text(componentType.c_str());

						ImGui::SameLine(ImGui::GetContentRegionAvail().x - (childPadding + 42), 5);

						// Pushes	
						ImGui::PushItemWidth(-1.0f);						
						ImGui::PushStyleColor(ImGuiCol_Border, componentBorderColor);
						ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, Vector2(1.0f, 1.0f));
						ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 1, 1, 0));

						std::string expandID = "##expandIcon-" + i;
						std::string trashcanID = "##trashIcon-" + i;
						std::string openFileID = "##openFileIcon-" + i;

						ImGui::SetCursorPos(Vector2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() - 3));
						// Trash Can Icon for removing Component from Focused Object
						if (RenderImageButton(trashcanID.c_str(), trashTexture))
							queuedForDelete = components[i]->GetID();

						ImGui::SameLine(0, 5);

						ImGui::SetCursorPos(Vector2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() - 3));
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
							ImGui::SetCursorPos(Vector2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() - 15));
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
							std::string componentItemID = "##ComponentItem-" + componentType;
							ImGui::PushStyleColor(ImGuiCol_ChildBg, singleItemColor);
							ImGui::BeginChild(componentItemID.c_str(), Vector2(0, 0), autoResizeChildFlags);

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
								long id = transform->GetID();

								// Active Checkbox
								if (L_IsActiveCheckbox(_isActive))
									transform->SetActive(_isActive);

								// Render Table
								if (PushTable("##TransformProperties" + std::to_string(id), 2))
								{
									RenderFloatDragTableRow("##xPosition" + std::to_string(id), "X Position", xPos, 0.1f, -FLT_MAX, -FLT_MAX);
									RenderFloatDragTableRow("##yPosition" + std::to_string(id), "Y Position", yPos, 0.1f, -FLT_MAX, -FLT_MAX);
									transform->SetPosition(Vector2(xPos, yPos));
									RenderFloatDragTableRow("##rotation" + std::to_string(id), "Rotation", rotation, 0.1f, -360, 360);
									transform->SetRotation(rotation);
									RenderFloatDragTableRow("##xScaleDrag" + std::to_string(id), "X Scale", scaleX, 0.1f, 0.001f, 1000);
									RenderFloatDragTableRow("##yScaleDrag" + std::to_string(id), "Y Scale", scaleY, 0.1f, 0.001f, 1000);
									transform->SetScale(Vector2(scaleX, scaleY));
									PopTable();
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
								Vector2 textureScale = sprite->GetScale();
								float xScale = textureScale.x;
								float yScale = textureScale.y;
								int renderOrder = sprite->GetRenderOrder();
								bool _isActive = sprite->IsActive();
								Vector2 offset = sprite->GetOffset();
								float xOffset = offset.x;
								float yOffset = offset.y;
								std::string pathString = "Path: ";
								std::string textureWidthString = std::to_string(textureWidth);
								std::string textureHeightString = std::to_string(textureHeight);
								long id = sprite->GetID();

								// Active Checkbox
								if (L_IsActiveCheckbox(_isActive))
									sprite->SetActive(_isActive);								

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
								std::string spriteID = "##spritePath" + std::to_string(id);
								ImGui::PushStyleColor(ImGuiCol_FrameBg, inputColor);
								if (ImGui::InputText(spriteID.c_str(), newPath, IM_ARRAYSIZE(newPath), flags))
									sprite->SetTexture(newPath);
								ImGui::PopStyleColor();

								// Render Table
								if (PushTable("##SpriteProperties" + std::to_string(id), 2))
								{
									if (RenderFloatDragTableRow("##xSpriteScaleDrag" + std::to_string(id), "X Scale", xScale, 0.1f, 0.001f, 1000))
										sprite->SetScale(Vector2(xScale, yScale));
									if (RenderFloatDragTableRow("##ySpriteScaleDrag" + std::to_string(id), "Y Scale", yScale, 0.1f, 0.001f, 1000))
										sprite->SetScale(Vector2(xScale, yScale));																
									if (RenderFloatDragTableRow("##xSpriteOffsetDrag" + std::to_string(id), "X Offset", xOffset, 0.1f, -FLT_MAX, -FLT_MAX))
										sprite->SetOffset(Vector2(xOffset, yOffset));
									if (RenderFloatDragTableRow("##ySpriteOffsetDrag" + std::to_string(id), "Y Offset", yOffset, 0.1f, -FLT_MAX, -FLT_MAX))
										sprite->SetOffset(Vector2(xOffset, yOffset));										
									if (RenderIntDragTableRow("##renderOrder" + std::to_string(id), "Render Order", renderOrder, 1, 0, (int)maxSpriteLayers))
										sprite->SetRenderOrder(renderOrder);
									RenderTextTableRow("##textureWidth" + std::to_string(id), "Texture width", textureWidthString);
									RenderTextTableRow("##textureHeight" + std::to_string(id), "Texture height", textureHeightString);
									PopTable();
								}
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
								long id = camera->GetID();

								// Active Checkbox
								if (L_IsActiveCheckbox(_isActive))
									camera->SetActive(_isActive);

								// Render Table
								if (PushTable("##CameraProperties" + std::to_string(id), 2))
								{
									RenderFloatDragTableRow("##cameraWidth" + std::to_string(id), "Camera width", width, 0.1f, 0, 1000);
									RenderFloatDragTableRow("##cameraHeight" + std::to_string(id), "Camera height", height, 0.1f, 0, 1000);
									camera->SetDimensions(width, height);
									RenderFloatDragTableRow("##cameraZoom" + std::to_string(id), "Camera zoom", zoom, 0.1f, 1, 100);
									camera->SetZoom(zoom);
									PopTable();
								}

								// Frustrum color picker
								std::string frustrumID = "##FrustrumColor" + std::to_string(id);
								ImVec4 color = ImVec4(frustrumColor.x / 255.0f, frustrumColor.y / 255.0f, frustrumColor.z / 255.0f, frustrumColor.w / 255.0f);
								ImGui::ColorEdit4(frustrumID.c_str(), (float*)&color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
								ImGui::SameLine(0, 5);
								ImGui::Text("Frustrum color");
								camera->SetFrustrumColor(ImVec4(color.x * 255.0f, color.y * 255.0f, color.z * 255.0f, color.w * 255.0f));

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
								long id = script->GetID();

								// Active Checkbox
								if (L_IsActiveCheckbox(_isActive))
									script->SetActive(_isActive);

								std::string inputId = "##scriptName" + std::to_string(id);
								if (RenderInput(inputId, "Name: ", path))
									script->SetAttachedScript(path);
							}
							else if (componentType == "Button")
							{
								std::shared_ptr<FlatEngine::Button> button = std::static_pointer_cast<FlatEngine::Button>(components[i]);
								bool _isActive = button->IsActive();
								float activeWidth = button->GetActiveWidth();
								float activeHeight = button->GetActiveHeight();
								Vector2 activeOffset = button->GetActiveOffset();
								int activeLayer = button->GetActiveLayer();	
								long id = button->GetID();

								// Active Checkbox
								if (L_IsActiveCheckbox(_isActive))
									button->SetActive(_isActive);

								// Render Table
								if (PushTable("##ButtonProperties" + std::to_string(id), 2))
								{
									RenderFloatDragTableRow("##activeWidth" + std::to_string(id), "Active width", activeWidth, 0.1f, 0, 1000);
									RenderFloatDragTableRow("##activeHeight" + std::to_string(id), "Active height", activeHeight, 0.1f, 0, 1000);
									button->SetActiveDimensions(activeWidth, activeHeight);
									RenderFloatDragTableRow("##activeoffsetx" + std::to_string(id), "X Offset", activeOffset.x, 0.1f, -FLT_MAX, -FLT_MAX);
									RenderFloatDragTableRow("##activeoffsety" + std::to_string(id), "Y Offset", activeOffset.y, 0.1f, -FLT_MAX, -FLT_MAX);
									button->SetActiveOffset(activeOffset);
									PopTable();
								}
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
								long id = canvas->GetID();

								// Active Checkbox
								if (L_IsActiveCheckbox(_isActive))
									canvas->SetActive(_isActive);

								// Render Table
								if (PushTable("##CanvasProperties" + std::to_string(id), 2))
								{
									RenderIntDragTableRow("##layerNumber" + std::to_string(id), "Canvas layer", layerNumber, 1, 20, 20);
									canvas->SetLayerNumber(layerNumber);
									RenderFloatDragTableRow("##Canvas width" + std::to_string(id), "Width", canvasWidth, 0.1f, 0.1f, -FLT_MAX);
									RenderFloatDragTableRow("##Canvas height" + std::to_string(id), "Height", canvasHeight, 0.1f, 0.1f, -FLT_MAX);
									canvas->SetDimensions(canvasWidth, canvasHeight);
									PopTable();
								}

								// _BlocksLayers Checkbox
								if (RenderCheckbox("Blocks Layers:", _blocksLayers))
									canvas->SetBlocksLayers(_blocksLayers);							
							}
							else if (componentType == "Animation")
							{
								std::shared_ptr<FlatEngine::Animation> animation = std::static_pointer_cast<FlatEngine::Animation>(components[i]);

								std::string path = animation->GetAnimationPath();
								bool _isActive = animation->IsActive();
								long id = animation->GetID();

								// Active Checkbox
								if (L_IsActiveCheckbox(_isActive))
									animation->SetActive(_isActive);

								bool _canOpenFiles = true;
								RenderInput("##animationPath" + std::to_string(id), "Path: ", path, _canOpenFiles);
								animation->SetAnimationPath(path);

								if (GameLoopStarted() && !GameLoopPaused())
								{
									if (RenderButton("Play Animation"))
										animation->Play(GetEllapsedGameTimeInMs());

									if (animation->GetAnimationPath() != "")
										ImGui::SameLine(0, 5);
								}
									
								if (animation->GetAnimationPath() != "")
								{									
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
								long id = audio->GetID();

								// Active Checkbox
								if (L_IsActiveCheckbox(_isActive))
									audio->SetActive(_isActive);

								// Path Input
								bool _canOpenFiles = true;
								std::string inputId = "##audioPath_" + std::to_string(id);
								bool _pathChanged = RenderInput(inputId, "Path: ", path, _canOpenFiles);
								audio->SetPath(path);						
								bool _soundTypeChanged = RenderCheckbox("Is Music", _isMusic);

								// Reload the effect as music or chunk if changed	
								if (_pathChanged || _soundTypeChanged)
								{
									if (_isMusic)
										audio->LoadMusic(path);
									else
										audio->LoadEffect(path);

									audio->SetIsMusic(_isMusic);
								}

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
								std::shared_ptr<Texture> texture = text->GetTexture();
								float textureWidth = (float)texture->getWidth();
								float textureHeight = (float)texture->getHeight();
								int renderOrder = text->GetRenderOrder();
								SDL_Color color = text->GetColor();
								Vector2 offset = text->GetOffset();
								float xOffset = offset.x;
								float yOffset = offset.y;
								long id = text->GetID();

								// Active Checkbox
								if (L_IsActiveCheckbox(_isActive))
									text->SetActive(_isActive);

								std::string textText = text->GetText();
								if (RenderInput("##TextContent" + std::to_string(id), "Text: ", textText))
								{
									text->SetText(textText);
									text->LoadText();
								}
								bool _canOpenFiles = true;
								std::string fontPath = text->GetFontPath();
								if (RenderInput("##FontPath" + std::to_string(id), "Font path: ", fontPath, _canOpenFiles))
									text->SetFontPath(fontPath);

								// Render Table
								if (PushTable("##TextProperties" + std::to_string(id), 2))
								{
									RenderTextTableRow("##textWidth" + std::to_string(id), "Text width", std::to_string(textureWidth));
									RenderTextTableRow("##textHeight" + std::to_string(id), "Text height", std::to_string(textureHeight));
									RenderTextTableRow("##xTextOffset" + std::to_string(id), "X offset", std::to_string(xOffset));
									RenderTextTableRow("##yTextOffset" + std::to_string(id), "Y offset", std::to_string(yOffset));
									RenderIntDragTableRow("##TextRenderOrder" + std::to_string(id), "Render Order", renderOrder, 1, 0, (int)maxSpriteLayers);
									text->SetRenderOrder(renderOrder);
									PopTable();
								}
							}
							else if (componentType == "CharacterController")
							{						
								std::shared_ptr<CharacterController> characterController = std::static_pointer_cast<CharacterController>(components[i]);
								bool _isActive = characterController->IsActive();
								float maxAcceleration = characterController->GetMaxAcceleration();								
								float maxSpeed = characterController->GetMaxSpeed();
								float airControl = characterController->GetAirControl();
								bool _isMoving = characterController->IsMoving();	
								long id = characterController->GetID();
								std::string isMoving = "false";
								if (characterController->IsMoving())
									isMoving = "true";

								// Active Checkbox
								if (L_IsActiveCheckbox(_isActive))
									characterController->SetActive(_isActive);

								// Render Table
								if (PushTable("##CharacterControllerProps" + std::to_string(id), 2))
								{
									if (RenderFloatDragTableRow("##MaxAccelerationDrag" + std::to_string(id), "Max Acceleration", maxAcceleration, 0.01f, 0.0f, 20.0f))
										characterController->SetMaxAcceleration(maxAcceleration);
									if (RenderFloatDragTableRow("##MaxSpeedDrag" + std::to_string(id), "Max Speed", maxSpeed, 0.01f, 0.0f, 1000.0f))
										characterController->SetMaxSpeed(maxSpeed);
									if (RenderFloatDragTableRow("##AirControlDrag" + std::to_string(id), "Air Control", airControl, 0.01f, 0.0f, 1000.0f))
										characterController->SetAirControl(airControl);
									RenderTextTableRow("##IsMoving" + std::to_string(id), "Is Moving", isMoving);
									PopTable();
								}
							}
							else if (componentType == "BoxCollider")
							{
								std::shared_ptr<BoxCollider> boxCollider = std::static_pointer_cast<BoxCollider>(components[i]);
								bool _isActive = boxCollider->IsActive();
								bool _isColliding = boxCollider->IsColliding();
								float activeWidth = boxCollider->GetActiveWidth();
								float activeHeight = boxCollider->GetActiveHeight();
								ImVec4 activeEdges = boxCollider->GetActiveEdges();
								Vector2 activeOffset = boxCollider->GetActiveOffset();
								bool _isContinuous = boxCollider->IsContinuous();
								bool _isStatic = boxCollider->IsStatic();
								bool _isSolid = boxCollider->IsSolid();
								bool _showActiveRadius = boxCollider->GetShowActiveRadius();
								int activeLayer = boxCollider->GetActiveLayer();	
								long id = boxCollider->GetID();
								std::string isCollidingString = "false";
								if (_isColliding)
									isCollidingString = "true";

								// Active Checkbox
								if (L_IsActiveCheckbox(_isActive))
									boxCollider->SetActive(_isActive);

								// Render Table
								if (PushTable("##BoxColliderProps" + std::to_string(id), 2))
								{
									if (RenderFloatDragTableRow("##BoxColliderWidth" + std::to_string(id), "Width", activeWidth, 0.01f, 0.0f, 20.0f))
										boxCollider->SetActiveDimensions(activeWidth, activeHeight);
									if (RenderFloatDragTableRow("##BoxColliderHeight" + std::to_string(id), "Height", activeHeight, 0.01f, 0.0f, 20.0f))
										boxCollider->SetActiveDimensions(activeWidth, activeHeight);
									if (RenderFloatDragTableRow("##ActiveOffsetBoxColliderX" + std::to_string(id), "X Offset", activeOffset.x, 0.01f, -FLT_MAX, -FLT_MAX))
										boxCollider->SetActiveOffset(activeOffset);
									if (RenderFloatDragTableRow("##ActiveOffsetBoxColliderY" + std::to_string(id), "Y Offset", activeOffset.y, 0.01f, -FLT_MAX, -FLT_MAX))
										boxCollider->SetActiveOffset(activeOffset);
									if (RenderIntDragTableRow("##BoxColliderActiveLayer" + std::to_string(id), "Active layer", activeLayer, 1, 0, 100))
										boxCollider->SetActiveLayer(activeLayer);
									RenderTextTableRow("##BoxColliderIsColliding" + std::to_string(id), "Is Colliding", isCollidingString);
									PopTable();
								}

								ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);							
								RenderCheckbox(" Is Continuous", _isContinuous);
								boxCollider->SetIsContinuous(_isContinuous);
								RenderCheckbox(" Is Static", _isStatic);
								boxCollider->SetIsStatic(_isStatic);
								RenderCheckbox(" Is Solid", _isSolid);
								boxCollider->SetIsSolid(_isSolid);
								RenderCheckbox(" Show Active Radius", _showActiveRadius);
								boxCollider->SetShowActiveRadius(_showActiveRadius);
							}
							else if (componentType == "CircleCollider")
							{
								std::shared_ptr<CircleCollider> circleCollider = std::static_pointer_cast<CircleCollider>(components[i]);
								bool _isActive = circleCollider->IsActive();
								bool _isColliding = circleCollider->IsColliding();
								float activeRadius = circleCollider->GetActiveRadiusGrid();
								Vector2 activeOffset = circleCollider->GetActiveOffset();
								bool _isContinuous = circleCollider->IsContinuous();
								bool _isStatic = circleCollider->IsStatic();
								bool _isSolid = circleCollider->IsSolid();								
								int activeLayer = circleCollider->GetActiveLayer();
								long id = circleCollider->GetID();
								std::string isCollidingString = "false";
								if (_isColliding)
									isCollidingString = "true";

								// Active Checkbox
								if (L_IsActiveCheckbox(_isActive))
									circleCollider->SetActive(_isActive);

								// Render Table
								if (PushTable("##CircleColliderProps" + std::to_string(id), 2))
								{
									RenderFloatDragTableRow("##CircleColliderActiveRadius" + std::to_string(id), "Radius", activeRadius, 0.01f, 0.0f, 20.0f);
									circleCollider->SetActiveRadiusGrid(activeRadius);
									RenderFloatDragTableRow("##ActiveOffsetCircleColliderX" + std::to_string(id), "X Offset", activeOffset.x, 0.01f, -FLT_MAX, -FLT_MAX);
									RenderFloatDragTableRow("##ActiveOffsetCircleColliderY" + std::to_string(id), "Y Offset", activeOffset.y, 0.01f, -FLT_MAX, -FLT_MAX);
									circleCollider->SetActiveOffset(activeOffset);
									RenderIntDragTableRow("##CircleColliderActiveLayer" + std::to_string(id), "Active layer", activeLayer, 1, 0, 100);
									circleCollider->SetActiveLayer(activeLayer);
									RenderTextTableRow("##CircleColliderIsColliding" + std::to_string(id), "Is Colliding", isCollidingString);
									PopTable();
								}

								ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
								RenderCheckbox(" Is Continuous", _isContinuous);
								circleCollider->SetIsContinuous(_isContinuous);
								RenderCheckbox(" Is Static", _isStatic);
								circleCollider->SetIsStatic(_isStatic);
								RenderCheckbox(" Is Solid", _isSolid);
								circleCollider->SetIsSolid(_isSolid);
							}
							else if (componentType == "RigidBody")
							{
								std::shared_ptr<RigidBody> rigidBody = std::static_pointer_cast<RigidBody>(components[i]);
								long id = rigidBody->GetID();
								bool _isActive = rigidBody->IsActive();
								float mass = rigidBody->GetMass();
								float gravity = rigidBody->GetGravity();
								float angularDrag = rigidBody->GetAngularDrag();
								float fallingGravity = rigidBody->GetFallingGravity();
								float terminalVelocity = rigidBody->GetTerminalVelocity();
								float windResistance = rigidBody->GetWindResistance();
								float friction = rigidBody->GetFriction();
								float equilibriumForce = rigidBody->GetEquilibriumForce();
								bool _isKinematic = rigidBody->IsKinematic();
								bool _isStatic = rigidBody->IsStatic();
								bool _isGrounded = rigidBody->IsGrounded();

								// Read only
								Vector2 velocity = rigidBody->GetVelocity();
								Vector2 pendingForces = rigidBody->GetPendingForces();
								Vector2 acceleration = rigidBody->GetAcceleration();

								std::string isGroundedString = "false";
								if (_isGrounded)
									isGroundedString = "true";

								// Active Checkbox
								if (L_IsActiveCheckbox(_isActive))
									rigidBody->SetActive(_isActive);

								// Render Table
								if (PushTable("##RigidBodyProps" + std::to_string(id), 2))
								{
									if (RenderFloatDragTableRow("##Mass" + std::to_string(id), "Mass", mass, 0.01f, 0.0f, -FLT_MAX))
										rigidBody->SetMass(mass);
									if (RenderFloatDragTableRow("##GravityScale" + std::to_string(id), "Gravity Scale", gravity, 0.01f, -FLT_MAX, -FLT_MAX))
										rigidBody->SetGravity(gravity);
									if (RenderFloatDragTableRow("##FallingGravityScale" + std::to_string(id), "Falling Gravity", fallingGravity, 0.01f, -FLT_MAX, -FLT_MAX))
										rigidBody->SetFallingGravity(fallingGravity);
									if (RenderFloatDragTableRow("##TerminalVelocity" + std::to_string(id), "Terminal Velocity", terminalVelocity, 0.01f, 0.001f, 1000))
										rigidBody->SetTerminalVelocity(terminalVelocity);
									if (RenderFloatDragTableRow("##WindResistance" + std::to_string(id), "Wind Resistance", windResistance, 0.01f, 0, 1))
										rigidBody->SetWindResistance(windResistance);
									if (RenderFloatDragTableRow("##Friction" + std::to_string(id), "Friction", friction, 0.01f, 0, 1))
										rigidBody->SetFriction(friction);
									if (RenderFloatDragTableRow("##EquilibriumForce" + std::to_string(id), "Equilibrium Force", equilibriumForce, 0.01f, 0, 1000))
										rigidBody->SetEquilibriumForce(equilibriumForce);
									RenderTextTableRow("##VelocityX" + std::to_string(id), "X Velocity", std::to_string(velocity.x));
									RenderTextTableRow("##VelocityY" + std::to_string(id), "Y Velocity", std::to_string(velocity.y));
									RenderTextTableRow("##AccelerationX" + std::to_string(id), "X Acceleration", std::to_string(acceleration.x));
									RenderTextTableRow("##AccelerationY" + std::to_string(id), "Y Acceleration", std::to_string(acceleration.y));
									RenderTextTableRow("##PendingForcesX" + std::to_string(id), "X Pending Forces", std::to_string(pendingForces.x));
									RenderTextTableRow("##PendingForcesY" + std::to_string(id), "Y Pending Forces", std::to_string(pendingForces.y));									
									RenderTextTableRow("##RigidBodyGrounded" + std::to_string(id), "Is Grounded", isGroundedString);
									PopTable();
								}								

								ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
								// Kinematic Checkbox
								RenderCheckbox(" Is Kinematic", _isKinematic);
								rigidBody->SetIsKinematic(_isKinematic);

								ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
								// Static Checkbox
								RenderCheckbox(" Is Static", _isStatic);
								rigidBody->SetIsStatic(_isStatic);
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

			ImGui::EndChild(); // ComponentsSectionBg

			// Render the Adding Components button
			RenderButton("Add Component", Vector2(ImGui::GetContentRegionAvail().x, 0));
			if (ImGui::BeginPopupContextItem("##AddComponent", ImGuiPopupFlags_MouseButtonLeft)) // <-- use last item id as popup id
			{
				L_ShowAddComponentsWindow();
				ImGui::EndPopup();
			}
		}

		EndWindow();
	}
}
}