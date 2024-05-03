#include "FlatEngine.h"
#include "Camera.h"
#include "Transform.h"
#include "Sprite.h"
#include "imgui.h"
#include "Text.h"
#include "math.h"

namespace FlatEngine { namespace FlatGui {

	ImVec4 darker = ImVec4(float(0.2), float(0.2), float(0.2), float(1));
	ImVec4 lighter = ImVec4(float(0.8), float(0.8), float(0.8), float(1));
	ImVec4 light = ImVec4(float(0.7), float(0.7), float(0.7), float(1));
	ImVec4 dark = ImVec4(float(0.3), float(0.3), float(0.3), float(1));
	ImVec4 white = ImVec4(float(0.9), float(0.9), float(0.9), float(1));
	ImVec4 transformAnimationNode = ImVec4(float(0.1), float(0.76), float(0.08), float(.8));
	ImVec4 scrubberBackground = ImVec4(float(0.32), float(0.22), float(0.19), float(1));
	ImVec4 scrubberBackgroundDark = ImVec4(float(0.12), float(0.02), float(0.0), float(1));


	void RenderAnimator()
	{
		ImGuiChildFlags padding_child_flags = ImGuiChildFlags_::ImGuiChildFlags_AlwaysUseWindowPadding;

		PushWindowStyles();
		// 16 | 8 are flags for noScrollbar and noscrollwithmouse
		ImGui::Begin("Animator", 0, 16 | 8);
		PopWindowStyles();

		// Animated Properties BeginChild()
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ChildBg, outerWindowColor);
		ImGui::BeginChild("Animated Properties", ImVec2(ImGui::GetContentRegionMax().x / 3, 0), padding_child_flags | ImGuiChildFlags_ResizeX);
		ImGui::PopStyleColor();

		std::string animationName = "-No Animation Selected-";
		if (GetFocusedAnimation()->animationName != "")
			animationName = GetFocusedAnimation()->animationName;

		ImGui::PushStyleColor(ImGuiCol_ChildBg, white);
		ImGuiChildFlags child_flags = ImGuiChildFlags_::ImGuiChildFlags_AlwaysUseWindowPadding | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_::ImGuiChildFlags_AlwaysAutoResize;
		ImGui::BeginChild(animationName.c_str(), ImVec2(0, 0), child_flags);
		ImGui::PopStyleColor();
		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(1, 9, 0, 255));
		ImGui::Text(animationName.c_str());
		ImGui::PopStyleColor();
		ImGui::EndChild();

		ImGui::Separator();
		ImGui::PushStyleColor(ImGuiCol_ChildBg, innerWindowColor);
		ImGui::BeginChild("Select Animation", ImVec2(0, 0), child_flags);
		ImGui::PopStyleColor();

		ImGui::Text("Manage Animation:");

		static std::string animationFilePath = "";

		ImGui::SameLine(0, 15);
		if (RenderImageButton("##NewAnimationFile", newFileTexture))
		{
			std::string animationFilePath = OpenSaveFileExplorer();
			if (animationFilePath != "")
			{
				// Create S_AnimationProperties struct to store the properties of the json file in
				std::shared_ptr<Animation::S_AnimationProperties> animationProperties = std::make_shared<Animation::S_AnimationProperties>();
				animationProperties->animationName = "New Animation";

				CreateNewAnimationFile(animationFilePath);
				SaveAnimationFile(animationProperties, animationFilePath);
			}
		}
		ImGui::SameLine(0, 5);
		
		if (RenderImageButton("#OpenAnim", openFileTexture))
		{
			animationFilePath = OpenLoadFileExplorer();
			if (animationFilePath != "")
			{
				SetFocusedAnimation(LoadAnimationFile(animationFilePath));
				loadedProject->SetLoadedPreviewAnimationPath(animationFilePath);
			}
		}
		ImGui::SameLine(0, 5);
		if (RenderImageButton("#SaveAnimationFile", saveFileTexture))
		{
			//std::string animationFilePath = OpenSaveFileExplorer();
			//if (animationFilePath != "")
			//{
			//	SaveAnimationFile(GetFocusedAnimation(), animationFilePath);
			//}
			SaveAnimationFile(GetFocusedAnimation(), animationFilePath);
		}

		ImGui::EndChild(); // Select Animation

		std::shared_ptr<Animation::S_AnimationProperties> animProps = GetFocusedAnimation();

		const char* properties[] = { "Add Property", "Transform", "Sprite", "Camera", "Script", "Button", "Canvas", "Audio", "Text", "BoxCollider", "CircleCollider", "RigidBody", "CharacterController" };
		static int current_property = 0;
		static std::string node_clicked = "";

