#include "FlatEngine.h"
#include "FlatGui.h"
#include "GameObject.h"
#include "Project.h"
#include "Camera.h"
#include "Transform.h"
#include "Sprite.h"
#include "Text.h"
#include "Texture.h"

#include "imgui.h"
#include "math.h"
#include <fstream>

namespace FL = FlatEngine;

namespace FlatGui 
{
	Vector4 darker = Vector4(float(0.2), float(0.2), float(0.2), float(1));
	Vector4 lighter = Vector4(float(0.8), float(0.8), float(0.8), float(1));
	Vector4 light = Vector4(float(0.7), float(0.7), float(0.7), float(1));
	Vector4 dark = Vector4(float(0.3), float(0.3), float(0.3), float(1));


	void RenderAnimator()
	{
		FL::PushWindowStyles();
		// 16 | 8 are flags for noScrollbar and noscrollwithmouse
			ImGui::Begin("Animator", &FG_b_showAnimator, 16 | 8);
		FL::PopWindowStyles();

		// Animated Properties BeginChild()
		FL::BeginResizeWindowChild("Animated Properties");

		// Border around object
		auto propsWindowPos = ImGui::GetWindowPos();
		auto propsWindowSize = ImGui::GetWindowSize();  // This is the size of the current box, perfect for getting the exact dimensions for a border
		ImGui::GetWindowDrawList()->AddRect({ propsWindowPos.x + 2, propsWindowPos.y + 2 }, { propsWindowPos.x + propsWindowSize.x - 2, propsWindowPos.y + propsWindowSize.y - 2 }, FL::GetColor32("componentBorder"), 0);

		std::string animationName = "-No Animation Selected-";
		if (GetFocusedAnimation() != nullptr && GetFocusedAnimation()->animationName != "")
			animationName = "Loaded Animation: " + GetFocusedAnimation()->animationName;

			ImGui::BeginChild("Manage Animation", Vector2(0, 0), FL::F_autoResizeChildFlags);

		static std::string animationFilePath;
		if (GetFocusedAnimation() != nullptr)
		{
			animationFilePath = GetFocusedAnimation()->animationPath;
		}

		static std::string newFileName = "";
		static bool b_openAnimationModal = false;

		if (FL::RenderInputModal("Create Animation", "Enter a name for the Animation:", newFileName, b_openAnimationModal))
		{
			FL::CreateNewAnimationFile(newFileName, FL::GetDir("animations"));
		}

		// Three dots
		FL::RenderSectionHeader(animationName);
		FL::MoveScreenCursor(ImGui::GetContentRegionAvail().x - 29, -31);
		FL::RenderImageButton("##AnimationHamburgerMenu", FL::GetTexture("threeDots"), Vector2(16, 16), 1, FL::GetColor("transparent"));
		FL::PushMenuStyles();
		if (ImGui::BeginPopupContextItem("##AnimationHamburgerMenu", ImGuiPopupFlags_MouseButtonLeft))
		{
			if (ImGui::MenuItem("Save Animation"))
			{
				if (animationFilePath != "")
					FL::SaveAnimationData(GetFocusedAnimation(), animationFilePath);
				ImGui::CloseCurrentPopup();
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Load Animation"))
			{
				animationFilePath = FL::OpenLoadFileExplorer();
				if (animationFilePath != "")
				{
					SetFocusedAnimation(FL::LoadAnimationFile(animationFilePath));
					FL::F_LoadedProject.SetLoadedPreviewAnimationPath(animationFilePath);
				}
				ImGui::CloseCurrentPopup();
			}
			ImGui::Separator();
			if (ImGui::MenuItem("New Animation"))
			{
				b_openAnimationModal = true;
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
		FL::PopMenuStyles();



		std::shared_ptr<Animation::S_AnimationProperties> animProps = GetFocusedAnimation();

		const char* properties[] = { "- select property -", "Event", "Transform", "Sprite", "Camera", "Script", "Button", "Canvas", "Audio", "Text", "BoxCollider", "CircleCollider", "RigidBody", "CharacterController" };
		static int current_property = 0;
		static std::string node_clicked = "";

		// Lambda
		auto L_PushBackKeyFrame = [&](std::string property)
		{
			// Add property to animation object
			if (property == "Event")
			{
				std::shared_ptr<Animation::S_Event> eventProperties = std::make_shared<Animation::S_Event>();
				eventProperties->name = "Event";
				animProps->eventProps.push_back(eventProperties);
			}
			else if (property == "Transform")
			{
				std::shared_ptr<Animation::S_Transform> transformProperties = std::make_shared<Animation::S_Transform>();
				transformProperties->name = "Transform";
				animProps->transformProps.push_back(transformProperties);
			}
			else if (property == "Sprite")
			{
				std::shared_ptr<Animation::S_Sprite> spriteProperties = std::make_shared<Animation::S_Sprite>();
				spriteProperties->name = "Sprite";
				animProps->spriteProps.push_back(spriteProperties);
			}
			else if (property == "Camera")
			{
				std::shared_ptr<Animation::S_Camera> cameraProperties = std::make_shared<Animation::S_Camera>();
				cameraProperties->name = "Camera";
				animProps->cameraProps.push_back(cameraProperties);
			}
			else if (property == "Script")
			{
				std::shared_ptr<Animation::S_Script> scriptProperties = std::make_shared<Animation::S_Script>();
				scriptProperties->name = "Script";
				animProps->scriptProps.push_back(scriptProperties);
			}
			else if (property == "Button")
			{
				std::shared_ptr<Animation::S_Button> buttonProperties = std::make_shared<Animation::S_Button>();
				buttonProperties->name = "Button";
				animProps->buttonProps.push_back(buttonProperties);
			}
			else if (property == "Canvas")
			{
				std::shared_ptr<Animation::S_Canvas> canvasProperties = std::make_shared<Animation::S_Canvas>();
				canvasProperties->name = "Canvas";
				animProps->canvasProps.push_back(canvasProperties);
			}
			else if (property == "Audio")
			{
				std::shared_ptr<Animation::S_Audio> audioProperties = std::make_shared<Animation::S_Audio>();
				audioProperties->name = "Audio";
				animProps->audioProps.push_back(audioProperties);
			}
			else if (property == "Text")
			{
				std::shared_ptr<Animation::S_Text> textProperties = std::make_shared<Animation::S_Text>();
				textProperties->name = "Text";
				animProps->textProps.push_back(textProperties);
			}
			else if (property == "BoxCollider")
			{
				std::shared_ptr<Animation::S_BoxCollider> boxColliderProperties = std::make_shared<Animation::S_BoxCollider>();
				boxColliderProperties->name = "BoxCollider";
				animProps->boxColliderProps.push_back(boxColliderProperties);
			}
			else if (property == "CircleCollider")
			{
				std::shared_ptr<Animation::S_CircleCollider> circleColliderProperties = std::make_shared<Animation::S_CircleCollider>();
				circleColliderProperties->name = "CircleCollider";
				animProps->circleColliderProps.push_back(circleColliderProperties);
			}
			else if (property == "RigidBody")
			{
				std::shared_ptr<Animation::S_RigidBody> rigidBodyProperties = std::make_shared<Animation::S_RigidBody>();
				rigidBodyProperties->name = "RigidBody";
				animProps->rigidBodyProps.push_back(rigidBodyProperties);
			}
			else if (property == "CharacterController")
			{
				std::shared_ptr<Animation::S_CharacterController> characterControllerProperties = std::make_shared<Animation::S_CharacterController>();
				characterControllerProperties->name = "CharacterController";
				animProps->characterControllerProps.push_back(characterControllerProperties);
			}
		};
		// Lambda
		auto L_RemoveKeyFrame = [&](std::string property)
			{
				// Add property to animation object
				if (property == "Event")
				{
					animProps->eventProps.clear();
				}
				else if (property == "Transform")
				{
					animProps->transformProps.clear();
				}
				else if (property == "Sprite")
				{
					animProps->spriteProps.clear();
				}
				else if (property == "Camera")
				{
					animProps->cameraProps.clear();
				}
				else if (property == "Script")
				{
					animProps->scriptProps.clear();
				}
				else if (property == "Button")
				{
					animProps->buttonProps.clear();
				}
				else if (property == "Canvas")
				{
					animProps->canvasProps.clear();
				}
				else if (property == "Audio")
				{
					animProps->audioProps.clear();
				}
				else if (property == "Text")
				{
					animProps->textProps.clear();
				}
				else if (property == "BoxCollider")
				{
					animProps->boxColliderProps.clear();
				}
				else if (property == "CircleCollider")
				{
					animProps->circleColliderProps.clear();
				}
				else if (property == "RigidBody")
				{
					animProps->rigidBodyProps.clear();
				}
				else if (property == "CharacterController")
				{
					animProps->characterControllerProps.clear();
				}
			};

		if (animProps != nullptr && animProps->animationName != "")
		{
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 12);
			Animation* animation = nullptr;
			if (objectForFocusedAnimation != nullptr)
				animation = objectForFocusedAnimation->GetAnimation();

			if (FL::RenderCheckbox("Loop Animation", animProps->_loop) && animation != nullptr && animation->IsPlaying())
			{
				animation->Stop();
				animation->Play(FL::GetEngineTime());
			}

			static std::string selected_property = "";			
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
			ImGui::Text("Add Properties:");
			std::vector<std::string> props = std::vector<std::string>();
			FL::PushComboStyles();
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 34);
			if (ImGui::BeginCombo("##properties", properties[current_property]))
			{
				for (int n = 0; n < IM_ARRAYSIZE(properties); n++)
				{
					if (animProps->eventProps.size() == 0 && properties[n] == "Event" ||
						animProps->transformProps.size() == 0 && properties[n] == "Transform" ||
						animProps->spriteProps.size() == 0 && properties[n] == "Sprite" ||
						animProps->cameraProps.size() == 0 && properties[n] == "Camera" ||
						animProps->scriptProps.size() == 0 && properties[n] == "Script" ||
						animProps->buttonProps.size() == 0 && properties[n] == "Button" ||
						animProps->canvasProps.size() == 0 && properties[n] == "Canvas" ||
						animProps->audioProps.size() == 0 && properties[n] == "Audio" ||
						animProps->textProps.size() == 0 && properties[n] == "Text" ||
						animProps->boxColliderProps.size() == 0 && properties[n] == "BoxCollider" ||
						animProps->circleColliderProps.size() == 0 && properties[n] == "CircleCollider" ||
						animProps->rigidBodyProps.size() == 0 && properties[n] == "RigidBody" ||
						animProps->characterControllerProps.size() == 0 && properties[n] == "CharacterController"
						)
					{
						bool is_selected = (properties[current_property] == properties[n]);
						if (ImGui::Selectable(properties[n], is_selected))
							current_property = n;
						if (is_selected)
							ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
			FL::PopComboStyles();
			ImGui::SameLine(0, 5);
			if (FL::RenderButton("Add"))
			{
				L_PushBackKeyFrame(properties[current_property]);
				// Reset selector box to default
				current_property = 0;
			}


			// List properties in this animation
			ImGui::PushStyleColor(ImGuiCol_FrameBg, FL::GetColor("innerWindow"));
			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, Vector2(0, 0));
			FL::PushMenuStyles();

			// Conditionally begin the table
			if (animProps->eventProps.size() > 0 ||
				animProps->transformProps.size() > 0 ||
				animProps->spriteProps.size() > 0 ||
				animProps->cameraProps.size() > 0 ||
				animProps->scriptProps.size() > 0 ||
				animProps->buttonProps.size() > 0 ||
				animProps->canvasProps.size() > 0 ||
				animProps->audioProps.size() > 0 ||
				animProps->textProps.size() > 0 ||
				animProps->boxColliderProps.size() > 0 ||
				animProps->circleColliderProps.size() > 0 ||
				animProps->rigidBodyProps.size() > 0 || 
				animProps->characterControllerProps.size() > 0)
			if (ImGui::BeginTable("##AnimationProperties", 1, FL::F_tableFlags))
			{
				ImGui::TableSetupColumn("##PROPERTY", 0, ImGui::GetContentRegionAvail().x + 1);
		

				auto RenderPropertyButton = [&](std::string property, int size, std::string& node_clicked)
				{
					ImGuiTreeNodeFlags node_flags;

					std::string treeID = property + "_node";
					if (node_clicked == property)
						node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_Selected;
					else
						node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;

					//// TreeNode Opener - No TreePop because it's a leaf
					if (size > 0)
					{
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						//ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);

						ImGui::TreeNodeEx((void*)(intptr_t)treeID.c_str(), node_flags, property.c_str());
						if (ImGui::IsItemClicked())
							node_clicked = property;

						ImGui::PushID(treeID.c_str());
						ImGui::PopID();
					}
				};


				RenderPropertyButton("Event", (int)animProps->eventProps.size(), node_clicked);
				RenderPropertyButton("Transform", (int)animProps->transformProps.size(), node_clicked);
				RenderPropertyButton("Sprite", (int)animProps->spriteProps.size(), node_clicked);
				RenderPropertyButton("Camera", (int)animProps->cameraProps.size(), node_clicked);
				RenderPropertyButton("Script", (int)animProps->scriptProps.size(), node_clicked);
				RenderPropertyButton("Button", (int)animProps->buttonProps.size(), node_clicked);
				RenderPropertyButton("Canvas", (int)animProps->canvasProps.size(), node_clicked);
				RenderPropertyButton("Audio", (int)animProps->audioProps.size(), node_clicked);
				RenderPropertyButton("Text", (int)animProps->textProps.size(), node_clicked);
				RenderPropertyButton("BoxCollider", (int)animProps->boxColliderProps.size(), node_clicked);
				RenderPropertyButton("CircleCollider", (int)animProps->circleColliderProps.size(), node_clicked);
				RenderPropertyButton("RigidBody", (int)animProps->rigidBodyProps.size(), node_clicked);
				RenderPropertyButton("CharacterController", (int)animProps->characterControllerProps.size(), node_clicked);

			
				ImGui::EndTable();
			}
			FL::PopMenuStyles();
			ImGui::PopStyleVar();
			ImGui::PopStyleColor();
		}

			ImGui::EndChild(); // Manage Animation
		ImGui::EndChild();

		ImGui::SameLine(0, 5);

			// Timeline Events
			ImGui::PushStyleColor(ImGuiCol_ChildBg, FL::GetColor("outerWindow"));
			ImGui::BeginChild("Animation Timeline", Vector2(0, 0), FL::F_childFlags);
			ImGui::PopStyleColor();

			// Border around object
			auto timelineWindowPos = ImGui::GetWindowPos();
			auto timelineWindowSize = ImGui::GetWindowSize();  // This is the size of the current box, perfect for getting the exact dimensions for a border
			ImGui::GetWindowDrawList()->AddRect({ timelineWindowPos.x + 2, timelineWindowPos.y + 2 }, { timelineWindowPos.x + timelineWindowSize.x - 2, timelineWindowPos.y + timelineWindowSize.y - 2 }, FL::GetColor32("componentBorder"), 0);


		FL::RenderSectionHeader("Animation Timeline");

			ImGui::BeginChild("Property Header", Vector2(0,0), FL::F_headerFlags);

		float availableSpace = ImGui::GetContentRegionAvail().x / 2;
		ImGui::SetNextItemWidth(availableSpace);
		ImGui::Text("Property Selected: ");

		// Draw Property selected and background rect for it
		if (node_clicked != "")
		{
			ImGui::SameLine(0, 5);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 3);
			Vector2 textSize = ImGui::CalcTextSize(node_clicked.c_str());
			Vector2 cursorScreen = ImGui::GetCursorScreenPos();
			ImGui::GetWindowDrawList()->AddRectFilled(cursorScreen, Vector2(cursorScreen.x + textSize.x + 6, cursorScreen.y + textSize.y + 6), ImGui::GetColorU32(FL::GetColor("tableCellLight")), 2);
			ImGui::SetCursorPos(Vector2(ImGui::GetCursorPosX() + 3, ImGui::GetCursorPosY() + 3));
			ImGui::Text(node_clicked.c_str());
		}
		
		if (node_clicked != "")
		{
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 6);
			if (FL::RenderButton("Add Keyframe"))
			{
				L_PushBackKeyFrame(node_clicked);
			}
			ImGui::SameLine(0, 5);			
			if (FL::RenderButton("Remove Property"))
			{
				L_RemoveKeyFrame(node_clicked);
			}
		}

		if (objectForFocusedAnimation != nullptr)
		{
			// Animate the focused object
			Animation* animation = objectForFocusedAnimation->GetAnimation();
			std::string playID = "##playAnimationPreview";
			std::string stopID = "##StopGameloopIcon";
			bool _isPreviewing = false;
			
			if (animation != nullptr)
				_isPreviewing = animation->IsPlaying();

			// Play Button
			if (_isPreviewing)
			{
				ImGui::BeginDisabled();
				FL::RenderImageButton(playID.c_str(), FL::GetTexture("play"), Vector2(14, 14), 0, FL::GetColor("button"), FL::GetColor("white"), FL::GetColor("buttonHovered"), FL::GetColor("buttonActive"));
				ImGui::EndDisabled();
			}
			else
			{
				if (FL::RenderImageButton(playID.c_str(), FL::GetTexture("play"), Vector2(14, 14), 0, FL::GetColor("button"), FL::GetColor("white"), FL::GetColor("buttonHovered"), FL::GetColor("buttonActive")))
				{
					if (animation != nullptr)
					{
						previewAnimationStartTime = FL::GetEllapsedGameTimeInMs();
						previewAnimationTime = FL::GetEllapsedGameTimeInMs();
						animation->Play(previewAnimationStartTime);
						_isPreviewing = true;
					}
				}
			}

			ImGui::SameLine(0, 5);

			// Stop button
			if (!_isPreviewing)
			{
				ImGui::BeginDisabled();
				FL::RenderImageButton(stopID.c_str(), FL::GetTexture("stop"), Vector2(14, 14), 0, FL::GetColor("button"), FL::GetColor("white"), FL::GetColor("buttonHovered"), FL::GetColor("buttonActive"));
				ImGui::EndDisabled();
			}
			else
			{
				if (FL::RenderImageButton(stopID.c_str(), FL::GetTexture("stop"), Vector2(14, 14), 0, FL::GetColor("button"), FL::GetColor("white"), FL::GetColor("buttonHovered"), FL::GetColor("buttonActive")))
				{
					animation->Stop();
					_isPreviewing = false;
					previewAnimationTime = 0;
				}
			}

			ImGui::SameLine(0, 5);

			ImGui::PushStyleColor(ImGuiCol_Text, lighter);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 4);
			ImGui::Text("Preview Animation");
			ImGui::PopStyleColor();
		}

			ImGui::EndChild(); // Properties Header

		static float animatorGridStep = 50;
		// Using InvisibleButton() as a convenience 1) it will advance the layout cursor and 2) allows us to use IsItemHovered()/IsItemActive()
		Vector2 canvas_p0 = ImGui::GetCursorScreenPos();
		Vector2 canvas_sz = ImGui::GetContentRegionAvail();
		if (canvas_sz.x < 50.0f) canvas_sz.x = 50.0f;
		if (canvas_sz.y < 50.0f) canvas_sz.y = 50.0f;
		Vector2 canvas_p1 = Vector2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);
		static Vector2 scrolling = Vector2(0, 0);
		ImGuiIO& inputOutput = ImGui::GetIO();
		
