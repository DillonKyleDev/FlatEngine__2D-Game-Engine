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
#include "Script.h"
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
#include "TileMap.h"
#include "TileSet.h"

namespace FL = FlatEngine;

namespace FlatGui 
{
	// ImGui Wrappers
	void BeginComponent(FL::Component* component, long &queuedForDelete)
	{
		bool _isCollapsed = component->IsCollapsed();
		long id = component->GetID();

		std::string componentType = component->GetTypeString();
		std::string componentID = componentType + std::to_string(component->GetID());

		// Begin Component Child
		ImGui::PushStyleColor(ImGuiCol_ChildBg, FL::GetColor("innerWindow"));
		ImGui::PushStyleColor(ImGuiCol_Border, FL::GetColor("componentBorder"));
		ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 2);
		ImGui::BeginChild(componentID.c_str(), Vector2(0, 0), FL::F_autoResizeChildFlags);
		ImGui::PopStyleVar();
		ImGui::PopStyleVar();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();

		// Border around each component
		auto wPos = ImGui::GetWindowPos();
		auto wSize = ImGui::GetWindowSize();		
		ImGui::GetWindowDrawList()->AddRect({ wPos.x + 2, wPos.y + 2 }, { wPos.x + wSize.x - 2, wPos.y + wSize.y - 2 }, FL::GetColor32("componentBorder"), 2);

		// Tooltip for Component meta data
		FL::RenderInvisibleButton("ComponentToolTipButton-" + std::to_string(id), { wPos.x, wPos.y }, { wSize.x, 27 });
		if (ImGui::IsItemHovered() && ImGui::GetIO().KeyAlt)
		{
			FL::BeginToolTip("Component Info");
			FL::RenderToolTipLong("ID", id);
			FL::EndToolTip();
		}
		ImGui::SetCursorScreenPos({ wPos.x + 5, wPos.y + 5 });
		
		// Component Name
		ImGui::SetCursorPos(Vector2(ImGui::GetCursorPosX() + 5, ImGui::GetCursorPosY() + 5));
		ImGui::Text(componentType.c_str());

		ImGui::SameLine(ImGui::GetContentRegionAvail().x - (FL::F_childPadding + 36), 5);

