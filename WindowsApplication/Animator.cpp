#include "FlatEngine.h"
#include "Camera.h"
#include "Transform.h"
#include "Sprite.h"
#include "imgui.h"
#include "Text.h"

namespace FlatEngine { namespace FlatGui {

	int animationTime = 0;
	ImVec4 darker = ImVec4(float(0.2), float(0.2), float(0.2), float(1));
	ImVec4 lighter = ImVec4(float(0.8), float(0.8), float(0.8), float(1));
	ImVec4 light = ImVec4(float(0.7), float(0.7), float(0.7), float(1));
	ImVec4 dark = ImVec4(float(0.3), float(0.3), float(0.3), float(1));
	ImVec4 white = ImVec4(float(0.9), float(0.9), float(0.9), float(1));
	ImVec4 transformAnimationNode = ImVec4(float(0.1), float(0.76), float(0.08), float(.8));


	void RenderAnimator()
	{
		// 16 | 8 are flags for noScrollbar and noscrollwithmouse
		ImGui::Begin("Animator", 0, 16 | 8);

		ImGuiChildFlags padding_child_flags = ImGuiChildFlags_::ImGuiChildFlags_AlwaysUseWindowPadding;

		// Animated Properties BeginChild()
		//
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ChildBg, outerWindowColor);
		ImGui::BeginChild("Animated Properties", ImVec2(ImGui::GetContentRegionMax().x / 3, 0), padding_child_flags | ImGuiChildFlags_ResizeX);
		ImGui::PopStyleColor();

		std::string animationName = "-No Animation Selected-";

		if (GetFocusedAnimation()->animationName != "")
			animationName = GetFocusedAnimation()->animationName;


		////////////////
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
		ImGui::SameLine(0, 15);
		if (ImGui::ImageButton("##NewAnimationFile", newFileTexture, ImVec2(12, 12), uv0, uv1, bg_col, tint_col))
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
		if (ImGui::ImageButton("##OpenAnimationFile", openFileTexture, ImVec2(12, 12), uv0, uv1, bg_col, tint_col))
		{
			std::string animationFilePath = OpenLoadFileExplorer();
			if (animationFilePath != "")
			{
				SetFocusedAnimation(LoadAnimationFile(animationFilePath));
			}
		}
		ImGui::SameLine(0, 5);
		if (ImGui::ImageButton("#SaveAnimationFile", saveFileTexture, ImVec2(12, 12), uv0, uv1, bg_col, tint_col))
		{
			std::string animationFilePath = OpenSaveFileExplorer();
			if (animationFilePath != "")
			{
				SaveAnimationFile(GetFocusedAnimation(), animationFilePath);
			}
		}


		ImGui::EndChild();


		if (GetFocusedAnimation()->animationName != "")
		{
			static std::string selected_property = "";

			ImGui::Separator();
			ImGui::PushStyleColor(ImGuiCol_ChildBg, innerWindowColor);
			ImGui::BeginChild("Properties Bar", ImVec2(0, 0), child_flags);
			ImGui::PopStyleColor();
			ImGui::Text("Add Properties");
			const char* properties[] = { "Add Property", "Transform", "Sprite", "Camera", "Script", "Button", "Canvas", "Audio", "Text", "BoxCollider", "CircleCollider", "RigidBody", "CharacterController" };
			std::vector<std::string> props = std::vector<std::string>();
			static int current_property = 0;
			//ImGui::BeginCombo("##properties", &current_property, &props.front(), props.size());
			if (ImGui::BeginCombo("##properties", properties[current_property]))
			{
				for (int n = 0; n < IM_ARRAYSIZE(properties); n++)
				{
					if (GetFocusedAnimation()->transformProperties.size() == 0 && properties[n] == "Transform" ||
						GetFocusedAnimation()->spriteProperties.size() == 0 && properties[n] == "Sprite" ||
						GetFocusedAnimation()->cameraProperties.size() == 0 && properties[n] == "Camera" ||
						GetFocusedAnimation()->scriptProperties.size() == 0 && properties[n] == "Script" ||
						GetFocusedAnimation()->buttonProperties.size() == 0 && properties[n] == "Button" ||
						GetFocusedAnimation()->canvasProperties.size() == 0 && properties[n] == "Canvas" ||
						GetFocusedAnimation()->audioProperties.size() == 0 && properties[n] == "Audio" ||
						GetFocusedAnimation()->textProperties.size() == 0 && properties[n] == "Text" ||
						GetFocusedAnimation()->boxColliderProperties.size() == 0 && properties[n] == "BoxCollider" ||
						GetFocusedAnimation()->circleColliderProperties.size() == 0 && properties[n] == "CircleCollider" ||
						GetFocusedAnimation()->rigidBodyProperties.size() == 0 && properties[n] == "RigidBody" ||
						GetFocusedAnimation()->characterControllerProperties.size() == 0 && properties[n] == "CharacterController"
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
				// Add property to animation object
				if (properties[current_property] == "Transform" && GetFocusedAnimation()->transformProperties.size() == 0)
				{
					Animation::S_Transform transformProperties;
					GetFocusedAnimation()->transformProperties.push_back(transformProperties);
				}
				else if (properties[current_property] == "Sprite" && GetFocusedAnimation()->spriteProperties.size() == 0)
				{
					Animation::S_Sprite spriteProperties;
					GetFocusedAnimation()->spriteProperties.push_back(spriteProperties);
				}
				else if (properties[current_property] == "Camera" && GetFocusedAnimation()->cameraProperties.size() == 0)
				{
					Animation::S_Camera cameraProperties;
					GetFocusedAnimation()->cameraProperties.push_back(cameraProperties);
				}
				else if (properties[current_property] == "Script" && GetFocusedAnimation()->scriptProperties.size() == 0)
				{
					Animation::S_Script scriptProperties;
					GetFocusedAnimation()->scriptProperties.push_back(scriptProperties);
				}
				else if (properties[current_property] == "Button" && GetFocusedAnimation()->buttonProperties.size() == 0)
				{
					Animation::S_Button buttonProperties;
					GetFocusedAnimation()->buttonProperties.push_back(buttonProperties);
				}
				else if (properties[current_property] == "Canvas" && GetFocusedAnimation()->canvasProperties.size() == 0)
				{
					Animation::S_Canvas canvasProperties;
					GetFocusedAnimation()->canvasProperties.push_back(canvasProperties);
				}
				else if (properties[current_property] == "Audio" && GetFocusedAnimation()->audioProperties.size() == 0)
				{
					Animation::S_Audio audioProperties;
					GetFocusedAnimation()->audioProperties.push_back(audioProperties);
				}
				else if (properties[current_property] == "Text" && GetFocusedAnimation()->textProperties.size() == 0)
				{
					Animation::S_Text textProperties;
					GetFocusedAnimation()->textProperties.push_back(textProperties);
				}
				else if (properties[current_property] == "BoxCollider" && GetFocusedAnimation()->boxColliderProperties.size() == 0)
				{
					Animation::S_BoxCollider boxColliderProperties;
					GetFocusedAnimation()->boxColliderProperties.push_back(boxColliderProperties);
				}
				else if (properties[current_property] == "CircleCollider" && GetFocusedAnimation()->circleColliderProperties.size() == 0)
				{
					Animation::S_CircleCollider circleColliderProperties;
					GetFocusedAnimation()->circleColliderProperties.push_back(circleColliderProperties);
				}
				else if (properties[current_property] == "RigidBody" && GetFocusedAnimation()->rigidBodyProperties.size() == 0)
				{
					Animation::S_RigidBody rigidBodyProperties;
					GetFocusedAnimation()->rigidBodyProperties.push_back(rigidBodyProperties);
				}
				else if (properties[current_property] == "CharacterController" && GetFocusedAnimation()->characterControllerProperties.size() == 0)
				{
					Animation::S_CharacterController characterControllerProperties;
					GetFocusedAnimation()->characterControllerProperties.push_back(characterControllerProperties);
				}

				// Reset selector box to default
				current_property = 0;
			}
			ImGui::EndChild();


			//////// List properties on this animation
			ImGui::PushStyleColor(ImGuiCol_FrameBg, innerWindowColor);
			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0, -5));
			ImGui::BeginListBox("##AnimationProperties", ImVec2(-FLT_MIN, -FLT_MIN));
			ImGui::PopStyleVar();
			ImGui::PopStyleColor();

			ImGuiTreeNodeFlags node_flags;
			static std::string node_clicked = "";

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, 0.0f });