		// Lambda
		auto L_PushBackKeyFrame = [&](std::string property)
		{
			// Add property to animation object
			if (property == "Transform")
			{
				std::shared_ptr<Animation::S_Transform> transformProperties = std::make_shared<Animation::S_Transform>();
				transformProperties->name = "Transform";
				animProps->transformProperties.push_back(transformProperties);
			}
			else if (property == "Sprite")
			{
				std::shared_ptr<Animation::S_Sprite> spriteProperties = std::make_shared<Animation::S_Sprite>();
				spriteProperties->name = "Sprite";
				animProps->spriteProperties.push_back(spriteProperties);
			}
			else if (property == "Camera")
			{
				std::shared_ptr<Animation::S_Camera> cameraProperties = std::make_shared<Animation::S_Camera>();
				cameraProperties->name = "Camera";
				animProps->cameraProperties.push_back(cameraProperties);
			}
			else if (property == "Script")
			{
				std::shared_ptr<Animation::S_Script> scriptProperties = std::make_shared<Animation::S_Script>();
				scriptProperties->name = "Script";
				animProps->scriptProperties.push_back(scriptProperties);
			}
			else if (property == "Button")
			{
				std::shared_ptr<Animation::S_Button> buttonProperties = std::make_shared<Animation::S_Button>();
				buttonProperties->name = "Button";
				animProps->buttonProperties.push_back(buttonProperties);
			}
			else if (property == "Canvas")
			{
				std::shared_ptr<Animation::S_Canvas> canvasProperties = std::make_shared<Animation::S_Canvas>();
				canvasProperties->name = "Canvas";
				animProps->canvasProperties.push_back(canvasProperties);
			}
			else if (property == "Audio")
			{
				std::shared_ptr<Animation::S_Audio> audioProperties = std::make_shared<Animation::S_Audio>();
				audioProperties->name = "Audio";
				animProps->audioProperties.push_back(audioProperties);
			}
			else if (property == "Text")
			{
				std::shared_ptr<Animation::S_Text> textProperties = std::make_shared<Animation::S_Text>();
				textProperties->name = "Text";
				animProps->textProperties.push_back(textProperties);
			}
			else if (property == "BoxCollider")
			{
				std::shared_ptr<Animation::S_BoxCollider> boxColliderProperties = std::make_shared<Animation::S_BoxCollider>();
				boxColliderProperties->name = "BoxCollider";
				animProps->boxColliderProperties.push_back(boxColliderProperties);
			}
			else if (property == "CircleCollider")
			{
				std::shared_ptr<Animation::S_CircleCollider> circleColliderProperties = std::make_shared<Animation::S_CircleCollider>();
				circleColliderProperties->name = "CircleCollider";
				animProps->circleColliderProperties.push_back(circleColliderProperties);
			}
			else if (property == "RigidBody")
			{
				std::shared_ptr<Animation::S_RigidBody> rigidBodyProperties = std::make_shared<Animation::S_RigidBody>();
				rigidBodyProperties->name = "RigidBody";
				animProps->rigidBodyProperties.push_back(rigidBodyProperties);
			}
			else if (property == "CharacterController")
			{
				std::shared_ptr<Animation::S_CharacterController> characterControllerProperties = std::make_shared<Animation::S_CharacterController>();
				characterControllerProperties->name = "CharacterController";
				animProps->characterControllerProperties.push_back(characterControllerProperties);
			}
		};

