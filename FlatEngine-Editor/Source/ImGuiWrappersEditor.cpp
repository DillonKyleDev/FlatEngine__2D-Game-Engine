#include "FlatEngine.h"
#include "FlatGui.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "TagList.h"
#include "Component.h"
#include "Transform.h"
#include "Sprite.h"
#include "Camera.h"
#include "Scene.h"
#include "ScriptComponent.h"
#include "Button.h"
#include "Canvas.h"
#include "Animation.h"
#include "Audio.h"
#include "Text.h"
#include "CharacterController.h"
#include "BoxCollider.h"
#include "CircleCollider.h"
#include "RigidBody.h"
#include "Project.h"


namespace FlatGui 
{
	// ImGui Wrappers
	void BeginComponent(FlatEngine::Component* component, long &queuedForDelete)
	{
		// Is Collapsed
		bool _isCollapsed = component->IsCollapsed();
		long id = component->GetID();

		// Component Name
		std::string componentType = component->GetTypeString();
		// Get Component ID in to keep the child unique
		std::string componentID = componentType + std::to_string(component->GetID());

		// Begin Component Child
		ImGui::PushStyleColor(ImGuiCol_ChildBg, FlatEngine::F_innerWindowColor);
		ImGui::PushStyleColor(ImGuiCol_Border, FlatEngine::F_componentBorderColor);
		ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f);

		ImGui::BeginChild(componentID.c_str(), Vector2(0, 0), FlatEngine::F_autoResizeChildFlags);

		ImGui::PopStyleVar();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();

		// Border around each component
		auto wPos = ImGui::GetWindowPos();
		auto wSize = ImGui::GetWindowSize();		
		
		// Tooltip for Component meta data
		ImGui::GetWindowDrawList()->AddRect({ wPos.x + 2, wPos.y + 2 }, { wPos.x + wSize.x - 2, wPos.y + wSize.y - 2 }, ImColor(FlatEngine::F_componentBorderColor.x, FlatEngine::F_componentBorderColor.y, FlatEngine::F_componentBorderColor.z, FlatEngine::F_componentBorderColor.w));
		FlatEngine::RenderInvisibleButton("ComponentToolTipButton-" + std::to_string(id), { wPos.x, wPos.y }, { wSize.x, 40 });
		if (ImGui::IsItemHovered() && ImGui::GetIO().KeyAlt)
		{
			FlatEngine::BeginToolTip("Component Info");
			FlatEngine::RenderToolTipLong("ID", id);
			FlatEngine::EndToolTip();
		}
		ImGui::SetCursorScreenPos({ wPos.x + 5, wPos.y + 5 });
		
		// Component Name
		ImGui::SetCursorPos(Vector2(ImGui::GetCursorPosX() + 5, ImGui::GetCursorPosY() + 5));
		ImGui::Text(componentType.c_str());

		ImGui::SameLine(ImGui::GetContentRegionAvail().x - (FlatEngine::F_childPadding + 42), 5);