			///// Transform Node
			//// If this node is selected, use the nodeFlag_selected to highlight it
			if (node_clicked == "Transform")
				node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_Selected;
			else
				node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;
			//// TreeNode Opener - No TreePop because it's a leaf
			if (GetFocusedAnimation()->transformProperties.size() > 0)
				ImGui::TreeNodeEx((void*)(intptr_t)"TransformNode", node_flags, "Transform");
			if (ImGui::IsItemClicked())
				node_clicked = "Transform";

			///// Sprite Node
			//// If this node is selected, use the nodeFlag_selected to highlight it
			if (node_clicked == "Sprite")
				node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_Selected;
			else
				node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;
			//// TreeNode Opener - No TreePop because it's a leaf
			if (GetFocusedAnimation()->spriteProperties.size() > 0)
			{
				ImGui::TreeNodeEx((void*)(intptr_t)"SpriteNode", node_flags, "Sprite");
				if (ImGui::IsItemClicked())
					node_clicked = "Sprite";
			}


			///// Camera Node
			//// If this node is selected, use the nodeFlag_selected to highlight it
			if (node_clicked == "Camera")
				node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_Selected;
			else
				node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;
			//// TreeNode Opener - No TreePop because it's a leaf
			if (GetFocusedAnimation()->cameraProperties.size() > 0)
			{
				ImGui::TreeNodeEx((void*)(intptr_t)"CameraNode", node_flags, "Camera");
				if (ImGui::IsItemClicked())
					node_clicked = "Camera";
			}

			///// Script Node
			//// If this node is selected, use the nodeFlag_selected to highlight it
			if (node_clicked == "Script")
				node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_Selected;
			else
				node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;
			//// TreeNode Opener - No TreePop because it's a leaf
			if (GetFocusedAnimation()->scriptProperties.size() > 0)
			{
				ImGui::TreeNodeEx((void*)(intptr_t)"ScriptNode", node_flags, "Script");
				if (ImGui::IsItemClicked())
					node_clicked = "Script";
			}

			///// Button Node
			//// If this node is selected, use the nodeFlag_selected to highlight it
			if (node_clicked == "Button")
				node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_Selected;
			else
				node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;
			//// TreeNode Opener - No TreePop because it's a leaf
			if (GetFocusedAnimation()->buttonProperties.size() > 0)
			{
				ImGui::TreeNodeEx((void*)(intptr_t)"ButtonNode", node_flags, "Button");
				if (ImGui::IsItemClicked())
					node_clicked = "Button";
			}

			///// Canvas Node
			//// If this node is selected, use the nodeFlag_selected to highlight it
			if (node_clicked == "Canvas")
				node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_Selected;
			else
				node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;
			//// TreeNode Opener - No TreePop because it's a leaf
			if (GetFocusedAnimation()->canvasProperties.size() > 0)
			{
				ImGui::TreeNodeEx((void*)(intptr_t)"CanvasNode", node_flags, "Canvas");
				if (ImGui::IsItemClicked())
					node_clicked = "Canvas";
			}

			///// Audio Node
			//// If this node is selected, use the nodeFlag_selected to highlight it
			if (node_clicked == "Audio")
				node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_Selected;
			else
				node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;
			//// TreeNode Opener - No TreePop because it's a leaf
			if (GetFocusedAnimation()->audioProperties.size() > 0)
			{
				ImGui::TreeNodeEx((void*)(intptr_t)"AudioNode", node_flags, "Audio");
				if (ImGui::IsItemClicked())
					node_clicked = "Audio";
			}

			///// Text Node
			//// If this node is selected, use the nodeFlag_selected to highlight it
			if (node_clicked == "Text")
				node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_Selected;
			else
				node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;
			//// TreeNode Opener - No TreePop because it's a leaf
			if (GetFocusedAnimation()->textProperties.size() > 0)
			{
				ImGui::TreeNodeEx((void*)(intptr_t)"TextNode", node_flags, "Text");
				if (ImGui::IsItemClicked())
					node_clicked = "Text";
			}

			///// BoxCollider Node
			//// If this node is selected, use the nodeFlag_selected to highlight it
			if (node_clicked == "BoxCollider")
				node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_Selected;
			else
				node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;
			//// TreeNode Opener - No TreePop because it's a leaf
			if (GetFocusedAnimation()->boxColliderProperties.size() > 0)
			{
				ImGui::TreeNodeEx((void*)(intptr_t)"BoxColliderNode", node_flags, "BoxCollider");
				if (ImGui::IsItemClicked())
					node_clicked = "BoxCollider";
			}

			///// CircleCollider Node
			//// If this node is selected, use the nodeFlag_selected to highlight it
			if (node_clicked == "CircleCollider")
				node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_Selected;
			else
				node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;
			//// TreeNode Opener - No TreePop because it's a leaf
			if (GetFocusedAnimation()->circleColliderProperties.size() > 0)
			{
				ImGui::TreeNodeEx((void*)(intptr_t)"CircleColliderNode", node_flags, "CircleCollider");
				if (ImGui::IsItemClicked())
					node_clicked = "CircleCollider";
			}

			///// RigidBody Node
			//// If this node is selected, use the nodeFlag_selected to highlight it
			if (node_clicked == "RigidBody")
				node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_Selected;
			else
				node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;
			//// TreeNode Opener - No TreePop because it's a leaf
			if (GetFocusedAnimation()->rigidBodyProperties.size() > 0)
			{
				ImGui::TreeNodeEx((void*)(intptr_t)"RigidBodyNode", node_flags, "RigidBody");
				if (ImGui::IsItemClicked())
					node_clicked = "RigidBody";
			}

			///// CharacterController Node
			//// If this node is selected, use the nodeFlag_selected to highlight it
			if (node_clicked == "CharacterController")
				node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_Selected;
			else
				node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;
			//// TreeNode Opener - No TreePop because it's a leaf
			if (GetFocusedAnimation()->characterControllerProperties.size() > 0)
			{
				ImGui::TreeNodeEx((void*)(intptr_t)"CharacterControllerNode", node_flags, "CharacterController");
				if (ImGui::IsItemClicked())
					node_clicked = "CharacterController";
			}

