#include "FlatEngine.h"
#include "Camera.h"
#include "Transform.h"
#include "Sprite.h"
#include "imgui.h"
#include "Text.h"
#include "math.h"
#include <fstream>

namespace FlatEngine { namespace FlatGui {

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
		PushWindowStyles();
		// 16 | 8 are flags for noScrollbar and noscrollwithmouse
		ImGui::Begin("Animator", &_showAnimator, 16 | 8);
		PopWindowStyles();

		// Animated Properties BeginChild()
		BeginResizeWindowChild("Animated Properties");

		std::string animationName = "-No Animation Selected-";
		if (GetFocusedAnimation()->animationName != "")
			animationName = "Loaded Animation: " + GetFocusedAnimation()->animationName;

		ImGui::BeginChild("Manage Animation", Vector2(0, 0), autoResizeChildFlags);
	
		RenderSectionHeader(animationName, 18);
		ImGui::SetCursorPos(Vector2(ImGui::GetCursorPosX() + 8, ImGui::GetCursorPos().y - 36));

		static std::string animationFilePath;
		if (GetFocusedAnimation() != nullptr)
			animationFilePath = GetFocusedAnimation()->animationPath;

		// Loading, saving and opening animation json files
		if (RenderImageButton("##NewAnimationFile", newFileTexture, Vector2(16, 16), 1, transparentColor))
		{
			animationFilePath = OpenSaveFileExplorer();

			if (animationFilePath != "")
			{
				// Create S_AnimationProperties struct to store the properties of the json file in
				std::shared_ptr<Animation::S_AnimationProperties> animationProperties = std::make_shared<Animation::S_AnimationProperties>();
				animationProperties->animationName = "New Animation";

				CreateNewAnimationFile(animationFilePath);
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
		if (RenderImageButton("#OpenAnim", openFileTexture, Vector2(16, 16), 1, transparentColor))
		{
			animationFilePath = OpenLoadFileExplorer();
			if (animationFilePath != "")
			{
				SetFocusedAnimation(LoadAnimationFile(animationFilePath));
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
		if (RenderImageButton("#SaveAnimationFile", saveFileTexture, Vector2(16, 16), 1, transparentColor))
		{
			if (animationFilePath != "")
				SaveAnimationFile(GetFocusedAnimation(), animationFilePath);
		}
		// Tooltip
		if (ImGui::BeginItemTooltip())
		{
			ImGui::Text("Save animation");
			ImGui::EndTooltip();
		}

		ImGui::SameLine(0, 2);
		if (RenderImageButton("#SaveAsAnimationFile", saveAsFileTexture, Vector2(16, 16), 1, transparentColor))
		{
			animationFilePath = OpenSaveFileExplorer();
			if (animationFilePath != "")
				SaveAnimationFile(GetFocusedAnimation(), animationFilePath);
		}
		// Tooltip
		if (ImGui::BeginItemTooltip())
		{
			ImGui::Text("Save animation as...");
			ImGui::EndTooltip();
		}

		std::shared_ptr<Animation::S_AnimationProperties> animProps = GetFocusedAnimation();

		const char* properties[] = { "- select property -", "Transform", "Sprite", "Camera", "Script", "Button", "Canvas", "Audio", "Text", "BoxCollider", "CircleCollider", "RigidBody", "CharacterController" };
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
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 12);
			std::shared_ptr<Animation> animation = nullptr;
			if (objectForFocusedAnimation != nullptr)
				animation = objectForFocusedAnimation->GetAnimationComponent();

			if (RenderCheckbox("Loop Animation", animProps->_loop) && animation != nullptr && animation->IsPlaying())
			{
				animation->Stop();
				animation->Play(GetEngineTime());
			}

			static std::string selected_property = "";			
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
			ImGui::Text("Add Properties:");
			std::vector<std::string> props = std::vector<std::string>();
			PushComboStyles();
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 34);
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
			PopComboStyles();
			ImGui::SameLine(0, 5);
			if (RenderButton("Add"))
			{
				L_PushBackKeyFrame(properties[current_property]);
				// Reset selector box to default
				current_property = 0;
			}
			ImGui::EndChild();

			// List properties on this animation


			ImGui::PushStyleColor(ImGuiCol_FrameBg, innerWindowColor);
			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, Vector2(0, 0));
			PushMenuStyles();

			// Conditionally begin the table
			if (animProps->transformProperties.size() > 0 ||
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
			if (ImGui::BeginTable("##AnimationProperties", 1, tableFlags))
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
			PopMenuStyles();
			ImGui::PopStyleVar();
			ImGui::PopStyleColor();
		}

		ImGui::EndChild(); // Animator Properties EndChild()

		ImGui::SameLine(0, 5);

		// Timeline Events
		ImGui::PushStyleColor(ImGuiCol_ChildBg, outerWindowColor);
		ImGui::BeginChild("##AnimationTimeline", Vector2(0, 0), childFlags);
		ImGui::PopStyleColor();

		RenderSectionHeader("Animation Timeline");

		ImGui::BeginChild("Property Header", Vector2(0,0), headerFlags);
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
			ImGui::GetWindowDrawList()->AddRectFilled(cursorScreen, Vector2(cursorScreen.x + textSize.x + 6, cursorScreen.y + textSize.y + 6), ImGui::GetColorU32(tableCellLightColor), 2);
			ImGui::SetCursorPos(Vector2(ImGui::GetCursorPosX() + 3, ImGui::GetCursorPosY() + 3));
			ImGui::Text(node_clicked.c_str());
		}
		
