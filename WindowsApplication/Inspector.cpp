#include "FlatEngine.h"
#include "Scene.h"
#include "Sprite.h"
#include "Camera.h"
#include "Text.h"

namespace FlatEngine { namespace FlatGui {

	void RenderInspector()
	{
		ImGui::Begin("Inspector");
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
			if (ImGui::InputText("##GameObject Name", newName, IM_ARRAYSIZE(newName), flags))
				focusedObject->SetName(newName);

			bool _isActive = focusedObject->IsActive();

			if (ImGui::Checkbox("Active", &_isActive))
				focusedObject->SetActive(_isActive);

			// Components section
			ImGui::Text("Components:");

			std::vector<std::shared_ptr<Component>> components = focusedObject->GetComponents();

			if (components.size() > 0)
			{
				// Get Expander, Trashcan, and open file icons for components

				long queuedForDelete = -1;
				// Open File Icon
				openFileIcon->loadFromFile("assets/images/OpenFileIcon.png");
				SDL_Texture* openFileTexture = openFileIcon->getTexture();
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
						ImGui::Separator();
						std::string componentType = components[i]->GetTypeString();
						// Get Component ID in to keep the child unique
						std::string componentID = componentType + std::to_string(components[i]->GetID());

						// Begin Component Child
						ImGui::PushStyleColor(ImGuiCol_ChildBg, innerWindowColor);
						ImGui::BeginChild(componentID.c_str(), ImVec2(0, 0), child_flags);
						ImGui::PopStyleColor();
						ImGui::Text(componentType.c_str());

						// Same Line
						ImGui::SameLine(ImGui::GetContentRegionMax().x - (16 + childPadding + 20), 5); // Add the expander icon on the same line

						// Pushes	
						ImGui::PushItemWidth(-1.0f);
						ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1.0f, 1.0f));
						ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 1, 1, 0));

						std::string expandID = "##expandIcon-" + i;
						std::string trashcanID = "##trashIcon-" + i;
						std::string openFileID = "##openFileIcon-" + i;


						// Trash Can Icon for removing Component from Focused Object
						if (ImGui::ImageButton(trashcanID.c_str(), trashTexture, ImVec2(12, 12), uv0, uv1, bg_col, tint_col))
							queuedForDelete = components[i]->GetID();

						// Set Mouse Cursor
						if (ImGui::IsItemHovered())
							ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_Hand);

						ImGui::SameLine(0, 5);

						// Draw Expand Icon for expanding/collapsing current component information
						if (_isCollapsed)
						{
							if (ImGui::ImageButton(expandID.c_str(), expandTexture, ImVec2(12, 12), uv0, uv1, bg_col, tint_col))
								components[i]->SetCollapsed(!_isCollapsed);
							// Set Mouse Cursor
							if (ImGui::IsItemHovered())
								ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_Hand);
						}
						else
						{
							if (ImGui::ImageButton(expandID.c_str(), expandFlippedTexture, ImVec2(12, 12), uv0, uv1, bg_col, tint_col))
								components[i]->SetCollapsed(!_isCollapsed);
							// Set Mouse Cursor
							if (ImGui::IsItemHovered())
								ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_Hand);
						}


						// Pops
						ImGui::PopStyleColor();
						ImGui::PopStyleVar();
						ImGui::PopItemWidth();


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
								static ImGuiSliderFlags flags = ImGuiSliderFlags_::ImGuiSliderFlags_None;

								// Push Item Width
								ImGui::PushItemWidth(ImGui::GetContentRegionMax().x / 3 - 5);

								// Drags for position editing
								//
								// Render Text for Positions + Rotation
								ImGui::Text("xPos:");
								ImGui::SameLine(ImGui::GetContentRegionMax().x / 3 + 5, 0);
								ImGui::Text("yPos:");
								ImGui::SameLine((ImGui::GetContentRegionMax().x / 3 * 2) + 5, 0);
								ImGui::Text("Rotation:");

								// Render Drags for Positions + Rotation
								ImGui::DragFloat("##xPos", &xPos, 0.5f, -FLT_MAX, -FLT_MAX, "%.3f", flags);
								if (ImGui::IsItemHovered())
									ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_ResizeEW);
								ImGui::SameLine(0, 5);
								ImGui::DragFloat("##yPos", &yPos, 0.5f, -FLT_MAX, -FLT_MAX, "%.3f", flags);
								// Set cursor type
								if (ImGui::IsItemHovered())
									ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_ResizeEW);
								ImGui::SameLine(0, 5);
								ImGui::DragFloat("##rotation", &rotation, 1.0f, -360, 360, "%.3f", flags);
								// Set cursor type
								if (ImGui::IsItemHovered())
									ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_ResizeEW);

								// Assign the new slider values to the transforms position
								transform->SetPosition(Vector2(xPos, yPos));
								transform->SetRotation(rotation);

								// Render Drags for scale of transform
								// Push
								ImGui::PushItemWidth(ImGui::GetContentRegionMax().x / 2 - 5);

								// Render text for scales
								ImGui::Text("Scale x:");
								ImGui::SameLine(ImGui::GetContentRegionMax().x / 2 + 5, 0);
								ImGui::Text("Scale y:");

								// Render Drags for scales
								ImGui::DragFloat("##xScale", &scaleX, 0.05f, 0, -FLT_MAX, "%.3f", flags);
								// Set cursor type
								if (ImGui::IsItemHovered())
									ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_ResizeEW);
								ImGui::SameLine(0, 5);
								ImGui::DragFloat("##yScale", &scaleY, 0.05f, -FLT_MAX, -FLT_MAX, "%.3f", flags);
								// Set cursor type
								if (ImGui::IsItemHovered())
									ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_ResizeEW);

								// Pop Width Setting
								ImGui::PopItemWidth();

								// Assign the new slider values to the sprites pivotPoint
								transform->SetScale(Vector2(scaleX, scaleY));
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

								if (ImGui::ImageButton(openFileID.c_str(), openFileTexture, ImVec2(12, 12), uv0, uv1, bg_col, tint_col))
								{
									std::string assetPath = OpenLoadFileExplorer();
									strcpy_s(newPath, assetPath.c_str());
									sprite->SetTexture(newPath);
								}

								// Set Mouse Cursor
								if (ImGui::IsItemHovered())
									ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_Hand);

								ImGui::SameLine(0, 5);
								if (ImGui::InputText("##spritePath", newPath, IM_ARRAYSIZE(newPath), flags))
									sprite->SetTexture(newPath);
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
								ImGui::DragFloat("##xOffset", &xOffset, 0.5f, -FLT_MAX, -FLT_MAX, "%.3f", flags);
								// Set cursor type
								if (ImGui::IsItemHovered())
									ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_ResizeEW);
								ImGui::SameLine(0, 5);
								ImGui::DragFloat("##yOffset", &yOffset, 0.5f, -FLT_MAX, -FLT_MAX, "%.3f", flags);
								// Set cursor type
								if (ImGui::IsItemHovered())
									ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_ResizeEW);

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
								ImGui::DragFloat("##CameraWidth", &width, 0.5f, 0, -FLT_MAX, "%.3f", flags);
								// Set cursor type
								if (ImGui::IsItemHovered())
									ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_ResizeEW);
								ImGui::SameLine(0, 5);
								ImGui::DragFloat("##CameraHeight", &height, 0.5f, 0, -FLT_MAX, "%.3f", flags);
								// Set cursor type
								if (ImGui::IsItemHovered())
									ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_ResizeEW);

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
									if (ImGui::Checkbox("Is Primary Camera", &_isPrimary))
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
								if (ImGui::InputText(inputId.c_str(), newPath, IM_ARRAYSIZE(newPath), flags))
									script->SetAttachedScript(newPath);

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

								ImGui::DragFloat("##activeWidth", &activeWidth, 0.5f, 0.1f, -FLT_MAX, "%.3f", flags);
								// Set cursor type
								if (ImGui::IsItemHovered())
									ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_ResizeEW);
								ImGui::SameLine(0, 5);
								ImGui::DragFloat("##activeHeight", &activeHeight, 0.5f, 0.1f, -FLT_MAX, "%.3f", flags);
								// Set cursor type
								if (ImGui::IsItemHovered())
									ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_ResizeEW);

								ImGui::Text("Active offset x:");
								ImGui::SameLine(ImGui::GetContentRegionMax().x / 2, 5);
								ImGui::Text("Active offset y:");

								ImGui::DragFloat("##activeoffsetx", &activeOffset.x, 0.1f, -FLT_MAX, -FLT_MAX, "%.3f", flags);
								// Set cursor type
								if (ImGui::IsItemHovered())
									ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_ResizeEW);
								ImGui::SameLine(0, 5);
								ImGui::DragFloat("##activeoffsety", &activeOffset.y, 0.1f, -FLT_MAX, -FLT_MAX, "%.3f", flags);
								// Set cursor type
								if (ImGui::IsItemHovered())
									ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_ResizeEW);

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

								// Push Item Width Setting
								ImGui::PushItemWidth(ImGui::GetContentRegionMax().x / 2 - 5);

								ImGui::DragFloat("##canvasWidth", &canvasWidth, 0.5f, 0.1f, -FLT_MAX, "%.3f", flags);
								// Set cursor type
								if (ImGui::IsItemHovered())
									ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_ResizeEW);
								ImGui::SameLine(0, 5);
								ImGui::DragFloat("##canvasHeight", &canvasHeight, 0.5f, 0.1f, -FLT_MAX, "%.3f", flags);
								// Set cursor type
								if (ImGui::IsItemHovered())
									ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_ResizeEW);

								// Assign new values to Canvas
								canvas->SetDimensions(canvasWidth, canvasHeight);


								ImGui::PopItemWidth();
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

								if (ImGui::ImageButton(buttonID.c_str(), openFileTexture, ImVec2(12, 12), uv0, uv1, bg_col, tint_col))
								{
									std::string assetPath = OpenLoadFileExplorer();
									strcpy_s(newPath, assetPath.c_str());
									animation->SetAnimationPath(newPath);
								}
								// Set Mouse Cursor
								if (ImGui::IsItemHovered())
									ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_Hand);
								ImGui::SameLine(0, 5);
								if (ImGui::InputText("##animationPath", newPath, IM_ARRAYSIZE(newPath), flags))
									animation->SetAnimationPath(newPath);


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
								if (ImGui::Button("Add Frame"))
									animation->AddFrame();

								//ImGui::SliderInt("Animation Time", &previewAnimationTime, 0, 12000);
								// Play Animation Button
								if (ImGui::Button("Play Animation"))
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
								if (ImGui::InputText(inputId.c_str(), newPath, IM_ARRAYSIZE(newPath), flags))
								{
									if (_isMusic)
										audio->LoadMusic(newPath);
									else
										audio->LoadEffect(newPath);

									audio->SetPath(newPath);
								}

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
								if (ImGui::Button("Play"))
									audio->Play();
								ImGui::SameLine(0, 5);
								// Pause Audio
								if (ImGui::Button("Pause"))
									audio->Pause();
								ImGui::SameLine(0, 5);
								// Stop Audio
								if (ImGui::Button("Stop"))
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
								if (ImGui::InputText("##textString", newTextString, IM_ARRAYSIZE(newTextString), flags))
								{
									text->SetText(newTextString);
									text->LoadText();
								}


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
								if (ImGui::InputText("##fontPath", newPath, IM_ARRAYSIZE(newPath), flags))
									text->SetFontPath(newPath);

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
								ImGui::DragFloat("##xOffset", &xOffset, 0.5f, -FLT_MAX, -FLT_MAX, "%.3f", flags);
								// Set cursor type
								if (ImGui::IsItemHovered())
									ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_ResizeEW);
								ImGui::SameLine(0, 5);
								ImGui::DragFloat("##yOffset", &yOffset, 0.5f, -FLT_MAX, -FLT_MAX, "%.3f", flags);
								// Set cursor type
								if (ImGui::IsItemHovered())
									ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_ResizeEW);
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

							// Pops
							ImGui::PopItemWidth();
							ImGui::PopStyleColor();

							ImGui::EndChild();
						}
					}

					ImGui::EndChild();

					if (i == components.size() - 1)
						ImGui::Separator();
				}

				if (queuedForDelete != -1)
				{
					focusedObject->RemoveComponent(queuedForDelete);
					queuedForDelete = -1;
				}
			}

			// For checking if SceneObject has these components yet
			std::shared_ptr<FlatEngine::Component> transformComponent = focusedObject->GetComponent(ComponentTypes::Transform);
			std::shared_ptr<FlatEngine::Component> spriteComponent = focusedObject->GetComponent(ComponentTypes::Sprite);
			std::shared_ptr<FlatEngine::Component> cameraComponent = focusedObject->GetComponent(ComponentTypes::Camera);
			std::shared_ptr<FlatEngine::Component> canvasComponent = focusedObject->GetComponent(ComponentTypes::Canvas);
			std::shared_ptr<FlatEngine::Component> animationComponent = focusedObject->GetComponent(ComponentTypes::Animation);

			// Render the Adding Components button
			ImGui::Button("Add Component", ImVec2(ImGui::GetContentRegionMax().x, 0));
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
					if (ImGui::Button("Transform", ImVec2(ImGui::GetContentRegionMax().x, 0)))
					{
						focusedObject->AddComponent(ComponentTypes::Transform);
						ImGui::CloseCurrentPopup();
					}

				if (spriteComponent == nullptr)
					if (ImGui::Button("Sprite", ImVec2(ImGui::GetContentRegionMax().x, 0)))
					{
						focusedObject->AddComponent(ComponentTypes::Sprite);
						ImGui::CloseCurrentPopup();
					}

				if (ImGui::Button("Button", ImVec2(ImGui::GetContentRegionMax().x, 0)))
				{
					focusedObject->AddComponent(ComponentTypes::Button);
					ImGui::CloseCurrentPopup();
				}

				if (cameraComponent == nullptr)
					if (ImGui::Button("Camera", ImVec2(ImGui::GetContentRegionMax().x, 0)))
					{
						focusedObject->AddComponent(ComponentTypes::Camera);
						ImGui::CloseCurrentPopup();
					}

				if (canvasComponent == nullptr)
					if (ImGui::Button("Canvas", ImVec2(ImGui::GetContentRegionMax().x, 0)))
					{
						focusedObject->AddComponent(ComponentTypes::Canvas);
						ImGui::CloseCurrentPopup();
					}

				if (animationComponent == nullptr)
					if (ImGui::Button("Animation", ImVec2(ImGui::GetContentRegionMax().x, 0)))
					{
						focusedObject->AddComponent(ComponentTypes::Animation);
						ImGui::CloseCurrentPopup();
					}

				if (ImGui::Button("Audio", ImVec2(ImGui::GetContentRegionMax().x, 0)))
				{
					focusedObject->AddComponent(ComponentTypes::Audio);
					ImGui::CloseCurrentPopup();
				}

				if (ImGui::Button("Text", ImVec2(ImGui::GetContentRegionMax().x, 0)))
				{
					focusedObject->AddComponent(ComponentTypes::Text);
					ImGui::CloseCurrentPopup();
				}

				if (ImGui::Button("Script", ImVec2(ImGui::GetContentRegionMax().x, 0)))
				{
					focusedObject->AddComponent(ComponentTypes::Script);
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndListBox();

				// Pop button bg color styles
				ImGui::PopStyleColor();
				ImGui::EndPopup();
			}

			if (ImGui::Button("Delete GameObject"))
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