			ImGui::PopStyleVar();
			ImGui::EndListBox();
		}


		// Animator Properties EndChild()
		ImGui::EndChild();



		ImGui::SameLine(0, 5);



		// Timeline Events BeginChild()
		// 
		ImGui::PushStyleColor(ImGuiCol_ChildBg, innerWindowColor);
		ImGui::BeginChild("Timeline Events", ImVec2(0, 0), padding_child_flags);
		ImGui::PopStyleColor();

		////////////////
		ImGui::PushStyleColor(ImGuiCol_ChildBg, white);
		ImGui::BeginChild("Animation Timeline", ImVec2(0, 0), child_flags);
		ImGui::PopStyleColor();
		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(1, 9, 0, 255));
		ImGui::Text("Animation Timeline");
		ImGui::PopStyleColor();
		ImGui::EndChild();
		////////////////


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

		// For panning the scene view
		const float mouse_threshold_for_pan = 0.0f;
		if (is_active && ImGui::IsMouseDragging(ImGuiMouseButton_Right, mouse_threshold_for_pan))
		{
			scrolling.x += inputOutput.MouseDelta.x;
			scrolling.y += inputOutput.MouseDelta.y;
		}

		static float animatorGridStep = 50;

		Vector2 mousePos = Vector2(inputOutput.MousePos.x, inputOutput.MousePos.y);
		float scrollInput = inputOutput.MouseWheel;
		float weight = 0.08f;

		// Change scrolling offset based on mouse position and weight
		if (is_hovered)
		{
			if (scrollInput > 0)
			{
				animatorGridStep += 1;
			}
			else if (scrollInput < 0 && animatorGridStep > 2)
			{
				animatorGridStep -= 1;
			}
		}

		if (scrolling.x > 0)
			scrolling.x = 0;
		if (scrolling.y > 0)
			scrolling.y = 0;
		if (scrolling.y < -1500)
			scrolling.y = -1500;

		ImVec2 zeroPoint = ImVec2(0, 0);


		RenderAnimationTimelineGrid(zeroPoint, scrolling, canvas_p0, canvas_p1, canvas_sz, animatorGridStep);


		// Get all keyFramePip positions
		//
		float propertyYPos = -0.5f; // Value in grid space
		int propertyCounter = 0;    // For values in screenspace below
		float animationLength = GetFocusedAnimation()->animationLength;

		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		LogFloat(scrolling.y, "Scrolling y");
		LogFloat(animatorGridStep, "Grid step: ");
		// Draw colored box for transform keyframes
		if (GetFocusedAnimation()->transformProperties.size() > 0)
		{
			float topYPos = canvas_p0.y + scrolling.y + (propertyCounter * animatorGridStep);
			float bottomYPos = topYPos + animatorGridStep;
			// prevents being drawn off screen and introducing scrollbar
			if (topYPos > canvas_p1.y)
				topYPos = canvas_p1.y;
			if (bottomYPos > canvas_p1.y)
				bottomYPos = canvas_p1.y;
			ImVec2 topLeftCorner = ImVec2(canvas_p0.x, topYPos);
			ImVec2 bottomRightCorner = ImVec2(canvas_p1.x, bottomYPos);
			ImU32 transformRectColor = IM_COL32(214, 8, 118, 100);
			draw_list->AddRectFilled(topLeftCorner, bottomRightCorner, transformRectColor);

			// Draw pips
			for (Animation::S_Transform& transformFrame : GetFocusedAnimation()->transformProperties)
			{
				// Get keyFrame time and convert to seconds
				float keyFrameX = transformFrame.time / 1000;
				Vector2 keyFramePos = Vector2(keyFrameX, propertyYPos);
				if (zeroPoint.y + (propertyYPos * animatorGridStep * -1) < canvas_p1.y && zeroPoint.y + (propertyYPos * animatorGridStep * -1) + 6 < canvas_p1.y)
					RenderAnimationTimelineKeyFrames(keyFramePos, zeroPoint, scrolling, canvas_p0, canvas_p1, canvas_sz, animatorGridStep);
			}
			propertyYPos--;
			propertyCounter++;
		}

		// Draw colored box for sprite keyframes
		if (GetFocusedAnimation()->spriteProperties.size() > 0)
		{
			float topYPos = canvas_p0.y + scrolling.y + (propertyCounter * animatorGridStep);
			float bottomYPos = topYPos + animatorGridStep;
			// prevents being drawn off screen and introducing scrollbar
			if (topYPos > canvas_p1.y)
				topYPos = canvas_p1.y;
			if (bottomYPos > canvas_p1.y)
				bottomYPos = canvas_p1.y;
			ImVec2 topLeftCorner = ImVec2(canvas_p0.x, topYPos);
			ImVec2 bottomRightCorner = ImVec2(canvas_p1.x, bottomYPos);
			ImU32 spriteRectColor = IM_COL32(83, 214, 8, 100);
			draw_list->AddRectFilled(topLeftCorner, bottomRightCorner, spriteRectColor);

			// Draw pips
			for (Animation::S_Sprite& spriteFrame : GetFocusedAnimation()->spriteProperties)
			{
				// Get keyFrame time and convert to seconds
				float keyFrameX = spriteFrame.time / 1000;
				Vector2 keyFramePos = Vector2(keyFrameX, propertyYPos);
				if (zeroPoint.y + (propertyYPos * animatorGridStep * -1) < canvas_p1.y && zeroPoint.y + (propertyYPos * animatorGridStep * -1) + 6 < canvas_p1.y)
					RenderAnimationTimelineKeyFrames(keyFramePos, zeroPoint, scrolling, canvas_p0, canvas_p1, canvas_sz, animatorGridStep);
			}
			propertyYPos--;
			propertyCounter++;
		}

		// Draw colored box for camera keyframes
		if (GetFocusedAnimation()->cameraProperties.size() > 0)
		{
			float topYPos = canvas_p0.y + scrolling.y + (propertyCounter * animatorGridStep);
			float bottomYPos = topYPos + animatorGridStep;
			// prevents being drawn off screen and introducing scrollbar
			if (topYPos > canvas_p1.y)
				topYPos = canvas_p1.y;
			if (bottomYPos > canvas_p1.y)
				bottomYPos = canvas_p1.y;
			ImVec2 topLeftCorner = ImVec2(canvas_p0.x, topYPos);
			ImVec2 bottomRightCorner = ImVec2(canvas_p1.x, bottomYPos);
			ImU32 cameraRectColor = IM_COL32(206, 108, 4, 100);
			draw_list->AddRectFilled(topLeftCorner, bottomRightCorner, cameraRectColor);

			// Draw pips
			for (Animation::S_Camera& cameraFrame : GetFocusedAnimation()->cameraProperties)
			{
				// Get keyFrame time and convert to seconds
				float keyFrameX = cameraFrame.time / 1000;
				Vector2 keyFramePos = Vector2(keyFrameX, propertyYPos);
				if (zeroPoint.y + (propertyYPos * animatorGridStep * -1) < canvas_p1.y && zeroPoint.y + (propertyYPos * animatorGridStep * -1) + 6 < canvas_p1.y)
					RenderAnimationTimelineKeyFrames(keyFramePos, zeroPoint, scrolling, canvas_p0, canvas_p1, canvas_sz, animatorGridStep);
			}
			propertyYPos--;
			propertyCounter++;
		}

		// Draw colored box for script keyframes
		if (GetFocusedAnimation()->scriptProperties.size() > 0)
		{
			float topYPos = canvas_p0.y + scrolling.y + (propertyCounter * animatorGridStep);
			float bottomYPos = topYPos + animatorGridStep;
			// prevents being drawn off screen and introducing scrollbar
			if (topYPos > canvas_p1.y)
				topYPos = canvas_p1.y;
			if (bottomYPos > canvas_p1.y)
				bottomYPos = canvas_p1.y;
			ImVec2 topLeftCorner = ImVec2(canvas_p0.x, topYPos);
			ImVec2 bottomRightCorner = ImVec2(canvas_p1.x, bottomYPos);
			ImU32 scriptRectColor = IM_COL32(4, 159, 206, 100);
			draw_list->AddRectFilled(topLeftCorner, bottomRightCorner, scriptRectColor);

			// Draw pips
			for (Animation::S_Script& scriptFrame : GetFocusedAnimation()->scriptProperties)
			{
				// Get keyFrame time and convert to seconds
				float keyFrameX = scriptFrame.time / 1000;
				Vector2 keyFramePos = Vector2(keyFrameX, propertyYPos);
				if (zeroPoint.y + (propertyYPos * animatorGridStep * -1) < canvas_p1.y && zeroPoint.y + (propertyYPos * animatorGridStep * -1) + 6 < canvas_p1.y)
					RenderAnimationTimelineKeyFrames(keyFramePos, zeroPoint, scrolling, canvas_p0, canvas_p1, canvas_sz, animatorGridStep);
			}
			propertyYPos--;
			propertyCounter++;
		}

		// Draw colored box for button keyframes
		if (GetFocusedAnimation()->buttonProperties.size() > 0)
		{
			float topYPos = canvas_p0.y + scrolling.y + (propertyCounter * animatorGridStep);
			float bottomYPos = topYPos + animatorGridStep;
			// prevents being drawn off screen and introducing scrollbar
			if (topYPos > canvas_p1.y)
				topYPos = canvas_p1.y;
			if (bottomYPos > canvas_p1.y)
				bottomYPos = canvas_p1.y;
			ImVec2 topLeftCorner = ImVec2(canvas_p0.x, topYPos);
			ImVec2 bottomRightCorner = ImVec2(canvas_p1.x, bottomYPos);
			ImU32 buttonRectColor = IM_COL32(152, 16, 198, 100);
			draw_list->AddRectFilled(topLeftCorner, bottomRightCorner, buttonRectColor);

			// Draw pips
			for (Animation::S_Button& buttonFrame : GetFocusedAnimation()->buttonProperties)
			{
				// Get keyFrame time and convert to seconds
				float keyFrameX = buttonFrame.time / 1000;
				Vector2 keyFramePos = Vector2(keyFrameX, propertyYPos);
				if (zeroPoint.y + (propertyYPos * animatorGridStep * -1) < canvas_p1.y && zeroPoint.y + (propertyYPos * animatorGridStep * -1) + 6 < canvas_p1.y)
					RenderAnimationTimelineKeyFrames(keyFramePos, zeroPoint, scrolling, canvas_p0, canvas_p1, canvas_sz, animatorGridStep);
			}
			propertyYPos--;
			propertyCounter++;
		}

		// Draw colored box for canvas keyframes
		if (GetFocusedAnimation()->canvasProperties.size() > 0)
		{
			float topYPos = canvas_p0.y + scrolling.y + (propertyCounter * animatorGridStep);
			float bottomYPos = topYPos + animatorGridStep;
			// prevents being drawn off screen and introducing scrollbar
			if (topYPos > canvas_p1.y)
				topYPos = canvas_p1.y;
			if (bottomYPos > canvas_p1.y)
				bottomYPos = canvas_p1.y;
			ImVec2 topLeftCorner = ImVec2(canvas_p0.x, topYPos);
			ImVec2 bottomRightCorner = ImVec2(canvas_p1.x, bottomYPos);
			ImU32 canvasRectColor = IM_COL32(224, 81, 15, 100);
			draw_list->AddRectFilled(topLeftCorner, bottomRightCorner, canvasRectColor);

			// Draw pips
			for (Animation::S_Canvas& canvasFrame : GetFocusedAnimation()->canvasProperties)
			{
				// Get keyFrame time and convert to seconds
				float keyFrameX = canvasFrame.time / 1000;
				Vector2 keyFramePos = Vector2(keyFrameX, propertyYPos);
				if (zeroPoint.y + (propertyYPos * animatorGridStep * -1) < canvas_p1.y && zeroPoint.y + (propertyYPos * animatorGridStep * -1) + 6 < canvas_p1.y)
					RenderAnimationTimelineKeyFrames(keyFramePos, zeroPoint, scrolling, canvas_p0, canvas_p1, canvas_sz, animatorGridStep);
			}
			propertyYPos--;
			propertyCounter++;
		}

		// Draw colored box for audio keyframes
		if (GetFocusedAnimation()->audioProperties.size() > 0)
		{
			float topYPos = canvas_p0.y + scrolling.y + (propertyCounter * animatorGridStep);
			float bottomYPos = topYPos + animatorGridStep;
			// prevents being drawn off screen and introducing scrollbar
			if (topYPos > canvas_p1.y)
				topYPos = canvas_p1.y;
			if (bottomYPos > canvas_p1.y)
				bottomYPos = canvas_p1.y;
			ImVec2 topLeftCorner = ImVec2(canvas_p0.x, topYPos);
			ImVec2 bottomRightCorner = ImVec2(canvas_p1.x, bottomYPos);
			ImU32 audioRectColor = IM_COL32(237, 244, 14, 100);
			draw_list->AddRectFilled(topLeftCorner, bottomRightCorner, audioRectColor);

			// Draw pips
			for (Animation::S_Audio& audioFrame : GetFocusedAnimation()->audioProperties)
			{
				// Get keyFrame time and convert to seconds
				float keyFrameX = audioFrame.time / 1000;
				Vector2 keyFramePos = Vector2(keyFrameX, propertyYPos);
				if (zeroPoint.y + (propertyYPos * animatorGridStep * -1) < canvas_p1.y && zeroPoint.y + (propertyYPos * animatorGridStep * -1) + 6 < canvas_p1.y)
					RenderAnimationTimelineKeyFrames(keyFramePos, zeroPoint, scrolling, canvas_p0, canvas_p1, canvas_sz, animatorGridStep);
			}
			propertyYPos--;
			propertyCounter++;
		}

		// Draw colored box for text keyframes
		if (GetFocusedAnimation()->textProperties.size() > 0)
		{
			float topYPos = canvas_p0.y + scrolling.y + (propertyCounter * animatorGridStep);
			float bottomYPos = topYPos + animatorGridStep;
			// prevents being drawn off screen and introducing scrollbar
			if (topYPos > canvas_p1.y)
				topYPos = canvas_p1.y;
			if (bottomYPos > canvas_p1.y)
				bottomYPos = canvas_p1.y;
			ImVec2 topLeftCorner = ImVec2(canvas_p0.x, topYPos);
			ImVec2 bottomRightCorner = ImVec2(canvas_p1.x, bottomYPos);
			ImU32 textRectColor = IM_COL32(15, 224, 200, 100);
			draw_list->AddRectFilled(topLeftCorner, bottomRightCorner, textRectColor);

			// Draw pips
			for (Animation::S_Text& textFrame : GetFocusedAnimation()->textProperties)
			{
				// Get keyFrame time and convert to seconds
				float keyFrameX = textFrame.time / 1000;
				Vector2 keyFramePos = Vector2(keyFrameX, propertyYPos);
				if (zeroPoint.y + (propertyYPos * animatorGridStep * -1) < canvas_p1.y && zeroPoint.y + (propertyYPos * animatorGridStep * -1) + 6 < canvas_p1.y)
					RenderAnimationTimelineKeyFrames(keyFramePos, zeroPoint, scrolling, canvas_p0, canvas_p1, canvas_sz, animatorGridStep);
			}
			propertyYPos--;
			propertyCounter++;
		}

		// Draw colored box for boxCollider keyframes
		if (GetFocusedAnimation()->boxColliderProperties.size() > 0)
		{
			float topYPos = canvas_p0.y + scrolling.y + (propertyCounter * animatorGridStep);
			float bottomYPos = topYPos + animatorGridStep;
			// prevents being drawn off screen and introducing scrollbar
			if (topYPos > canvas_p1.y)
				topYPos = canvas_p1.y;
			if (bottomYPos > canvas_p1.y)
				bottomYPos = canvas_p1.y;
			ImVec2 topLeftCorner = ImVec2(canvas_p0.x, topYPos);
			ImVec2 bottomRightCorner = ImVec2(canvas_p1.x, bottomYPos);
			ImU32 boxColliderRectColor = IM_COL32(224, 158, 15, 100);
			draw_list->AddRectFilled(topLeftCorner, bottomRightCorner, boxColliderRectColor);

			// Draw pips
			for (Animation::S_BoxCollider& boxColliderFrame : GetFocusedAnimation()->boxColliderProperties)
			{
				// Get keyFrame time and convert to seconds
				float keyFrameX = boxColliderFrame.time / 1000;
				Vector2 keyFramePos = Vector2(keyFrameX, propertyYPos);
				if (zeroPoint.y + (propertyYPos * animatorGridStep * -1) < canvas_p1.y && zeroPoint.y + (propertyYPos * animatorGridStep * -1) + 6 < canvas_p1.y)
					RenderAnimationTimelineKeyFrames(keyFramePos, zeroPoint, scrolling, canvas_p0, canvas_p1, canvas_sz, animatorGridStep);
			}
			propertyYPos--;
			propertyCounter++;
		}

		// Draw colored box for circleCollider keyframes
		if (GetFocusedAnimation()->circleColliderProperties.size() > 0)
		{
			float topYPos = canvas_p0.y + scrolling.y + (propertyCounter * animatorGridStep);
			float bottomYPos = topYPos + animatorGridStep;
			// prevents being drawn off screen and introducing scrollbar
			if (topYPos > canvas_p1.y)
				topYPos = canvas_p1.y;
			if (bottomYPos > canvas_p1.y)
				bottomYPos = canvas_p1.y;
			ImVec2 topLeftCorner = ImVec2(canvas_p0.x, topYPos);
			ImVec2 bottomRightCorner = ImVec2(canvas_p1.x, bottomYPos);
			ImU32 circleColliderRectColor = IM_COL32(11, 42, 183, 100);
			draw_list->AddRectFilled(topLeftCorner, bottomRightCorner, circleColliderRectColor);

			// Draw pips
			for (Animation::S_CircleCollider& circleColliderFrame : GetFocusedAnimation()->circleColliderProperties)
			{
				// Get keyFrame time and convert to seconds
				float keyFrameX = circleColliderFrame.time / 1000;
				Vector2 keyFramePos = Vector2(keyFrameX, propertyYPos);
				if (zeroPoint.y + (propertyYPos * animatorGridStep * -1) < canvas_p1.y && zeroPoint.y + (propertyYPos * animatorGridStep * -1) + 6 < canvas_p1.y)
					RenderAnimationTimelineKeyFrames(keyFramePos, zeroPoint, scrolling, canvas_p0, canvas_p1, canvas_sz, animatorGridStep);
			}
			propertyYPos--;
			propertyCounter++;
		}

		// Draw colored box for rigidBody keyframes
		if (GetFocusedAnimation()->rigidBodyProperties.size() > 0)
		{
			float topYPos = canvas_p0.y + scrolling.y + (propertyCounter * animatorGridStep);
			float bottomYPos = topYPos + animatorGridStep;
			// prevents being drawn off screen and introducing scrollbar
			if (topYPos > canvas_p1.y)
				topYPos = canvas_p1.y;
			if (bottomYPos > canvas_p1.y)
				bottomYPos = canvas_p1.y;
			ImVec2 topLeftCorner = ImVec2(canvas_p0.x, topYPos);
			ImVec2 bottomRightCorner = ImVec2(canvas_p1.x, bottomYPos);
			ImU32 rigidBodyRectColor = IM_COL32(166, 11, 183, 100);
			draw_list->AddRectFilled(topLeftCorner, bottomRightCorner, rigidBodyRectColor);

			// Draw pips
			for (Animation::S_RigidBody& rigidBodyFrame : GetFocusedAnimation()->rigidBodyProperties)
			{
				// Get keyFrame time and convert to seconds
				float keyFrameX = rigidBodyFrame.time / 1000;
				Vector2 keyFramePos = Vector2(keyFrameX, propertyYPos);
				if (zeroPoint.y + (propertyYPos * animatorGridStep * -1) < canvas_p1.y && zeroPoint.y + (propertyYPos * animatorGridStep * -1) + 6 < canvas_p1.y)
					RenderAnimationTimelineKeyFrames(keyFramePos, zeroPoint, scrolling, canvas_p0, canvas_p1, canvas_sz, animatorGridStep);
			}
			propertyYPos--;
			propertyCounter++;
		}

		// Draw colored box for characterController keyframes
		if (GetFocusedAnimation()->characterControllerProperties.size() > 0)
		{
			float topYPos = canvas_p0.y + scrolling.y + (propertyCounter * animatorGridStep);
			float bottomYPos = topYPos + animatorGridStep;
			// prevents being drawn off screen and introducing scrollbar
			if (topYPos > canvas_p1.y)
				topYPos = canvas_p1.y;
			if (bottomYPos > canvas_p1.y)
				bottomYPos = canvas_p1.y;
			ImVec2 topLeftCorner = ImVec2(canvas_p0.x, topYPos);
			ImVec2 bottomRightCorner = ImVec2(canvas_p1.x, bottomYPos);
			ImU32 characterControllerRectColor = IM_COL32(85, 183, 11, 100);
			draw_list->AddRectFilled(topLeftCorner, bottomRightCorner, characterControllerRectColor);

			// Draw pips
			for (Animation::S_CharacterController& characterControllerFrame : GetFocusedAnimation()->characterControllerProperties)
			{
				LogFloat(zeroPoint.y + (propertyYPos * animatorGridStep * -1) + 6, "PropertyPos: ");
				LogFloat(canvas_p0.y);
				// Get keyFrame time and convert to seconds
				float keyFrameX = characterControllerFrame.time / 1000;
				Vector2 keyFramePos = Vector2(keyFrameX, propertyYPos);
				if (zeroPoint.y + (propertyYPos * animatorGridStep * -1) < canvas_p1.y && zeroPoint.y + (propertyYPos * animatorGridStep * -1) + 6 < canvas_p1.y)
					RenderAnimationTimelineKeyFrames(keyFramePos, zeroPoint, scrolling, canvas_p0, canvas_p1, canvas_sz, animatorGridStep);
			}
			propertyYPos--;
			propertyCounter++;
		}

		// Timeline Events BeginChild()
		//
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

		RenderAnimationPreviewGrid(centerPoint, scrolling, canvas_p0, canvas_p1, canvas_sz, 50);

		std::shared_ptr<GameObject> focusedObject = FlatEngine::GetObjectById(GetFocusedGameObjectID());

		if (focusedObject != nullptr)
			RenderAnimationPreviewObject(centerPoint, scrolling, canvas_p0, canvas_p1, canvas_sz, 50, focusedObject);

		// Animation Preview EndChild()
		//
		ImGui::EndChild();
		ImGui::End();
	}

	void RenderAnimationTimelineGrid(ImVec2& zeroPoint, ImVec2 scrolling, ImVec2 canvas_p0, ImVec2 canvas_p1, ImVec2 canvas_sz, float gridStep)
	{
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(FlatEngine::FlatGui::darker.x * 255, darker.y * 255, darker.z * 255, 255));

		// Our grid step determines the largest gap between each grid point so our centerpoints must fall on
		// one of those gridstep locations. We get the total grid steps that will render given the current viewport
		// size and divide that by two to get the closest spot to the center of the viewport. It's okay that this
		// is not exactly center at all, the viewport width will never be the perfect size, we just need a starting
		// point and for that point. We need to update this value every pass of the scene view because the gridStep
		// value will change over time and we need to keep these in sync.          
		// 
		//                   V
		// |  |  |  |  |  |  |  |  |  |  |  |  |

		// X = 0 starts the drawing at the left most edge of the entire app window.

		// Draw vertical grid lines
		for (float x = trunc(fmodf(scrolling.x + canvas_p0.x, gridStep)); x < canvas_p0.x + canvas_sz.x; x += gridStep)
		{
			FlatEngine::DrawLine(ImVec2(x, canvas_p0.y), ImVec2(x, canvas_p1.y), IM_COL32(dark.x * 255, dark.y * 255, dark.z * 255, 255), 1.0f, draw_list);
		}
		for (float x = trunc(fmodf(scrolling.x + canvas_p0.x, gridStep * 2)); x < canvas_p0.x + canvas_sz.x; x += gridStep * 2)
		{
			FlatEngine::DrawLine(ImVec2(x, canvas_p0.y), ImVec2(x, canvas_p1.y), IM_COL32(light.x * 255, light.y * 255, light.z * 255, 255), 1.0f, draw_list);
		}
		// Draw horizontal grid lines
		for (float y = trunc(fmodf(scrolling.y + canvas_p0.y, gridStep)); y < canvas_p0.y + canvas_sz.y; y += gridStep / 2)
		{
			if (y > canvas_p0.y)
				FlatEngine::DrawLine(ImVec2(canvas_p0.x, y), ImVec2(canvas_p1.x, y), IM_COL32(dark.x * 255, dark.y * 255, dark.z * 255, 255), 1.0f, draw_list);
		}


		// Draw our x and y axis blue and green lines
		//
		float divX = trunc(scrolling.x / gridStep);
		float modX = fmodf(scrolling.x, gridStep);
		float offsetX = (gridStep * divX) + modX;
		float divY = trunc(scrolling.y / gridStep);
		float modY = fmodf(scrolling.y, gridStep);
		float offsetY = (gridStep * divY) + modY;

		// Blue, green and pink colors for axis and center
		ImU32 xColor = IM_COL32(1, 210, 35, 255);
		ImU32 yColor = IM_COL32(1, 1, 255, 255);
		ImU32 centerColor = IM_COL32(255, 1, 247, 255);

		// x axis bounds check + color change (lighten) if out of bounds
		if (offsetX > canvas_p1.x - 1)
		{
			offsetX = canvas_p1.x - 1;
			xColor = IM_COL32(1, 210, 35, 100);
		}
		else if (offsetX < canvas_p0.x)
		{
			offsetX = canvas_p0.x;
			xColor = IM_COL32(1, 210, 35, 100);
		}
		// y axis bounds check + color change (lighten) if out of bounds
		if (offsetY > canvas_p1.y - 1)
		{
			offsetY = canvas_p1.y - 1;
			yColor = IM_COL32(1, 1, 255, 150);
		}
		else if (offsetY < canvas_p0.y)
		{
			offsetY = canvas_p0.y;
			yColor = IM_COL32(1, 1, 255, 150);
		}

		zeroPoint = ImVec2(offsetX + scrolling.x, offsetY + scrolling.y);
		//FlatEngine::DrawPoint(ImVec2(offsetX + 5, offsetY + 5), centerColor, draw_list);
		//AddImageToDrawList(playTexture, Vector2(0, 0), ImVec2(offsetX + 5, offsetY + 5), 10, 10, Vector2(5,5), Vector2(1,1), false, 50, draw_list);
	}

	void RenderAnimationTimelineKeyFrames(Vector2& pipPosition, ImVec2 zeroPoint, ImVec2 scrolling, ImVec2 canvas_p0, ImVec2 canvas_p1, ImVec2 canvas_sz, float gridStep)
	{
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

			ImGui::InvisibleButton("KeyFramePip", ImVec2(12, 12), ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight | 4096);
			const bool _isClicked = ImGui::IsItemClicked();
			const bool _isHovered = ImGui::IsItemHovered(); // Hovered
			const bool _isActive = ImGui::IsItemActive();   // Held


			LogString(std::to_string(_isClicked) + "Is Clicked: ");
			if (_isHovered)
				ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_Hand);
			if (_isClicked)
				LogString("KeyFrame Clicked!");
		}
	}

	void RenderAnimationPreviewGrid(ImVec2& centerPoint, ImVec2 scrolling, ImVec2 canvas_p0, ImVec2 canvas_p1, ImVec2 canvas_sz, float gridStep)
	{
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(darker.x * 255, darker.y * 255, darker.z * 255, 255));

		// Our grid step determines the largest gap between each grid point so our centerpoints must fall on
		// one of those gridstep locations. We get the total grid steps that will render given the current viewport
		// size and divide that by two to get the closest spot to the center of the viewport. It's okay that this
		// is not exactly center at all, the viewport width will never be the perfect size, we just need a starting
		// point and for that point. We need to update this value every pass of the scene view because the gridStep
		// value will change over time and we need to keep these in sync.          
		// 
		//                   V
		// |  |  |  |  |  |  |  |  |  |  |  |  |

		// X = 0 starts the drawing at the left most edge of the entire app window.

		// Draw vertical grid lines
		for (float x = trunc(fmodf(scrolling.x + canvas_p0.x, gridStep)); x < canvas_p0.x + canvas_sz.x; x += gridStep)
		{
			FlatEngine::DrawLine(ImVec2(x, canvas_p0.y), ImVec2(x, canvas_p1.y), IM_COL32(dark.x * 255, dark.y * 255, dark.z * 255, 255), 1.0f, draw_list);
		}
		// Draw horizontal grid lines
		for (float y = trunc(fmodf(scrolling.y + canvas_p0.y, gridStep)); y < canvas_p0.y + canvas_sz.y; y += gridStep)
		{
			if (y > canvas_p0.y)
				FlatEngine::DrawLine(ImVec2(canvas_p0.x, y), ImVec2(canvas_p1.x, y), IM_COL32(dark.x * 255, dark.y * 255, dark.z * 255, 255), 1.0f, draw_list);
		}


		// Draw our x and y axis blue and green lines
		//
		float divX = trunc(scrolling.x / gridStep);
		float modX = fmodf(scrolling.x, gridStep);
		float offsetX = (gridStep * divX) + modX + canvas_p0.x + (trunc(400 / gridStep) / 2 * gridStep);
		float divY = trunc(scrolling.y / gridStep);
		float modY = fmodf(scrolling.y, gridStep);
		float offsetY = (gridStep * divY) + modY + canvas_p0.y + (trunc(400 / gridStep) / 2 * gridStep);

		// Blue, green and pink colors for axis and center
		ImU32 xColor = IM_COL32(1, 210, 35, 255);
		ImU32 yColor = IM_COL32(1, 1, 255, 255);
		ImU32 centerColor = IM_COL32(255, 1, 247, 255);

		// x axis bounds check + color change (lighten) if out of bounds
		if (offsetX > canvas_p1.x - 1)
		{
			offsetX = canvas_p1.x - 1;
			xColor = IM_COL32(1, 210, 35, 100);
		}
		else if (offsetX < canvas_p0.x)
		{
			offsetX = canvas_p0.x;
			xColor = IM_COL32(1, 210, 35, 100);
		}
		// y axis bounds check + color change (lighten) if out of bounds
		if (offsetY > canvas_p1.y - 1)
		{
			offsetY = canvas_p1.y - 1;
			yColor = IM_COL32(1, 1, 255, 150);
		}
		else if (offsetY < canvas_p0.y)
		{
			offsetY = canvas_p0.y;
			yColor = IM_COL32(1, 1, 255, 150);
		}

		centerPoint = ImVec2(offsetX, offsetY);

		// Draw the axis and center point
		FlatEngine::DrawLine(ImVec2(offsetX, canvas_p0.y), ImVec2(offsetX, canvas_p1.y), xColor, 1.0f, draw_list);
		FlatEngine::DrawLine(ImVec2(canvas_p0.x, offsetY), ImVec2(canvas_p1.x, offsetY), yColor, 1.0f, draw_list);
		FlatEngine::DrawPoint(ImVec2(offsetX, offsetY), centerColor, draw_list);
	}

	void RenderAnimationPreviewObject(ImVec2 centerPoint, ImVec2 scrolling, ImVec2 canvas_p0, ImVec2 canvas_p1, ImVec2 canvas_sz, float gridStep, std::shared_ptr<GameObject> animatedObject)
	{
		// Split our drawlist into multiple channels for different rendering orders
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		ImDrawListSplitter* drawSplitter = new ImDrawListSplitter();

		// 4 channels for now in this scene view. 0 = scene objects, 1 & 2 = other UI (camera icon, etc), 4 = transform arrow
		drawSplitter->Split(draw_list, maxSpriteLayers + 5);

		// If this Scene Object doesn't have a parent, render it and all of its children
		if (animatedObject->GetParentID() == -1 && animatedObject->IsActive())
		{
			// Start off with a 0,0 parentOffset because this is the top level object to be rendered.
			Vector2 parentOffset(0, 0);
			Vector2 parentScale(1, 1);

			// Render self and children recursively
			AnimationPreview_RenderSelfThenChildren(centerPoint, animatedObject, parentOffset, parentScale, scrolling, canvas_p0, canvas_sz, draw_list, drawSplitter);
		}

		drawSplitter->Merge(draw_list);
		delete drawSplitter;
		drawSplitter = nullptr;
	}

	void AnimationPreview_RenderSelfThenChildren(ImVec2& centerPoint, std::shared_ptr<GameObject> self, Vector2 parentOffset, Vector2 parentScale, ImVec2 scrolling, ImVec2 canvas_p0, ImVec2 canvas_sz, ImDrawList* draw_list, ImDrawListSplitter* drawSplitter)
	{
		std::shared_ptr<Component> transformComponent = self->GetComponent(ComponentTypes::Transform);
		std::shared_ptr<Component> spriteComponent = self->GetComponent(ComponentTypes::Sprite);
		std::shared_ptr<Component> cameraComponent = self->GetComponent(ComponentTypes::Camera);
		std::shared_ptr<Component> buttonComponent = self->GetComponent(ComponentTypes::Button);
		std::shared_ptr<Component> canvasComponent = self->GetComponent(ComponentTypes::Canvas);
		std::shared_ptr<Component> textComponent = self->GetComponent(ComponentTypes::Text);

		// Check if each object has a Transform component
		if (transformComponent != nullptr)
		{
			float divX = trunc(scrolling.x / 50);
			float modX = fmodf(scrolling.x, 50);
			float offsetX = (50 * divX) + modX + canvas_p0.x + (trunc(400 / 50) / 2 * 50);
			float divY = trunc(scrolling.y / 50);
			float modY = fmodf(scrolling.y, 50);
			float offsetY = (50 * divY) + modY + canvas_p0.y + (trunc(400 / 50) / 2 * 50);

			long focusedObjectID = FlatEngine::GetFocusedGameObjectID();
			std::shared_ptr<Transform> transformCasted = std::static_pointer_cast<Transform>(transformComponent);
			Vector2 position = Vector2(transformCasted->GetPosition().x + parentOffset.x, transformCasted->GetPosition().y + parentOffset.y);
			//position = Vector2(position.x + offsetX, position.y + offsetY);
			Vector2 transformScale = Vector2(transformCasted->GetScale().x * parentScale.x, transformCasted->GetScale().y * parentScale.y);

			// If it has a sprite component, render that sprite texture at the objects transform position with offsets
			if (spriteComponent != nullptr)
			{
				// Cast the component to Sprite shared_ptr
				std::shared_ptr<Sprite> spriteCasted = std::static_pointer_cast<Sprite>(spriteComponent);
				SDL_Texture* spriteTexture = spriteCasted->GetTexture();
				float spriteTextureWidth = (float)spriteCasted->GetTextureWidth();
				float spriteTextureHeight = (float)spriteCasted->GetTextureHeight();
				Vector2 spriteOffset = spriteCasted->GetOffset();
				bool _spriteScalesWithZoom = true;
				int renderOrder = spriteCasted->GetRenderOrder();

				// If there is a valid Texture loaded into the Sprite Component
				if (spriteTexture != nullptr)
				{
					// Change the draw channel for the scene object
					if (renderOrder <= maxSpriteLayers && renderOrder >= 0)
						drawSplitter->SetCurrentChannel(draw_list, renderOrder);
					else
						drawSplitter->SetCurrentChannel(draw_list, 0);

					// Draw the texture
					AddImageToDrawList(spriteTexture, position, centerPoint, spriteTextureWidth, spriteTextureHeight, spriteOffset, transformScale, _spriteScalesWithZoom, 50, draw_list);
				}
			}

			// If it has a text component, render that text texture at the objects transform position
			if (textComponent != nullptr)
			{
				// Cast the component to Text shared_ptr
				std::shared_ptr<Text> textCasted = std::static_pointer_cast<Text>(textComponent);
				std::shared_ptr<Texture> textTexture = textCasted->GetTexture();
				textCasted->LoadText();
				SDL_Texture* texture = textTexture->getTexture();
				float textWidth = (float)textTexture->getWidth();
				float textHeight = (float)textTexture->getHeight();
				int renderOrder = textCasted->GetRenderOrder();
				Vector2 offset = textCasted->GetOffset();
				bool _spriteScalesWithZoom = true;


				// If there is a valid Texture loaded into the Sprite Component
				if (textTexture != nullptr)
				{
					// Change the draw channel for the scene object
					if (renderOrder <= maxSpriteLayers && renderOrder >= 0)
						drawSplitter->SetCurrentChannel(draw_list, renderOrder);
					else
						drawSplitter->SetCurrentChannel(draw_list, 0);

					// Draw the texture
					AddImageToDrawList(textTexture->getTexture(), position, scrolling, textWidth, textHeight, offset, transformScale, _spriteScalesWithZoom, 50, draw_list);
				}
			}

			// Renders the camera
			if (cameraComponent != nullptr && false)
			{
				std::shared_ptr<Camera> camera = std::static_pointer_cast<Camera>(cameraComponent);
				float cameraWidth = camera->GetWidth();
				float cameraHeight = camera->GetHeight();

				float cameraLeftEdge = WorldToViewport(scrolling.x, position.x - cameraWidth / 2 * transformScale.x, 50);
				float cameraRightEdge = WorldToViewport(scrolling.x, position.x + cameraWidth / 2 * transformScale.x, 50);
				float cameraTopEdge = WorldToViewport(scrolling.y, -(position.y + cameraHeight / 2 * transformScale.y), 50);
				float cameraBottomEdge = WorldToViewport(scrolling.y, -(position.y - cameraHeight / 2 * transformScale.y), 50);

				ImVec2 topLeftCorner = ImVec2(cameraLeftEdge, cameraTopEdge);
				ImVec2 bottomRightCorner = ImVec2(cameraRightEdge, cameraBottomEdge);
				ImVec2 topRightCorner = ImVec2(cameraRightEdge, cameraTopEdge);
				ImVec2 bottomLeftCorner = ImVec2(cameraLeftEdge, cameraBottomEdge);

				cameraTexture->loadFromFile("assets/images/camera.png");
				SDL_Texture* texture = cameraTexture->getTexture();
				float cameraTextureWidth = (float)cameraTexture->getWidth() * 3;
				float cameraTextureHeight = (float)cameraTexture->getHeight() * 3;
				bool _scalesWithZoom = false;
				Vector2 cameraTextureOffset = { cameraTextureWidth / 2, cameraTextureHeight / 2 };
				Vector2 cameraTextureScale = { 1, 1 };
				Vector2 offsetPosition = Vector2(position.x - cameraTextureWidth / 2, position.y + cameraTextureHeight / 2);

				// Draw channel 2 for Lower UI
				drawSplitter->SetCurrentChannel(draw_list, maxSpriteLayers + 2);

				// Draw a rectangle to the scene view to represent the camera frustrum
				FlatEngine::DrawRectangle(topLeftCorner, bottomRightCorner, canvas_p0, canvas_sz, IM_COL32(255, 30, 30, 70), 2.0f, draw_list);
				FlatEngine::DrawLine(topLeftCorner, bottomRightCorner, IM_COL32(255, 30, 30, 70), 2.0f, draw_list);
				FlatEngine::DrawLine(topRightCorner, bottomLeftCorner, IM_COL32(255, 30, 30, 70), 2.0f, draw_list);

				// Draw actual camera icon
				AddImageToDrawList(texture, position, scrolling, cameraTextureWidth, cameraTextureHeight, cameraTextureOffset, cameraTextureScale, _scalesWithZoom, 50, draw_list, IM_COL32(255, 255, 255, iconTransparency));
			}


			// Renders Canvas Component
			if (canvasComponent != nullptr)
			{
				std::shared_ptr<Canvas> canvas = std::static_pointer_cast<Canvas>(canvasComponent);

				float activeWidth = canvas->GetWidth();
				float activeHeight = canvas->GetHeight();
				int layerNumber = canvas->GetLayerNumber();
				bool _blocksLayers = canvas->GetBlocksLayers();

				float canvasLeft = WorldToViewport(scrolling.x, position.x - activeWidth / 2, 50, false);
				float canvasRight = WorldToViewport(scrolling.x, position.x + activeWidth / 2, 50, false);
				float canvasTop = WorldToViewport(scrolling.y, position.y + activeHeight / 2, 50, true);
				float canvasBottom = WorldToViewport(scrolling.y, position.y - activeHeight / 2, 50, true);

				Vector2 canvasTopLeft = { canvasLeft, canvasTop };
				Vector2 canvasBottomRight = { canvasRight, canvasBottom };

				drawSplitter->SetCurrentChannel(draw_list, maxSpriteLayers + 2);

				FlatEngine::DrawRectangle(canvasTopLeft, canvasBottomRight, canvas_p0, canvas_sz, FlatEngine::CanvasBorder, 3.0f, draw_list);
			}


			// Renders Button Component
			if (buttonComponent != nullptr)
			{
				std::shared_ptr<Button> button = std::static_pointer_cast<Button>(buttonComponent);

				float activeWidth = button->GetActiveWidth();
				float activeHeight = button->GetActiveHeight();
				Vector2 activeOffset = button->GetActiveOffset();
				bool _isActive = button->IsActive();

				float activeLeft = WorldToViewport(scrolling.x, position.x + activeOffset.x - (activeWidth / 2 * transformScale.x), 50, false);
				float activeRight = WorldToViewport(scrolling.x, position.x + activeOffset.x + (activeWidth / 2 * transformScale.x), 50, false);
				float activeTop = WorldToViewport(scrolling.y, position.y + activeOffset.y + (activeHeight / 2 * transformScale.y), 50, true);
				float activeBottom = WorldToViewport(scrolling.y, position.y + activeOffset.y - (activeHeight / 2 * transformScale.y), 50, true);

				Vector2 topLeft = { activeLeft, activeTop };
				Vector2 bottomRight = { activeRight, activeBottom };

				drawSplitter->SetCurrentChannel(draw_list, maxSpriteLayers + 2);

				if (_isActive)
					FlatEngine::DrawRectangle(topLeft, bottomRight, canvas_p0, canvas_sz, FlatEngine::ActiveButtonColor, 3.0f, draw_list);
				else
					FlatEngine::DrawRectangle(topLeft, bottomRight, canvas_p0, canvas_sz, FlatEngine::InactiveButtonColor, 3.0f, draw_list);
			}


			// Renders Transform Arrow // 
			//
			// Should be last in line here to be rendered top-most
			// 
			// If a sceneObject is focused and the currently focused object is the same as this loop iteration,
			// render the focused objects TransformArrow for moving it within the scene view
			if (focusedObjectID != -1 && focusedObjectID == self->GetID())
			{
				// Get focused GameObject and transformArrow png
				std::shared_ptr<GameObject> focusedObject = FlatEngine::GetObjectById(focusedObjectID);
				transformArrow->loadFromFile("assets/images/transformArrow.png");
				SDL_Texture* texture = transformArrow->getTexture();
				// * 3 because the texture is so small. If we change the scale, it will change the render starting
				// position, which we don't want. We only want to change the render ending position so we adjust dimensions only
				float arrowWidth = (float)transformArrow->getWidth() * 3;
				float arrowHeight = (float)transformArrow->getHeight() * 3;
				Vector2 arrowScale = { 1, 1 };
				Vector2 arrowOffset = { 0, arrowHeight };
				bool _scalesWithZoom = false;

				// Draw channel maxSpriteLayers + 3 for Upper UI Transform Arrow
				drawSplitter->SetCurrentChannel(draw_list, maxSpriteLayers + 3);
				AddImageToDrawList(texture, position, scrolling, arrowWidth, arrowHeight, arrowOffset, arrowScale, _scalesWithZoom, 50, draw_list, IM_COL32(255, 255, 255, 255));
			}
		}

		if (self->HasChildren())
		{
			if (transformComponent != nullptr)
			{
				std::shared_ptr<Transform> transformCasted = std::static_pointer_cast<Transform>(transformComponent);
				parentOffset.x += transformCasted->GetPosition().x;
				parentOffset.y += transformCasted->GetPosition().y;
				parentScale.x *= transformCasted->GetScale().x;
				parentScale.y *= transformCasted->GetScale().y;
			}

			for (int c = 0; c < self->GetChildren().size(); c++)
			{
				std::shared_ptr<GameObject> child = FlatEngine::GetObjectById(self->GetChildren()[c]);

				if (child->IsActive())
					AnimationPreview_RenderSelfThenChildren(centerPoint, child, parentOffset, parentScale, scrolling, canvas_p0, canvas_sz, draw_list, drawSplitter);
			}
		}
	}
}	
}