		// This will catch our interactions
		// 4096 is ImGuiButtonFlags_AllowOverlap but it's not working here for some reason
		ImGui::InvisibleButton("Animator Timeline", canvas_sz, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight | 4096);
		const bool is_hovered = ImGui::IsItemHovered(); // Hovered
		const bool is_active = ImGui::IsItemActive();   // Held

		// For panning
		const float mouse_threshold_for_pan = 0.0f;
		if (is_active && ImGui::IsMouseDragging(ImGuiMouseButton_Right, mouse_threshold_for_pan))
		{
			scrolling.x += inputOutput.MouseDelta.x;
			scrolling.y += inputOutput.MouseDelta.y;
		}

		Vector2 mousePos = Vector2(inputOutput.MousePos.x, inputOutput.MousePos.y);
		float scrollInput = inputOutput.MouseWheel;
		float weight = 0.08f;

		if (is_hovered)
		{
			if (scrollInput > 0)
			{
				animatorGridStep += 10;
			}
			else if (scrollInput < 0 && animatorGridStep > 2)
			{
				animatorGridStep -= 10;
			}
		}

		if (scrolling.x > 0)
			scrolling.x = 0;
		if (scrolling.y > 0)
			scrolling.y = 0;
		if (scrolling.y < -1500)
			scrolling.y = -1500;

