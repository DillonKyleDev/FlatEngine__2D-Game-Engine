#include "FlatEngine.h"
#include "Scene.h"
#include "Sprite.h"
#include "Camera.h"
#include "Text.h"
#include "Audio.h"
#include "CharacterController.h"

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
			// Name editing
			std::string nameLabel = "Name: ";
			char newName[1024];
			strcpy_s(newName, focusedObject->GetName().c_str());
			ImGuiInputTextFlags flags = ImGuiInputTextFlags_::ImGuiInputTextFlags_AutoSelectAll;

			// Edit field
			ImGui::Text(nameLabel.c_str());
			ImGui::SameLine(0, 5);

			ImGui::PushStyleColor(ImGuiCol_FrameBg, inputColor);
			if (ImGui::InputText("##GameObject Name", newName, IM_ARRAYSIZE(newName), flags))
				focusedObject->SetName(newName);
			ImGui::PopStyleColor();

			bool _isActive = focusedObject->IsActive();

			if (ImGui::Checkbox("Active", &_isActive))
				focusedObject->SetActive(_isActive);

			// Components section
			ImGui::Text("Components:");
			ImGui::Separator();
			ImGui::Separator();

			std::vector<std::shared_ptr<Component>> components = focusedObject->GetComponents();

			if (components.size() > 0)
			{
				// Get Expander, Trashcan, and open file icons for components

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
						ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 5));

						// Same Line
						ImGui::SameLine(ImGui::GetContentRegionMax().x - (16 + childPadding + 20), 5); // Add the expander icon on the same line

						// Pushes	
						ImGui::PushItemWidth(-1.0f);
						
						ImGui::PushStyleColor(ImGuiCol_Border, componentBorderColor);
						ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1.0f, 1.0f));
						ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 1, 1, 0));

						std::string expandID = "##expandIcon-" + i;
						std::string trashcanID = "##trashIcon-" + i;
						std::string openFileID = "##openFileIcon-" + i;


						// Trash Can Icon for removing Component from Focused Object
						if (RenderImageButton(trashcanID.c_str(), trashTexture))
							queuedForDelete = components[i]->GetID();

						ImGui::SameLine(0, 5);

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

								static ImGuiTableFlags tableFlags = ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable;

								if (ImGui::BeginTable("##TransformProperties", 2, tableFlags))
								{					
									ImGui::TableSetupColumn("##PROPERTY");
									ImGui::TableSetupColumn("##VALUE");
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

								// Sprite Path Strings
								std::string pathString = "Path: ";
								std::string textureWidthString = "Texture width: " + std::to_string(textureWidth);
								std::string textureHeightString = "Texture height: " + std::to_string(textureHeight);

								// Render Sprite Path
								ImGui::Text(pathString.c_str());
								ImGui::SameLine(0, 5);

								if (RenderImageButton(openFileID.c_str(), openFileTexture))
								{
									std::string assetPath = OpenLoadFileExplorer();
									strcpy_s(newPath, assetPath.c_str());
									sprite->SetTexture(newPath);
								}
								
								ImGui::SameLine(0, 5);
								ImGui::PushStyleColor(ImGuiCol_FrameBg, inputColor);
								if (ImGui::InputText("##spritePath", newPath, IM_ARRAYSIZE(newPath), flags))
									sprite->SetTexture(newPath);
								ImGui::PopStyleColor();
								ImGui::Text(textureWidthString.c_str());
								ImGui::Text(textureHeightString.c_str());

								// Render Order
								ImGui::Text("Render order: ");
								if (ImGui::SliderInt("Sprite Render Order", &renderOrder, 0, maxSpriteLayers, "%d"))
									sprite->SetRenderOrder(renderOrder);

								// Push Item Width Setting
								ImGui::PushItemWidth(ImGui::GetContentRegionMax().x / 2 - 5);

								// Sprite offset variables
								Vector2 offset = sprite->GetOffset();
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

								// Pop Width Setting
								ImGui::PopItemWidth();

								// Assign the new slider values to the sprites pivotPoint
								sprite->SetOffset(Vector2(xOffset, yOffset));
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
									ImGui::PushStyleColor(ImGuiCol_FrameBg, checkboxBg);
									if (ImGui::Checkbox("Is Primary Camera", &_isPrimary))
									{
										if (_isPrimary)
											FlatEngine::GetLoadedScene()->SetPrimaryCamera(camera);
										else
											FlatEngine::GetLoadedScene()->RemovePrimaryCamera();
									}
									ImGui::PopStyleColor();
								}
								else
								{
									bool temp = false;
									if (ImGui::Checkbox("Is Primary Camera", &temp))
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

								// _isActive checkbox
								std::string checkboxId = "Active##" + std::to_string(script->GetID());
								ImGui::Checkbox(checkboxId.c_str(), &_isActive);
								script->SetActive(_isActive);
							}
							else if (componentType == "Button")
							{
								std::shared_ptr<FlatEngine::Button> button = std::static_pointer_cast<FlatEngine::Button>(components[i]);
								bool _isActive = button->IsActive();
								float activeWidth = button->GetActiveWidth();
								float activeHeight = button->GetActiveHeight();
								Vector2 activeOffset = button->GetActiveOffset();
								int activeLayer = button->GetActiveLayer();

								// Active Checkbox
								ImGui::Checkbox("Active:", &_isActive);
								button->SetActive(_isActive);

								// Push Item Width Setting
								ImGui::PushItemWidth(ImGui::GetContentRegionMax().x / 2 - 5);

								// Drags for active width, height and offsets
								ImGui::Text("Active width:");
								ImGui::SameLine(ImGui::GetContentRegionMax().x / 2, 5);
								ImGui::Text("Active height:");

								RenderDragFloat("##activeWidth", 0, activeWidth, 0.5f, 0.1f, -FLT_MAX);
								ImGui::SameLine(0, 5);
								RenderDragFloat("##activeHeight", 0, activeHeight, 0.5f, 0.1f, -FLT_MAX);

								ImGui::Text("Active offset x:");
								ImGui::SameLine(ImGui::GetContentRegionMax().x / 2, 5);
								ImGui::Text("Active offset y:");

								RenderDragFloat("##activeoffsetx", 0, activeOffset.x, 0.1f, -FLT_MAX, -FLT_MAX);
								ImGui::SameLine(0, 5);
								RenderDragFloat("##activeoffsety", 0, activeOffset.y, 0.1f, -FLT_MAX, -FLT_MAX);

								// Assign the new slider values
								button->SetActiveDimensions(activeWidth, activeHeight);
								button->SetActiveOffset(activeOffset);

								// Pop Width Setting
								ImGui::PopItemWidth();
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

								// _BlocksLayers Checkbox
								ImGui::Checkbox("Blocks Layers:", &_blocksLayers);
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

								// Retrieve Animation values
								float ticksPerFrame = animation->GetTicksPerFrame();
								//std::vector<std::shared_ptr<GameObject>> frames = animation->GetFrames();

								// Text
								ImGui::Text("Ticks per frame: ");

								// Set Ticks Per Frame
								ImGui::SliderFloat("##ticksPerFrame", &ticksPerFrame, 0.5f, -FLT_MAX, "%.3f");
								animation->SetTicksPerFrame(ticksPerFrame);

								// Set cursor type
								if (ImGui::IsItemHovered())
									ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_Hand);

								// Total Frames Text
								//std::string totalFrames = "Total frames: " + std::to_string(frames.size());
								//ImGui::Text(totalFrames.c_str());

								// Add Frame Button
								if (RenderButton("Add Frame"))
									animation->AddFrame();

								//ImGui::SliderInt("Animation Time", &previewAnimationTime, 0, 12000);
								// Play Animation Button
								if (RenderButton("Play Animation"))
									animation->Play(GetEllapsedGameTime());

								// Set cursor type
								if (ImGui::IsItemHovered())
									ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_Hand);
							}
							else if (componentType == "Audio")
							{
								std::shared_ptr<FlatEngine::Audio> audio = std::static_pointer_cast<FlatEngine::Audio>(components[i]);

								// Retrieve Audio values
								std::string path = audio->GetPath();
								bool _isMusic = audio->IsMusic();

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
								if (ImGui::Checkbox(checkboxId.c_str(), &_isMusic))
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
								float walkSpeed = characterController->GetWalkSpeed();
								float runSpeed = characterController->GetRunSpeed();
								float gravity = characterController->GetGravity();
								bool _isMoving = characterController->IsMoving();
								float velocity = characterController->GetVelocity();

								static ImGuiTableFlags tableFlags = ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV |
									ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable ;

								if (ImGui::BeginTable("##CharacterControllerProps", 2, tableFlags, ImVec2(-1, 0)))
								{

									ImGui::TableSetupColumn("##PROPERTY");
									ImGui::TableSetupColumn("##DATA");

									ImGui::TableNextRow();

									ImGui::TableSetColumnIndex(0);
									ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
									ImGui::Text("Walk Speed");
									ImGui::TableSetColumnIndex(1);
									ImGui::PushStyleColor(ImGuiCol_FrameBg, outerWindowColor);
									if (RenderDragFloat("##walkSpeedSlider", ImGui::GetContentRegionAvail().x, walkSpeed, 0.01f, 0.0f, 20))
										characterController->SetWalkSpeed(walkSpeed);
									ImGui::PopStyleColor();
									ImGui::PushID("WalkSpeed");
									ImGui::PopID();

									ImGui::TableNextRow();
									ImGui::TableSetColumnIndex(0);
									ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
									ImGui::Text("Run Speed");
									ImGui::TableSetColumnIndex(1);									
									ImGui::PushStyleColor(ImGuiCol_FrameBg, outerWindowColor);
									if (RenderDragFloat("##runSpeedSlider", ImGui::GetContentRegionAvail().x, runSpeed, 0.01f, 0.0f, 20))
										characterController->SetRunSpeed(runSpeed);
									ImGui::PopStyleColor();
									ImGui::PushID("RunSpeed");
									ImGui::PopID();

									ImGui::TableNextRow();
									ImGui::TableSetColumnIndex(0);
									ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
									ImGui::Text("Gravity");
									ImGui::TableSetColumnIndex(1);
									ImGui::PushStyleColor(ImGuiCol_FrameBg, outerWindowColor);
									if (RenderDragFloat("##gravitySlider", ImGui::GetContentRegionAvail().x, gravity, 0.05f, 0.0f, 100))
										characterController->SetGravity(gravity);
									ImGui::PopStyleColor();
									ImGui::PushID("Gravity");
									ImGui::PopID();

									ImGui::TableNextRow();
									ImGui::TableSetColumnIndex(0);
									ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
									ImGui::Text("Velocity");
									ImGui::TableSetColumnIndex(1);
									ImGui::Text(std::to_string(velocity).c_str());
									ImGui::PushID("Velocity");
									ImGui::PopID();

									ImGui::TableNextRow();
									ImGui::TableSetColumnIndex(0);
									ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
									ImGui::Text("Is Moving");
									ImGui::TableSetColumnIndex(1);
									std::string isMoving = "false";
									if (characterController->IsMoving())
										isMoving = "true";
									ImGui::Text(isMoving.c_str());
									ImGui::PushID("IsMoving");
									ImGui::PopID();

									ImGui::EndTable();
								}
							}

							// Pops
							ImGui::PopItemWidth();
							ImGui::PopStyleColor();

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

			// For checking if SceneObject has these components yet
			std::shared_ptr<Component> transformComponent = focusedObject->GetComponent(ComponentTypes::Transform);
			std::shared_ptr<Component> spriteComponent = focusedObject->GetComponent(ComponentTypes::Sprite);
			std::shared_ptr<Component> cameraComponent = focusedObject->GetComponent(ComponentTypes::Camera);
			std::shared_ptr<Component> canvasComponent = focusedObject->GetComponent(ComponentTypes::Canvas);
			std::shared_ptr<Component> animationComponent = focusedObject->GetComponent(ComponentTypes::Animation);
			std::shared_ptr<Component> characterController = focusedObject->GetComponent(ComponentTypes::CharacterController);

			// Render the Adding Components button
			RenderButton("Add Component", ImVec2(ImGui::GetContentRegionMax().x, 0));
			if (ImGui::BeginPopupContextItem("##AddComponent", ImGuiPopupFlags_MouseButtonLeft)) // <-- use last item id as popup id
			{
				ImGui::PushStyleColor(ImGuiCol_FrameBg, outerWindowColor);
				ImGui::BeginListBox("##SceneObjects", ImVec2(220, 100));
				ImGui::PopStyleColor();

				// Push button bg color style
				ImGui::PushStyleColor(ImGuiCol_Button, innerWindowColor);

				// Add all the component types you can add to this GameObject
				//
				if (transformComponent == nullptr)
					if (RenderButton("Transform", ImVec2(ImGui::GetContentRegionMax().x, 0)))
					{
						focusedObject->AddComponent(ComponentTypes::Transform);
						ImGui::CloseCurrentPopup();
					}

				if (spriteComponent == nullptr)
					if (RenderButton("Sprite", ImVec2(ImGui::GetContentRegionMax().x, 0)))
					{
						focusedObject->AddComponent(ComponentTypes::Sprite);
						ImGui::CloseCurrentPopup();
					}

				if (RenderButton("Button", ImVec2(ImGui::GetContentRegionMax().x, 0)))
				{
					focusedObject->AddComponent(ComponentTypes::Button);
					ImGui::CloseCurrentPopup();
				}

				if (cameraComponent == nullptr)
					if (RenderButton("Camera", ImVec2(ImGui::GetContentRegionMax().x, 0)))
					{
						focusedObject->AddComponent(ComponentTypes::Camera);
						ImGui::CloseCurrentPopup();
					}

				if (canvasComponent == nullptr)
					if (RenderButton("Canvas", ImVec2(ImGui::GetContentRegionMax().x, 0)))
					{
						focusedObject->AddComponent(ComponentTypes::Canvas);
						ImGui::CloseCurrentPopup();
					}

				if (animationComponent == nullptr)
					if (RenderButton("Animation", ImVec2(ImGui::GetContentRegionMax().x, 0)))
					{
						focusedObject->AddComponent(ComponentTypes::Animation);
						ImGui::CloseCurrentPopup();
					}

				if (RenderButton("Audio", ImVec2(ImGui::GetContentRegionMax().x, 0)))
				{
					focusedObject->AddComponent(ComponentTypes::Audio);
					ImGui::CloseCurrentPopup();
				}

				if (RenderButton("Text", ImVec2(ImGui::GetContentRegionMax().x, 0)))
				{
					focusedObject->AddComponent(ComponentTypes::Text);
					ImGui::CloseCurrentPopup();
				}

				if (RenderButton("Script", ImVec2(ImGui::GetContentRegionMax().x, 0)))
				{
					focusedObject->AddComponent(ComponentTypes::Script);
					ImGui::CloseCurrentPopup();
				}

				if (characterController == nullptr)
					if (RenderButton("Character Controller", ImVec2(ImGui::GetContentRegionMax().x, 0)))
					{
						focusedObject->AddComponent(ComponentTypes::CharacterController);
						ImGui::CloseCurrentPopup();
					}

				ImGui::EndListBox();

				// Pop button bg color styles
				ImGui::PopStyleColor();
				ImGui::EndPopup();
			}

			if (RenderButton("Delete GameObject"))
			{
				// Unfocus GameObject first
				int tempID = focusedObjectID;
				FlatEngine::SetFocusedGameObjectID(-1);
				FlatEngine::DeleteGameObject(tempID);
			}
		}
		ImGui::PopStyleColor();
		ImGui::EndChild();

		ImGui::End();
	}
}
}