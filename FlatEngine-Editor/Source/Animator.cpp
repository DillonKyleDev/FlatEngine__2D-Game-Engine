#include "FlatEngine.h"
#include "FlatGui.h"
#include "GameObject.h"
#include "Project.h"
#include "Camera.h"
#include "Transform.h"
#include "Sprite.h"
#include "imgui.h"
#include "Text.h"
#include "math.h"
#include <fstream>

namespace FlatGui {

	Vector4 darker = Vector4(float(0.2), float(0.2), float(0.2), float(1));
	Vector4 lighter = Vector4(float(0.8), float(0.8), float(0.8), float(1));
	Vector4 light = Vector4(float(0.7), float(0.7), float(0.7), float(1));
	Vector4 dark = Vector4(float(0.3), float(0.3), float(0.3), float(1));
	Vector4 white = Vector4(float(0.9), float(0.9), float(0.9), float(1));
	Vector4 transformAnimationNode = Vector4(float(0.1), float(0.76), float(0.08), float(.8));
	Vector4 scrubberBackground = Vector4(float(0.22), float(0.22), float(0.25), float(1));
	Vector4 scrubberBackgroundDark = Vector4(float(0.12), float(0.02), float(0.0), float(1));


	void RenderAnimator()
	{
		FlatEngine::PushWindowStyles();
		// 16 | 8 are flags for noScrollbar and noscrollwithmouse
		ImGui::Begin("Animator", &_showAnimator, 16 | 8);
		FlatEngine::PopWindowStyles();

		// Animated Properties BeginChild()
		FlatEngine::BeginResizeWindowChild("Animated Properties");

		std::string animationName = "-No Animation Selected-";
		if (GetFocusedAnimation() != nullptr && GetFocusedAnimation()->animationName != "")
			animationName = "Loaded Animation: " + GetFocusedAnimation()->animationName;

		ImGui::BeginChild("Manage Animation", Vector2(0, 0), FlatEngine::F_autoResizeChildFlags);
	
		FlatEngine::RenderSectionHeader(animationName, 18);
		ImGui::SetCursorPos(Vector2(ImGui::GetCursorPosX() + 8, ImGui::GetCursorPos().y - 36));

		static std::string animationFilePath;
		if (GetFocusedAnimation() != nullptr)
			animationFilePath = GetFocusedAnimation()->animationPath;

		// Loading, saving and opening animation json files
		if (FlatEngine::RenderImageButton("##NewAnimationFile", FlatEngine::F_newFileIcon.GetTexture(), Vector2(16, 16), 1, FlatEngine::F_transparentColor))
		{
			animationFilePath = FlatEngine::OpenSaveFileExplorer();

			if (animationFilePath != "")
			{
				// Create S_AnimationProperties struct to store the properties of the json file in
				std::shared_ptr<Animation::S_AnimationProperties> animationProperties = std::make_shared<Animation::S_AnimationProperties>();
				animationProperties->animationName = "New Animation";

				FlatEngine::CreateNewAnimationFile(animationFilePath);
				SaveAnimationFile(animationProperties, animationFilePath);
			}
		}
		// Tooltip
		if (ImGui::BeginItemTooltip())
		{
			ImGui::Text("Create new animation");
			ImGui::EndTooltip();
		}

		ImGui::SameLine(0, 2);			
		if (FlatEngine::RenderImageButton("#OpenAnim", FlatEngine::F_openFileIcon.GetTexture(), Vector2(16, 16), 1, FlatEngine::F_transparentColor))
		{
			animationFilePath = FlatEngine::OpenLoadFileExplorer();
			if (animationFilePath != "")
			{
				SetFocusedAnimation(FlatEngine::LoadAnimationFile(animationFilePath));
				loadedProject->SetLoadedPreviewAnimationPath(animationFilePath);
			}
		}
		// Tooltip
		if (ImGui::BeginItemTooltip())
		{
			ImGui::Text("Open animation");
			ImGui::EndTooltip();
		}

		ImGui::SameLine(0, 2);		
		if (FlatEngine::RenderImageButton("#SaveAnimationFile", FlatEngine::F_saveFileIcon.GetTexture(), Vector2(16, 16), 1, FlatEngine::F_transparentColor))
		{
			if (animationFilePath != "")
				FlatEngine::SaveAnimationFile(GetFocusedAnimation(), animationFilePath);
		}
		// Tooltip
		if (ImGui::BeginItemTooltip())
		{
			ImGui::Text("Save animation");
			ImGui::EndTooltip();
		}

		ImGui::SameLine(0, 2);
		if (FlatEngine::RenderImageButton("#SaveAsAnimationFile", FlatEngine::F_saveAsFileIcon.GetTexture(), Vector2(16, 16), 1, FlatEngine::F_transparentColor))
		{
			animationFilePath = FlatEngine::OpenSaveFileExplorer();
			if (animationFilePath != "")
				FlatEngine::SaveAnimationFile(GetFocusedAnimation(), animationFilePath);
		}
		// Tooltip
		if (ImGui::BeginItemTooltip())
		{
			ImGui::Text("Save animation as...");
			ImGui::EndTooltip();
		}

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
				animProps->eventProperties.push_back(eventProperties);
			}
			else if (property == "Transform")
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
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 12);
			Animation* animation = nullptr;
			if (objectForFocusedAnimation != nullptr)
				animation = objectForFocusedAnimation.GetAnimation();

			if (FlatEngine::RenderCheckbox("Loop Animation", animProps->_loop) && animation != nullptr && animation->IsPlaying())
			{
				animation->Stop();
				animation->Play(FlatEngine::GetEngineTime());
			}

			static std::string selected_property = "";			
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
			ImGui::Text("Add Properties:");
			std::vector<std::string> props = std::vector<std::string>();
			FlatEngine::PushComboStyles();
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 34);
			if (ImGui::BeginCombo("##properties", properties[current_property]))
			{
				for (int n = 0; n < IM_ARRAYSIZE(properties); n++)
				{
					if (animProps->eventProperties.size() == 0 && properties[n] == "Event" ||
						animProps->transformProperties.size() == 0 && properties[n] == "Transform" ||
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
						bool is_selected = (properties[current_property] == properties[n]);
						if (ImGui::Selectable(properties[n], is_selected))
							current_property = n;
						if (is_selected)
							ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
			FlatEngine::PopComboStyles();
			ImGui::SameLine(0, 5);
			if (FlatEngine::RenderButton("Add"))
			{
				L_PushBackKeyFrame(properties[current_property]);
				// Reset selector box to default
				current_property = 0;
			}
			ImGui::EndChild();

			// List properties on this animation


			ImGui::PushStyleColor(ImGuiCol_FrameBg, FlatEngine::F_innerWindowColor);
			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, Vector2(0, 0));
			FlatEngine::PushMenuStyles();

			// Conditionally begin the table
			if (animProps->eventProperties.size() > 0 ||
				animProps->transformProperties.size() > 0 ||
				animProps->spriteProperties.size() > 0 ||
				animProps->cameraProperties.size() > 0 ||
				animProps->scriptProperties.size() > 0 ||
				animProps->buttonProperties.size() > 0 ||
				animProps->canvasProperties.size() > 0 ||
				animProps->audioProperties.size() > 0 ||
				animProps->textProperties.size() > 0 ||
				animProps->boxColliderProperties.size() > 0 ||
				animProps->circleColliderProperties.size() > 0 ||
				animProps->rigidBodyProperties.size() > 0 || 
				animProps->characterControllerProperties.size() > 0)
			if (ImGui::BeginTable("##AnimationProperties", 1, FlatEngine::F_tableFlags))
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


				RenderPropertyButton("Event", (int)animProps->eventProperties.size(), node_clicked);
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

			
				ImGui::EndTable();
			}
			FlatEngine::PopMenuStyles();
			ImGui::PopStyleVar();
			ImGui::PopStyleColor();
		}

		ImGui::EndChild(); // Animator Properties EndChild()

		ImGui::SameLine(0, 5);

		// Timeline Events
		ImGui::PushStyleColor(ImGuiCol_ChildBg, FlatEngine::F_outerWindowColor);
		ImGui::BeginChild("##AnimationTimeline", Vector2(0, 0), FlatEngine::F_childFlags);
		ImGui::PopStyleColor();

		FlatEngine::RenderSectionHeader("Animation Timeline");

		ImGui::BeginChild("Property Header", Vector2(0,0), FlatEngine::F_headerFlags);
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
			ImGui::GetWindowDrawList()->AddRectFilled(cursorScreen, Vector2(cursorScreen.x + textSize.x + 6, cursorScreen.y + textSize.y + 6), ImGui::GetColorU32(FlatEngine::F_tableCellLightColor), 2);
			ImGui::SetCursorPos(Vector2(ImGui::GetCursorPosX() + 3, ImGui::GetCursorPosY() + 3));
			ImGui::Text(node_clicked.c_str());
		}
		
		if (node_clicked != "")
		{
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 6);
			if (FlatEngine::RenderButton("Add Keyframe"))
				L_PushBackKeyFrame(node_clicked);
			ImGui::SameLine(0, 5);
			//ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3);
			if (FlatEngine::RenderButton("Remove Property"))
				L_PushBackKeyFrame(node_clicked);
		}

		if (objectForFocusedAnimation != nullptr)
		{
			// Animate the focused object
			Animation* animation = objectForFocusedAnimation.GetAnimation();
			std::string playID = "##playAnimationPreview";
			std::string stopID = "##StopGameloopIcon";
			bool _isPreviewing = false;
			
			if (animation != nullptr)
				_isPreviewing = animation->IsPlaying();

			// Play Button
			if (_isPreviewing)
			{
				ImGui::BeginDisabled();
				FlatEngine::RenderImageButton(playID.c_str(), FlatEngine::F_playIcon.GetTexture(), Vector2(14, 14), 0, FlatEngine::F_buttonColor, FlatEngine::F_whiteColor, FlatEngine::F_buttonHoveredColor, FlatEngine::F_buttonActiveColor);
				ImGui::EndDisabled();
			}
			else
			{
				if (FlatEngine::RenderImageButton(playID.c_str(), FlatEngine::F_playIcon.GetTexture(), Vector2(14, 14), 0, FlatEngine::F_buttonColor, FlatEngine::F_whiteColor, FlatEngine::F_buttonHoveredColor, FlatEngine::F_buttonActiveColor))
				{
					if (animation != nullptr)
					{
						previewAnimationStartTime = FlatEngine::GetEllapsedGameTimeInMs();
						previewAnimationTime = FlatEngine::GetEllapsedGameTimeInMs();
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
				FlatEngine::RenderImageButton(stopID.c_str(), FlatEngine::F_stopIcon.GetTexture(), Vector2(14, 14), 0, FlatEngine::F_buttonColor, FlatEngine::F_whiteColor, FlatEngine::F_buttonHoveredColor, FlatEngine::F_buttonActiveColor);
				ImGui::EndDisabled();
			}
			else
			{
				if (FlatEngine::RenderImageButton(stopID.c_str(), FlatEngine::F_stopIcon.GetTexture(), Vector2(14, 14), 0, FlatEngine::F_buttonColor, FlatEngine::F_whiteColor, FlatEngine::F_buttonHoveredColor, FlatEngine::F_buttonActiveColor))
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

		static float animatorGridStep = 50;
		// Using InvisibleButton() as a convenience 1) it will advance the layout cursor and 2) allows us to use IsItemHovered()/IsItemActive()
		Vector2 canvas_p0 = ImGui::GetCursorScreenPos();
		Vector2 canvas_sz = ImGui::GetContentRegionAvail();
		if (canvas_sz.x < 50.0f) canvas_sz.x = 50.0f;
		if (canvas_sz.y < 50.0f) canvas_sz.y = 50.0f;
		Vector2 canvas_p1 = Vector2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);
		static Vector2 scrolling = Vector2(0, 0);

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
				animatorGridStep += 10;
			else if (scrollInput < 0 && animatorGridStep > 2)
				animatorGridStep -= 10;

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
			if (FlatEngine::F_keyFrameIcon.GetTexture() != nullptr)
			{
				Vector2 pipStartingPoint = AddImageToDrawList(FlatEngine::F_keyFrameIcon.GetTexture(), pipPosition, zeroPoint, 12, 12, Vector2(6, 6), Vector2(1, 1), _spriteScalesWithZoom, animatorGridStep, draw_list);

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
			if (animProps->eventProperties.size() > 0)
			{
				rectColor = Vector4(255, 255, 255, 100);
				std::vector<float> keyFrameTimes = std::vector<float>();
				L_RenderPropertyInTimeline("Event", keyFrameTimes, rectColor);

				for (std::shared_ptr<Animation::S_Event> keyFrame : animProps->eventProperties)
				{
					std::string ID = "Event";
					// Get keyFrame time and convert to seconds
					float keyFrameX = keyFrame->time / 1000;
					Vector2 keyFramePos = Vector2(keyFrameX, propertyYPos);
					//if (zeroPoint.y + (propertyYPos * animatorGridStep * -1) < canvas_p1.y && zeroPoint.y + (propertyYPos * animatorGridStep * -1) + 6 < canvas_p1.y && zeroPoint.y + (propertyYPos * animatorGridStep * -1) > canvas_p0.y)
					L_RenderAnimationTimelineKeyFrames(keyFrame, IDCounter, keyFramePos, zeroPoint, scrolling, canvas_p0, canvas_p1, canvas_sz, animatorGridStep);
					IDCounter++;
				}
				propertyYPos--;
				propertyCounter++;
			}
			if (animProps->transformProperties.size() > 0)
			{
				rectColor = Vector4(214, 8, 118, 100);
				std::vector<float> keyFrameTimes = std::vector<float>();
				L_RenderPropertyInTimeline("Transform", keyFrameTimes, rectColor);

				for (std::shared_ptr<Animation::S_Transform> keyFrame : animProps->transformProperties)
				{
					std::string ID = "Transform";
					// Get keyFrame time and convert to seconds
					float keyFrameX = keyFrame->time / 1000;
					Vector2 keyFramePos = Vector2(keyFrameX, propertyYPos);
					//if (zeroPoint.y + (propertyYPos * animatorGridStep * -1) < canvas_p1.y && zeroPoint.y + (propertyYPos * animatorGridStep * -1) + 6 < canvas_p1.y && zeroPoint.y + (propertyYPos * animatorGridStep * -1) > canvas_p0.y)
					L_RenderAnimationTimelineKeyFrames(keyFrame, IDCounter, keyFramePos, zeroPoint, scrolling, canvas_p0, canvas_p1, canvas_sz, animatorGridStep);
					IDCounter++;
				}
				propertyYPos--;
				propertyCounter++;
			}
			if (animProps->spriteProperties.size() > 0)
			{
				rectColor = Vector4(83, 214, 8, 100);
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
				rectColor = Vector4(206, 108, 4, 100);
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
				rectColor = Vector4(4, 159, 206, 100);
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
				rectColor = Vector4(152, 16, 198, 100);
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
				rectColor = Vector4(224, 81, 15, 100);
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
				rectColor = Vector4(237, 244, 14, 100);
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
				rectColor = Vector4(15, 224, 200, 100);
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
				rectColor = Vector4(224, 158, 15, 100);
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
				rectColor = Vector4(11, 42, 183, 100);
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
				rectColor = Vector4(166, 11, 183, 100);
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
				rectColor = Vector4(85, 183, 11, 100);
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
		}

		ImGui::EndChild();
		// Timeline Events BeginChild()
		ImGui::EndChild();
		ImGui::End(); // Animator
	}

	void RenderAnimationPreview()
	{
		static Vector2 step = Vector2(50, 50);

		FlatEngine::BeginWindow("Animator Preview", _showAnimationPreview);

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
			focusedObjectVector.push_back(&objectForFocusedAnimation);
	
			// Animate the focused object
			if (_playPreviewAnimation)
			{				
				Animation* animation = objectForFocusedAnimation.GetAnimation();

				// If animation component is playing, play the animation
				if (animation != nullptr && animation->IsPlaying())
				{
					previewAnimationTime = FlatEngine::GetEngineTime();
					animation->PlayAnimation(previewAnimationTime);
				}
			}

			RenderViewObjects(focusedObjectVector, centerPoint, canvas_p0, canvas_sz, step.x);
		}

		FlatEngine::EndWindow();
	}

	void RenderAnimationTimelineGrid(Vector2& zeroPoint, Vector2 scrolling, Vector2 canvas_p0, Vector2 canvas_p1, Vector2 canvas_sz, float gridStep)
	{
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(darker.x * 255, darker.y * 255, darker.z * 255, 255));
		zeroPoint = Vector2(scrolling.x + canvas_p0.x, canvas_p0.y + scrolling.y);

		// Draw vertical grid lines
		for (float x = trunc(fmodf(zeroPoint.x, gridStep)); x < canvas_p0.x + canvas_sz.x; x += gridStep)
		{
			FlatEngine::DrawLine(Vector2(x, canvas_p0.y), Vector2(x, canvas_p1.y), dark, 1.0f, draw_list);
		}
		for (float x = trunc(fmodf(zeroPoint.x, gridStep * 2)); x < canvas_p0.x + canvas_sz.x; x += gridStep * 2)
		{
			FlatEngine::DrawLine(Vector2(x, canvas_p0.y), Vector2(x, canvas_p1.y), light, 1.0f, draw_list);
		}
		// Draw horizontal grid lines
		for (float y = trunc(fmodf(zeroPoint.y, gridStep)); y < canvas_p0.y + canvas_sz.y; y += gridStep / 2)
		{
			if (y > canvas_p0.y)
				FlatEngine::DrawLine(Vector2(canvas_p0.x, y), Vector2(canvas_p1.x, y), dark, 1.0f, draw_list);
		}
	}

	void RenderKeyFrameEditor()
	{
		FlatEngine::BeginWindow("Keyframe Editor", _showKeyFrameEditor);

		std::string keyFrameProperty = "No KeyFrame Selected";
		if (selectedKeyFrameToEdit != nullptr)
			keyFrameProperty = selectedKeyFrameToEdit->name;

		FlatEngine::RenderSectionHeader(keyFrameProperty);

		if (selectedKeyFrameToEdit != nullptr)
		{
			// Here we should steal code from the components rendering in Inpsector window.
			if (selectedKeyFrameToEdit->name == "Event")
			{
				std::shared_ptr<Animation::S_Event> event = std::static_pointer_cast<Animation::S_Event>(selectedKeyFrameToEdit);
				std::string functionName = event->functionName;
				if (FlatEngine::RenderInput("AnimationEventName", "Function Name", functionName))
					event->functionName = functionName;
			}
			else if (selectedKeyFrameToEdit->name == "Transform")
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
				const char* interpType[] = { "- Select -", "Lerp", "Slerp" };
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
				// Sprite offset variables
				float xOffset = sprite->xOffset;
				float yOffset = sprite->yOffset;
				ImGuiSliderFlags flags = ImGuiSliderFlags_::ImGuiSliderFlags_None;
				Vector4 tintColor = sprite->tintColor;
				bool _instantlyChangeTint = sprite->_instantTintChange;

				// Sprite Path Strings
				std::string pathString = "Path: ";

				// Render Sprite Path
				ImGui::Text(pathString.c_str());
				ImGui::SameLine(0, 5);

				std::string openFileID = "##openFileIconForAnimatorSprite";
				if (FlatEngine::RenderImageButton(openFileID.c_str(), FlatEngine::F_openFileIcon.GetTexture()))
				{
					std::string assetPath = FlatEngine::OpenLoadFileExplorer();
					strcpy_s(newPath, assetPath.c_str());
					sprite->path = newPath;
				}

				ImGui::SameLine(0, 5);
				
				ImGui::PushStyleColor(ImGuiCol_FrameBg, FlatEngine::F_inputColor);
				if (ImGui::InputText("##spritePath", newPath, IM_ARRAYSIZE(newPath)))
					sprite->path = newPath;
				ImGui::PopStyleColor();

				// Render Table
				if (FlatEngine::PushTable("##AnimatedSpriteProperties", 2))
				{
					if (FlatEngine::RenderFloatDragTableRow("##AnimatedxSpriteOffsetDrag", "X Offset", xOffset, 0.1f, -FLT_MAX, -FLT_MAX))
						sprite->xOffset = xOffset;
					if (FlatEngine::RenderFloatDragTableRow("##AnimatedySpriteOffsetDrag", "Y Offset", yOffset, 0.1f, -FLT_MAX, -FLT_MAX))
						sprite->yOffset = yOffset;
					FlatEngine::PopTable();
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

				if (FlatEngine::RenderCheckbox("Instantly change tint", _instantlyChangeTint))
					sprite->_instantTintChange = _instantlyChangeTint;
			}
		}
		else
		{	
			ImGui::TextWrapped("Select a keyframe to edit from the Animation Timeline...");
		}

		FlatEngine::EndWindow();
	}

	void SetFocusedAnimation(std::shared_ptr<Animation::S_AnimationProperties> animation)
	{
		FocusedAnimation = animation;
	}

	std::shared_ptr<Animation::S_AnimationProperties> GetFocusedAnimation()
	{
		return FocusedAnimation;
	}
}