		Vector2 zeroPoint = Vector2(0, 0);

		// Render the Timeline Grid
		RenderAnimationTimelineGrid(zeroPoint, scrolling, canvas_p0, canvas_p1, canvas_sz, animatorGridStep);

		// Get all keyFramePip positions
		float propertyYPos = -0.5f; // Value in grid space
		int propertyCounter = 0;    // For values in screenspace below
		float animationLength = 0.0f;
		if (animProps != nullptr)
			animationLength = animProps->animationLength;
		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		// Lambda
		auto L_RenderPropertyInTimeline = [&](std::string property, std::vector<float> keyFrameTimes, Vector4 rectColor)
		{
			float topYPos = canvas_p0.y + scrolling.y + (propertyCounter * animatorGridStep);
			float bottomYPos = topYPos + animatorGridStep;
			ImU32 color;
			if (node_clicked == property)
				color = IM_COL32(rectColor.x, rectColor.y, rectColor.z, rectColor.w);
			else
				color = IM_COL32(rectColor.x, rectColor.y, rectColor.z, 20);
		
			// prevents being drawn off screen and introducing scrollbar
			if (topYPos < canvas_p0.y)
				topYPos = canvas_p0.y;
			if (bottomYPos < canvas_p0.y)
				bottomYPos = canvas_p0.y;

			if (topYPos > canvas_p1.y)
				topYPos = canvas_p1.y;
			if (bottomYPos > canvas_p1.y)
				bottomYPos = canvas_p1.y;

			Vector2 topLeftCorner = Vector2(canvas_p0.x, topYPos);
			Vector2 bottomRightCorner = Vector2(canvas_p1.x, bottomYPos);
			draw_list->AddRectFilled(topLeftCorner, bottomRightCorner, color);
		};
		// Lambda
		auto L_RenderAnimationTimelineKeyFrames = [](std::shared_ptr<Animation::S_Property> keyFrame, int counter, Vector2& pipPosition, Vector2 zeroPoint, Vector2 scrolling, Vector2 canvas_p0, Vector2 canvas_p1, Vector2 canvas_sz, float gridStep)
		{
			std::string ID = keyFrame->name;
			ImDrawList* draw_list = ImGui::GetWindowDrawList();
			float spriteTextureWidth = 10;
			float spriteTextureHeight = 10;
			Vector2 spriteOffset = Vector2(5, 5);
			bool _spriteScalesWithZoom = false;
			int renderOrder = 1;

			// If there is a valid Icon.GetTexture() loaded into the Sprite Component
			if (FL::GetTexture("keyFrame") != nullptr)
			{
				Vector2 pipStartingPoint = FL::AddImageToDrawList(FL::GetTexture("keyFrame"), pipPosition, zeroPoint, 12, 12, Vector2(6, 6), Vector2(1, 1), _spriteScalesWithZoom, animatorGridStep, draw_list);

				ImGui::SetCursorScreenPos(pipStartingPoint);
				std::string pipID = ID + std::to_string(counter) + "-KeyFramePip";
				ImGui::InvisibleButton(pipID.c_str(), Vector2(12, 12), ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight | 4096);
				const bool _isClicked = ImGui::IsItemClicked();
				const bool _isHovered = ImGui::IsItemHovered();
				const bool _isActive = ImGui::IsItemActive();   // Held

				if (_isHovered)
					ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_Hand);
	
				if (_isActive || _isHovered)
				{
					// Mouse Hover Tooltip - Mouse Over Tooltip
					std::string keyTimeText = "Time: " + std::to_string(keyFrame->time / 1000) + " sec";
					Vector2 m = ImGui::GetIO().MousePos;
					ImGui::SetNextWindowPos(Vector2(m.x + 15, m.y + 5));
					ImGui::Begin("1", NULL, ImGuiWindowFlags_Tooltip | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar);
					ImGui::Text(keyTimeText.c_str());
					ImGui::End();
				}
				
				if (_isActive && ImGui::IsMouseDragging(ImGuiMouseButton_Left, 0))
				{
					ImGuiIO& inputOutput = ImGui::GetIO();
					if (keyFrame->time + inputOutput.MouseDelta.x / animatorGridStep * 1000 >= 0)
						keyFrame->time += inputOutput.MouseDelta.x / animatorGridStep * 1000;
				}

				if (_isClicked)
				{
					node_clicked = ID;
					selectedKeyFrameToEdit = keyFrame;
				}	
			}
		};

		Vector4 rectColor;
		int IDCounter = 0;

		// Draw colored box for transform keyframes
		if (animProps != nullptr)
		{
			if (animProps->eventProps.size() > 0)
			{
				rectColor = Vector4(255, 255, 255, 100);
				std::vector<float> keyFrameTimes = std::vector<float>();
				L_RenderPropertyInTimeline("Event", keyFrameTimes, rectColor);

				for (std::shared_ptr<Animation::S_Event> frame : animProps->eventProps)
				{					
					std::string ID = "Event";
					// Get keyFrame time and convert to seconds
					float keyFrameX = frame->time / 1000;
					Vector2 keyFramePos = Vector2(keyFrameX, propertyYPos);					
					L_RenderAnimationTimelineKeyFrames(frame, IDCounter, keyFramePos, zeroPoint, scrolling, canvas_p0, canvas_p1, canvas_sz, animatorGridStep);
					IDCounter++;
				}
				propertyYPos--;
				propertyCounter++;
			}
			if (animProps->transformProps.size() > 0)
			{
				rectColor = Vector4(214, 8, 118, 100);
				std::vector<float> keyFrameTimes = std::vector<float>();
				L_RenderPropertyInTimeline("Transform", keyFrameTimes, rectColor);

				for (std::shared_ptr<Animation::S_Transform> frame : animProps->transformProps)
				{
					std::string ID = "Transform";					
					float keyFrameX = frame->time / 1000;
					Vector2 keyFramePos = Vector2(keyFrameX, propertyYPos);					
					L_RenderAnimationTimelineKeyFrames(frame, IDCounter, keyFramePos, zeroPoint, scrolling, canvas_p0, canvas_p1, canvas_sz, animatorGridStep);
					IDCounter++;
				}
				propertyYPos--;
				propertyCounter++;
			}
			if (animProps->spriteProps.size() > 0)
			{
				rectColor = Vector4(83, 214, 8, 100);
				std::vector<float> keyFrameTimes = std::vector<float>();
				L_RenderPropertyInTimeline("Sprite", keyFrameTimes, rectColor);

				for (std::shared_ptr<Animation::S_Sprite> frame : animProps->spriteProps)
				{
					std::string ID = "Sprite";					
					float keyFrameX = frame->time / 1000;
					Vector2 keyFramePos = Vector2(keyFrameX, propertyYPos);
					if (zeroPoint.y + (propertyYPos * animatorGridStep * -1) < canvas_p1.y && zeroPoint.y + (propertyYPos * animatorGridStep * -1) + 6 < canvas_p1.y && zeroPoint.y + (propertyYPos * animatorGridStep * -1) > canvas_p0.y)
						L_RenderAnimationTimelineKeyFrames(frame, IDCounter, keyFramePos, zeroPoint, scrolling, canvas_p0, canvas_p1, canvas_sz, animatorGridStep);
					IDCounter++;
				}
				propertyYPos--;
				propertyCounter++;
			}
			if (animProps->cameraProps.size() > 0)
			{
				rectColor = Vector4(206, 108, 4, 100);
				std::vector<float> keyFrameTimes = std::vector<float>();
				L_RenderPropertyInTimeline("Camera", keyFrameTimes, rectColor);

				for (std::shared_ptr<Animation::S_Camera> frame : animProps->cameraProps)
				{
					std::string ID = "Camera";					
					float keyFrameX = frame->time / 1000;
					Vector2 keyFramePos = Vector2(keyFrameX, propertyYPos);
					if (zeroPoint.y + (propertyYPos * animatorGridStep * -1) < canvas_p1.y && zeroPoint.y + (propertyYPos * animatorGridStep * -1) + 6 < canvas_p1.y && zeroPoint.y + (propertyYPos * animatorGridStep * -1) > canvas_p0.y)
						L_RenderAnimationTimelineKeyFrames(frame, IDCounter, keyFramePos, zeroPoint, scrolling, canvas_p0, canvas_p1, canvas_sz, animatorGridStep);
					IDCounter++;
				}
				propertyYPos--;
				propertyCounter++;
			}
			if (animProps->scriptProps.size() > 0)
			{
				rectColor = Vector4(4, 159, 206, 100);
				std::vector<float> keyFrameTimes = std::vector<float>();
				L_RenderPropertyInTimeline("Script", keyFrameTimes, rectColor);

				for (std::shared_ptr<Animation::S_Script> frame : animProps->scriptProps)
				{
					std::string ID = "Script";					
					float keyFrameX = frame->time / 1000;
					Vector2 keyFramePos = Vector2(keyFrameX, propertyYPos);
					if (zeroPoint.y + (propertyYPos * animatorGridStep * -1) < canvas_p1.y && zeroPoint.y + (propertyYPos * animatorGridStep * -1) + 6 < canvas_p1.y && zeroPoint.y + (propertyYPos * animatorGridStep * -1) > canvas_p0.y)
						L_RenderAnimationTimelineKeyFrames(frame, IDCounter, keyFramePos, zeroPoint, scrolling, canvas_p0, canvas_p1, canvas_sz, animatorGridStep);
					IDCounter++;
				}
				propertyYPos--;
				propertyCounter++;
			}
			if (animProps->buttonProps.size() > 0)
			{
				rectColor = Vector4(152, 16, 198, 100);
				std::vector<float> keyFrameTimes = std::vector<float>();
				L_RenderPropertyInTimeline("Button", keyFrameTimes, rectColor);

				for (std::shared_ptr<Animation::S_Button> frame : animProps->buttonProps)
				{
					std::string ID = "Button";					
					float keyFrameX = frame->time / 1000;
					Vector2 keyFramePos = Vector2(keyFrameX, propertyYPos);
					if (zeroPoint.y + (propertyYPos * animatorGridStep * -1) < canvas_p1.y && zeroPoint.y + (propertyYPos * animatorGridStep * -1) + 6 < canvas_p1.y && zeroPoint.y + (propertyYPos * animatorGridStep * -1) > canvas_p0.y)
						L_RenderAnimationTimelineKeyFrames(frame, IDCounter, keyFramePos, zeroPoint, scrolling, canvas_p0, canvas_p1, canvas_sz, animatorGridStep);
					IDCounter++;
				}
				propertyYPos--;
				propertyCounter++;
			}
			if (animProps->canvasProps.size() > 0)
			{
				rectColor = Vector4(224, 81, 15, 100);
				std::vector<float> keyFrameTimes = std::vector<float>();
				L_RenderPropertyInTimeline("Canvas", keyFrameTimes, rectColor);

				for (std::shared_ptr<Animation::S_Canvas> frame : animProps->canvasProps)
				{
					std::string ID = "Canvas";					
					float keyFrameX = frame->time / 1000;
					Vector2 keyFramePos = Vector2(keyFrameX, propertyYPos);
					if (zeroPoint.y + (propertyYPos * animatorGridStep * -1) < canvas_p1.y && zeroPoint.y + (propertyYPos * animatorGridStep * -1) + 6 < canvas_p1.y && zeroPoint.y + (propertyYPos * animatorGridStep * -1) > canvas_p0.y)
						L_RenderAnimationTimelineKeyFrames(frame, IDCounter, keyFramePos, zeroPoint, scrolling, canvas_p0, canvas_p1, canvas_sz, animatorGridStep);
					IDCounter++;
				}
				propertyYPos--;
				propertyCounter++;
			}
			if (animProps->audioProps.size() > 0)
			{
				rectColor = Vector4(237, 244, 14, 100);
				std::vector<float> keyFrameTimes = std::vector<float>();
				L_RenderPropertyInTimeline("Audio", keyFrameTimes, rectColor);

				for (std::shared_ptr<Animation::S_Audio> frame : animProps->audioProps)
				{
					std::string ID = "Audio";					
					float keyFrameX = frame->time / 1000;
					Vector2 keyFramePos = Vector2(keyFrameX, propertyYPos);
					if (zeroPoint.y + (propertyYPos * animatorGridStep * -1) < canvas_p1.y && zeroPoint.y + (propertyYPos * animatorGridStep * -1) + 6 < canvas_p1.y && zeroPoint.y + (propertyYPos * animatorGridStep * -1) > canvas_p0.y)
						L_RenderAnimationTimelineKeyFrames(frame, IDCounter, keyFramePos, zeroPoint, scrolling, canvas_p0, canvas_p1, canvas_sz, animatorGridStep);
					IDCounter++;
				}
				propertyYPos--;
				propertyCounter++;
			}
			if (animProps->textProps.size() > 0)
			{
				rectColor = Vector4(15, 224, 200, 100);
				std::vector<float> keyFrameTimes = std::vector<float>();
				L_RenderPropertyInTimeline("Text", keyFrameTimes, rectColor);

				for (std::shared_ptr<Animation::S_Text> frame : animProps->textProps)
				{
					std::string ID = "Text";					
					float keyFrameX = frame->time / 1000;
					Vector2 keyFramePos = Vector2(keyFrameX, propertyYPos);
					if (zeroPoint.y + (propertyYPos * animatorGridStep * -1) < canvas_p1.y && zeroPoint.y + (propertyYPos * animatorGridStep * -1) + 6 < canvas_p1.y && zeroPoint.y + (propertyYPos * animatorGridStep * -1) > canvas_p0.y)
						L_RenderAnimationTimelineKeyFrames(frame, IDCounter, keyFramePos, zeroPoint, scrolling, canvas_p0, canvas_p1, canvas_sz, animatorGridStep);
					IDCounter++;
				}
				propertyYPos--;
				propertyCounter++;
			}
			if (animProps->boxColliderProps.size() > 0)
			{
				rectColor = Vector4(224, 158, 15, 100);
				std::vector<float> keyFrameTimes = std::vector<float>();
				L_RenderPropertyInTimeline("BoxCollider", keyFrameTimes, rectColor);

				for (std::shared_ptr<Animation::S_BoxCollider> frame : animProps->boxColliderProps)
				{
					std::string ID = "BoxCollider";					
					float keyFrameX = frame->time / 1000;
					Vector2 keyFramePos = Vector2(keyFrameX, propertyYPos);
					if (zeroPoint.y + (propertyYPos * animatorGridStep * -1) < canvas_p1.y && zeroPoint.y + (propertyYPos * animatorGridStep * -1) + 6 < canvas_p1.y && zeroPoint.y + (propertyYPos * animatorGridStep * -1) > canvas_p0.y)
						L_RenderAnimationTimelineKeyFrames(frame, IDCounter, keyFramePos, zeroPoint, scrolling, canvas_p0, canvas_p1, canvas_sz, animatorGridStep);
					IDCounter++;
				}
				propertyYPos--;
				propertyCounter++;
			}
			if (animProps->circleColliderProps.size() > 0)
			{
				rectColor = Vector4(11, 42, 183, 100);
				std::vector<float> keyFrameTimes = std::vector<float>();
				L_RenderPropertyInTimeline("CircleCollider", keyFrameTimes, rectColor);

				for (std::shared_ptr<Animation::S_CircleCollider> frame : animProps->circleColliderProps)
				{
					std::string ID = "CircleCollider";					
					float keyFrameX = frame->time / 1000;
					Vector2 keyFramePos = Vector2(keyFrameX, propertyYPos);
					if (zeroPoint.y + (propertyYPos * animatorGridStep * -1) < canvas_p1.y && zeroPoint.y + (propertyYPos * animatorGridStep * -1) + 6 < canvas_p1.y && zeroPoint.y + (propertyYPos * animatorGridStep * -1) > canvas_p0.y)
						L_RenderAnimationTimelineKeyFrames(frame, IDCounter, keyFramePos, zeroPoint, scrolling, canvas_p0, canvas_p1, canvas_sz, animatorGridStep);
					IDCounter++;
				}
				propertyYPos--;
				propertyCounter++;
			}
			if (animProps->rigidBodyProps.size() > 0)
			{
				rectColor = Vector4(166, 11, 183, 100);
				std::vector<float> keyFrameTimes = std::vector<float>();
				L_RenderPropertyInTimeline("RigidBody", keyFrameTimes, rectColor);

				for (std::shared_ptr<Animation::S_RigidBody> frame : animProps->rigidBodyProps)
				{
					std::string ID = "RigidBody";					
					float keyFrameX = frame->time / 1000;
					Vector2 keyFramePos = Vector2(keyFrameX, propertyYPos);
					if (zeroPoint.y + (propertyYPos * animatorGridStep * -1) < canvas_p1.y && zeroPoint.y + (propertyYPos * animatorGridStep * -1) + 6 < canvas_p1.y && zeroPoint.y + (propertyYPos * animatorGridStep * -1) > canvas_p0.y)
						L_RenderAnimationTimelineKeyFrames(frame, IDCounter, keyFramePos, zeroPoint, scrolling, canvas_p0, canvas_p1, canvas_sz, animatorGridStep);
					IDCounter++;
				}
				propertyYPos--;
				propertyCounter++;
			}
			if (animProps->characterControllerProps.size() > 0)
			{
				rectColor = Vector4(85, 183, 11, 100);
				std::vector<float> keyFrameTimes = std::vector<float>();
				L_RenderPropertyInTimeline("CharacterController", keyFrameTimes, rectColor);

				for (std::shared_ptr<Animation::S_CharacterController> frame : animProps->characterControllerProps)
				{
					std::string ID = "CharacterController";					
					float keyFrameX = frame->time / 1000;
					Vector2 keyFramePos = Vector2(keyFrameX, propertyYPos);
					if (zeroPoint.y + (propertyYPos * animatorGridStep * -1) < canvas_p1.y && zeroPoint.y + (propertyYPos * animatorGridStep * -1) + 6 < canvas_p1.y && zeroPoint.y + (propertyYPos * animatorGridStep * -1) > canvas_p0.y)
						L_RenderAnimationTimelineKeyFrames(frame, IDCounter, keyFramePos, zeroPoint, scrolling, canvas_p0, canvas_p1, canvas_sz, animatorGridStep);
					IDCounter++;
				}
				propertyYPos--;
				propertyCounter++;
			}
		}

		ImGui::EndChild(); // Animation Timeline
		ImGui::End(); // Animator
	}

	void RenderAnimationPreview()
	{
		static Vector2 step = Vector2(50, 50);

		FL::BeginWindow("Animator Preview", FG_b_showAnimationPreview);

		Vector2 canvas_p0 = ImGui::GetCursorScreenPos();
		Vector2 canvas_sz = ImGui::GetContentRegionAvail();
		if (canvas_sz.x < 50.0f) canvas_sz.x = 50.0f;
		if (canvas_sz.y < 50.0f) canvas_sz.y = 50.0f;
		Vector2 canvas_p1 = Vector2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);
		static Vector2 scrolling = Vector2(0, 0);
		static Vector2 viewPortDimensions = Vector2(0, 0);
		static bool _firstRenderPassDone = false;
		static bool _viewportSizeTaken = false;

		// Set initial viewport dimensions once
		if (_firstRenderPassDone)
		{
			if (!_viewportSizeTaken)
			{
				viewPortDimensions = Vector2(canvas_sz.x, canvas_sz.y);
			}
			_viewportSizeTaken = true;
		}
		_firstRenderPassDone = true;

		// Get Input and Output
		ImGuiIO& inputOutput = ImGui::GetIO();

		// This will catch our interactions
		ImGui::InvisibleButton("AnimationPreview", canvas_sz, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
		const bool is_hovered = ImGui::IsItemHovered(); // Hovered
		const bool is_active = ImGui::IsItemActive();   // Held

		// For panning the scene view
		const float mouse_threshold_for_pan = 0.0f;
		if (is_active && ImGui::IsMouseDragging(ImGuiMouseButton_Right, mouse_threshold_for_pan))
		{
			scrolling.x += inputOutput.MouseDelta.x;
			scrolling.y += inputOutput.MouseDelta.y;
		}
		
		Vector2 centerPoint = Vector2(0, 0);

		bool _weightedScroll = false;
		RenderGridView(centerPoint, scrolling, _weightedScroll, canvas_p0, canvas_p1, canvas_sz, step, Vector2(viewPortDimensions.x / 2, viewPortDimensions.y / 2));


		if (objectForFocusedAnimation != nullptr)
		{
			std::vector<GameObject> focusedObjectVector;
			focusedObjectVector.push_back(*objectForFocusedAnimation);
	
			// Animate the focused object
			if (_playPreviewAnimation)
			{				
				Animation* animation = objectForFocusedAnimation->GetAnimation();

				// If animation component is playing, play the animation
				if (animation != nullptr && animation->IsPlaying())
				{
					previewAnimationTime = FL::GetEngineTime();
					animation->PlayAnimation(previewAnimationTime);
				}
			}

			RenderViewObjects(focusedObjectVector, centerPoint, canvas_p0, canvas_sz, step.x);
		}

		FL::EndWindow();
	}

	void RenderAnimationTimelineGrid(Vector2& zeroPoint, Vector2 scrolling, Vector2 canvas_p0, Vector2 canvas_p1, Vector2 canvas_sz, float gridStep)
	{
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		draw_list->AddRectFilled(canvas_p0, canvas_p1, FL::GetColor32("timelineGridBg"));
		zeroPoint = Vector2(scrolling.x + canvas_p0.x, canvas_p0.y + scrolling.y);

		// Draw vertical grid lines
		for (float x = trunc(fmodf(zeroPoint.x, gridStep)); x < canvas_p0.x + canvas_sz.x; x += gridStep)
		{
			FL::DrawLine(Vector2(x, canvas_p0.y), Vector2(x, canvas_p1.y), dark, 1.0f, draw_list);
		}
		for (float x = trunc(fmodf(zeroPoint.x, gridStep * 2)); x < canvas_p0.x + canvas_sz.x; x += gridStep * 2)
		{
			FL::DrawLine(Vector2(x, canvas_p0.y), Vector2(x, canvas_p1.y), light, 1.0f, draw_list);
		}
		// Draw horizontal grid lines
		for (float y = trunc(fmodf(zeroPoint.y, gridStep)); y < canvas_p0.y + canvas_sz.y; y += gridStep / 2)
		{
			if (y > canvas_p0.y)
				FL::DrawLine(Vector2(canvas_p0.x, y), Vector2(canvas_p1.x, y), dark, 1.0f, draw_list);
		}
	}

	void RenderKeyFrameEditor()
	{
		FL::BeginWindow("Keyframe Editor", FG_b_showKeyFrameEditor);

		std::string keyFrameProperty = "No KeyFrame Selected";
		if (selectedKeyFrameToEdit != nullptr)
			keyFrameProperty = selectedKeyFrameToEdit->name;

		FL::RenderSectionHeader(keyFrameProperty);

		if (selectedKeyFrameToEdit != nullptr)
		{
			if (selectedKeyFrameToEdit->name == "Event")
			{
				std::shared_ptr<Animation::S_Event> event = std::static_pointer_cast<Animation::S_Event>(selectedKeyFrameToEdit);
				std::string functionName = event->functionName;
				if (FL::RenderInput("AnimationEventName", "Function Name", functionName))
					event->functionName = functionName;
			}
			else if (selectedKeyFrameToEdit->name == "Transform")
			{
				std::shared_ptr<Animation::S_Transform> transform = std::static_pointer_cast<Animation::S_Transform>(selectedKeyFrameToEdit);
				float xPos = transform->xPos;
				float yPos = transform->yPos;
				float xScale = transform->xScale;
				float yScale = transform->yScale;
				static ImGuiSliderFlags flags = ImGuiSliderFlags_::ImGuiSliderFlags_None;

				ImGui::PushItemWidth(ImGui::GetContentRegionMax().x / 3 - 5);

				ImGui::Text("xPos:");
				ImGui::SameLine(ImGui::GetContentRegionMax().x / 2 + 5, 0);
				ImGui::Text("yPos:");

				ImGui::DragFloat("##xPos", &xPos, 0.5f, -FLT_MAX, -FLT_MAX, "%.3f", flags);
				if (ImGui::IsItemHovered())
					ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_ResizeEW);
				ImGui::SameLine(0, 5);
				ImGui::DragFloat("##yPos", &yPos, 0.5f, -FLT_MAX, -FLT_MAX, "%.3f", flags);

				if (ImGui::IsItemHovered())
					ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_ResizeEW);
				//ImGui::SameLine(0, 5);
				//ImGui::DragFloat("##rotation", &rotation, 1.0f, -360, 360, "%.3f", flags);
				// Set cursor type
				if (ImGui::IsItemHovered())
					ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_ResizeEW);

				transform->xPos = xPos;
				transform->yPos = yPos;

				ImGui::PushItemWidth(ImGui::GetContentRegionMax().x / 2 - 5);
				ImGui::Text("Scale x:");
				ImGui::SameLine(ImGui::GetContentRegionMax().x / 2 + 5, 0);
				ImGui::Text("Scale y:");

				ImGui::DragFloat("##xScale", &xScale, 0.05f, 0, -FLT_MAX, "%.3f", flags);

				if (ImGui::IsItemHovered())
					ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_ResizeEW);
				ImGui::SameLine(0, 5);
				ImGui::DragFloat("##yScale", &yScale, 0.05f, -FLT_MAX, -FLT_MAX, "%.3f", flags);

				if (ImGui::IsItemHovered())
					ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_ResizeEW);
				ImGui::PopItemWidth();

				transform->xScale = xScale;
				transform->yScale = yScale;

				const char* interpType[] = { "- Select -", "Lerp", "Slerp" };
				static int current_type = 0;
				static std::string node_clicked = "";
				static std::string selected_property = "";
				
				if (transform->transformInterpType == Animation::I_Lerp)
					current_type = 1;
				else if (transform->transformInterpType == Animation::I_Slerp)
					current_type = 2;

				ImGui::Separator();
				ImGui::Text("Interpolation Type");
				if (ImGui::BeginCombo("##interpolationType", interpType[current_type]))
				{
					for (int n = 0; n < IM_ARRAYSIZE(interpType); n++)
					{
						bool is_selected = (interpType[current_type] == interpType[n]);
						if (ImGui::Selectable(interpType[n], is_selected))
						{
							current_type = n;
							if (interpType[n] == "Lerp")
								transform->transformInterpType = Animation::I_Lerp;
							else if (interpType[n] == "Slerp")
								transform->transformInterpType = Animation::I_Slerp;
						}
							
						if (is_selected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}
			}
			else if (selectedKeyFrameToEdit->name == "Sprite")
			{
				std::shared_ptr<Animation::S_Sprite> sprite = std::static_pointer_cast<Animation::S_Sprite>(selectedKeyFrameToEdit);
				std::string path = sprite->path;
				float xOffset = sprite->xOffset;
				float yOffset = sprite->yOffset;
				ImGuiSliderFlags flags = ImGuiSliderFlags_::ImGuiSliderFlags_None;
				Vector4 tintColor = sprite->tintColor;
				bool _instantlyChangeTint = sprite->_instantTintChange;
				
				int droppedValue = -1;
				std::string openedPath = "";
				if (FL::DropInputCanOpenFiles("##spritePathKeyFrameEditor", "Path", path, FL::F_fileExplorerTarget, droppedValue, openedPath, "Drop image files here from the File Explorer"))
				{
					if (openedPath != "")
					{
						sprite->path = openedPath;
					}
					else if (droppedValue != -1)
					{
						std::filesystem::path fs_path(FL::F_selectedFiles[droppedValue - 1]);
						if (fs_path.extension() == ".png")
						{
							sprite->path = fs_path.string();
							Texture texture = Texture();
							texture.LoadFromFile(fs_path.string());
							if (texture.GetTexture() != nullptr)
							{
								sprite->xOffset = (float)(texture.GetWidth() / 2);
								sprite->yOffset = (float)(texture.GetHeight() / 2);
							}
						}
						else
						{
							FL::LogError("File must be of type .png to drop here.");
						}
					}
				}

				// Render Table
				if (FL::PushTable("##AnimatedSpriteProperties", 2))
				{
					if (FL::RenderFloatDragTableRow("##AnimatedxSpriteOffsetDrag", "X Offset", xOffset, 0.1f, -FLT_MAX, -FLT_MAX))
						sprite->xOffset = xOffset;
					if (FL::RenderFloatDragTableRow("##AnimatedySpriteOffsetDrag", "Y Offset", yOffset, 0.1f, -FLT_MAX, -FLT_MAX))
						sprite->yOffset = yOffset;
					FL::PopTable();
				}

				// Tint color picker
				std::string tintID = "##AnimationFrameSpriteTintColorPicker";
				ImVec4 color = ImVec4(tintColor.x * 255.0f, tintColor.y * 255.0f, tintColor.z * 255.0f, tintColor.w * 255.0f);
				if (ImGui::ColorEdit4(tintID.c_str(), (float*)&tintColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel))
				{
					sprite->tintColor = tintColor;
				}
				ImGui::SameLine(0, 5);
				ImGui::Text("Tint color");

				if (FL::RenderCheckbox("Instantly change tint", _instantlyChangeTint))
					sprite->_instantTintChange = _instantlyChangeTint;
			}
		}
		else
		{	
			ImGui::TextWrapped("Select a keyframe to edit from the Animation Timeline...");
		}

		FL::EndWindow();
	}

	void SetFocusedAnimation(std::shared_ptr<FL::Animation::S_AnimationProperties> animation)
	{
		FG_FocusedAnimation = animation;
	}

	std::shared_ptr<FL::Animation::S_AnimationProperties> GetFocusedAnimation()
	{
		return FG_FocusedAnimation;
	}
}