		// Pushes	
		ImGui::PushItemWidth(-1.0f);
		ImGui::PushStyleColor(ImGuiCol_Border, FlatEngine::F_componentBorderColor);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, Vector2(1.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 1, 1, 0));

		std::string expandID = "##expandIcon-" + std::to_string(id);
		std::string trashcanID = "##trashIcon-" + std::to_string(id);
		std::string openFileID = "##openFileIcon-" + std::to_string(id);

		ImGui::SetCursorPos(Vector2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() - 3));
		// Trash Can Icon for removing Component from Focused Object
		if (FlatEngine::RenderImageButton(trashcanID.c_str(), FlatEngine::F_trashIcon.GetTexture()))
			queuedForDelete = component->GetID();

		ImGui::SameLine(0, 5);

		ImGui::SetCursorPos(Vector2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() - 3));
		// Draw Expand Icon for expanding/collapsing current component information
		if (_isCollapsed)
		{
			if (FlatEngine::RenderImageButton(expandID.c_str(), FlatEngine::F_expandIcon.GetTexture()))
				component->SetCollapsed(!_isCollapsed);
		}
		else
			if (FlatEngine::RenderImageButton(expandID.c_str(), FlatEngine::F_expandFlippedIcon.GetTexture()))
				component->SetCollapsed(!_isCollapsed);

		if (!_isCollapsed)
		{
			ImGui::Separator();
			ImGui::Separator();
		}
		else {
			ImGui::SetCursorPos(Vector2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() - 15));
			ImGui::Text("");
		}

		 //Component Data - Give it background color and padding
		std::string componentItemID = "##ComponentItem-" + componentType;
		ImGui::PushStyleColor(ImGuiCol_ChildBg, FlatEngine::F_singleItemColor);
		// Make full width Push

		if (!component->IsCollapsed())
			ImGui::BeginChild(componentItemID.c_str(), Vector2(0, 0), FlatEngine::F_autoResizeChildFlags);
	}

	void EndComponent(Component* component)
	{
		// Pops
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleVar();
		ImGui::PopStyleColor();

		// Add some space to the bottom of each component
		if (!component->IsCollapsed())
		{
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
			ImGui::EndChild();
		}

		ImGui::EndChild();
	}

	bool RenderIsActiveCheckbox(bool& _isActive)
	{
		ImGui::SetCursorPos(Vector2(ImGui::GetCursorPosX() + 3, ImGui::GetCursorPosY() + 1));
		bool _checked = FlatEngine::RenderCheckbox("Active", _isActive);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3);
		ImGui::Separator();
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 4);

		return _checked;
	}

	void RenderTransformComponent(Transform* transform)
	{
		// Position, scale, and rotation of transform
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
		if (RenderIsActiveCheckbox(_isActive))
			transform->SetActive(_isActive);

		// Render Table
		if (FlatEngine::PushTable("##TransformProperties" + std::to_string(id), 2))
		{
			FlatEngine::RenderFloatDragTableRow("##xPosition" + std::to_string(id), "X Position", xPos, 0.1f, -FLT_MAX, -FLT_MAX);
			FlatEngine::RenderFloatDragTableRow("##yPosition" + std::to_string(id), "Y Position", yPos, 0.1f, -FLT_MAX, -FLT_MAX);
			transform->SetPosition(Vector2(xPos, yPos));
			FlatEngine::RenderFloatDragTableRow("##rotation" + std::to_string(id), "Rotation", rotation, 0.1f, -360, 360);
			transform->SetRotation(rotation);
			FlatEngine::RenderFloatDragTableRow("##xScaleDrag" + std::to_string(id), "X Scale", scaleX, 0.1f, 0.001f, 1000);
			FlatEngine::RenderFloatDragTableRow("##yScaleDrag" + std::to_string(id), "Y Scale", scaleY, 0.1f, 0.001f, 1000);
			transform->SetScale(Vector2(scaleX, scaleY));
			FlatEngine::PopTable();
		}
	}

	void RenderSpriteComponent(Sprite* sprite)
	{
		std::string path = sprite->GetPath();
		char newPath[1024];
		strcpy_s(newPath, path.c_str());
		float textureWidth = sprite->GetTextureWidth();
		float textureHeight = sprite->GetTextureHeight();
		Vector2 textureScale = sprite->GetScale();
		Sprite::PivotPoint pivotPoint = sprite->GetPivotPoint();
		std::string pivotString = sprite->GetPivotPointString();
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
		Vector4 tintColor = sprite->GetTintColor();
		long id = sprite->GetID();

		// Active Checkbox
		if (RenderIsActiveCheckbox(_isActive))
			sprite->SetActive(_isActive);								

		// Render Sprite Path
		ImGui::Text(pathString.c_str());
		ImGui::SameLine(0, 5);

		// Load From File
		std::string openFileID = "##OpenFileIcon" + std::to_string(sprite->GetID());
		if (FlatEngine::RenderImageButton(openFileID.c_str(), FlatEngine::F_openFileIcon.GetTexture()))
		{
			std::string assetPath = FlatEngine::OpenLoadFileExplorer();
			strcpy_s(newPath, assetPath.c_str());
			sprite->SetTexture(newPath);
		}
		ImGui::SameLine(0, 5);
								
		// Sprite Path Edit
		std::string spriteID = "##spritePath" + std::to_string(id);
		ImGui::PushStyleColor(ImGuiCol_FrameBg, FlatEngine::F_inputColor);
		if (ImGui::InputText(spriteID.c_str(), newPath, IM_ARRAYSIZE(newPath), FlatEngine::F_inputFlags))
			sprite->SetTexture(newPath);
		ImGui::PopStyleColor();

		// Render Table
		if (FlatEngine::PushTable("##SpriteProperties" + std::to_string(id), 2))
		{
			if (FlatEngine::RenderFloatDragTableRow("##xSpriteScaleDrag" + std::to_string(id), "X Scale", xScale, 0.1f, 0.001f, 1000))
				sprite->SetScale(Vector2(xScale, yScale));
			if (FlatEngine::RenderFloatDragTableRow("##ySpriteScaleDrag" + std::to_string(id), "Y Scale", yScale, 0.1f, 0.001f, 1000))
				sprite->SetScale(Vector2(xScale, yScale));																
			if (FlatEngine::RenderFloatDragTableRow("##xSpriteOffsetDrag" + std::to_string(id), "X Offset", xOffset, 0.1f, -FLT_MAX, -FLT_MAX))
				sprite->SetOffset(Vector2(xOffset, yOffset));
			if (FlatEngine::RenderFloatDragTableRow("##ySpriteOffsetDrag" + std::to_string(id), "Y Offset", yOffset, 0.1f, -FLT_MAX, -FLT_MAX))
				sprite->SetOffset(Vector2(xOffset, yOffset));										
			if (FlatEngine::RenderIntDragTableRow("##renderOrder" + std::to_string(id), "Render Order", renderOrder, 1, 0, (int)maxSpriteLayers))
				sprite->SetRenderOrder(renderOrder);
			FlatEngine::RenderTextTableRow("##textureWidth" + std::to_string(id), "Texture width", textureWidthString);
			FlatEngine::RenderTextTableRow("##textureHeight" + std::to_string(id), "Texture height", textureHeightString);
			FlatEngine::PopTable();
		}

		// Pivot Point Buttons							
		ImGui::SetCursorScreenPos(Vector2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y + 5));
		Vector2 cellSize = Vector2(76, 78);
		Vector2 cursorScreen = Vector2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y);

		// TopLeft, Top, TopRight
		ImGui::GetWindowDrawList()->AddRectFilled(cursorScreen, Vector2(cursorScreen.x + cellSize.x, cursorScreen.y + cellSize.y), ImGui::GetColorU32(FlatEngine::F_logBgColor));
		ImGui::SetCursorScreenPos(Vector2(cursorScreen.x + 5, cursorScreen.y + 5));
		if (FlatEngine::RenderImageButton("##PivotTopLeftButton", FlatEngine::F_upLeftIcon.GetTexture(), Vector2(16, 16), 1, FlatEngine::F_imageButtonDarkColor))
			sprite->SetPivotPoint(Sprite::PivotPoint::TopLeft);
		ImGui::SameLine(0, 3);
		if (FlatEngine::RenderImageButton("##PivotTopButton", FlatEngine::F_upIcon.GetTexture(), Vector2(16, 16), 1, FlatEngine::F_imageButtonDarkColor))
			sprite->SetPivotPoint(Sprite::PivotPoint::Top);
		ImGui::SameLine(0, 3);
		if (FlatEngine::RenderImageButton("##PivotTopRightButton", FlatEngine::F_upRightIcon.GetTexture(), Vector2(16, 16), 1, FlatEngine::F_imageButtonDarkColor))
			sprite->SetPivotPoint(Sprite::PivotPoint::TopRight);
								
		ImGui::SameLine(0, 17);
		ImGui::Text("Pivot Point:");

		// Left, Center, Right
		ImGui::SetCursorScreenPos(Vector2(ImGui::GetCursorScreenPos().x + 5, ImGui::GetCursorScreenPos().y));
		if (FlatEngine::RenderImageButton("##PivotLeftButton", FlatEngine::F_leftIcon.GetTexture(), Vector2(16, 16), 1, FlatEngine::F_imageButtonDarkColor))
			sprite->SetPivotPoint(Sprite::PivotPoint::Left);
		ImGui::SameLine(0, 3);
		if (FlatEngine::RenderImageButton("##PivotCenterButton", FlatEngine::F_centerIcon.GetTexture(), Vector2(16, 16), 1, FlatEngine::F_imageButtonDarkColor))
			sprite->SetPivotPoint(Sprite::PivotPoint::Center);
		ImGui::SameLine(0, 3);
		if (FlatEngine::RenderImageButton("##PivotRightButton", FlatEngine::F_rightIcon.GetTexture(), Vector2(16, 16), 1, FlatEngine::F_imageButtonDarkColor))
			sprite->SetPivotPoint(Sprite::PivotPoint::Right);
								
		ImGui::SameLine(0, 17);
		ImGui::Text(pivotString.c_str());

		// BottomLeft, Bottom, BottomRight
		ImGui::SetCursorScreenPos(Vector2(ImGui::GetCursorScreenPos().x + 5, ImGui::GetCursorScreenPos().y));
		if (FlatEngine::RenderImageButton("##PivotBottomLeftButton", FlatEngine::F_downLeftIcon.GetTexture(), Vector2(16, 16), 1, FlatEngine::F_imageButtonDarkColor))
			sprite->SetPivotPoint(Sprite::PivotPoint::BottomLeft);
		ImGui::SameLine(0, 3);
		if (FlatEngine::RenderImageButton("##PivotBottomButton", FlatEngine::F_downIcon.GetTexture(), Vector2(16, 16), 1, FlatEngine::F_imageButtonDarkColor))
			sprite->SetPivotPoint(Sprite::PivotPoint::Bottom);
		ImGui::SameLine(0, 3);
		if (FlatEngine::RenderImageButton("##PivotBottomRightButton", FlatEngine::F_downRightIcon.GetTexture(), Vector2(16, 16), 1, FlatEngine::F_imageButtonDarkColor))
			sprite->SetPivotPoint(Sprite::PivotPoint::BottomRight);

		ImGui::SetCursorScreenPos(Vector2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y + 10));

		// Tint color picker
		std::string tintID = "##SpriteTintColor" + std::to_string(id) + "-" + std::to_string(id);
		ImVec4 color = ImVec4(tintColor.x * 255.0f, tintColor.y * 255.0f, tintColor.z * 255.0f, tintColor.w * 255.0f);
		if (ImGui::ColorEdit4(tintID.c_str(), (float*)&tintColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel))
		{
			sprite->SetTintColor(tintColor);
		}
		ImGui::SameLine(0, 5);
		ImGui::Text("Tint color");
	}

	void RenderCameraComponent(Camera* camera)
	{	
		float width = camera->GetWidth();
		float height = camera->GetHeight();
		bool _isPrimary = camera->IsPrimary();
		float zoom = camera->GetZoom();
		ImVec4 frustrumColor = camera->GetFrustrumColor();
		bool _isActive = camera->IsActive();
		long id = camera->GetID();
		bool _follow = camera->GetShouldFollow();
		std::string following = "";
		if (camera->GetFollowing() != -1)
			following = FlatEngine::GetObjectById(camera->GetFollowing())->GetName();
		float followSmoothing = camera->GetFollowSmoothing();

		// Active Checkbox
		if (RenderIsActiveCheckbox(_isActive))
			camera->SetActive(_isActive);

		// Render Table
		if (FlatEngine::PushTable("##CameraProperties" + std::to_string(id), 2))
		{
			if (FlatEngine::RenderFloatDragTableRow("##cameraWidth" + std::to_string(id), "Camera width", width, 0.1f, 0, 1000))
				camera->SetDimensions(width, height);
			if (FlatEngine::RenderFloatDragTableRow("##cameraHeight" + std::to_string(id), "Camera height", height, 0.1f, 0, 1000))
				camera->SetDimensions(width, height);
			if (FlatEngine::RenderFloatDragTableRow("##cameraZoom" + std::to_string(id), "Camera zoom", zoom, 0.1f, 1, 100))
				camera->SetZoom(zoom);
			if (FlatEngine::RenderCheckboxTableRow("##CameraShouldFollowTargetCheckbox", "Follow", _follow))
				camera->SetShouldFollow(_follow);
			if (FlatEngine::RenderFloatDragTableRow("##cameraFollowSmoothing" + std::to_string(id), "Follow Smoothing", followSmoothing, 0.01f, 0, 1))
				camera->SetFollowSmoothing(followSmoothing);
			FlatEngine::PopTable();
		}

		if (FlatEngine::RenderInput("##FollowInput", "To Follow", following))
			camera->SetFollowing(FlatEngine::GetObjectByName(following)->GetID());

		// Frustrum color picker
		std::string frustrumID = "##FrustrumColor" + std::to_string(id);
		ImVec4 color = ImVec4(frustrumColor.x / 255.0f, frustrumColor.y / 255.0f, frustrumColor.z / 255.0f, frustrumColor.w / 255.0f);
		ImGui::ColorEdit4(frustrumID.c_str(), (float*)&color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
		ImGui::SameLine(0, 5);
		ImGui::Text("Frustrum color");
		camera->SetFrustrumColor(ImVec4(color.x * 255.0f, color.y * 255.0f, color.z * 255.0f, color.w * 255.0f));

		// Before allowing this camera to be set as primary, we need to ensure it has a transform component
		if (camera->GetParent()->HasComponent(ComponentTypes::Transform))
		{									
			if (FlatEngine::RenderCheckbox("Is Primary Camera", _isPrimary))
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
			if (FlatEngine::RenderCheckbox("Is Primary Camera", temp))
				FlatEngine::LogString("FlatGui::RenderInspector() - Attempt to set Camera component as primary failed: No Transform component found...");
			temp = false;
			ImGui::TextWrapped("*A Camera Component must be coupled with a Transform Component to be set as the primary camera.*");
		}

		camera->SetPrimaryCamera(_isPrimary);
	}

	void RenderScriptComponent(ScriptComponent* script)
	{		
		std::string path = script->GetAttachedScript();
		bool _isActive = script->IsActive();
		long id = script->GetID();

		// Active Checkbox
		if (RenderIsActiveCheckbox(_isActive))
			script->SetActive(_isActive);

		std::string inputId = "##scriptName" + std::to_string(id);
		if (FlatEngine::RenderInput(inputId, "Name: ", path))
			script->SetAttachedScript(path);
	}

	void RenderButtonComponent(Button* button)
	{
		bool _isActive = button->IsActive();
		float activeWidth = button->GetActiveWidth();
		float activeHeight = button->GetActiveHeight();
		Vector2 activeOffset = button->GetActiveOffset();
		int activeLayer = button->GetActiveLayer();	
		long id = button->GetID();

		// Active Checkbox
		if (RenderIsActiveCheckbox(_isActive))
			button->SetActive(_isActive);

		// Render Table
		if (FlatEngine::PushTable("##ButtonProperties" + std::to_string(id), 2))
		{
			FlatEngine::RenderFloatDragTableRow("##activeWidth" + std::to_string(id), "Active width", activeWidth, 0.1f, 0, 1000);
			FlatEngine::RenderFloatDragTableRow("##activeHeight" + std::to_string(id), "Active height", activeHeight, 0.1f, 0, 1000);
			button->SetActiveDimensions(activeWidth, activeHeight);
			FlatEngine::RenderFloatDragTableRow("##activeoffsetx" + std::to_string(id), "X Offset", activeOffset.x, 0.1f, -FLT_MAX, -FLT_MAX);
			FlatEngine::RenderFloatDragTableRow("##activeoffsety" + std::to_string(id), "Y Offset", activeOffset.y, 0.1f, -FLT_MAX, -FLT_MAX);
			button->SetActiveOffset(activeOffset);
			FlatEngine::PopTable();
		}
	}

	void RenderCanvasComponent(Canvas* canvas)
	{
		// Retrieve Canvas values
		float canvasWidth = canvas->GetWidth();
		float canvasHeight = canvas->GetHeight();
		int layerNumber = canvas->GetLayerNumber();
		bool _blocksLayers = canvas->GetBlocksLayers();
		std::vector<std::shared_ptr<Button>> canvasButtons = canvas->GetButtons();
		bool _isActive = canvas->IsActive();
		long id = canvas->GetID();

		// Active Checkbox
		if (RenderIsActiveCheckbox(_isActive))
			canvas->SetActive(_isActive);

		// Render Table
		if (FlatEngine::PushTable("##CanvasProperties" + std::to_string(id), 2))
		{
			FlatEngine::RenderIntDragTableRow("##layerNumber" + std::to_string(id), "Canvas layer", layerNumber, 1, 20, 20);
			canvas->SetLayerNumber(layerNumber);
			FlatEngine::RenderFloatDragTableRow("##Canvas width" + std::to_string(id), "Width", canvasWidth, 0.1f, 0.1f, -FLT_MAX);
			FlatEngine::RenderFloatDragTableRow("##Canvas height" + std::to_string(id), "Height", canvasHeight, 0.1f, 0.1f, -FLT_MAX);
			canvas->SetDimensions(canvasWidth, canvasHeight);
			FlatEngine::PopTable();
		}

		// _BlocksLayers Checkbox
		if (FlatEngine::RenderCheckbox("Blocks Layers:", _blocksLayers))
			canvas->SetBlocksLayers(_blocksLayers);							
	}

	void RenderAnimationComponent(Animation* animation)
	{
		std::string path = animation->GetAnimationPath();
		bool _isActive = animation->IsActive();
		long id = animation->GetID();

		// Active Checkbox
		if (RenderIsActiveCheckbox(_isActive))
			animation->SetActive(_isActive);

		bool _canOpenFiles = true;
		FlatEngine::RenderInput("##animationPath" + std::to_string(id), "Path: ", path, _canOpenFiles);
		animation->SetAnimationPath(path);

		if (FlatEngine::GameLoopStarted() && !FlatEngine::GameLoopPaused())
		{
			if (FlatEngine::RenderButton("Play Animation"))
				animation->Play(FlatEngine::GetEllapsedGameTimeInMs());

			if (animation->GetAnimationPath() != "")
				ImGui::SameLine(0, 5);
		}
		
		if (animation->GetAnimationPath() != "")
		{									
			if (FlatEngine::RenderButton("Edit Animation"))
			{
				_showAnimator = true;
				_showAnimationPreview = true;

				SetFocusedAnimation(FlatEngine::LoadAnimationFile(animation->GetAnimationPath()));
				loadedProject->SetLoadedPreviewAnimationPath(animation->GetAnimationPath());
			}
		}
	}

	void RenderAudioComponent(Audio* audio)
	{
		// Retrieve Audio values
		std::string path = audio->GetPath();
		bool _isMusic = audio->IsMusic();
		bool _isActive = audio->IsActive();
		long id = audio->GetID();

		// Active Checkbox
		if (RenderIsActiveCheckbox(_isActive))
			audio->SetActive(_isActive);

		// Path Input
		bool _canOpenFiles = true;
		std::string inputId = "##audioPath_" + std::to_string(id);
		bool _pathChanged = FlatEngine::RenderInput(inputId, "Path: ", path, _canOpenFiles);
		audio->SetPath(path);						
		bool _soundTypeChanged = FlatEngine::RenderCheckbox("Is Music", _isMusic);

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
		if (FlatEngine::RenderButton("Play"))
			audio->Play();
		ImGui::SameLine(0, 5);
		// Pause Audio
		if (FlatEngine::RenderButton("Pause"))
			audio->Pause();
		ImGui::SameLine(0, 5);
		// Stop Audio
		if (FlatEngine::RenderButton("Stop"))
			audio->Stop();
	}

	void RenderTextComponent(Text* text)
	{
		bool _isActive = text->IsActive();
		Texture texture = text->GetTexture();
		float textureWidth = (float)texture.GetWidth();
		float textureHeight = (float)texture.GetHeight();
		int renderOrder = text->GetRenderOrder();
		SDL_Color color = text->GetColor();
		Vector2 offset = text->GetOffset();
		float xOffset = offset.x;
		float yOffset = offset.y;
		long id = text->GetID();

		// Active Checkbox
		if (RenderIsActiveCheckbox(_isActive))
			text->SetActive(_isActive);

		std::string textText = text->GetText();
		if (FlatEngine::RenderInput("##TextContent" + std::to_string(id), "Text: ", textText))
		{
			text->SetText(textText);
			text->LoadText();
		}
		bool _canOpenFiles = true;
		std::string fontPath = text->GetFontPath();
		if (FlatEngine::RenderInput("##FontPath" + std::to_string(id), "Font path: ", fontPath, _canOpenFiles))
			text->SetFontPath(fontPath);

		// Render Table
		if (FlatEngine::PushTable("##TextProperties" + std::to_string(id), 2))
		{
			FlatEngine::RenderTextTableRow("##textWidth" + std::to_string(id), "Text width", std::to_string(textureWidth));
			FlatEngine::RenderTextTableRow("##textHeight" + std::to_string(id), "Text height", std::to_string(textureHeight));
			FlatEngine::RenderTextTableRow("##xTextOffset" + std::to_string(id), "X offset", std::to_string(xOffset));
			FlatEngine::RenderTextTableRow("##yTextOffset" + std::to_string(id), "Y offset", std::to_string(yOffset));
			FlatEngine::RenderIntDragTableRow("##TextRenderOrder" + std::to_string(id), "Render Order", renderOrder, 1, 0, (int)maxSpriteLayers);
			text->SetRenderOrder(renderOrder);
			FlatEngine::PopTable();
		}
	}

	void RenderCharacterControllerComponent(CharacterController* characterController)
	{
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
		if (RenderIsActiveCheckbox(_isActive))
			characterController->SetActive(_isActive);

		// Render Table
		if (FlatEngine::PushTable("##CharacterControllerProps" + std::to_string(id), 2))
		{
			if (FlatEngine::RenderFloatDragTableRow("##MaxAccelerationDrag" + std::to_string(id), "Max Acceleration", maxAcceleration, 0.01f, 0.0f, 20.0f))
				characterController->SetMaxAcceleration(maxAcceleration);
			if (FlatEngine::RenderFloatDragTableRow("##MaxSpeedDrag" + std::to_string(id), "Max Speed", maxSpeed, 0.01f, 0.0f, 1000.0f))
				characterController->SetMaxSpeed(maxSpeed);
			if (FlatEngine::RenderFloatDragTableRow("##AirControlDrag" + std::to_string(id), "Air Control", airControl, 0.01f, 0.0f, 1000.0f))
				characterController->SetAirControl(airControl);
			FlatEngine::RenderTextTableRow("##IsMoving" + std::to_string(id), "Is Moving", isMoving);
			FlatEngine::PopTable();
		}
	}

	void RenderBoxColliderComponent(BoxCollider* boxCollider)
	{
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
		bool _isComposite = boxCollider->IsComposite();
		long id = boxCollider->GetID();
		std::string isCollidingString = "false";
		if (_isColliding)
			isCollidingString = "true";

		// Active Checkbox
		if (RenderIsActiveCheckbox(_isActive))
			boxCollider->SetActive(_isActive);

		// Render Table
		if (FlatEngine::PushTable("##BoxColliderProps" + std::to_string(id), 2))
		{
			if (FlatEngine::RenderFloatDragTableRow("##BoxColliderWidth" + std::to_string(id), "Width", activeWidth, 0.01f, 0.0f, 20.0f))
				boxCollider->SetActiveDimensions(activeWidth, activeHeight);
			if (FlatEngine::RenderFloatDragTableRow("##BoxColliderHeight" + std::to_string(id), "Height", activeHeight, 0.01f, 0.0f, 20.0f))
				boxCollider->SetActiveDimensions(activeWidth, activeHeight);
			if (FlatEngine::RenderFloatDragTableRow("##ActiveOffsetBoxColliderX" + std::to_string(id), "X Offset", activeOffset.x, 0.01f, -FLT_MAX, -FLT_MAX))
				boxCollider->SetActiveOffset(activeOffset);
			if (FlatEngine::RenderFloatDragTableRow("##ActiveOffsetBoxColliderY" + std::to_string(id), "Y Offset", activeOffset.y, 0.01f, -FLT_MAX, -FLT_MAX))
				boxCollider->SetActiveOffset(activeOffset);
			if (FlatEngine::RenderIntDragTableRow("##BoxColliderActiveLayer" + std::to_string(id), "Active layer", activeLayer, 1, 0, 100))
				boxCollider->SetActiveLayer(activeLayer);
			FlatEngine::RenderTextTableRow("##BoxColliderIsColliding" + std::to_string(id), "Is Colliding", isCollidingString);
			FlatEngine::PopTable();
		}

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);							
		if (FlatEngine::RenderCheckbox(" Is Continuous", _isContinuous))
			boxCollider->SetIsContinuous(_isContinuous);
		if (FlatEngine::RenderCheckbox(" Is Static", _isStatic))
			boxCollider->SetIsStatic(_isStatic);
		if (FlatEngine::RenderCheckbox(" Is Solid", _isSolid))
			boxCollider->SetIsSolid(_isSolid);
		if (FlatEngine::RenderCheckbox(" Show Active Radius", _showActiveRadius))
			boxCollider->SetShowActiveRadius(_showActiveRadius);
		if (FlatEngine::RenderCheckbox(" Is Composite", _isComposite))
			boxCollider->SetIsComposite(_isComposite);
	}

	void RenderCircleColliderComponent(CircleCollider* circleCollider)
	{
		bool _isActive = circleCollider->IsActive();
		bool _isColliding = circleCollider->IsColliding();
		float activeRadius = circleCollider->GetActiveRadiusGrid();
		Vector2 activeOffset = circleCollider->GetActiveOffset();
		bool _isContinuous = circleCollider->IsContinuous();
		bool _isStatic = circleCollider->IsStatic();
		bool _isSolid = circleCollider->IsSolid();								
		int activeLayer = circleCollider->GetActiveLayer();
		bool _isComposite = circleCollider->IsComposite();
		long id = circleCollider->GetID();
		std::string isCollidingString = "false";
		if (_isColliding)
			isCollidingString = "true";

		// Active Checkbox
		if (RenderIsActiveCheckbox(_isActive))
			circleCollider->SetActive(_isActive);

		// Render Table
		if (FlatEngine::PushTable("##CircleColliderProps" + std::to_string(id), 2))
		{
			if (FlatEngine::RenderFloatDragTableRow("##CircleColliderActiveRadius" + std::to_string(id), "Radius", activeRadius, 0.01f, 0.0f, 20.0f))
				circleCollider->SetActiveRadiusGrid(activeRadius);
			if (FlatEngine::RenderFloatDragTableRow("##ActiveOffsetCircleColliderX" + std::to_string(id), "X Offset", activeOffset.x, 0.01f, -FLT_MAX, -FLT_MAX))
				circleCollider->SetActiveOffset(activeOffset);
			if (FlatEngine::RenderFloatDragTableRow("##ActiveOffsetCircleColliderY" + std::to_string(id), "Y Offset", activeOffset.y, 0.01f, -FLT_MAX, -FLT_MAX))
				circleCollider->SetActiveOffset(activeOffset);
			if (FlatEngine::RenderIntDragTableRow("##CircleColliderActiveLayer" + std::to_string(id), "Active layer", activeLayer, 1, 0, 100))
				circleCollider->SetActiveLayer(activeLayer);
			FlatEngine::RenderTextTableRow("##CircleColliderIsColliding" + std::to_string(id), "Is Colliding", isCollidingString);
			FlatEngine::PopTable();
		}

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
		if (FlatEngine::RenderCheckbox(" Is Continuous", _isContinuous))
			circleCollider->SetIsContinuous(_isContinuous);
		if (FlatEngine::RenderCheckbox(" Is Static", _isStatic))
			circleCollider->SetIsStatic(_isStatic);
		if (FlatEngine::RenderCheckbox(" Is Solid", _isSolid))
			circleCollider->SetIsSolid(_isSolid);
		if (FlatEngine::RenderCheckbox(" Is Composite", _isComposite))
			circleCollider->SetIsComposite(_isComposite);
	}

	void RenderRigidBodyComponent(RigidBody* rigidBody)
	{
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
		if (RenderIsActiveCheckbox(_isActive))
			rigidBody->SetActive(_isActive);

		// Render Table
		if (FlatEngine::PushTable("##RigidBodyProps" + std::to_string(id), 2))
		{
			if (FlatEngine::RenderFloatDragTableRow("##Mass" + std::to_string(id), "Mass", mass, 0.01f, 0.0f, -FLT_MAX))
				rigidBody->SetMass(mass);
			if (FlatEngine::RenderFloatDragTableRow("##GravityScale" + std::to_string(id), "Gravity Scale", gravity, 0.01f, -FLT_MAX, -FLT_MAX))
				rigidBody->SetGravity(gravity);
			if (FlatEngine::RenderFloatDragTableRow("##FallingGravityScale" + std::to_string(id), "Falling Gravity", fallingGravity, 0.01f, -FLT_MAX, -FLT_MAX))
				rigidBody->SetFallingGravity(fallingGravity);
			if (FlatEngine::RenderFloatDragTableRow("##TerminalVelocity" + std::to_string(id), "Terminal Velocity", terminalVelocity, 0.01f, 0.001f, 1000))
				rigidBody->SetTerminalVelocity(terminalVelocity);
			if (FlatEngine::RenderFloatDragTableRow("##WindResistance" + std::to_string(id), "Wind Resistance", windResistance, 0.01f, 0, 1))
				rigidBody->SetWindResistance(windResistance);
			if (FlatEngine::RenderFloatDragTableRow("##Friction" + std::to_string(id), "Friction", friction, 0.01f, 0, 1))
				rigidBody->SetFriction(friction);
			if (FlatEngine::RenderFloatDragTableRow("##EquilibriumForce" + std::to_string(id), "Equilibrium Force", equilibriumForce, 0.01f, 0, 1000))
				rigidBody->SetEquilibriumForce(equilibriumForce);
			FlatEngine::RenderTextTableRow("##VelocityX" + std::to_string(id), "X Velocity", std::to_string(velocity.x));
			FlatEngine::RenderTextTableRow("##VelocityY" + std::to_string(id), "Y Velocity", std::to_string(velocity.y));
			FlatEngine::RenderTextTableRow("##AccelerationX" + std::to_string(id), "X Acceleration", std::to_string(acceleration.x));
			FlatEngine::RenderTextTableRow("##AccelerationY" + std::to_string(id), "Y Acceleration", std::to_string(acceleration.y));
			FlatEngine::RenderTextTableRow("##PendingForcesX" + std::to_string(id), "X Pending Forces", std::to_string(pendingForces.x));
			FlatEngine::RenderTextTableRow("##PendingForcesY" + std::to_string(id), "Y Pending Forces", std::to_string(pendingForces.y));
			FlatEngine::RenderTextTableRow("##RigidBodyGrounded" + std::to_string(id), "Is Grounded", isGroundedString);
			FlatEngine::PopTable();
		}								

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
		// Kinematic Checkbox
		FlatEngine::RenderCheckbox(" Is Kinematic", _isKinematic);
		rigidBody->SetIsKinematic(_isKinematic);

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
		// Static Checkbox
		FlatEngine::RenderCheckbox(" Is Static", _isStatic);
		rigidBody->SetIsStatic(_isStatic);
	}

	void BeginToolTip(std::string title)
	{
		// Add ImGui styling pushes here
		//
		ImGui::BeginTooltip();
		ImGui::Text(title.c_str());
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
		ImGui::Separator();
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
	}

	void EndToolTip()
	{
		// Add ImGui styling pops here
		ImGui::EndTooltip();
	}

	void RenderToolTipText(std::string label, std::string text)
	{
		std::string newLabel = label + "  |  ";
		ImGui::Text(newLabel.c_str());
		ImGui::SameLine();
		ImGui::Text(text.c_str());
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
		ImGui::Separator();
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
	}

	void RenderToolTipFloat(std::string label, float data)
	{
		std::string newLabel = label + "  |  ";
		ImGui::Text(newLabel.c_str());
		ImGui::SameLine();
		ImGui::Text(std::to_string(data).c_str());
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
		ImGui::Separator();
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
	}

	void RenderToolTipLong(std::string label, long data)
	{
		std::string newLabel = label + "  |  ";
		ImGui::Text(newLabel.c_str());
		ImGui::SameLine();
		ImGui::Text(std::to_string(data).c_str());
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
		ImGui::Separator();
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
	}

	void RenderToolTipLongVector(std::string label, std::vector<long> data)
	{
		std::string newLabel = label + "  |  ";
		ImGui::Text(newLabel.c_str());
		for (int i = 0; i < data.size(); i++)
		{
			std::string dataString = std::to_string(data[i]);
			if (i < data.size() - 1)
				dataString += ",";
			ImGui::SameLine();
			ImGui::Text(dataString.c_str());
		}
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
	}
}