		if (animProps != nullptr && animProps->animationName != "")
		{
			std::shared_ptr<Animation> animation = nullptr;
			if (objectForFocusedAnimation != nullptr)
				animation = objectForFocusedAnimation->GetAnimationComponent();

			if (RenderCheckbox("Loop Animation", animProps->_loop) && animation != nullptr && animation->IsPlaying())
			{
				animation->Stop();
				animation->Play(GetEngineTime());
			}

			static std::string selected_property = "";
			
			ImGui::Separator();
			ImGui::PushStyleColor(ImGuiCol_ChildBg, innerWindowColor);
			ImGui::BeginChild("Properties Bar", ImVec2(0, 0), child_flags);
			ImGui::PopStyleColor();
			ImGui::Text("Add Properties");
			std::vector<std::string> props = std::vector<std::string>();
			//ImGui::BeginCombo("##properties", &current_property, &props.front(), props.size());
			if (ImGui::BeginCombo("##properties", properties[current_property]))
			{
				for (int n = 0; n < IM_ARRAYSIZE(properties); n++)
				{
					if (animProps->transformProperties.size() == 0 && properties[n] == "Transform" ||
						animProps->spriteProperties.size() == 0 && properties[n] == "Sprite" ||
						animProps->cameraProperties.size() == 0 && properties[n] == "Camera" ||
						animProps->scriptProperties.size() == 0 && properties[n] == "Script" ||
						animProps->buttonProperties.size() == 0 && properties[n] == "Button" ||
						animProps->canvasProperties.size() == 0 && properties[n] == "Canvas" ||
						animProps->audioProperties.size() == 0 && properties[n] == "Audio" ||
						animProps->textProperties.size() == 0 && properties[n] == "Text" ||
						animProps->boxColliderProperties.size() == 0 && properties[n] == "BoxCollider" ||
						animProps->circleColliderProperties.size() == 0 && properties[n] == "CircleCollider" ||
						animProps->rigidBodyProperties.size() == 0 && properties[n] == "RigidBody" ||
						animProps->characterControllerProperties.size() == 0 && properties[n] == "CharacterController"
						)
					{
						bool is_selected = (properties[current_property] == properties[n]); // You can store your selection however you want, outside or inside your objects
						if (ImGui::Selectable(properties[n], is_selected))
							current_property = n;
						if (is_selected)
							ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
			ImGui::SameLine(0, 5);
			if (ImGui::Button("Add"))
			{
				L_PushBackKeyFrame(properties[current_property]);
				// Reset selector box to default
				current_property = 0;
			}
			ImGui::EndChild();

			// List properties on this animation
			ImGui::PushStyleColor(ImGuiCol_FrameBg, innerWindowColor);
			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0, -5));
			if (ImGui::BeginListBox("##AnimationProperties", ImVec2(-FLT_MIN, -FLT_MIN)))
			{
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, 0.0f });

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
							ImGui::TreeNodeEx((void*)(intptr_t)treeID.c_str(), node_flags, property.c_str());
							if (ImGui::IsItemClicked())
								node_clicked = property;
						}
					};

				RenderPropertyButton("Transform", (int)animProps->transformProperties.size(), node_clicked);
				RenderPropertyButton("Sprite", (int)animProps->spriteProperties.size(), node_clicked);
				RenderPropertyButton("Camera", (int)animProps->cameraProperties.size(), node_clicked);
				RenderPropertyButton("Script", (int)animProps->scriptProperties.size(), node_clicked);
				RenderPropertyButton("Button", (int)animProps->buttonProperties.size(), node_clicked);
				RenderPropertyButton("Canvas", (int)animProps->canvasProperties.size(), node_clicked);
				RenderPropertyButton("Audio", (int)animProps->audioProperties.size(), node_clicked);
				RenderPropertyButton("Text", (int)animProps->textProperties.size(), node_clicked);
				RenderPropertyButton("BoxCollider", (int)animProps->boxColliderProperties.size(), node_clicked);
				RenderPropertyButton("CircleCollider", (int)animProps->circleColliderProperties.size(), node_clicked);
				RenderPropertyButton("RigidBody", (int)animProps->rigidBodyProperties.size(), node_clicked);
				RenderPropertyButton("CharacterController", (int)animProps->characterControllerProperties.size(), node_clicked);

				ImGui::PopStyleVar();
				ImGui::EndListBox();
			}
			ImGui::PopStyleVar();
			ImGui::PopStyleColor();
		}

		ImGui::EndChild(); // Animator Properties EndChild()

		ImGui::SameLine(0, 5);

		// Timeline Events
		ImGui::PushStyleColor(ImGuiCol_ChildBg, innerWindowColor);
		ImGui::BeginChild("Timeline Events", ImVec2(0, 0), padding_child_flags);
		ImGui::PopStyleColor();

		ImGui::PushStyleColor(ImGuiCol_ChildBg, white);
		ImGui::BeginChild("Animation Timeline", ImVec2(0, 0), child_flags);
		ImGui::PopStyleColor();
		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(1, 9, 0, 255));
		ImGui::Text("Animation Timeline");
		ImGui::PopStyleColor();
		ImGui::EndChild();

		ImGui::PushStyleColor(ImGuiCol_ChildBg, singleItemColor);
		ImGui::BeginChild("Property Header", ImVec2(0, 60), child_flags);
		ImGui::PushStyleColor(ImGuiCol_Text, light);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3);
		ImGui::Text("Property Selected: ");
		ImGui::PopStyleColor();
		ImGui::SameLine(0, 5);
		ImGui::PushStyleColor(ImGuiCol_Text, white);

		ImGui::Text(node_clicked.c_str());
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		if (node_clicked != "")
		{
			ImGui::SameLine(0, 10);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 3);
			if (ImGui::Button("Add Keyframe##"))
				L_PushBackKeyFrame(node_clicked);
		}
		
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 4);

		if (objectForFocusedAnimation != nullptr)
		{
			// Animate the focused object
			std::shared_ptr<Animation> animation = objectForFocusedAnimation->GetAnimationComponent();
			std::string playID = "##playAnimationPreview";
			std::string stopID = "##StopGameloopIcon";
			bool _isPreviewing = animation->IsPlaying();

			// Play Button
			if (_isPreviewing)
			{
				ImGui::BeginDisabled();
				RenderImageButton(playID.c_str(), playTexture, ImVec2(14, 14));
				ImGui::EndDisabled();
			}
			else
			{
				if (RenderImageButton(playID.c_str(), playTexture, ImVec2(14, 14)))
				{
					if (animation != nullptr)
					{
						previewAnimationStartTime = GetEngineTime();
						previewAnimationTime = GetEngineTime();
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
				RenderImageButton(stopID.c_str(), stopTexture, ImVec2(16, 16));
				ImGui::EndDisabled();
			}
			else
			{
				if (RenderImageButton(stopID.c_str(), stopTexture, ImVec2(16, 16)))
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

		ImGui::EndChild();

		// Save zero point for rendering the scrubber slider
		ImVec2 scrubberZeroPoint = ImGui::GetCursorScreenPos();

		ImGui::PushStyleColor(ImGuiCol_ChildBg, scrubberBackground);
		ImGui::BeginChild("Timeline Scrubber", ImVec2(0,33), child_flags);
		ImGui::PopStyleColor();
		

		// Render animator scrubber
		static float scrubberTime = (float)previewAnimationTime / 1000;
		static Vector2 keyFramePos = Vector2(scrubberTime, -0.05f);
		static float animatorGridStep = 50;

		// Lambda
		auto L_RenderAnimationScrubber = [](Vector2& pipPosition, ImVec2 zeroPoint, float gridStep)
			{
				std::string ID = "TimelineScrubber";
				ImDrawList* draw_list = ImGui::GetWindowDrawList();
				float spriteTextureWidth = 14;
				float spriteTextureHeight = 26;
				Vector2 spriteOffset = Vector2(spriteTextureWidth / 2, 0);

				// If there is a valid Texture loaded into the Sprite Component
				if (keyFrameTexture != nullptr)
				{
					ImVec2 pipStartingPoint = AddImageToDrawList(timelineScrubberTexture, pipPosition, zeroPoint, spriteTextureWidth, spriteTextureHeight, spriteOffset, Vector2(1, 1), false, gridStep, draw_list);					
					ImGui::SetCursorScreenPos(pipStartingPoint);
					ImGui::InvisibleButton(ID.c_str(), ImVec2(spriteTextureWidth, spriteTextureHeight), ImGuiButtonFlags_MouseButtonLeft | 4096);
					const bool _isClicked = ImGui::IsItemClicked();
					const bool _isHovered = ImGui::IsItemHovered();
					const bool _isActive = ImGui::IsItemActive();   // Held

					if (_isActive || _isHovered)
					{
						// Mouse Hover Tooltip - Mouse Over Tooltip
						std::string keyTimeText = "Time: " + std::to_string(pipPosition.x) + " sec";
						ImVec2 m = ImGui::GetIO().MousePos;
						ImGui::SetNextWindowPos(ImVec2(m.x + 15, m.y + 5));
						ImGui::Begin("ScrubberTooltip", NULL, ImGuiWindowFlags_Tooltip | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar);
						ImGui::Text(keyTimeText.c_str());
						ImGui::End();
						ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_Hand);
					}

					if (_isActive && ImGui::IsMouseDragging(ImGuiMouseButton_Left, 0))
					{
						ImGuiIO& inputOutput = ImGui::GetIO();
						if (pipPosition.x + inputOutput.MouseDelta.x / gridStep >= 0)
						{
							pipPosition.x += inputOutput.MouseDelta.x / gridStep;
							previewAnimationTime = (int)(pipPosition.x * 1000);
						}
					}
				}
			};

		L_RenderAnimationScrubber(keyFramePos, scrubberZeroPoint, animatorGridStep);

		ImGui::EndChild();
		// Get Back to here

		// Using InvisibleButton() as a convenience 1) it will advance the layout cursor and 2) allows us to use IsItemHovered()/IsItemActive()
		ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();
		ImVec2 canvas_sz = ImGui::GetContentRegionAvail();
		if (canvas_sz.x < 50.0f) canvas_sz.x = 50.0f;
		if (canvas_sz.y < 50.0f) canvas_sz.y = 50.0f;
		ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);
		static ImVec2 scrolling = ImVec2(0, 0);

		// Get Input and Output
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
			if (scrollInput > 0)
				animatorGridStep += 1;
			else if (scrollInput < 0 && animatorGridStep > 2)
				animatorGridStep -= 1;

		if (scrolling.x > 0)
			scrolling.x = 0;
		if (scrolling.y > 0)
			scrolling.y = 0;
		if (scrolling.y < -1500)
			scrolling.y = -1500;

		ImVec2 zeroPoint = ImVec2(0, 0);

		// Render the Timeline Grid
		RenderAnimationTimelineGrid(zeroPoint, scrolling, canvas_p0, canvas_p1, canvas_sz, animatorGridStep);

		// Get all keyFramePip positions
		float propertyYPos = -0.5f; // Value in grid space
		int propertyCounter = 0;    // For values in screenspace below
		float animationLength = animProps->animationLength;
		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		// Lambda
		auto L_RenderPropertyInTimeline = [&](std::string property, std::vector<float> keyFrameTimes, ImVec4 rectColor)
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

			ImVec2 topLeftCorner = ImVec2(canvas_p0.x, topYPos);
			ImVec2 bottomRightCorner = ImVec2(canvas_p1.x, bottomYPos);
			draw_list->AddRectFilled(topLeftCorner, bottomRightCorner, color);
		};

		// Lambda
		auto L_RenderAnimationTimelineKeyFrames = [](std::shared_ptr<Animation::S_Property> keyFrame, int counter, Vector2& pipPosition, ImVec2 zeroPoint, ImVec2 scrolling, ImVec2 canvas_p0, ImVec2 canvas_p1, ImVec2 canvas_sz, float gridStep)
		{
			std::string ID = keyFrame->name;
			ImDrawList* draw_list = ImGui::GetWindowDrawList();
			float spriteTextureWidth = 10;
			float spriteTextureHeight = 10;
			Vector2 spriteOffset = Vector2(5, 5);
			bool _spriteScalesWithZoom = false;
			int renderOrder = 1;

			// If there is a valid Texture loaded into the Sprite Component
			if (keyFrameTexture != nullptr)
			{
				ImVec2 pipStartingPoint = AddImageToDrawList(keyFrameTexture, pipPosition, zeroPoint, 12, 12, Vector2(6, 6), Vector2(1, 1), false, gridStep, draw_list);

				ImGui::SetCursorScreenPos(pipStartingPoint);
				std::string pipID = ID + std::to_string(counter) + "-KeyFramePip";
				ImGui::InvisibleButton(pipID.c_str(), ImVec2(12, 12), ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight | 4096);
				const bool _isClicked = ImGui::IsItemClicked();
				const bool _isHovered = ImGui::IsItemHovered();
				const bool _isActive = ImGui::IsItemActive();   // Held

				if (_isHovered)
					ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_Hand);
	
				if (_isActive || _isHovered)
				{
					// Mouse Hover Tooltip - Mouse Over Tooltip
					std::string keyTimeText = "Time: " + std::to_string(keyFrame->time / 1000) + " sec";
					ImVec2 m = ImGui::GetIO().MousePos;
					ImGui::SetNextWindowPos(ImVec2(m.x + 15, m.y + 5));
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

		ImVec4 rectColor;
		int IDCounter = 0;

		// Draw colored box for transform keyframes
		if (animProps->transformProperties.size() > 0)
		{
			rectColor = ImVec4(214, 8, 118, 100);
			std::vector<float> keyFrameTimes = std::vector<float>();
			L_RenderPropertyInTimeline("Transform", keyFrameTimes, rectColor);

			for (std::shared_ptr<Animation::S_Transform> keyFrame : animProps->transformProperties)
			{
				std::string ID = "Transform";
				// Get keyFrame time and convert to seconds
				float keyFrameX = keyFrame->time / 1000;
				Vector2 keyFramePos = Vector2(keyFrameX, propertyYPos);
				if (zeroPoint.y + (propertyYPos * animatorGridStep * -1) < canvas_p1.y && zeroPoint.y + (propertyYPos * animatorGridStep * -1) + 6 < canvas_p1.y && zeroPoint.y + (propertyYPos * animatorGridStep * -1) > canvas_p0.y)
					L_RenderAnimationTimelineKeyFrames(keyFrame, IDCounter, keyFramePos, zeroPoint, scrolling, canvas_p0, canvas_p1, canvas_sz, animatorGridStep);
				IDCounter++;
			}
			propertyYPos--;
			propertyCounter++;
		}
		if (animProps->spriteProperties.size() > 0)
		{
			rectColor = ImVec4(83, 214, 8, 100);
			std::vector<float> keyFrameTimes = std::vector<float>();
			L_RenderPropertyInTimeline("Sprite", keyFrameTimes, rectColor);

			for (std::shared_ptr<Animation::S_Sprite>& keyFrame : animProps->spriteProperties)
			{
				std::string ID = "Sprite";
				// Get keyFrame time and convert to seconds
				float keyFrameX = keyFrame->time / 1000;
				Vector2 keyFramePos = Vector2(keyFrameX, propertyYPos);
				if (zeroPoint.y + (propertyYPos * animatorGridStep * -1) < canvas_p1.y && zeroPoint.y + (propertyYPos * animatorGridStep * -1) + 6 < canvas_p1.y && zeroPoint.y + (propertyYPos * animatorGridStep * -1) > canvas_p0.y)
					L_RenderAnimationTimelineKeyFrames(keyFrame, IDCounter, keyFramePos, zeroPoint, scrolling, canvas_p0, canvas_p1, canvas_sz, animatorGridStep);
				IDCounter++;
			}
			propertyYPos--;
			propertyCounter++;
		}
		if (animProps->cameraProperties.size() > 0)
		{
			rectColor = ImVec4(206, 108, 4, 100);
			std::vector<float> keyFrameTimes = std::vector<float>();
			L_RenderPropertyInTimeline("Camera", keyFrameTimes, rectColor);

			for (std::shared_ptr<Animation::S_Camera>& keyFrame : animProps->cameraProperties)
			{
				std::string ID = "Camera";
				// Get keyFrame time and convert to seconds
				float keyFrameX = keyFrame->time / 1000;
				Vector2 keyFramePos = Vector2(keyFrameX, propertyYPos);
				if (zeroPoint.y + (propertyYPos * animatorGridStep * -1) < canvas_p1.y && zeroPoint.y + (propertyYPos * animatorGridStep * -1) + 6 < canvas_p1.y && zeroPoint.y + (propertyYPos * animatorGridStep * -1) > canvas_p0.y)
					L_RenderAnimationTimelineKeyFrames(keyFrame, IDCounter, keyFramePos, zeroPoint, scrolling, canvas_p0, canvas_p1, canvas_sz, animatorGridStep);
				IDCounter++;
			}
			propertyYPos--;
			propertyCounter++;
		}
		if (animProps->scriptProperties.size() > 0)
		{
			rectColor = ImVec4(4, 159, 206, 100);
			std::vector<float> keyFrameTimes = std::vector<float>();
			L_RenderPropertyInTimeline("Script", keyFrameTimes, rectColor);

			for (std::shared_ptr<Animation::S_Script>& keyFrame : animProps->scriptProperties)
			{
				std::string ID = "Script";
				// Get keyFrame time and convert to seconds
				float keyFrameX = keyFrame->time / 1000;
				Vector2 keyFramePos = Vector2(keyFrameX, propertyYPos);
				if (zeroPoint.y + (propertyYPos * animatorGridStep * -1) < canvas_p1.y && zeroPoint.y + (propertyYPos * animatorGridStep * -1) + 6 < canvas_p1.y && zeroPoint.y + (propertyYPos * animatorGridStep * -1) > canvas_p0.y)
					L_RenderAnimationTimelineKeyFrames(keyFrame, IDCounter, keyFramePos, zeroPoint, scrolling, canvas_p0, canvas_p1, canvas_sz, animatorGridStep);
				IDCounter++;
			}
			propertyYPos--;
			propertyCounter++;
		}
		if (animProps->buttonProperties.size() > 0)
		{
			rectColor = ImVec4(152, 16, 198, 100);
			std::vector<float> keyFrameTimes = std::vector<float>();
			L_RenderPropertyInTimeline("Button", keyFrameTimes, rectColor);

			for (std::shared_ptr<Animation::S_Button>& keyFrame : animProps->buttonProperties)
			{
				std::string ID = "Button";
				// Get keyFrame time and convert to seconds
				float keyFrameX = keyFrame->time / 1000;
				Vector2 keyFramePos = Vector2(keyFrameX, propertyYPos);
				if (zeroPoint.y + (propertyYPos * animatorGridStep * -1) < canvas_p1.y && zeroPoint.y + (propertyYPos * animatorGridStep * -1) + 6 < canvas_p1.y && zeroPoint.y + (propertyYPos * animatorGridStep * -1) > canvas_p0.y)
					L_RenderAnimationTimelineKeyFrames(keyFrame, IDCounter, keyFramePos, zeroPoint, scrolling, canvas_p0, canvas_p1, canvas_sz, animatorGridStep);
				IDCounter++;
			}
			propertyYPos--;
			propertyCounter++;
		}
		if (animProps->canvasProperties.size() > 0)
		{
			rectColor = ImVec4(224, 81, 15, 100);
			std::vector<float> keyFrameTimes = std::vector<float>();
			L_RenderPropertyInTimeline("Canvas", keyFrameTimes, rectColor);

			for (std::shared_ptr<Animation::S_Canvas>& keyFrame : animProps->canvasProperties)
			{
				std::string ID = "Canvas";
				// Get keyFrame time and convert to seconds
				float keyFrameX = keyFrame->time / 1000;
				Vector2 keyFramePos = Vector2(keyFrameX, propertyYPos);
				if (zeroPoint.y + (propertyYPos * animatorGridStep * -1) < canvas_p1.y && zeroPoint.y + (propertyYPos * animatorGridStep * -1) + 6 < canvas_p1.y && zeroPoint.y + (propertyYPos * animatorGridStep * -1) > canvas_p0.y)
					L_RenderAnimationTimelineKeyFrames(keyFrame, IDCounter, keyFramePos, zeroPoint, scrolling, canvas_p0, canvas_p1, canvas_sz, animatorGridStep);
				IDCounter++;
			}
			propertyYPos--;
			propertyCounter++;
		}
		if (animProps->audioProperties.size() > 0)
		{
			rectColor = ImVec4(237, 244, 14, 100);
			std::vector<float> keyFrameTimes = std::vector<float>();
			L_RenderPropertyInTimeline("Audio", keyFrameTimes, rectColor);

			for (std::shared_ptr<Animation::S_Audio>& keyFrame : animProps->audioProperties)
			{
				std::string ID = "Audio";
				// Get keyFrame time and convert to seconds
				float keyFrameX = keyFrame->time / 1000;
				Vector2 keyFramePos = Vector2(keyFrameX, propertyYPos);
				if (zeroPoint.y + (propertyYPos * animatorGridStep * -1) < canvas_p1.y && zeroPoint.y + (propertyYPos * animatorGridStep * -1) + 6 < canvas_p1.y && zeroPoint.y + (propertyYPos * animatorGridStep * -1) > canvas_p0.y)
					L_RenderAnimationTimelineKeyFrames(keyFrame, IDCounter, keyFramePos, zeroPoint, scrolling, canvas_p0, canvas_p1, canvas_sz, animatorGridStep);
				IDCounter++;
			}
			propertyYPos--;
			propertyCounter++;
		}
		if (animProps->textProperties.size() > 0)
		{
			rectColor = ImVec4(15, 224, 200, 100);
			std::vector<float> keyFrameTimes = std::vector<float>();
			L_RenderPropertyInTimeline("Text", keyFrameTimes, rectColor);

			for (std::shared_ptr<Animation::S_Text>& keyFrame : animProps->textProperties)
			{
				std::string ID = "Text";
				// Get keyFrame time and convert to seconds
				float keyFrameX = keyFrame->time / 1000;
				Vector2 keyFramePos = Vector2(keyFrameX, propertyYPos);
				if (zeroPoint.y + (propertyYPos * animatorGridStep * -1) < canvas_p1.y && zeroPoint.y + (propertyYPos * animatorGridStep * -1) + 6 < canvas_p1.y && zeroPoint.y + (propertyYPos * animatorGridStep * -1) > canvas_p0.y)
					L_RenderAnimationTimelineKeyFrames(keyFrame, IDCounter, keyFramePos, zeroPoint, scrolling, canvas_p0, canvas_p1, canvas_sz, animatorGridStep);
				IDCounter++;
			}
			propertyYPos--;
			propertyCounter++;
		}
		if (animProps->boxColliderProperties.size() > 0)
		{
			rectColor = ImVec4(224, 158, 15, 100);
			std::vector<float> keyFrameTimes = std::vector<float>();
			L_RenderPropertyInTimeline("BoxCollider", keyFrameTimes, rectColor);

			for (std::shared_ptr<Animation::S_BoxCollider>& keyFrame : animProps->boxColliderProperties)
			{
				std::string ID = "BoxCollider";
				// Get keyFrame time and convert to seconds
				float keyFrameX = keyFrame->time / 1000;
				Vector2 keyFramePos = Vector2(keyFrameX, propertyYPos);
				if (zeroPoint.y + (propertyYPos * animatorGridStep * -1) < canvas_p1.y && zeroPoint.y + (propertyYPos * animatorGridStep * -1) + 6 < canvas_p1.y && zeroPoint.y + (propertyYPos * animatorGridStep * -1) > canvas_p0.y)
					L_RenderAnimationTimelineKeyFrames(keyFrame, IDCounter, keyFramePos, zeroPoint, scrolling, canvas_p0, canvas_p1, canvas_sz, animatorGridStep);
				IDCounter++;
			}
			propertyYPos--;
			propertyCounter++;
		}
		if (animProps->circleColliderProperties.size() > 0)
		{
			rectColor = ImVec4(11, 42, 183, 100);
			std::vector<float> keyFrameTimes = std::vector<float>();
			L_RenderPropertyInTimeline("CircleCollider", keyFrameTimes, rectColor);

			for (std::shared_ptr<Animation::S_CircleCollider>& keyFrame : animProps->circleColliderProperties)
			{
				std::string ID = "CircleCollider";
				// Get keyFrame time and convert to seconds
				float keyFrameX = keyFrame->time / 1000;
				Vector2 keyFramePos = Vector2(keyFrameX, propertyYPos);
				if (zeroPoint.y + (propertyYPos * animatorGridStep * -1) < canvas_p1.y && zeroPoint.y + (propertyYPos * animatorGridStep * -1) + 6 < canvas_p1.y && zeroPoint.y + (propertyYPos * animatorGridStep * -1) > canvas_p0.y)
					L_RenderAnimationTimelineKeyFrames(keyFrame, IDCounter, keyFramePos, zeroPoint, scrolling, canvas_p0, canvas_p1, canvas_sz, animatorGridStep);
				IDCounter++;
			}
			propertyYPos--;
			propertyCounter++;
		}
		if (animProps->rigidBodyProperties.size() > 0)
		{
			rectColor = ImVec4(166, 11, 183, 100);
			std::vector<float> keyFrameTimes = std::vector<float>();
			L_RenderPropertyInTimeline("RigidBody", keyFrameTimes, rectColor);

			for (std::shared_ptr<Animation::S_RigidBody>& keyFrame : animProps->rigidBodyProperties)
			{
				std::string ID = "RigidBody";
				// Get keyFrame time and convert to seconds
				float keyFrameX = keyFrame->time / 1000;
				Vector2 keyFramePos = Vector2(keyFrameX, propertyYPos);
				if (zeroPoint.y + (propertyYPos * animatorGridStep * -1) < canvas_p1.y && zeroPoint.y + (propertyYPos * animatorGridStep * -1) + 6 < canvas_p1.y && zeroPoint.y + (propertyYPos * animatorGridStep * -1) > canvas_p0.y)
					L_RenderAnimationTimelineKeyFrames(keyFrame, IDCounter, keyFramePos, zeroPoint, scrolling, canvas_p0, canvas_p1, canvas_sz, animatorGridStep);
				IDCounter++;
			}
			propertyYPos--;
			propertyCounter++;
		}
		if (animProps->characterControllerProperties.size() > 0)
		{
			rectColor = ImVec4(85, 183, 11, 100);
			std::vector<float> keyFrameTimes = std::vector<float>();
			L_RenderPropertyInTimeline("CharacterController", keyFrameTimes, rectColor);

			for (std::shared_ptr<Animation::S_CharacterController>& keyFrame : animProps->characterControllerProperties)
			{
				std::string ID = "CharacterController";
				// Get keyFrame time and convert to seconds
				float keyFrameX = keyFrame->time / 1000;
				Vector2 keyFramePos = Vector2(keyFrameX, propertyYPos);
				if (zeroPoint.y + (propertyYPos * animatorGridStep * -1) < canvas_p1.y && zeroPoint.y + (propertyYPos * animatorGridStep * -1) + 6 < canvas_p1.y && zeroPoint.y + (propertyYPos * animatorGridStep * -1) > canvas_p0.y)
					L_RenderAnimationTimelineKeyFrames(keyFrame, IDCounter, keyFramePos, zeroPoint, scrolling, canvas_p0, canvas_p1, canvas_sz, animatorGridStep);
				IDCounter++;
			}
			propertyYPos--;
			propertyCounter++;
		}


		// Timeline Events BeginChild()
		ImGui::EndChild();
		ImGui::End(); // Animator
	}

	void RenderAnimationPreview()
	{
		ImGuiChildFlags padding_child_flags = ImGuiChildFlags_::ImGuiChildFlags_AlwaysUseWindowPadding;
		ImGuiChildFlags child_flags = ImGuiChildFlags_::ImGuiChildFlags_AlwaysUseWindowPadding | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_::ImGuiChildFlags_AlwaysAutoResize;

		ImGui::Begin("Animation Preview");
		// Animation Preview BeginChild()
		// 
		ImGui::PushStyleColor(ImGuiCol_ChildBg, innerWindowColor);
		ImGui::BeginChild("Animation Preview", ImVec2(0, 0), padding_child_flags);
		ImGui::PopStyleColor();

		////////////////
		ImGui::PushStyleColor(ImGuiCol_ChildBg, white);
		ImGui::BeginChild("Animation Preview", ImVec2(0, 0), child_flags);
		ImGui::PopStyleColor();
		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(1, 9, 0, 255));
		ImGui::Text("Animation Preview");
		ImGui::PopStyleColor();
		ImGui::EndChild();
		////////////////

		RenderCheckbox("Preview Animation", _playPreviewAnimation);

		ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();
		ImVec2 canvas_sz = ImGui::GetContentRegionAvail();
		if (canvas_sz.x < 50.0f) canvas_sz.x = 50.0f;
		if (canvas_sz.y < 50.0f) canvas_sz.y = 50.0f;
		ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);
		static ImVec2 scrolling = ImVec2(0, 0);
		static ImVec2 viewPortDimensions = ImVec2(0, 0);
		static bool _firstRenderPassDone = false;
		static bool _viewportSizeTaken = false;

		// Set initial viewport dimensions once
		if (_firstRenderPassDone)
		{
			if (!_viewportSizeTaken)
			{
				viewPortDimensions = ImVec2(canvas_sz.x, canvas_sz.y);
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
		
		ImVec2 centerPoint = ImVec2(0, 0);

		RenderGridView(centerPoint, scrolling, canvas_p0, canvas_p1, canvas_sz, ImVec2(50,50), ImVec2(viewPortDimensions.x / 2, viewPortDimensions.y / 2));

		if (objectForFocusedAnimation != nullptr)
		{
			std::vector<std::shared_ptr<GameObject>> focusedObjectVector;
			focusedObjectVector.push_back(objectForFocusedAnimation);
	
			// Animate the focused object
			if (_playPreviewAnimation)
			{				
				std::shared_ptr<Animation> animation = objectForFocusedAnimation->GetAnimationComponent();

				// If animation component is playing, play the animation
				if (animation != nullptr && animation->IsPlaying())
				{
					previewAnimationTime = GetEngineTime();
					animation->PlayAnimation(previewAnimationTime);
				}
			}

			RenderViewObjects(focusedObjectVector, centerPoint, canvas_p0, canvas_sz);
		}

		// Animation Preview EndChild()
		ImGui::EndChild();
		ImGui::End();
	}

	void RenderAnimationTimelineGrid(ImVec2& zeroPoint, ImVec2 scrolling, ImVec2 canvas_p0, ImVec2 canvas_p1, ImVec2 canvas_sz, float gridStep)
	{
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(FlatEngine::FlatGui::darker.x * 255, darker.y * 255, darker.z * 255, 255));
		zeroPoint = ImVec2(scrolling.x + canvas_p0.x, canvas_p0.y + scrolling.y);

		// Draw vertical grid lines
		for (float x = trunc(fmodf(zeroPoint.x, gridStep)); x < canvas_p0.x + canvas_sz.x; x += gridStep)
		{
			FlatEngine::DrawLine(ImVec2(x, canvas_p0.y), ImVec2(x, canvas_p1.y), IM_COL32(dark.x * 255, dark.y * 255, dark.z * 255, 255), 1.0f, draw_list);
		}
		for (float x = trunc(fmodf(zeroPoint.x, gridStep * 2)); x < canvas_p0.x + canvas_sz.x; x += gridStep * 2)
		{
			FlatEngine::DrawLine(ImVec2(x, canvas_p0.y), ImVec2(x, canvas_p1.y), IM_COL32(light.x * 255, light.y * 255, light.z * 255, 255), 1.0f, draw_list);
		}
		// Draw horizontal grid lines
		for (float y = trunc(fmodf(zeroPoint.y, gridStep)); y < canvas_p0.y + canvas_sz.y; y += gridStep / 2)
		{
			if (y > canvas_p0.y)
				FlatEngine::DrawLine(ImVec2(canvas_p0.x, y), ImVec2(canvas_p1.x, y), IM_COL32(dark.x * 255, dark.y * 255, dark.z * 255, 255), 1.0f, draw_list);
		}
	}

	void RenderKeyFrameEditor()
	{
		ImGuiChildFlags padding_child_flags = ImGuiChildFlags_::ImGuiChildFlags_AlwaysUseWindowPadding;
		ImGuiChildFlags child_flags = ImGuiChildFlags_::ImGuiChildFlags_AlwaysUseWindowPadding | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_::ImGuiChildFlags_AlwaysAutoResize;

		ImGui::Begin("KeyFrame Editor");
		// Animation Preview BeginChild()
		// 
		ImGui::PushStyleColor(ImGuiCol_ChildBg, innerWindowColor);
		ImGui::BeginChild("KeyFrame Editor", ImVec2(0, 0), padding_child_flags);
		ImGui::PopStyleColor();

		std::string keyFrameProperty = "-No KeyFrame Selected-";
		if (selectedKeyFrameToEdit != nullptr)
			keyFrameProperty = selectedKeyFrameToEdit->name;

		////////////////
		ImGui::PushStyleColor(ImGuiCol_ChildBg, white);
		ImGui::BeginChild("KeyFrame Editor", ImVec2(0, 0), child_flags);
		ImGui::PopStyleColor();
		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(1, 9, 0, 255));
		ImGui::Text(keyFrameProperty.c_str());
		ImGui::PopStyleColor();
		ImGui::EndChild();
		////////////////

		if (selectedKeyFrameToEdit != nullptr)
		{
			static ImGuiSliderFlags flags = ImGuiSliderFlags_::ImGuiSliderFlags_None;
			// Flags for child padding and dimensions
			ImGuiChildFlags child_flags = ImGuiChildFlags_::ImGuiChildFlags_AlwaysUseWindowPadding | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_::ImGuiChildFlags_AlwaysAutoResize;

			// Here we should steal code from the components rendering in Inpsector window.
			if (selectedKeyFrameToEdit->name == "Transform")
			{
				// Position, scale, and rotation of transform
				std::shared_ptr<Animation::S_Transform> transform = std::static_pointer_cast<Animation::S_Transform>(selectedKeyFrameToEdit);
				float xMove = transform->xMove;
				float yMove = transform->yMove;
				float xScale = transform->xScale;
				float yScale = transform->yScale;
				//float rotation = transform->rotation;
				static ImGuiSliderFlags flags = ImGuiSliderFlags_::ImGuiSliderFlags_None;

				// Push Item Width
				ImGui::PushItemWidth(ImGui::GetContentRegionMax().x / 3 - 5);

				// Drags for position editing
				//
				// Render Text for Positions + Rotation
				ImGui::Text("xMove:");
				ImGui::SameLine(ImGui::GetContentRegionMax().x / 2 + 5, 0);
				ImGui::Text("yMove:");
				//ImGui::Text("Rotation:");

				// Render Drags for Positions + Rotation
				ImGui::DragFloat("##xMove", &xMove, 0.5f, -FLT_MAX, -FLT_MAX, "%.3f", flags);
				if (ImGui::IsItemHovered())
					ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_ResizeEW);
				ImGui::SameLine(0, 5);
				ImGui::DragFloat("##yMove", &yMove, 0.5f, -FLT_MAX, -FLT_MAX, "%.3f", flags);
				// Set cursor type
				if (ImGui::IsItemHovered())
					ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_ResizeEW);
				//ImGui::SameLine(0, 5);
				//ImGui::DragFloat("##rotation", &rotation, 1.0f, -360, 360, "%.3f", flags);
				// Set cursor type
				if (ImGui::IsItemHovered())
					ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_ResizeEW);

				// Assign the new slider values to the transforms position
				transform->xMove = xMove;
				transform->yMove = yMove;

				// Render Drags for scale of transform
				// Push
				ImGui::PushItemWidth(ImGui::GetContentRegionMax().x / 2 - 5);

				// Render text for scales
				ImGui::Text("Scale x:");
				ImGui::SameLine(ImGui::GetContentRegionMax().x / 2 + 5, 0);
				ImGui::Text("Scale y:");

				// Render Drags for scales
				ImGui::DragFloat("##xScale", &xScale, 0.05f, 0, -FLT_MAX, "%.3f", flags);
				// Set cursor type
				if (ImGui::IsItemHovered())
					ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_ResizeEW);
				ImGui::SameLine(0, 5);
				ImGui::DragFloat("##yScale", &yScale, 0.05f, -FLT_MAX, -FLT_MAX, "%.3f", flags);
				// Set cursor type
				if (ImGui::IsItemHovered())
					ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_ResizeEW);

				// Pop Width Setting
				ImGui::PopItemWidth();

				// Assign the new slider values to the sprites pivotPoint
				transform->xScale = xScale;
				transform->yScale = yScale;

				// Interpolation type
				const char* interpType[] = { "-Select-", "Lerp", "Slerp" };
				static int current_type = 0;
				static std::string node_clicked = "";
				static std::string selected_property = "";
				
				// Get already saved interpType to show up in the combo select as default
				if (transform->transformInterpType == Animation::Lerp)
					current_type = 1;
				else if (transform->transformInterpType == Animation::Slerp)
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
								transform->transformInterpType = Animation::Lerp;
							else if (interpType[n] == "Slerp")
								transform->transformInterpType = Animation::Slerp;
						}
							
						if (is_selected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}
			}
			else if (selectedKeyFrameToEdit->name == "Sprite")
			{
				// Sprite path and texture dimension variables
				std::shared_ptr<Animation::S_Sprite> sprite = std::static_pointer_cast<Animation::S_Sprite>(selectedKeyFrameToEdit);
				std::string path = sprite->path;
				char newPath[1024];
				strcpy_s(newPath, path.c_str());

				// Sprite Path Strings
				std::string pathString = "Path: ";

				// Render Sprite Path
				ImGui::Text(pathString.c_str());
				ImGui::SameLine(0, 5);

				std::string openFileID = "##openFileIconForAnimatorSprite";
				if (RenderImageButton(openFileID.c_str(), openFileTexture))
				{
					std::string assetPath = OpenLoadFileExplorer();
					strcpy_s(newPath, assetPath.c_str());
					sprite->path = newPath;
				}

				ImGui::SameLine(0, 5);
				
				ImGui::PushStyleColor(ImGuiCol_FrameBg, inputColor);
				if (ImGui::InputText("##spritePath", newPath, IM_ARRAYSIZE(newPath), flags))
					sprite->path = newPath;
				ImGui::PopStyleColor();

				//ImGui::Text(textureWidthString.c_str());
				//ImGui::Text(textureHeightString.c_str());

				// Render Order
				//ImGui::Text("Render order: ");
				//if (ImGui::SliderInt("Sprite Render Order", &renderOrder, 0, maxSpriteLayers, "%d"))
				//	sprite->SetRenderOrder(renderOrder);

				// Push Item Width Setting
				//ImGui::PushItemWidth(ImGui::GetContentRegionMax().x / 2 - 5);

				// Sprite offset variables
				//Vector2 offset = sprite->GetOffset();
				//float xOffset = offset.x;
				//float yOffset = offset.y;
				//ImGuiSliderFlags flags = ImGuiSliderFlags_::ImGuiSliderFlags_None;

				//// Render Drags for offset of texture editing
				//ImGui::Text("xOffset:");
				//ImGui::SameLine(ImGui::GetContentRegionMax().x / 2 + 5, 0);
				//ImGui::Text("yOffset:");
				//ImGui::DragFloat("##xOffset", &xOffset, 0.5f, -FLT_MAX, -FLT_MAX, "%.3f", flags);
				//// Set cursor type
				//if (ImGui::IsItemHovered())
				//	ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_ResizeEW);
				//ImGui::SameLine(0, 5);
				//ImGui::DragFloat("##yOffset", &yOffset, 0.5f, -FLT_MAX, -FLT_MAX, "%.3f", flags);
				//// Set cursor type
				//if (ImGui::IsItemHovered())
				//	ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_ResizeEW);

				// Pop Width Setting
				//ImGui::PopItemWidth();

				// Assign the new slider values to the sprites pivotPoint
				//sprite->SetOffset(Vector2(xOffset, yOffset));
			}
		}
		else
		{
			ImGui::Text("-Select a keyframe to edit-");
		}

		ImGui::EndChild();
		ImGui::End();
	}
}	
}