		// Pushes	
		ImGui::PushItemWidth(-1.0f);
		ImGui::PushStyleColor(ImGuiCol_Border, FL::GetColor("componentBorder"));
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, Vector2(1.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 1, 1, 0));

		std::string expandID = "##expandIcon-" + std::to_string(id);
		std::string trashcanID = "##trashIcon-" + std::to_string(id);
		std::string openFileID = "##openFileIcon-" + std::to_string(id);

		ImGui::SetCursorPos(Vector2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() - 3));
		if (FL::RenderImageButton(trashcanID.c_str(), FL::GetTexture("trash")))
			queuedForDelete = component->GetID();

		ImGui::SameLine(0, 5);

		ImGui::SetCursorPos(Vector2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() - 3));
		if (_isCollapsed)
		{
			if (FL::RenderImageButton(expandID.c_str(), FL::GetTexture("expand")))
				component->SetCollapsed(!_isCollapsed);
		}
		else
			if (FL::RenderImageButton(expandID.c_str(), FL::GetTexture("expandFlipped")))
				component->SetCollapsed(!_isCollapsed);

		if (!_isCollapsed)
			ImGui::Separator();
		
		if (!component->IsCollapsed())
		{
			//Component Data - Give it background color
			std::string componentItemID = "##ComponentItem-" + componentType;
			ImGui::PushStyleColor(ImGuiCol_ChildBg, FL::GetColor("componentBg"));
			ImGui::BeginChild(componentItemID.c_str(), Vector2(0, 0), FL::F_autoResizeChildFlags);
		}
	}

	void EndComponent(Component* component)
	{
		// Pops
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleVar();

		// Add some space to the bottom of each component
		if (!component->IsCollapsed())
		{
			ImGui::PopStyleColor();
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
			ImGui::EndChild();
		}

		ImGui::EndChild();
	}

	bool RenderIsActiveCheckbox(bool& _isActive)
	{
		ImGui::SetCursorPos(Vector2(ImGui::GetCursorPosX() + 3, ImGui::GetCursorPosY() + 1));
		bool _checked = FL::RenderCheckbox("Active", _isActive);
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
		if (FL::PushTable("##TransformProperties" + std::to_string(id), 2))
		{
			if (FL::RenderFloatDragTableRow("##xPosition" + std::to_string(id), "X Position", xPos, 0.1f, -FLT_MAX, -FLT_MAX))
			{
				transform->SetPosition(Vector2(xPos, yPos));
			}
			if (FL::RenderFloatDragTableRow("##yPosition" + std::to_string(id), "Y Position", yPos, 0.1f, -FLT_MAX, -FLT_MAX))
			{
				transform->SetPosition(Vector2(xPos, yPos));
			}	
			if (FL::RenderFloatDragTableRow("##rotation" + std::to_string(id), "Rotation", rotation, 0.1f, -360, 360))
				transform->SetRotation(rotation);
			if (FL::RenderFloatDragTableRow("##xScaleDrag" + std::to_string(id), "X Scale", scaleX, 0.1f, 0.001f, 1000) ||
				FL::RenderFloatDragTableRow("##yScaleDrag" + std::to_string(id), "Y Scale", scaleY, 0.1f, 0.001f, 1000))
				transform->SetScale(Vector2(scaleX, scaleY));
			FL::PopTable();
		}
	}

	void RenderSpriteComponent(Sprite* sprite)
	{
		std::string path = FL::GetFilenameFromPath(sprite->GetPath(), true);
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
		int droppedValue = -1;

		// Active Checkbox
		if (RenderIsActiveCheckbox(_isActive))
			sprite->SetActive(_isActive);								

		std::string openedPath = "";
		if (FL::DropInputCanOpenFiles("##InputSpritePath", "Path", FL::GetFilenameFromPath(path, true), "FILE_PATH_DRAGGED", droppedValue, openedPath, "Drop images here from File Explorer"))
		{
			if (openedPath != "")
			{
				sprite->SetTexture(openedPath);
			}
			else if (droppedValue != -1)
			{
				std::filesystem::path fs_path(FL::F_selectedFiles[droppedValue - 1]);
				if (fs_path.extension() == ".png")
				{
					sprite->SetTexture(fs_path.string());
				}
				else
					FL::LogString("ERROR : File must be of type .png to drop here.");
			}
		}

		//if (ImGui::IsItemHovered())
		//	FL::RenderTextToolTip("Drag an image here from the File Explorer");

		// Render Table
		if (FL::PushTable("##SpriteProperties" + std::to_string(id), 2))
		{
			if (FL::RenderFloatDragTableRow("##xSpriteScaleDrag" + std::to_string(id), "X Scale", xScale, 0.1f, 0.001f, 1000))
				sprite->SetScale(Vector2(xScale, yScale));
			if (FL::RenderFloatDragTableRow("##ySpriteScaleDrag" + std::to_string(id), "Y Scale", yScale, 0.1f, 0.001f, 1000))
				sprite->SetScale(Vector2(xScale, yScale));																
			if (FL::RenderFloatDragTableRow("##xSpriteOffsetDrag" + std::to_string(id), "X Offset", xOffset, 0.1f, -FLT_MAX, -FLT_MAX))
				sprite->SetOffset(Vector2(xOffset, yOffset));
			if (FL::RenderFloatDragTableRow("##ySpriteOffsetDrag" + std::to_string(id), "Y Offset", yOffset, 0.1f, -FLT_MAX, -FLT_MAX))
				sprite->SetOffset(Vector2(xOffset, yOffset));										
			if (FL::RenderIntDragTableRow("##renderOrder" + std::to_string(id), "Render Order", renderOrder, 1, 0, (int)FL::F_maxSpriteLayers))
				sprite->SetRenderOrder(renderOrder);
			FL::RenderTextTableRow("##textureWidth" + std::to_string(id), "Texture width", textureWidthString);
			FL::RenderTextTableRow("##textureHeight" + std::to_string(id), "Texture height", textureHeightString);
			FL::PopTable();
		}

		// Pivot Point Buttons							
		ImGui::SetCursorScreenPos(Vector2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y + 5));
		Vector2 cellSize = Vector2(76, 78);
		Vector2 cursorScreen = Vector2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y);

		// TopLeft, Top, TopRight
		ImGui::GetWindowDrawList()->AddRectFilled(cursorScreen, Vector2(cursorScreen.x + cellSize.x, cursorScreen.y + cellSize.y), FL::GetColor32("logBg"));
		ImGui::SetCursorScreenPos(Vector2(cursorScreen.x + 5, cursorScreen.y + 5));
		if (FL::RenderImageButton("##PivotTopLeftButton", FL::GetTexture("upLeft"), Vector2(16, 16), 1, FL::GetColor("imageButtonDark")))
			sprite->SetPivotPoint(Sprite::PivotPoint::TopLeft);
		ImGui::SameLine(0, 3);
		if (FL::RenderImageButton("##PivotTopButton", FL::GetTexture("up"), Vector2(16, 16), 1, FL::GetColor("imageButtonDark")))
			sprite->SetPivotPoint(Sprite::PivotPoint::Top);
		ImGui::SameLine(0, 3);
		if (FL::RenderImageButton("##PivotTopRightButton", FL::GetTexture("upRight"), Vector2(16, 16), 1, FL::GetColor("imageButtonDark")))
			sprite->SetPivotPoint(Sprite::PivotPoint::TopRight);
								
		ImGui::SameLine(0, 17);
		ImGui::Text("Pivot Point:");

		// Left, Center, Right
		ImGui::SetCursorScreenPos(Vector2(ImGui::GetCursorScreenPos().x + 5, ImGui::GetCursorScreenPos().y));
		if (FL::RenderImageButton("##PivotLeftButton", FL::GetTexture("left"), Vector2(16, 16), 1, FL::GetColor("imageButtonDark")))
			sprite->SetPivotPoint(Sprite::PivotPoint::Left);
		ImGui::SameLine(0, 3);
		if (FL::RenderImageButton("##PivotCenterButton", FL::GetTexture("center"), Vector2(16, 16), 1, FL::GetColor("imageButtonDark")))
			sprite->SetPivotPoint(Sprite::PivotPoint::Center);
		ImGui::SameLine(0, 3);
		if (FL::RenderImageButton("##PivotRightButton", FL::GetTexture("right"), Vector2(16, 16), 1, FL::GetColor("imageButtonDark")))
			sprite->SetPivotPoint(Sprite::PivotPoint::Right);
								
		ImGui::SameLine(0, 17);
		ImGui::Text(pivotString.c_str());

		// BottomLeft, Bottom, BottomRight
		ImGui::SetCursorScreenPos(Vector2(ImGui::GetCursorScreenPos().x + 5, ImGui::GetCursorScreenPos().y));
		if (FL::RenderImageButton("##PivotBottomLeftButton", FL::GetTexture("downLeft"), Vector2(16, 16), 1, FL::GetColor("imageButtonDark")))
			sprite->SetPivotPoint(Sprite::PivotPoint::BottomLeft);
		ImGui::SameLine(0, 3);
		if (FL::RenderImageButton("##PivotBottomButton", FL::GetTexture("down"), Vector2(16, 16), 1, FL::GetColor("imageButtonDark")))
			sprite->SetPivotPoint(Sprite::PivotPoint::Bottom);
		ImGui::SameLine(0, 3);
		if (FL::RenderImageButton("##PivotBottomRightButton", FL::GetTexture("downRight"), Vector2(16, 16), 1, FL::GetColor("imageButtonDark")))
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
			following = FL::GetObjectById(camera->GetFollowing())->GetName();
		float followSmoothing = camera->GetFollowSmoothing();

		// Active Checkbox
		if (RenderIsActiveCheckbox(_isActive))
			camera->SetActive(_isActive);

		// Render Table
		if (FL::PushTable("##CameraProperties" + std::to_string(id), 2))
		{
			if (FL::RenderFloatDragTableRow("##cameraWidth" + std::to_string(id), "Camera width", width, 0.1f, 0, 1000))
				camera->SetDimensions(width, height);
			if (FL::RenderFloatDragTableRow("##cameraHeight" + std::to_string(id), "Camera height", height, 0.1f, 0, 1000))
				camera->SetDimensions(width, height);
			if (FL::RenderFloatDragTableRow("##cameraZoom" + std::to_string(id), "Camera zoom", zoom, 0.1f, 1, 100))
				camera->SetZoom(zoom);
			if (FL::RenderCheckboxTableRow("##CameraShouldFollowTargetCheckbox", "Follow", _follow))
				camera->SetShouldFollow(_follow);
			if (FL::RenderFloatDragTableRow("##cameraFollowSmoothing" + std::to_string(id), "Follow Smoothing", followSmoothing, 0.01f, 0, 1))
				camera->SetFollowSmoothing(followSmoothing);
			FL::PopTable();
		}

		int droppedValue = -1;
		if (FL::DropInput("##CameraFollowObject", "Following", following, "DND_HIERARCHY_OBJECT", droppedValue, "Drag a GameObject here from the Hierarchy"))
		{
			if (droppedValue != -1 && FL::GetObjectById(droppedValue) != nullptr)
			{
				camera->SetFollowing(droppedValue);
			}
			else
				FL::LogString("ERROR : Something went wrong, item must be a GameObject with a Transform component");
		}

		// Frustrum color picker
		std::string frustrumID = "##FrustrumColor" + std::to_string(id);
		ImVec4 color = ImVec4(frustrumColor.x / 255.0f, frustrumColor.y / 255.0f, frustrumColor.z / 255.0f, frustrumColor.w / 255.0f);
		ImGui::ColorEdit4(frustrumID.c_str(), (float*)&color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
		ImGui::SameLine(0, 5);
		ImGui::Text("Frustrum color");
		camera->SetFrustrumColor(ImVec4(color.x * 255.0f, color.y * 255.0f, color.z * 255.0f, color.w * 255.0f));

		// Before allowing this camera to be set as primary, we need to ensure it has a transform component
		if (camera->GetParent()->HasComponent(ComponentTypes::T_Transform))
		{									
			if (FL::RenderCheckbox("Is Primary Camera", _isPrimary))
			{
				if (_isPrimary)
					FL::GetLoadedScene()->SetPrimaryCamera(camera);
				else
					FL::GetLoadedScene()->RemovePrimaryCamera();
			}									
		}
		else
		{
			bool temp = false;
			if (FL::RenderCheckbox("Is Primary Camera", temp))
				FL::LogString("FlatGui::RenderInspector() - Attempt to set Camera component as primary failed: No Transform component found...");
			temp = false;
			ImGui::TextWrapped("*A Camera Component must be coupled with a Transform Component to be set as the primary camera.*");
		}

		camera->SetPrimaryCamera(_isPrimary);
	}

	void RenderScriptComponent(Script* script)
	{		
		static int currentLuaScript = 0;
		std::string attachedScriptName = script->GetAttachedScript();
		bool _isActive = script->IsActive();
		long id = script->GetID();

		// Active Checkbox
		if (RenderIsActiveCheckbox(_isActive))
			script->SetActive(_isActive);

		for (int i = 0; i < FL::F_luaScriptNames.size(); i++)
		{
			if (attachedScriptName == FL::F_luaScriptNames[i])
			{
				currentLuaScript = i;
				break;
			}
		}

		std::string newScriptModalLabel = "Create Lua Script";
		std::string newScriptName = "";

		// Select which Lua script is attached to this Script
		if (FL::RenderSelectable("##SelectLuaScript", FL::F_luaScriptNames, currentLuaScript))
			script->SetAttachedScript(FL::F_luaScriptNames[currentLuaScript]);
	
		bool b_openModal = false;
		if (FL::RenderButton("New Script", Vector2(100, 20)))
			b_openModal = true;

		// Create new Lua script modal
		if (FL::RenderInputModal(newScriptModalLabel.c_str(), "Enter a name for the Lua script:", newScriptName, b_openModal))
			FL::CreateNewLuaScript(newScriptName);
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
		if (FL::PushTable("##ButtonProperties" + std::to_string(id), 2))
		{
			FL::RenderFloatDragTableRow("##activeWidth" + std::to_string(id), "Active width", activeWidth, 0.1f, 0, 1000);
			FL::RenderFloatDragTableRow("##activeHeight" + std::to_string(id), "Active height", activeHeight, 0.1f, 0, 1000);
			button->SetActiveDimensions(activeWidth, activeHeight);
			FL::RenderFloatDragTableRow("##activeoffsetx" + std::to_string(id), "X Offset", activeOffset.x, 0.1f, -FLT_MAX, -FLT_MAX);
			FL::RenderFloatDragTableRow("##activeoffsety" + std::to_string(id), "Y Offset", activeOffset.y, 0.1f, -FLT_MAX, -FLT_MAX);
			button->SetActiveOffset(activeOffset);
			FL::PopTable();
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
		if (FL::PushTable("##CanvasProperties" + std::to_string(id), 2))
		{
			FL::RenderIntDragTableRow("##layerNumber" + std::to_string(id), "Canvas layer", layerNumber, 1, 20, 20);
			canvas->SetLayerNumber(layerNumber);
			FL::RenderFloatDragTableRow("##Canvas width" + std::to_string(id), "Width", canvasWidth, 0.1f, 0.1f, -FLT_MAX);
			FL::RenderFloatDragTableRow("##Canvas height" + std::to_string(id), "Height", canvasHeight, 0.1f, 0.1f, -FLT_MAX);
			canvas->SetDimensions(canvasWidth, canvasHeight);
			FL::PopTable();
		}

		// _BlocksLayers Checkbox
		if (FL::RenderCheckbox("Blocks Layers:", _blocksLayers))
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
		FL::RenderInput("##animationPath" + std::to_string(id), "Path: ", path, _canOpenFiles);
		animation->SetAnimationPath(path);

		if (FL::GameLoopStarted() && !FL::GameLoopPaused())
		{
			if (FL::RenderButton("Play Animation"))
				animation->Play(FL::GetEllapsedGameTimeInMs());

			if (animation->GetAnimationPath() != "")
				ImGui::SameLine(0, 5);
		}
		
		if (animation->GetAnimationPath() != "")
		{									
			if (FL::RenderButton("Edit Animation"))
			{
				FG_b_showAnimator = true;
				FG_b_showAnimationPreview = true;

				SetFocusedAnimation(FL::LoadAnimationFile(animation->GetAnimationPath()));
				FL::F_LoadedProject.SetLoadedPreviewAnimationPath(animation->GetAnimationPath());
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
		bool _pathChanged = FL::RenderInput(inputId, "Path: ", path, _canOpenFiles);
		audio->SetPath(path);						
		bool _soundTypeChanged = FL::RenderCheckbox("Is Music", _isMusic);

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
		if (FL::RenderButton("Play"))
			audio->Play();
		ImGui::SameLine(0, 5);
		// Pause Audio
		if (FL::RenderButton("Pause"))
			audio->Pause();
		ImGui::SameLine(0, 5);
		// Stop Audio
		if (FL::RenderButton("Stop"))
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
		if (FL::RenderInput("##TextContent" + std::to_string(id), "Text: ", textText))
		{
			text->SetText(textText);
			text->LoadText();
		}
		bool _canOpenFiles = true;
		std::string fontPath = text->GetFontPath();
		if (FL::RenderInput("##FontPath" + std::to_string(id), "Font path: ", fontPath, _canOpenFiles))
			text->SetFontPath(fontPath);

		// Render Table
		if (FL::PushTable("##TextProperties" + std::to_string(id), 2))
		{
			FL::RenderTextTableRow("##textWidth" + std::to_string(id), "Text width", std::to_string(textureWidth));
			FL::RenderTextTableRow("##textHeight" + std::to_string(id), "Text height", std::to_string(textureHeight));
			FL::RenderTextTableRow("##xTextOffset" + std::to_string(id), "X offset", std::to_string(xOffset));
			FL::RenderTextTableRow("##yTextOffset" + std::to_string(id), "Y offset", std::to_string(yOffset));
			FL::RenderIntDragTableRow("##TextRenderOrder" + std::to_string(id), "Render Order", renderOrder, 1, 0, (int)FL::F_maxSpriteLayers);
			text->SetRenderOrder(renderOrder);
			FL::PopTable();
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
		if (FL::PushTable("##CharacterControllerProps" + std::to_string(id), 2))
		{
			if (FL::RenderFloatDragTableRow("##MaxAccelerationDrag" + std::to_string(id), "Max Acceleration", maxAcceleration, 0.01f, 0.0f, 20.0f))
				characterController->SetMaxAcceleration(maxAcceleration);
			if (FL::RenderFloatDragTableRow("##MaxSpeedDrag" + std::to_string(id), "Max Speed", maxSpeed, 0.01f, 0.0f, 1000.0f))
				characterController->SetMaxSpeed(maxSpeed);
			if (FL::RenderFloatDragTableRow("##AirControlDrag" + std::to_string(id), "Air Control", airControl, 0.01f, 0.0f, 1000.0f))
				characterController->SetAirControl(airControl);
			FL::RenderTextTableRow("##IsMoving" + std::to_string(id), "Is Moving", isMoving);
			FL::PopTable();
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
		if (FL::PushTable("##BoxColliderProps" + std::to_string(id), 2))
		{
			if (FL::RenderFloatDragTableRow("##BoxColliderWidth" + std::to_string(id), "Width", activeWidth, 0.01f, 0.0f, 20.0f))
				boxCollider->SetActiveDimensions(activeWidth, activeHeight);
			if (FL::RenderFloatDragTableRow("##BoxColliderHeight" + std::to_string(id), "Height", activeHeight, 0.01f, 0.0f, 20.0f))
				boxCollider->SetActiveDimensions(activeWidth, activeHeight);
			if (FL::RenderFloatDragTableRow("##ActiveOffsetBoxColliderX" + std::to_string(id), "X Offset", activeOffset.x, 0.01f, -FLT_MAX, -FLT_MAX))
				boxCollider->SetActiveOffset(activeOffset);
			if (FL::RenderFloatDragTableRow("##ActiveOffsetBoxColliderY" + std::to_string(id), "Y Offset", activeOffset.y, 0.01f, -FLT_MAX, -FLT_MAX))
				boxCollider->SetActiveOffset(activeOffset);
			if (FL::RenderIntDragTableRow("##BoxColliderActiveLayer" + std::to_string(id), "Active layer", activeLayer, 1, 0, 100))
				boxCollider->SetActiveLayer(activeLayer);
			FL::RenderTextTableRow("##BoxColliderIsColliding" + std::to_string(id), "Is Colliding", isCollidingString);
			FL::PopTable();
		}

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);							
		if (FL::RenderCheckbox(" Is Continuous", _isContinuous))
			boxCollider->SetIsContinuous(_isContinuous);
		if (FL::RenderCheckbox(" Is Static", _isStatic))
			boxCollider->SetIsStatic(_isStatic);
		if (FL::RenderCheckbox(" Is Solid", _isSolid))
			boxCollider->SetIsSolid(_isSolid);
		if (FL::RenderCheckbox(" Show Active Radius", _showActiveRadius))
			boxCollider->SetShowActiveRadius(_showActiveRadius);
		if (FL::RenderCheckbox(" Is Composite", _isComposite))
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
		if (FL::PushTable("##CircleColliderProps" + std::to_string(id), 2))
		{
			if (FL::RenderFloatDragTableRow("##CircleColliderActiveRadius" + std::to_string(id), "Radius", activeRadius, 0.01f, 0.0f, 20.0f))
				circleCollider->SetActiveRadiusGrid(activeRadius);
			if (FL::RenderFloatDragTableRow("##ActiveOffsetCircleColliderX" + std::to_string(id), "X Offset", activeOffset.x, 0.01f, -FLT_MAX, -FLT_MAX))
				circleCollider->SetActiveOffset(activeOffset);
			if (FL::RenderFloatDragTableRow("##ActiveOffsetCircleColliderY" + std::to_string(id), "Y Offset", activeOffset.y, 0.01f, -FLT_MAX, -FLT_MAX))
				circleCollider->SetActiveOffset(activeOffset);
			if (FL::RenderIntDragTableRow("##CircleColliderActiveLayer" + std::to_string(id), "Active layer", activeLayer, 1, 0, 100))
				circleCollider->SetActiveLayer(activeLayer);
			FL::RenderTextTableRow("##CircleColliderIsColliding" + std::to_string(id), "Is Colliding", isCollidingString);
			FL::PopTable();
		}

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
		if (FL::RenderCheckbox(" Is Continuous", _isContinuous))
			circleCollider->SetIsContinuous(_isContinuous);
		if (FL::RenderCheckbox(" Is Static", _isStatic))
			circleCollider->SetIsStatic(_isStatic);
		if (FL::RenderCheckbox(" Is Solid", _isSolid))
			circleCollider->SetIsSolid(_isSolid);
		if (FL::RenderCheckbox(" Is Composite", _isComposite))
			circleCollider->SetIsComposite(_isComposite);
	}

	void RenderRigidBodyComponent(RigidBody* rigidBody)
	{
		long id = rigidBody->GetID();
		bool _isActive = rigidBody->IsActive();
		float mass = rigidBody->GetMass();
		float gravity = rigidBody->GetGravity();
		float fallingGravity = rigidBody->GetFallingGravity();
		float terminalVelocity = rigidBody->GetTerminalVelocity();
		float angularDrag = rigidBody->GetAngularDrag();
		float windResistance = rigidBody->GetWindResistance();
		float friction = rigidBody->GetFriction();
		float equilibriumForce = rigidBody->GetEquilibriumForce();	
		bool _allowTorques = rigidBody->TorquesAllowed();
		bool _isStatic = rigidBody->IsStatic();
		bool _isGrounded = rigidBody->IsGrounded();

		// Read only
		float inertialMass = rigidBody->GetI();
		Vector2 velocity = rigidBody->GetVelocity();
		Vector2 acceleration = rigidBody->GetAcceleration();
		Vector2 pendingForces = rigidBody->GetPendingForces();
		float angularVelocity = rigidBody->GetAngularVelocity();
		float angularAcceleration = rigidBody->GetAngularAcceleration();
		float pendingTorques = rigidBody->GetPendingTorques();

		std::string isGroundedString = "false";
		if (_isGrounded)
			isGroundedString = "true";

		// Active Checkbox
		if (RenderIsActiveCheckbox(_isActive))
			rigidBody->SetActive(_isActive);

		// Render Table
		if (FL::PushTable("##RigidBodyProps" + std::to_string(id), 2))
		{
			if (FL::RenderFloatDragTableRow("##Mass" + std::to_string(id), "Mass", mass, 0.01f, 0.0f, -FLT_MAX))
				rigidBody->SetMass(mass);
			FL::RenderTextTableRow("##InertialMass" + std::to_string(id), "Inertial Mass", std::to_string(inertialMass));
			if (FL::RenderFloatDragTableRow("##GravityScale" + std::to_string(id), "Gravity Scale", gravity, 0.01f, -FLT_MAX, -FLT_MAX))
				rigidBody->SetGravity(gravity);
			if (FL::RenderFloatDragTableRow("##FallingGravityScale" + std::to_string(id), "Falling Gravity", fallingGravity, 0.01f, -FLT_MAX, -FLT_MAX))
				rigidBody->SetFallingGravity(fallingGravity);
			if (FL::RenderFloatDragTableRow("##TerminalVelocity" + std::to_string(id), "Terminal Velocity", terminalVelocity, 0.01f, 0.001f, 1000))
				rigidBody->SetTerminalVelocity(terminalVelocity);
			if (FL::RenderFloatDragTableRow("##WindResistance" + std::to_string(id), "Wind Resistance", windResistance, 0.01f, 0, 1))
				rigidBody->SetWindResistance(windResistance);
			if (FL::RenderFloatDragTableRow("##Friction" + std::to_string(id), "Friction", friction, 0.01f, 0, 1))
				rigidBody->SetFriction(friction);
			if (FL::RenderFloatDragTableRow("##AngularDrag" + std::to_string(id), "Angular Drag", angularDrag, 0.01f, 0, 1))
				rigidBody->SetAngularDrag(angularDrag);
			if (FL::RenderFloatDragTableRow("##EquilibriumForce" + std::to_string(id), "Equilibrium Force", equilibriumForce, 0.01f, 0, 1000))
				rigidBody->SetEquilibriumForce(equilibriumForce);
			// Linear
			FL::RenderTextTableRow("##VelocityX" + std::to_string(id), "X Velocity", std::to_string(velocity.x));
			FL::RenderTextTableRow("##VelocityY" + std::to_string(id), "Y Velocity", std::to_string(velocity.y));
			FL::RenderTextTableRow("##AccelerationX" + std::to_string(id), "X Acceleration", std::to_string(acceleration.x));
			FL::RenderTextTableRow("##AccelerationY" + std::to_string(id), "Y Acceleration", std::to_string(acceleration.y));
			FL::RenderTextTableRow("##PendingForcesX" + std::to_string(id), "X Pending Forces", std::to_string(pendingForces.x));
			FL::RenderTextTableRow("##PendingForcesY" + std::to_string(id), "Y Pending Forces", std::to_string(pendingForces.y));
			// Rotational
			FL::RenderTextTableRow("##AngularVelocity" + std::to_string(id), "Angular Velocity (deg)", std::to_string(angularVelocity));
			FL::RenderTextTableRow("##AngularAcceleration" + std::to_string(id), "Angular Acceleration (deg)", std::to_string(angularAcceleration));
			FL::RenderTextTableRow("##PendingTorques" + std::to_string(id), "Pending Torques", std::to_string(pendingTorques));
			FL::RenderTextTableRow("##RigidBodyGrounded" + std::to_string(id), "Is Grounded", isGroundedString);
			FL::PopTable();
		}								

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
		// Static Checkbox
		FL::RenderCheckbox(" Is Static", _isStatic);
		rigidBody->SetIsStatic(_isStatic);
		FL::RenderCheckbox(" Allow Torques", _allowTorques);
		rigidBody->SetTorquesAllowed(_allowTorques);
	}

	void RenderTileMapComponent(TileMap* tileMap)
	{
		long id = tileMap->GetID();
		bool _isActive = tileMap->IsActive();
		int width = tileMap->GetWidth();
		int height = tileMap->GetHeight();
		int tileWidth = tileMap->GetTileWidth();
		int tileHeight = tileMap->GetTileHeight();
		std::vector<std::string> tileSets = tileMap->GetTileSets();

		// Active Checkbox
		if (RenderIsActiveCheckbox(_isActive))
			tileMap->SetActive(_isActive);

		// Render Table
		if (FL::PushTable("##tileMapProps" + std::to_string(id), 2))
		{
			if (FL::RenderIntDragTableRow("##Width" + std::to_string(id), "Width", width, 1, 1, INT_MAX))
			{
				tileMap->SetWidth(width);
			}
			if (FL::RenderIntDragTableRow("##Height" + std::to_string(id), "Height", height, 1, 1, INT_MAX))
			{
				tileMap->SetHeight(height);
			}
			if (FL::RenderIntDragTableRow("##TileWidth" + std::to_string(id), "Tile Width", tileWidth, 1, 1, INT_MAX))
			{
				tileMap->SetTileWidth(tileWidth);
			}
			if (FL::RenderIntDragTableRow("##TileHeight" + std::to_string(id), "Tile Height", tileHeight, 1, 1, INT_MAX))
			{
				tileMap->SetTileHeight(tileHeight);
			}
			FL::PopTable();
		}

		ImGui::Text("Add TileSets to TileMap");

		static int currentSelectableTileSet = 0;
		std::string activeTileSet = tileMap->GetSelectedTileSet();
		std::vector<std::string> tileSetNames;

		// Collect TileSets not already in this TileMap that are available to add to it
		for (int i = 0; i < FL::F_TileSets.size(); i++)
		{
			bool b_alreadyInTileMap = false;

			for (int j = 0; j < tileSets.size(); j++)
			{
				if (FL::F_TileSets[i].GetName() == tileSets[j])
					b_alreadyInTileMap = true;
			}

			if (!b_alreadyInTileMap)
				tileSetNames.push_back(FL::F_TileSets[i].GetName());
		}

		FL::RenderSelectable("##SelectTileSet", tileSetNames, currentSelectableTileSet);		
		if (FL::RenderButton("Add TileSet", Vector2(100, 20)))
		{			
			if (tileSetNames.size() >= currentSelectableTileSet + 1)
				tileMap->AddTileSet(tileSetNames[currentSelectableTileSet]);
		}

		Vector2 dirStartPos = ImGui::GetCursorScreenPos();
		Vector2 dirEndPos = Vector2(dirStartPos.x + ImGui::GetContentRegionAvail().x, dirStartPos.y + 24);
		ImGui::GetWindowDrawList()->AddRectFilled(dirStartPos, dirEndPos, FL::GetColor32("panelTitleBg"));
		ImGui::SetCursorScreenPos(Vector2(dirStartPos.x + 5, dirStartPos.y + 5));
		ImGui::Text("TileSets");
		ImGui::SetCursorScreenPos(Vector2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y + 5));
		ImGui::PushStyleColor(ImGuiCol_FrameBg, FL::GetColor("innerWindow"));
		ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, Vector2(0, 0));
		FL::PushMenuStyles();

		if (ImGui::BeginTable("##TileSetsTable", 1, FL::F_tableFlags))
		{
			ImGui::TableSetupColumn("##TileSets", 0, ImGui::GetContentRegionAvail().x);

			for (std::string tileSetName : tileSets)
			{
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);

				ImGuiTreeNodeFlags nodeFlags;
				std::string treeID = "##SelectActiveTileSetTree";

				// If node selected
				if (activeTileSet == tileSetName)
					nodeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_Selected;
				else
					nodeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;
				
				// render a leaf
				ImGui::TreeNodeEx((void*)(intptr_t)treeID.c_str(), nodeFlags, tileSetName.c_str());
				if (ImGui::IsItemClicked())
				{
					tileMap->SetSelectedTileSet(tileSetName);
				}
			}

			ImGui::EndTable();
		}
		FL::PopMenuStyles();
		ImGui::PopStyleVar();
		ImGui::PopStyleColor();

		// Render Tiles within selected TileSet
		static float iconSize = 30;
		FL::PushComboStyles();
		ImGui::SliderFloat("##IconSizeSlider", &iconSize, 10, 128, "%.3f");
		FL::PopComboStyles();

		TileSet* selectedTileSet = FL::GetTileSet(activeTileSet);
		if (selectedTileSet != nullptr)
		{
			float availableWidth = ImGui::GetContentRegionMax().x;
			int iconsThisRow = 0;
			std::map<int, std::pair<Vector2, Vector2>> allTiles = selectedTileSet->GetTileSet();

			for (int index : selectedTileSet->GetTileSetIndices())
			{
				float horizontalSpacing = 5;
				float verticalSpacing = 5;
				
				int maxIconsPerRow = (int)(availableWidth / (iconSize + horizontalSpacing) - 1);
				


				// Drawing the first button in the row
				if (iconsThisRow == 0)
					ImGui::SetCursorScreenPos(Vector2(ImGui::GetCursorScreenPos().x + (horizontalSpacing / 2), (ImGui::GetCursorScreenPos().y + verticalSpacing)));

				Vector2 currentPos = ImGui::GetCursorScreenPos();

				


				SDL_Texture* texture = selectedTileSet->GetTexture()->GetTexture();
				int textureWidth = selectedTileSet->GetTexture()->GetWidth();
				int textureHeight = selectedTileSet->GetTexture()->GetHeight();

				Vector2 uvStart = allTiles.at(index).first;
				Vector2 uvEnd = allTiles.at(index).second;

				uvStart = Vector2(uvStart.x / textureWidth, uvStart.y / textureHeight);
				uvEnd = Vector2(uvEnd.x / textureWidth, uvEnd.y / textureHeight);

				std::string tileButtonID = "##TileSelect" + std::to_string(index);
				if (FL::RenderImageButton(tileButtonID, texture, Vector2(iconSize, iconSize), 0, FL::GetColor("imageButton"), FL::GetColor("imageButtonTint"), FL::GetColor("imageButtonHovered"), FL::GetColor("imageButtonActive"), uvStart, uvEnd))
				{
					std::pair<std::string, int> tileBrushPair = { selectedTileSet->GetName(), index };
					FL::F_tileSetAndIndexOnBrush = tileBrushPair;
					FL::F_CursorMode = FL::F_CURSOR_MODE::TILE_BRUSH;
				}



				if (iconsThisRow != maxIconsPerRow)
				{
					ImGui::SetCursorScreenPos(Vector2(currentPos.x + iconSize + horizontalSpacing, currentPos.y)); // Ready to draw the next button
					iconsThisRow++;
				}
				else
					iconsThisRow = 0;
			}
		}


		// BoxCollider Areas
		static std::string selectedCollisionArea = "";
		std::map<std::string, BoxCollider> boxCollisionAreas = tileMap->GetBoxCollisionAreas();

		std::vector<std::string> areaNames;
		for (std::pair<std::string, BoxCollider> collisionArea : boxCollisionAreas)
		{
			areaNames.push_back(collisionArea.first);

		}
		FL::RenderSelectable("##BoxColliderAreas", tileSetNames, currentSelectableTileSet);

		if (FL::RenderButton("Add BoxCollision Area", Vector2(150, 20)))
		{
			BoxCollider newCollisionArea = BoxCollider();
			newCollisionArea.SetActiveDimensions(10, 5);

			tileMap->AddBoxCollisionArea("Ground Collision", newCollisionArea);
		}

		dirStartPos = ImGui::GetCursorScreenPos();
		dirEndPos = Vector2(dirStartPos.x + ImGui::GetContentRegionAvail().x, dirStartPos.y + 24);
		ImGui::GetWindowDrawList()->AddRectFilled(dirStartPos, dirEndPos, FL::GetColor32("panelTitleBg"));
		ImGui::SetCursorScreenPos(Vector2(dirStartPos.x + 5, dirStartPos.y + 5));
		ImGui::Text("Collision Areas");
		ImGui::SetCursorScreenPos(Vector2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y + 5));
		ImGui::PushStyleColor(ImGuiCol_FrameBg, FL::GetColor("innerWindow"));
		ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, Vector2(0, 0));
		FL::PushMenuStyles();

		if (ImGui::BeginTable("##CollisionAreasTable", 1, FL::F_tableFlags))
		{
			ImGui::TableSetupColumn("##CollisionAreaName", 0, ImGui::GetContentRegionAvail().x);

			for (std::pair<std::string, BoxCollider> collisionArea : boxCollisionAreas)
			{
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);

				ImGuiTreeNodeFlags nodeFlags;
				std::string treeID = "##SelectActiveCollisionAreaTree";

				// If node selected
				if (selectedCollisionArea == collisionArea.first)
					nodeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_Selected;
				else
					nodeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;

				// render a leaf
				ImGui::TreeNodeEx((void*)(intptr_t)treeID.c_str(), nodeFlags, collisionArea.first.c_str());
				if (ImGui::IsItemClicked())
				{
					selectedCollisionArea = collisionArea.first;
				}
			}

			ImGui::EndTable();
		}
		FL::PopMenuStyles();
		ImGui::PopStyleVar();
		ImGui::PopStyleColor();

		// Bottom padding
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
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