		if (node_clicked != "")
		{
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 6);
			if (RenderButton("Add Keyframe"))
				L_PushBackKeyFrame(node_clicked);
			ImGui::SameLine(0, 5);
			//ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3);
			if (RenderButton("Remove Property"))
				L_PushBackKeyFrame(node_clicked);
		}

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
				RenderImageButton(playID.c_str(), playTexture, Vector2(14, 14), 0, buttonColor, whiteColor, buttonHoveredColor, buttonActiveColor);
				ImGui::EndDisabled();
			}
			else
			{
				if (RenderImageButton(playID.c_str(), playTexture, Vector2(14, 14), 0, buttonColor, whiteColor, buttonHoveredColor, buttonActiveColor))
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
				RenderImageButton(stopID.c_str(), stopTexture, Vector2(14, 14), 0, buttonColor, whiteColor, buttonHoveredColor, buttonActiveColor);
				ImGui::EndDisabled();
			}
			else
			{
				if (RenderImageButton(stopID.c_str(), stopTexture, Vector2(14, 14), 0, buttonColor, whiteColor, buttonHoveredColor, buttonActiveColor))
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

		//// Save zero point for rendering the scrubber slider
		//Vector2 scrubberZeroPoint = ImGui::GetCursorScreenPos();

		//ImGui::PushStyleColor(ImGuiCol_ChildBg, scrubberBackground);
		//ImGui::BeginChild("Timeline Scrubber", Vector2(0,50), childFlags);
		//ImGui::PopStyleColor();
		

		// Render animator scrubber
		static float scrubberTime = (float)previewAnimationTime / 1000;
		static Vector2 keyFramePos = Vector2(scrubberTime, -0.05f);
		static float animatorGridStep = 50;

		// Lambda
		//auto L_RenderAnimationScrubber = [](Vector2& pipPosition, Vector2 zeroPoint, float gridStep)
		//	{
		//		std::string ID = "TimelineScrubber";
		//		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		//		float spriteTextureWidth = 14;
		//		float spriteTextureHeight = 26;
		//		Vector2 spriteOffset = Vector2(spriteTextureWidth / 2, 0);

		//		// If there is a valid Texture loaded into the Sprite Component
		//		if (keyFrameTexture != nullptr)
		//		{
		//			Vector2 pipStartingPoint = AddImageToDrawList(timelineScrubberTexture, pipPosition, zeroPoint, spriteTextureWidth, spriteTextureHeight, spriteOffset, Vector2(1, 1), false, gridStep, draw_list);					
		//			ImGui::SetCursorScreenPos(pipStartingPoint);
		//			ImGui::InvisibleButton(ID.c_str(), Vector2(spriteTextureWidth, spriteTextureHeight), ImGuiButtonFlags_MouseButtonLeft | 4096);
		//			const bool _isClicked = ImGui::IsItemClicked();
		//			const bool _isHovered = ImGui::IsItemHovered();
		//			const bool _isActive = ImGui::IsItemActive();   // Held

		//			if (_isActive || _isHovered)
		//			{
		//				// Mouse Hover Tooltip - Mouse Over Tooltip
		//				std::string keyTimeText = "Time: " + std::to_string(pipPosition.x) + " sec";
		//				Vector2 m = ImGui::GetIO().MousePos;
		//				ImGui::SetNextWindowPos(Vector2(m.x + 15, m.y + 5));
		//				ImGui::Begin("ScrubberTooltip", NULL, ImGuiWindowFlags_Tooltip | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar);
		//				ImGui::Text(keyTimeText.c_str());
		//				ImGui::End();
		//				ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_Hand);
		//			}

		//			if (_isActive && ImGui::IsMouseDragging(ImGuiMouseButton_Left, 0))
		//			{
		//				ImGuiIO& inputOutput = ImGui::GetIO();
		//				if (pipPosition.x + inputOutput.MouseDelta.x / gridStep >= 0)
		//				{
		//					pipPosition.x += inputOutput.MouseDelta.x / gridStep;
		//					previewAnimationTime = (int)(pipPosition.x * 1000);
		//				}
		//			}
		//		}
		//	};

		//L_RenderAnimationScrubber(keyFramePos, scrubberZeroPoint, animatorGridStep);

		//ImGui::EndChild();
		// Get Back to here

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
				animatorGridStep += 1;
			else if (scrollInput < 0 && animatorGridStep > 2)
				animatorGridStep -= 1;

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
		float animationLength = animProps->animationLength;
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

			// If there is a valid Texture loaded into the Sprite Component
			if (keyFrameTexture != nullptr)
			{
				Vector2 pipStartingPoint = AddImageToDrawList(keyFrameTexture, pipPosition, zeroPoint, 12, 12, Vector2(6, 6), Vector2(1, 1), _spriteScalesWithZoom, animatorGridStep, draw_list);

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


		// Timeline Events BeginChild()
		ImGui::EndChild();
		ImGui::End(); // Animator
	}

	void RenderAnimationPreview()
	{
		static Vector2 step = Vector2(50, 50);

		BeginWindow("Animator Preview", _showAnimationPreview);

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

			RenderViewObjects(focusedObjectVector, centerPoint, canvas_p0, canvas_sz, step.x);
		}

		EndWindow();
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
		BeginWindow("Keyframe Editor", _showKeyFrameEditor);

		std::string keyFrameProperty = "No KeyFrame Selected";
		if (selectedKeyFrameToEdit != nullptr)
			keyFrameProperty = selectedKeyFrameToEdit->name;

		RenderSectionHeader(keyFrameProperty);

		if (selectedKeyFrameToEdit != nullptr)
		{
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
				if (ImGui::InputText("##spritePath", newPath, IM_ARRAYSIZE(newPath)))
					sprite->path = newPath;
				ImGui::PopStyleColor();

				// Sprite offset variables
				float xOffset = sprite->xOffset;
				float yOffset = sprite->yOffset;
				ImGuiSliderFlags flags = ImGuiSliderFlags_::ImGuiSliderFlags_None;

				// Render Table
				if (PushTable("##AnimatedSpriteProperties", 2))
				{
					/*if (RenderFloatDragTableRow("##xSpriteScaleDrag" + std::to_string(id), "X Scale", xScale, 0.1f, 0.001f, 1000))
						sprite->SetScale(Vector2(xScale, yScale));
					if (RenderFloatDragTableRow("##ySpriteScaleDrag" + std::to_string(id), "Y Scale", yScale, 0.1f, 0.001f, 1000))
						sprite->SetScale(Vector2(xScale, yScale));*/
					if (RenderFloatDragTableRow("##AnimatedxSpriteOffsetDrag", "X Offset", xOffset, 0.1f, -FLT_MAX, -FLT_MAX))
						sprite->xOffset = xOffset;
					if (RenderFloatDragTableRow("##AnimatedySpriteOffsetDrag", "Y Offset", yOffset, 0.1f, -FLT_MAX, -FLT_MAX))
						sprite->yOffset = yOffset;
					/*	if (RenderIntDragTableRow("##AnimatedrenderOrder", "Render Order", renderOrder, 1, 0, (int)maxSpriteLayers))
						sprite->SetRenderOrder(renderOrder);*/
					PopTable();
				}
			}
		}
		else
		{	
			ImGui::TextWrapped("Select a keyframe to edit from the Animation Timeline...");
		}

		EndWindow();
	}

	void CreateNewAnimationFile(std::string path)
	{
		// Declare file and input stream
		std::ofstream file_obj;

		// Delete old contents of the file
		file_obj.open(path, std::ofstream::out | std::ofstream::trunc);
		file_obj.close();

		// Opening file in append mode
		file_obj.open(path, std::ios::app);

		// Array that will hold our gameObject json objects
		json animationObjectsJsonArray;

		// Set animationObjectsJsonArray to empty
		animationObjectsJsonArray.push_back("NULL");

		// Create the GameObjects json object and add the empty array as the content
		json newFileObject = json::object({ {"Animation Properties", animationObjectsJsonArray } });

		// Add the GameObjects object contents to the file
		file_obj << newFileObject.dump(4).c_str() << std::endl;

		// Close the file
		file_obj.close();
	}

	void SaveAnimationFile(std::shared_ptr<Animation::S_AnimationProperties> propertiesObject, std::string path)
	{
		// Declare file and input stream
		std::ofstream file_obj;
		std::ifstream ifstream(path);

		// Delete old contents of the file
		file_obj.open(path, std::ofstream::out | std::ofstream::trunc);
		file_obj.close();

		// Opening file in append mode
		file_obj.open(path, std::ios::app);

		// Array that will hold our gameObject json objects
		json animationProperties;

		// Create Animation Property Json data object
		json animationName = json::object({
			{ "Name", propertiesObject->animationName },
			{ "Length", propertiesObject->animationLength },
			{ "Loop", propertiesObject->_loop }
			});
		animationProperties.push_back(animationName);


		for (std::shared_ptr<Animation::S_Transform> transformProp : propertiesObject->transformProperties)
		{
			// Declare components array json object for components
			json transformPropertiesArray = json::array();

			// Get the objects fields
			json jsonData = {
				{ "transformInterpType", transformProp->transformInterpType },
				{ "transformSpeed", transformProp->transformSpeed },
				{ "scaleInterpType", transformProp->scaleInterpType },
				{ "scaleSpeed", transformProp->scaleSpeed },
				{ "time", transformProp->time },
				{ "xMove", transformProp->xMove },
				{ "yMove", transformProp->yMove },
				{ "xScale", transformProp->xScale },
				{ "yScale", transformProp->yScale }
			};

			// Dumped json object with required data for saving
			std::string data = jsonData.dump();

			// Save to the json array
			transformPropertiesArray.push_back(json::parse(data));

			// Create Animation Property Json data object
			json animationProperty = json::object({
				{ "Property", "Transform" },
				{ "Frames", transformPropertiesArray }
				});

			// Finally, add the Animation Property json to the animationProperties
			animationProperties.push_back(animationProperty);
		}
		for (std::shared_ptr<Animation::S_Sprite> spriteProp : propertiesObject->spriteProperties)
		{
			// Declare components array json object for components
			json spritePropertiesArray = json::array();

			// Get the objects fields
			json jsonData = {
				{ "interpType", spriteProp->interpType },
				{ "speed", spriteProp->speed },
				{ "time", spriteProp->time },
				{ "path", spriteProp->path },
				{ "xOffset", spriteProp->xOffset },
				{ "yOffset", spriteProp->yOffset }
			};

			// Dumped json object with required data for saving
			std::string data = jsonData.dump();

			// Save to the json array
			spritePropertiesArray.push_back(json::parse(data));

			// Create Animation Property Json data object
			json animationProperty = json::object({
				{ "Property", "Sprite" },
				{ "Frames", spritePropertiesArray }
				});

			// Finally, add the Animation Property json to the animationProperties
			animationProperties.push_back(animationProperty);
		}
		for (std::shared_ptr<Animation::S_Camera> cameraProp : propertiesObject->cameraProperties)
		{
			// Declare components array json object for components
			json cameraPropertiesArray = json::array();

			// Get the objects fields
			json jsonData = {
				{ "time", cameraProp->time },
				{ "_isPrimaryCamera", cameraProp->_isPrimaryCamera }
			};

			// Dumped json object with required data for saving
			std::string data = jsonData.dump();

			// Save to the json array
			cameraPropertiesArray.push_back(json::parse(data));

			// Create Animation Property Json data object
			json animationProperty = json::object({
				{ "Property", "Camera" },
				{ "Frames", cameraPropertiesArray }
				});

			// Finally, add the Animation Property json to the animationProperties
			animationProperties.push_back(animationProperty);
		}
		for (std::shared_ptr<Animation::S_Script> scriptProp : propertiesObject->scriptProperties)
		{
			// Declare components array json object for components
			json scriptPropertiesArray = json::array();

			// Get the objects fields
			json jsonData = {
				{ "time", scriptProp->time },
				{ "path", scriptProp->path }
			};

			// Dumped json object with required data for saving
			std::string data = jsonData.dump();

			// Save to the json array
			scriptPropertiesArray.push_back(json::parse(data));

			// Create Animation Property Json data object
			json animationProperty = json::object({
				{ "Property", "Script" },
				{ "Frames", scriptPropertiesArray }
				});

			// Finally, add the Animation Property json to the animationProperties
			animationProperties.push_back(animationProperty);
		}
		for (std::shared_ptr<Animation::S_Button> buttonProp : propertiesObject->buttonProperties)
		{
			// Declare components array json object for components
			json buttonPropertiesArray = json::array();

			// Get the objects fields
			json jsonData = {
				{ "time", buttonProp->time },
				{ "_isActive", buttonProp->_isActive }
			};

			// Dumped json object with required data for saving
			std::string data = jsonData.dump();

			// Save to the json array
			buttonPropertiesArray.push_back(json::parse(data));

			// Create Animation Property Json data object
			json animationProperty = json::object({
				{ "Property", "Button" },
				{ "Frames", buttonPropertiesArray }
				});

			// Finally, add the Animation Property json to the animationProperties
			animationProperties.push_back(animationProperty);
		}
		for (std::shared_ptr<Animation::S_Canvas> canvasProp : propertiesObject->canvasProperties)
		{
			// Declare components array json object for components
			json canvasPropertiesArray = json::array();

			// Get the objects fields
			json jsonData = {
				{ "time", canvasProp->time }
			};

			// Dumped json object with required data for saving
			std::string data = jsonData.dump();

			// Save to the json array
			canvasPropertiesArray.push_back(json::parse(data));

			// Create Animation Property Json data object
			json animationProperty = json::object({
				{ "Property", "Canvas" },
				{ "Frames", canvasPropertiesArray }
				});

			// Finally, add the Animation Property json to the animationProperties
			animationProperties.push_back(animationProperty);
		}
		for (std::shared_ptr<Animation::S_Audio> audioProp : propertiesObject->audioProperties)
		{
			// Declare components array json object for components
			json audioPropertiesArray = json::array();

			// Get the objects fields
			json jsonData = {
				{ "time", audioProp->time },
				{ "path", audioProp->path },
				{ "_isMusic", audioProp->_isMusic }
			};

			// Dumped json object with required data for saving
			std::string data = jsonData.dump();

			// Save to the json array
			audioPropertiesArray.push_back(json::parse(data));

			// Create Animation Property Json data object
			json animationProperty = json::object({
				{ "Property", "Audio" },
				{ "Frames", audioPropertiesArray }
				});

			// Finally, add the Animation Property json to the animationProperties
			animationProperties.push_back(animationProperty);
		}
		for (std::shared_ptr<Animation::S_Text> textProp : propertiesObject->textProperties)
		{
			// Declare components array json object for components
			json textPropertiesArray = json::array();

			// Get the objects fields
			json jsonData = {
				{ "time", textProp->time },
				{ "path", textProp->path },
				{ "text", textProp->text },
				{ "color", textProp->color }
			};

			// Dumped json object with required data for saving
			std::string data = jsonData.dump();

			// Save to the json array
			textPropertiesArray.push_back(json::parse(data));

			// Create Animation Property Json data object
			json animationProperty = json::object({
				{ "Property", "Text" },
				{ "Frames", textPropertiesArray }
				});

			// Finally, add the Animation Property json to the animationProperties
			animationProperties.push_back(animationProperty);
		}
		for (std::shared_ptr<Animation::S_BoxCollider> boxColliderProp : propertiesObject->boxColliderProperties)
		{
			// Declare components array json object for components
			json boxColliderPropertiesArray = json::array();

			// Get the objects fields
			json jsonData = {
				{ "time", boxColliderProp->time },
				{ "_isActive", boxColliderProp->_isActive }
			};

			// Dumped json object with required data for saving
			std::string data = jsonData.dump();

			// Save to the json array
			boxColliderPropertiesArray.push_back(json::parse(data));

			// Create Animation Property Json data object
			json animationProperty = json::object({
				{ "Property", "BoxCollider" },
				{ "Frames", boxColliderPropertiesArray }
				});

			// Finally, add the Animation Property json to the animationProperties
			animationProperties.push_back(animationProperty);
		}
		for (std::shared_ptr<Animation::S_CircleCollider> circleColliderProp : propertiesObject->circleColliderProperties)
		{
			// Declare components array json object for components
			json circleColliderPropertiesArray = json::array();

			// Get the objects fields
			json jsonData = {
				{ "time", circleColliderProp->time },
				{ "_isActive", circleColliderProp->_isActive }
			};

			// Dumped json object with required data for saving
			std::string data = jsonData.dump();

			// Save to the json array
			circleColliderPropertiesArray.push_back(json::parse(data));

			// Create Animation Property Json data object
			json animationProperty = json::object({
				{ "Property", "CircleCollider" },
				{ "Frames", circleColliderPropertiesArray }
				});

			// Finally, add the Animation Property json to the animationProperties
			animationProperties.push_back(animationProperty);
		}
		for (std::shared_ptr<Animation::S_RigidBody> rigidBodyProp : propertiesObject->rigidBodyProperties)
		{
			// Declare components array json object for components
			json rigidBodyPropertiesArray = json::array();

			// Get the objects fields
			json jsonData = {
				{ "time", rigidBodyProp->time },
				{ "interpType", rigidBodyProp->interpType },
				{ "speed", rigidBodyProp->speed },
				{ "_isActive", rigidBodyProp->_isActive },
				{ "gravityScale", rigidBodyProp->gravityScale },
			};

			// Dumped json object with required data for saving
			std::string data = jsonData.dump();

			// Save to the json array
			rigidBodyPropertiesArray.push_back(json::parse(data));

			// Create Animation Property Json data object
			json animationProperty = json::object({
				{ "Property", "RigidBody" },
				{ "Frames", rigidBodyPropertiesArray }
				});

			// Finally, add the Animation Property json to the animationProperties
			animationProperties.push_back(animationProperty);
		}
		for (std::shared_ptr<Animation::S_CharacterController> characterControllerProp : propertiesObject->characterControllerProperties)
		{
			// Declare components array json object for components
			json characterControllerPropertiesArray = json::array();

			// Get the objects fields
			json jsonData = {
				{ "time", characterControllerProp->time },
				{ "_isActive", characterControllerProp->_isActive }
			};

			// Dumped json object with required data for saving
			std::string data = jsonData.dump();

			// Save to the json array
			characterControllerPropertiesArray.push_back(json::parse(data));

			// Create Animation Property Json data object
			json animationProperty = json::object({
				{ "Property", "CharacterController" },
				{ "Frames", characterControllerPropertiesArray }
				});

			// Finally, add the Animation Property json to the animationProperties
			animationProperties.push_back(animationProperty);
		}

		// Recreate the Animation Property json object and add the array as the content
		json newFileObject = json::object({ {"Animation Properties", animationProperties } });

		// Add the GameObjects object contents to the file
		file_obj << newFileObject.dump(4).c_str() << std::endl;

		// Close the file
		file_obj.close();
	}
}	
}