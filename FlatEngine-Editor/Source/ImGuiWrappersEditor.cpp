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
	void BeginComponent(FL::Component* component, FL::Component* &queuedForDelete, std::string typeNameOverride)
	{
		bool _isCollapsed = component->IsCollapsed();
		long id = component->GetID();

		std::string componentType = component->GetTypeString();
		if (typeNameOverride != "")
		{
			componentType = typeNameOverride;
		}
		std::string componentID = component->GetTypeString() + std::to_string(component->GetID());

		// Begin Component Child
		ImGui::PushStyleColor(ImGuiCol_ChildBg, FL::GetColor("innerWindow"));
		ImGui::PushStyleColor(ImGuiCol_Border, FL::GetColor("componentBorder"));
		ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 2);
		ImGui::SetNextWindowScroll(Vector2(0, 0));
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
			FL::BeginToolTip("Component Data");
			FL::RenderToolTipLong("ID", id);
			FL::RenderToolTipLong("ParentID", component->GetParentID());
			FL::EndToolTip();
		}
		ImGui::SetCursorScreenPos({ wPos.x + 5, wPos.y + 5 });
		
		// Component Name
		ImGui::SetCursorPos(Vector2(ImGui::GetCursorPosX() + 5, ImGui::GetCursorPosY() + 5));
		ImGui::Text(componentType.c_str());

		if (component->GetType() != FL::T_Transform)
		{
			ImGui::SameLine(ImGui::GetContentRegionAvail().x - (FL::F_childPadding + 36), 5);
		}
		else
		{
			ImGui::SameLine(ImGui::GetContentRegionAvail().x - (FL::F_childPadding + 13), 5);
		}

		// Pushes	
		ImGui::PushItemWidth(-1.0f);
		ImGui::PushStyleColor(ImGuiCol_Border, FL::GetColor("componentBorder"));
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, Vector2(1.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 1, 1, 0));

		std::string expandID = "##expandIcon-" + std::to_string(id);
		std::string trashcanID = "##trashIcon-" + std::to_string(id);
		std::string openFileID = "##openFileIcon-" + std::to_string(id);

		if (component->GetType() != FL::T_Transform)
		{			
			FL::MoveScreenCursor(0, -3);
			if (FL::RenderImageButton(trashcanID.c_str(), FL::GetTexture("trash")))
			{
				queuedForDelete = component;
			}
			ImGui::SameLine(0, 5);
		}

		FL::MoveScreenCursor(0, -3);
		if (_isCollapsed)
		{
			if (FL::RenderImageButton(expandID.c_str(), FL::GetTexture("expand")))
			{
				component->SetCollapsed(!_isCollapsed);
			}
		}
		else
		{
			if (FL::RenderImageButton(expandID.c_str(), FL::GetTexture("expandFlipped")))
			{
				component->SetCollapsed(!_isCollapsed);
			}
		}

		if (!_isCollapsed)
		{
			ImGui::Separator();
		}
		
		if (!component->IsCollapsed())
		{			
			std::string componentItemID = "##ComponentItem-" + component->GetTypeString();
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
		FL::MoveScreenCursor(1, 3);
		bool _checked = FL::RenderCheckbox("Active", _isActive);
		FL::MoveScreenCursor(0, 3);
		ImGui::Separator();
		FL::MoveScreenCursor(0, 3);

		return _checked;
	}

	void RenderTransformComponent(Transform* transform)
	{
		// Position, scale, and rotation of transform
		Vector2 position = transform->GetPosition();
		float xPos = position.x;
		float yPos = position.y;
		Vector2 scale = transform->GetScale();
		float xScale = scale.x;
		float yScale = scale.y;
		float rotation = transform->GetRotation();
		bool _isActive = transform->IsActive();
		long id = transform->GetID();

		// Active Checkbox
		if (RenderIsActiveCheckbox(_isActive))
		{
			transform->SetActive(_isActive);
		}

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
			{
				transform->SetRotation(rotation);
			}
			if (FL::RenderFloatDragTableRow("##xScaleDrag" + std::to_string(id), "X Scale", xScale, 0.1f, 0.001f, 1000))
			{
				if (xScale > 0)
				{
					transform->SetScale(Vector2(xScale, yScale));
				}
				else
				{
					FL::LogError("Scale must be greater than 0.");
				}
			}
			if (FL::RenderFloatDragTableRow("##yScaleDrag" + std::to_string(id), "Y Scale", yScale, 0.1f, 0.001f, 1000))
			{
				if (yScale > 0)
				{
					transform->SetScale(Vector2(xScale, yScale));
				}
				else
				{
					FL::LogError("Scale must be greater than 0.");
				}
			}
				
			FL::PopTable();
		}
	}

	void RenderSpriteComponent(Sprite* sprite)
	{
		std::string path = FL::GetFilenameFromPath(sprite->GetPath(), true);
		float textureWidth = sprite->GetTextureWidth();
		float textureHeight = sprite->GetTextureHeight();
		Vector2 textureScale = sprite->GetScale();
		FL::Pivot pivotPoint = sprite->GetPivotPoint();
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
		{
			sprite->SetActive(_isActive);
		}

		int droppedValue = -1;
		std::string openedPath = "";
		if (FL::DropInputCanOpenFiles("##InputSpritePath", "File", FL::GetFilenameFromPath(path, true), FL::F_fileExplorerTarget, droppedValue, openedPath, "Drop images here from File Explorer"))
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
				{
					FL::LogError("File must be of type .png to drop here.");
				}
			}
			else if (droppedValue == -1)
			{
				sprite->RemoveTexture();
			}
		}

		FL::RenderSeparator(2, 3);

		// Render Table
		if (FL::PushTable("##SpriteProperties" + std::to_string(id), 2))
		{
			if (FL::RenderFloatDragTableRow("##xSpriteScaleDrag" + std::to_string(id), "X Scale", xScale, 0.1f, 0.001f, 1000))
			{
				if (xScale > 0)
				{
					sprite->SetScale(Vector2(xScale, yScale));
				}
				else
				{
					FL::LogError("Scale must be greater than 0.");
				}
			}
			if (FL::RenderFloatDragTableRow("##ySpriteScaleDrag" + std::to_string(id), "Y Scale", yScale, 0.1f, 0.001f, 1000))
			{
				if (yScale > 0)
				{
					sprite->SetScale(Vector2(xScale, yScale));
				}
				else
				{
					FL::LogError("Scale must be greater than 0.");
				}
			}
			if (FL::RenderFloatDragTableRow("##xSpriteOffsetDrag" + std::to_string(id), "X Offset", xOffset, 0.1f, -FLT_MAX, -FLT_MAX))
			{
				sprite->SetOffset(Vector2(xOffset, yOffset));
			}
			if (FL::RenderFloatDragTableRow("##ySpriteOffsetDrag" + std::to_string(id), "Y Offset", yOffset, 0.1f, -FLT_MAX, -FLT_MAX))
			{
				sprite->SetOffset(Vector2(xOffset, yOffset));
			}
			if (FL::RenderIntDragTableRow("##renderOrder" + std::to_string(id), "Render Order", renderOrder, 1, 0, (int)FL::F_maxSpriteLayers))
			{
				sprite->SetRenderOrder(renderOrder);
			}
			FL::RenderTextTableRow("##textureWidth" + std::to_string(id), "Texture width", textureWidthString);
			FL::RenderTextTableRow("##textureHeight" + std::to_string(id), "Texture height", textureHeightString);
			FL::PopTable();
		}

		FL::RenderSeparator(3, 3);

		if (RenderPivotSelectionButtons("Sprite", pivotPoint))
		{
			sprite->SetPivotPoint(pivotPoint);
		}

		FL::RenderSeparator(6, 3);

		// Tint color picker
		std::string tintID = "##SpriteTintColor" + std::to_string(id) + "-" + std::to_string(id);
		//ImVec4 color = ImVec4(tintColor.x * 255.0f, tintColor.y * 255.0f, tintColor.z * 255.0f, tintColor.w * 255.0f);
		if (ImGui::ColorEdit4(tintID.c_str(), (float*)&tintColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel))
		{
			sprite->SetTintColor(tintColor);
		}
		ImGui::SameLine(0, 5);
		ImGui::Text("Tint color");
	}

	bool RenderPivotSelectionButtons(std::string componentType, FL::Pivot& pivot)
	{				
		ImGui::Text("Pivot Point: ");
		ImGui::SameLine();
		ImGui::Text(FL::F_PivotStrings[pivot].c_str());
		FL::MoveScreenCursor(0, 1);		

		bool b_pivotChanged = false;
		Vector2 cellSize = Vector2(ImGui::GetContentRegionAvail().x, 72);
		Vector2 cursorScreen = ImGui::GetCursorScreenPos();

		// TopLeft, Top, TopRight
		ImGui::GetWindowDrawList()->AddRectFilled(cursorScreen, Vector2(cursorScreen.x + cellSize.x, cursorScreen.y + cellSize.y), FL::GetColor32("pivotSelectionBg"));
		FL::MoveScreenCursor(5, 5);
		if (FL::RenderImageButton("##" + componentType + "PivotTopLeftButton", FL::GetTexture("upLeft"), Vector2(16, 16), 1, FL::GetColor("imageButtonDark"), FL::GetColor("imageButtonTint"), FL::GetColor("pivotButtonHovered")))
		{
			pivot = FL::Pivot::PivotTopLeft;
			b_pivotChanged = true;
		}
		ImGui::SameLine(0, 3);
		if (FL::RenderImageButton("##" + componentType + "PivotTopButton", FL::GetTexture("up"), Vector2(16, 16), 1, FL::GetColor("imageButtonDark"), FL::GetColor("imageButtonTint"), FL::GetColor("pivotButtonHovered")))
		{
			pivot = FL::Pivot::PivotTop;
			b_pivotChanged = true;
		}
		ImGui::SameLine(0, 3);
		if (FL::RenderImageButton("##" + componentType + "PivotTopRightButton", FL::GetTexture("upRight"), Vector2(16, 16), 1, FL::GetColor("imageButtonDark"), FL::GetColor("imageButtonTint"), FL::GetColor("pivotButtonHovered")))
		{
			pivot = FL::Pivot::PivotTopRight;
			b_pivotChanged = true;
		}

		// Left, Center, Right
		FL::MoveScreenCursor(5, 0);
		if (FL::RenderImageButton("##" + componentType + "PivotLeftButton", FL::GetTexture("left"), Vector2(16, 16), 1, FL::GetColor("imageButtonDark"), FL::GetColor("imageButtonTint"), FL::GetColor("pivotButtonHovered")))
		{
			pivot = FL::Pivot::PivotLeft;
			b_pivotChanged = true;
		}
		ImGui::SameLine(0, 3);
		if (FL::RenderImageButton("##" + componentType + "PivotCenterButton", FL::GetTexture("center"), Vector2(16, 16), 1, FL::GetColor("imageButtonDark"), FL::GetColor("imageButtonTint"), FL::GetColor("pivotButtonHovered")))
		{
			pivot = FL::Pivot::PivotCenter;
			b_pivotChanged = true;
		}
		ImGui::SameLine(0, 3);
		if (FL::RenderImageButton("##" + componentType + "PivotRightButton", FL::GetTexture("right"), Vector2(16, 16), 1, FL::GetColor("imageButtonDark"), FL::GetColor("imageButtonTint"), FL::GetColor("pivotButtonHovered")))
		{
			pivot = FL::Pivot::PivotRight;
			b_pivotChanged = true;
		}

		// BottomLeft, Bottom, BottomRight		
		FL::MoveScreenCursor(5, 0);
		if (FL::RenderImageButton("##" + componentType + "PivotBottomLeftButton", FL::GetTexture("downLeft"), Vector2(16, 16), 1, FL::GetColor("imageButtonDark"), FL::GetColor("imageButtonTint"), FL::GetColor("pivotButtonHovered")))
		{
			pivot = FL::Pivot::PivotBottomLeft;
			b_pivotChanged = true;
		}
		ImGui::SameLine(0, 3);
		if (FL::RenderImageButton("##" + componentType + "PivotBottomButton", FL::GetTexture("down"), Vector2(16, 16), 1, FL::GetColor("imageButtonDark"), FL::GetColor("imageButtonTint"), FL::GetColor("pivotButtonHovered")))
		{
			pivot = FL::Pivot::PivotBottom;
			b_pivotChanged = true;
		}
		ImGui::SameLine(0, 3);
		if (FL::RenderImageButton("##" + componentType + "PivotBottomRightButton", FL::GetTexture("downRight"), Vector2(16, 16), 1, FL::GetColor("imageButtonDark"), FL::GetColor("imageButtonTint"), FL::GetColor("pivotButtonHovered")))
		{
			pivot = FL::Pivot::PivotBottomRight;
			b_pivotChanged = true;
		}
				
		return b_pivotChanged;
	}

	void RenderCameraComponent(Camera* camera)
	{
		float width = camera->GetWidth();
		float height = camera->GetHeight();
		bool b_isPrimary = camera->IsPrimary();
		float zoom = camera->GetZoom();
		ImVec4 frustrumColor = camera->GetFrustrumColor();
		bool b_isActive = camera->IsActive();
		long id = camera->GetID();
		bool b_follow = camera->GetShouldFollow();
		std::string followingName = "";
		long toFollowID = camera->GetToFollowID();
		GameObject* followingObject = FL::GetObjectById(toFollowID);

		if (toFollowID != -1 && followingObject != nullptr)
		{
			followingName = FL::GetObjectById(toFollowID)->GetName();
		}
		else if (followingObject == nullptr)
		{
			camera->SetToFollowID(-1);
		}

		float followSmoothing = camera->GetFollowSmoothing();

		// Active Checkbox
		if (RenderIsActiveCheckbox(b_isActive))
		{
			camera->SetActive(b_isActive);
		}

		// Render Table
		if (FL::PushTable("##CameraProperties" + std::to_string(id), 2))
		{
			if (FL::RenderFloatDragTableRow("##cameraWidth" + std::to_string(id), "Camera width", width, 0.1f, 0, 1000))
			{
				camera->SetDimensions(width, height);
			}
			if (FL::RenderFloatDragTableRow("##cameraHeight" + std::to_string(id), "Camera height", height, 0.1f, 0, 1000))
			{
				camera->SetDimensions(width, height);
			}
			if (FL::RenderFloatDragTableRow("##cameraZoom" + std::to_string(id), "Camera zoom", zoom, 0.1f, 1, 100))
			{
				camera->SetZoom(zoom);
			}
			if (FL::RenderFloatDragTableRow("##cameraFollowSmoothing" + std::to_string(id), "Follow Smoothing", followSmoothing, 0.01f, 0, 1))
			{
				camera->SetFollowSmoothing(followSmoothing);
			}
			FL::PopTable();
		}

		FL::RenderSeparator(3, 3);

		int droppedValue = -1;
		if (FL::DropInput("##CameraFollowObject", "Following", followingName, "DND_HIERARCHY_OBJECT", droppedValue, "Drag a GameObject here from the Hierarchy"))
		{
			if (FL::GetObjectById(droppedValue) != nullptr || droppedValue == -1)
			{
				camera->SetToFollowID(droppedValue);
			}
		}

		ImGui::BeginDisabled(toFollowID == -1);
		if (FL::RenderCheckbox("Follow", b_follow))
		{
			camera->SetShouldFollow(b_follow);
		}
		ImGui::EndDisabled();

		FL::RenderSeparator(3, 3);
			
		if (FL::RenderCheckbox("Is Primary Camera", b_isPrimary))
		{
			camera->SetPrimaryCamera(b_isPrimary);
			if (b_isPrimary)
			{
				FL::GetLoadedScene()->SetPrimaryCamera(camera);
			}
			else
			{
				FL::GetLoadedScene()->RemovePrimaryCamera();
			}
		}		

		// Frustrum color picker
		std::string frustrumID = "##FrustrumColor" + std::to_string(id);
		ImVec4 color = ImVec4(frustrumColor.x / 255.0f, frustrumColor.y / 255.0f, frustrumColor.z / 255.0f, frustrumColor.w / 255.0f);
		ImGui::ColorEdit4(frustrumID.c_str(), (float*)&color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
		ImGui::SameLine(0, 5);
		ImGui::Text("Frustrum color");
		camera->SetFrustrumColor(ImVec4(color.x * 255.0f, color.y * 255.0f, color.z * 255.0f, color.w * 255.0f));

	}

	void RenderScriptComponent(Script* script)
	{		
		static int currentLuaScript = 0;
		std::string attachedScriptName = script->GetAttachedScript();
		bool _isActive = script->IsActive();
		long id = script->GetID();

		// Active Checkbox
		if (RenderIsActiveCheckbox(_isActive))
		{
			script->SetActive(_isActive);
		}

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
		{
			script->SetAttachedScript(FL::F_luaScriptNames[currentLuaScript]);
		}
	
		bool b_openModal = false;
		if (FL::RenderButton("New Script", Vector2(100, 20)))
		{
			b_openModal = true;
		}

		// Create new Lua script modal
		if (FL::RenderInputModal(newScriptModalLabel.c_str(), "Enter a name for the Lua script:", newScriptName, b_openModal))
		{
			FL::CreateNewLuaScript(newScriptName);
		}
	}

	void RenderButtonComponent(Button* button)
	{
		bool _isActive = button->IsActive();
		float activeWidth = button->GetActiveWidth();
		float activeHeight = button->GetActiveHeight();
		Vector2 activeOffset = button->GetActiveOffset();
		int activeLayer = button->GetActiveLayer();	
		long id = button->GetID();

		if (RenderIsActiveCheckbox(_isActive))
		{
			button->SetActive(_isActive);
		}

		if (FL::PushTable("##ButtonProperties" + std::to_string(id), 2))
		{
			if (FL::RenderIntDragTableRow("##activeLayer" + std::to_string(id), "Active layer", activeLayer, 1, 20, 20))
			{
				button->SetActiveLayer(activeLayer);
			}
			if (FL::RenderFloatDragTableRow("##activeWidth" + std::to_string(id), "Active width", activeWidth, 0.1f, 0, 1000))
			{
				button->SetActiveDimensions(activeWidth, activeHeight);
			}
			if (FL::RenderFloatDragTableRow("##activeHeight" + std::to_string(id), "Active height", activeHeight, 0.1f, 0, 1000))
			{
				button->SetActiveDimensions(activeWidth, activeHeight);
			}
			if (FL::RenderFloatDragTableRow("##activeoffsetx" + std::to_string(id), "X Offset", activeOffset.x, 0.1f, -FLT_MAX, -FLT_MAX))
			{
				button->SetActiveOffset(activeOffset);
			}
			if (FL::RenderFloatDragTableRow("##activeoffsety" + std::to_string(id), "Y Offset", activeOffset.y, 0.1f, -FLT_MAX, -FLT_MAX))
			{
				button->SetActiveOffset(activeOffset);
			}
			FL::PopTable();
		}
	}

	void RenderCanvasComponent(Canvas* canvas)
	{
		float canvasWidth = canvas->GetWidth();
		float canvasHeight = canvas->GetHeight();
		int layerNumber = canvas->GetLayerNumber();
		bool _blocksLayers = canvas->GetBlocksLayers();
		bool _isActive = canvas->IsActive();
		long id = canvas->GetID();

		if (RenderIsActiveCheckbox(_isActive))
		{
			canvas->SetActive(_isActive);
		}

		if (FL::PushTable("##CanvasProperties" + std::to_string(id), 2))
		{
			if (FL::RenderIntDragTableRow("##layerNumber" + std::to_string(id), "Canvas layer", layerNumber, 1, 20, 20))
			{
				canvas->SetLayerNumber(layerNumber);
			}
			if (FL::RenderFloatDragTableRow("##Canvas width" + std::to_string(id), "Width", canvasWidth, 0.1f, 0.1f, -FLT_MAX))
			{
				canvas->SetDimensions(canvasWidth, canvasHeight);
			}
			if (FL::RenderFloatDragTableRow("##Canvas height" + std::to_string(id), "Height", canvasHeight, 0.1f, 0.1f, -FLT_MAX))
			{
				canvas->SetDimensions(canvasWidth, canvasHeight);
			}
			FL::PopTable();
		}

		FL::RenderSeparator(3, 3);

		if (FL::RenderCheckbox("Blocks Layers:", _blocksLayers))
		{
			canvas->SetBlocksLayers(_blocksLayers);
		}
	}

	void RenderAnimationComponent(Animation* animation)
	{
		std::string path = animation->GetAnimationPath();
		bool _isActive = animation->IsActive();
		long id = animation->GetID();

		if (RenderIsActiveCheckbox(_isActive))
		{
			animation->SetActive(_isActive);
		}

		int droppedValue = -1;
		std::string openedPath = "";
		if (FL::DropInputCanOpenFiles("##AnimationPathInspectorwindow-" + std::to_string(id), "Path", FL::GetFilenameFromPath(path, true), FL::F_fileExplorerTarget, droppedValue, openedPath, "Drop animation files here from the File Explorer"))
		{
			if (openedPath != "")
			{
				animation->SetAnimationPath(openedPath);
			}
			else if (droppedValue != -1)
			{
				std::filesystem::path fs_path(FL::F_selectedFiles[droppedValue - 1]);
				if (fs_path.extension() == ".anm")
				{
					animation->SetAnimationPath(fs_path.string());
				}
				else
				{
					FL::LogError("File must be of type .anm to drop here.");
				}
			}
		}

		FL::RenderSeparator(3, 3);

		if (FL::GameLoopStarted() && !FL::GameLoopPaused())
		{
			if (FL::RenderButton("Play Animation"))
			{
				animation->Play(FL::GetEllapsedGameTimeInMs());
			}

			if (animation->GetAnimationPath() != "")
			{
				ImGui::SameLine(0, 5);
			}
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
		long id = audio->GetID();
		bool _isActive = audio->IsActive();
		std::vector<FL::SoundData> &sounds = audio->GetSounds();

		if (RenderIsActiveCheckbox(_isActive))
		{
			audio->SetActive(_isActive);
		}

		static std::string path = "";
		static std::string name = "";
		static bool b_isNewAudioMusic = false;

		FL::RenderSubTitle("Add Audio");

		FL::RenderInput("##NameNewAudioDataObject", "Name", name, false);

		FL::MoveScreenCursor(0, 3);

		int droppedValue = -1;
		std::string openedPath = "";
		if (FL::DropInputCanOpenFiles("##AddAudioFile", "File", FL::GetFilenameFromPath(path, true), FL::F_fileExplorerTarget, droppedValue, openedPath, "Drop font files here from File Explorer"))
		{
			if (openedPath != "")
			{
				path = openedPath;
			}
			else if (droppedValue != -1)
			{
				std::filesystem::path fs_path(FL::F_selectedFiles[droppedValue - 1]);
				if (fs_path.extension() == ".wav" || fs_path.extension() == ".mp4")
				{
					path = FL::F_selectedFiles[droppedValue - 1];
				}
				else
				{
					FL::LogError("File must be of type audio to drop here.");
				}
			}
		}
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 4);

		FL::RenderCheckbox("Is Music?", b_isNewAudioMusic);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 4);

		if (FL::RenderButton("Add Audio"))
		{
			if (name != "" && !audio->ContainsName(name) && path != "" && FL::DoesFileExist(path))
			{
				audio->AddSound(name, path, b_isNewAudioMusic);
				path = "";
				name = "";
				b_isNewAudioMusic = false;
			}
			else
			{
				if (audio->ContainsName(name))
				{
					FL::LogError("Name already taken in this Audio component, please choose a different one.");
				}
				if (name == "")
				{
					FL::LogError("Please enter a valid name for the sound object.");
				}
				if (path == "")
				{
					FL::LogError("Please enter a valid path for the sound object.");
				}
			}
		}

		if (sounds.size() > 0)
		{
			FL::RenderSeparator(4, 4);
			FL::RenderSubTitle("Attached Audio Files");
		}

		// Show existing Sounds in this Audio component
		int IDCounter = 0;
		for (std::vector<FL::SoundData>::iterator soundIter = sounds.begin(); soundIter != sounds.end(); soundIter++)
		{
			FL::SoundData &sound = (*soundIter);
			std::string audioPath = sound.path;
			std::string audioName = sound.name;
			std::string newName = audioName;
			bool b_isMusic = sound.b_isMusic;
			int newDroppedValue = -1;
			std::string inputId = "##audioPath_" + std::to_string(id) + sound.name + std::to_string(IDCounter);

			if (FL::RenderInput("##NameExistingAudioDataObject" + std::to_string(IDCounter), "Name", sound.name, false))
			{				
				sound.name = audioName;
			}
			FL::MoveScreenCursor(0, 4);

			if (FL::DropInput(inputId, "File", FL::GetFilenameFromPath(audioPath, true), FL::F_fileExplorerTarget, newDroppedValue, "Drop audio files here from the Explorer window"))
			{
				if (newDroppedValue != -1 && FL::F_selectedFiles.size() >= newDroppedValue)
				{
					audioPath = FL::F_selectedFiles[newDroppedValue - 1];
					sound.path = audioPath;
					if (sound.b_isMusic)
						sound.sound->LoadMusic(audioPath);
					else
						sound.sound->LoadEffect(audioPath);
				}
				else if (newDroppedValue == -1)
				{
					audioPath = "";
					sound.path = "";					
				}
			}
			FL::MoveScreenCursor(0, 4);

			if (FL::RenderCheckbox("Is Music##" + std::to_string(IDCounter), b_isMusic))
			{
				sound.b_isMusic = b_isMusic;
				audio->LoadAudio(sound);
			}

			// Play Audio
			if (FL::RenderImageButton("##ImageButtonPlay" + sound.name, FL::GetTexture("play")))
				audio->PlaySound(sound.name);
			ImGui::SameLine(0, 5);
			// Pause Audio
			if (FL::RenderImageButton("##ImageButtonPause" + sound.name, FL::GetTexture("pause")))
				audio->PauseSound(sound.name);
			ImGui::SameLine(0, 5);
			// Stop Audio
			if (FL::RenderImageButton("##ImageButtonStop" + sound.name, FL::GetTexture("stop")))
				audio->StopSound(sound.name);	

			ImGui::SameLine(ImGui::GetContentRegionAvail().x - 90, 0);

			if (FL::RenderButton("Remove Sound##"+ sound.name))
			{
				sounds.erase(soundIter);
				break;
			}

			if (sound.name != sounds.back().name)
			{
				FL::RenderSeparator(4, 6);
			}

			IDCounter++;
		}
	}

	void RenderTextComponent(Text* text)
	{
		bool _isActive = text->IsActive();
		std::shared_ptr<Texture> texture = text->GetTexture();
		float textureWidth = (float)texture->GetWidth();
		float textureHeight = (float)texture->GetHeight();
		int renderOrder = text->GetRenderOrder();
		int fontSize = text->GetFontSize();
		FL::Pivot pivotPoint = text->GetPivotPoint();
		Vector4 color = text->GetColor();
		Vector2 offset = text->GetOffset();
		float xOffset = offset.x;
		float yOffset = offset.y;
		long id = text->GetID();

		if (RenderIsActiveCheckbox(_isActive))
		{
			text->SetActive(_isActive);
		}

		std::string textText = text->GetText();
		if (FL::RenderInput("##TextContent" + std::to_string(id), "Text", textText))
		{
			text->SetText(textText);
			text->LoadText();
		}

		FL::MoveScreenCursor(0, 3);
		
		std::string fontPath = text->GetFontPath();
		int droppedValue = -1;
		std::string openedPath = "";
		if (FL::DropInputCanOpenFiles("##InputFontPath", "File", FL::GetFilenameFromPath(fontPath, true), FL::F_fileExplorerTarget, droppedValue, openedPath, "Drop font files here from File Explorer"))
		{
			if (openedPath != "")
			{
				text->SetFontPath(fontPath);
			}
			else if (droppedValue != -1)
			{
				std::filesystem::path fs_path(FL::F_selectedFiles[droppedValue - 1]);
				if (fs_path.extension() == ".ttf")
				{
					text->SetFontPath(fontPath);
				}
				else
				{
					FL::LogError("File must be of type .ttf to drop here.");
				}
			}
		}

		FL::RenderSeparator(3, 3);

		if (FL::PushTable("##TextProperties" + std::to_string(id), 2))
		{
			FL::RenderTextTableRow("##textWidth" + std::to_string(id), "Text width", std::to_string(textureWidth));
			FL::RenderTextTableRow("##textHeight" + std::to_string(id), "Text height", std::to_string(textureHeight));
			if (FL::RenderIntDragTableRow("##textFontSize" + std::to_string(id), "Font size", fontSize, 1, 0, 1000))
			{
				text->SetFontSize(fontSize);
			}
			if (FL::RenderFloatDragTableRow("##xTextOffset" + std::to_string(id), "X offset", xOffset, 0.1f, -FLT_MAX, -FLT_MAX))
			{
				text->SetOffset(Vector2(xOffset, yOffset));
			}
			if (FL::RenderFloatDragTableRow("##yTextOffset" + std::to_string(id), "Y offset", yOffset, 0.1f, -FLT_MAX, -FLT_MAX))
			{
				text->SetOffset(Vector2(xOffset, yOffset));
			}
			if (FL::RenderIntDragTableRow("##TextRenderOrder" + std::to_string(id), "Render Order", renderOrder, 1, 0, (int)FL::F_maxSpriteLayers))
			{
				text->SetRenderOrder(renderOrder);
			}
			FL::PopTable();
		}

		FL::RenderSeparator(3, 3);

		if (RenderPivotSelectionButtons("Text", pivotPoint))
		{
			text->SetPivotPoint(pivotPoint);
		}

		FL::RenderSeparator(6, 3);

		// Tint color picker
		std::string tintID = "##TextColor" + std::to_string(id) + "-" + std::to_string(id);
		//ImVec4 tempColor = ImVec4(color.x * 255.0f, color.y * 255.0f, color.z * 255.0f, color.w * 255.0f);
		if (ImGui::ColorEdit4(tintID.c_str(), (float*)&color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel))
		{
			text->SetColor(color);
		}
		ImGui::SameLine(0, 5);
		ImGui::Text("Text color");
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

		if (RenderIsActiveCheckbox(_isActive))
			characterController->SetActive(_isActive);

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

	void RenderBoxColliderComponent(BoxCollider* boxCollider, TileMap* tileMap, std::string collisionAreaName)
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

		if (RenderIsActiveCheckbox(_isActive))
			boxCollider->SetActive(_isActive);

		float widthIncrement = 0.01f;
		float heightIncrement = 0.01f;
		float offsetIncrementX = 0.01f;
		float offsetIncrementY = 0.01f;
		float maxWidth = 1000.0f;
		float maxHeight = 1000.0f;

		if (tileMap != nullptr && collisionAreaName != "")
		{
			float tileWidth = (float)tileMap->GetTileWidth();
			float tileHeight = (float)tileMap->GetTileHeight();
			float tileMapWidth = (float)tileMap->GetWidth();
			float tileMapHeight = (float)tileMap->GetHeight();

			widthIncrement = tileWidth / FL::F_pixelsPerGridSpace;
			heightIncrement = tileHeight / FL::F_pixelsPerGridSpace;
			offsetIncrementX = tileWidth / FL::F_pixelsPerGridSpace;
			offsetIncrementY = tileHeight / FL::F_pixelsPerGridSpace;
			maxWidth = tileMapWidth * tileWidth / FL::F_pixelsPerGridSpace;
			maxHeight = tileMapWidth * tileWidth / FL::F_pixelsPerGridSpace;
		}

		if (FL::PushTable("##BoxColliderProps" + std::to_string(id), 2))
		{
			if (FL::RenderFloatDragTableRow("##BoxColliderWidth" + std::to_string(id), "Width", activeWidth, widthIncrement, 0.0f, maxWidth))
				boxCollider->SetActiveDimensions(activeWidth, activeHeight);
			if (FL::RenderFloatDragTableRow("##BoxColliderHeight" + std::to_string(id), "Height", activeHeight, heightIncrement, 0.0f, maxHeight))
				boxCollider->SetActiveDimensions(activeWidth, activeHeight);
			if (FL::RenderFloatDragTableRow("##ActiveOffsetBoxColliderX" + std::to_string(id), "X Offset", activeOffset.x, offsetIncrementX, -FLT_MAX, -FLT_MAX))
				boxCollider->SetActiveOffset(activeOffset);
			if (FL::RenderFloatDragTableRow("##ActiveOffsetBoxColliderY" + std::to_string(id), "Y Offset", activeOffset.y, offsetIncrementY, -FLT_MAX, -FLT_MAX))
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

		// Enter Collision Area draw mode for this BoxCollider
		if (tileMap != nullptr && collisionAreaName != "")
		{
			if (FL::RenderImageButton("##DrawCollisionArea" + std::to_string(id), FL::GetTexture("tileColliderDraw")))
			{
				FG_currentSelectedColliderArea = collisionAreaName;
				FL::F_CursorMode = FL::F_CURSOR_MODE::TILE_COLLIDER_DRAW;
			}
			if (ImGui::IsItemHovered())
				FL::RenderTextToolTip("Draw this collision area directly in the Scene View");
		}
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

		if (RenderIsActiveCheckbox(_isActive))
			circleCollider->SetActive(_isActive);

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
		Vector2 velocity = rigidBody->GetVelocity();		
		Vector2 pendingForces = rigidBody->GetPendingForces();
		float angularVelocity = rigidBody->GetAngularVelocity();
		float pendingTorques = rigidBody->GetPendingTorques();

		std::string isGroundedString = "false";
		if (_isGrounded)
			isGroundedString = "true";

		if (RenderIsActiveCheckbox(_isActive))
			rigidBody->SetActive(_isActive);

		if (FL::PushTable("##RigidBodyProps" + std::to_string(id), 2))
		{
			if (FL::RenderFloatDragTableRow("##Mass" + std::to_string(id), "Mass", mass, 0.01f, 0.0f, -FLT_MAX))
				rigidBody->SetMass(mass);			
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
			FL::RenderTextTableRow("##PendingForcesX" + std::to_string(id), "X Pending Forces", std::to_string(pendingForces.x));
			FL::RenderTextTableRow("##PendingForcesY" + std::to_string(id), "Y Pending Forces", std::to_string(pendingForces.y));
			// Rotational
			FL::RenderTextTableRow("##AngularVelocity" + std::to_string(id), "Angular Velocity (deg)", std::to_string(angularVelocity));
			FL::RenderTextTableRow("##PendingTorques" + std::to_string(id), "Pending Torques", std::to_string(pendingTorques));
			FL::RenderTextTableRow("##RigidBodyGrounded" + std::to_string(id), "Is Grounded", isGroundedString);
			FL::PopTable();
		}								

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);

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
		int renderOrder = tileMap->GetRenderOrder();
		std::vector<std::string> tileSets = tileMap->GetTileSets();

		if (RenderIsActiveCheckbox(_isActive))
			tileMap->SetActive(_isActive);

		if (FL::PushTable("##tileMapProps" + std::to_string(id), 2))
		{
			if (FL::RenderIntDragTableRow("##Width" + std::to_string(id), "Width", width, 1, 1, INT_MAX))
			{
				tileMap->SetWidth(width);
				//tileMap->CalculateColliderValues();
			}
			if (FL::RenderIntDragTableRow("##Height" + std::to_string(id), "Height", height, 1, 1, INT_MAX))
			{
				tileMap->SetHeight(height);
				//tileMap->RecalcCollisionAreaValues();
			}
			if (FL::RenderIntDragTableRow("##TileWidth" + std::to_string(id), "Tile Width", tileWidth, 1, 1, INT_MAX))
			{
				tileMap->SetTileWidth(tileWidth);
				//tileMap->RecalcCollisionAreaValues();
			}
			if (FL::RenderIntDragTableRow("##TileHeight" + std::to_string(id), "Tile Height", tileHeight, 1, 1, INT_MAX))
			{
				tileMap->SetTileHeight(tileHeight);
				//tileMap->RecalcCollisionAreaValues();
			}
			if (FL::RenderIntDragTableRow("##RenderOrder" + std::to_string(id), "Render Order", renderOrder, 1, 0, FL::F_maxSpriteLayers))
			{
				tileMap->SetRenderOrder(renderOrder);
			}
			FL::PopTable();
		}

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 4);
		ImGui::Separator();
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 4);

		FL::RenderSubTitle("Add TileSets");

		static int currentSelectableTileSet = 0;
		static int currentSelectableCollisionArea = 0;
		std::string activeTileSet = tileMap->GetSelectedTileSet();
		std::vector<std::string> tileSetNames;

		// Collect TileSets not already in this TileMap that are available to add to it
		for (int i = 0; i < FL::F_TileSets.size(); i++)
		{
			bool b_alreadyInTileMap = false;

			for (int j = 0; j < tileSets.size(); j++)
			{
				if (FL::F_TileSets[i].GetName() == tileSets[j])
				{
					b_alreadyInTileMap = true;
				}
			}

			if (!b_alreadyInTileMap)
			{
				tileSetNames.push_back(FL::F_TileSets[i].GetName());
			}
		}
		
		if (tileSetNames.size() > 0)
		{
			FL::RenderSelectable("##SelectTileSet", tileSetNames, currentSelectableTileSet);
			if (FL::RenderButton("Add to Palettes", Vector2(120, 20)))
			{
				if (tileSetNames.size() >= currentSelectableTileSet + 1)
				{
					tileMap->AddTileSet(tileSetNames[currentSelectableTileSet]);
				}
			}
		}


		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 4);
		ImGui::Separator();
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 4);

		FL::RenderSubTitle("Tile Palettes");

		if (tileSets.size() > 0)
		{
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
		}

		Vector2 tileSetTilesStart = ImGui::GetCursorScreenPos();
		Vector2 regionAvailable = ImGui::GetContentRegionMax();

		// Render Tiles within selected TileSet
		TileSet* selectedTileSet = FL::GetTileSet(activeTileSet);
		if (selectedTileSet != nullptr)
		{
			// Tile icon size slider
			FL::MoveScreenCursor(10, 10);
			ImGui::Text("Icon Size");
			ImGui::SameLine();
			static float iconSize = 50;
			FL::MoveScreenCursor(0, -5);
			FL::PushComboStyles();
			ImGui::SliderFloat("##IconSizeSlider", &iconSize, 10, 128, "%.3f");
			FL::PopComboStyles();

			// Render Palette Tile Icons
			float availableWidth = ImGui::GetWindowSize().x;
			int iconsThisRow = 0;
			std::map<int, std::pair<Vector2, Vector2>> allTiles = selectedTileSet->GetTileSet();
			float horizontalSpacing = 5;
			float verticalSpacing = 2;
			int maxIconsPerRow = (int)(((regionAvailable.x - 20) / (iconSize + (horizontalSpacing))) - 1);

			for (int index : selectedTileSet->GetTileSetIndices())
			{				
				// Drawing the first button in the row
				if (iconsThisRow == 0)
				{
					FL::MoveScreenCursor(horizontalSpacing + 5, verticalSpacing);					
				}

				Vector2 currentPos = ImGui::GetCursorScreenPos();
				SDL_Texture* texture = selectedTileSet->GetTexture()->GetTexture();
				int textureWidth = selectedTileSet->GetTexture()->GetWidth();
				int textureHeight = selectedTileSet->GetTexture()->GetHeight();
				Vector2 uvStart = allTiles.at(index).first;
				Vector2 uvEnd = allTiles.at(index).second;
				uvStart = Vector2(uvStart.x / textureWidth, uvStart.y / textureHeight);
				uvEnd = Vector2(uvEnd.x / textureWidth, uvEnd.y / textureHeight);

				std::string tileButtonID = "##TileSelect" + std::to_string(index);				
				if (FL::RenderImageButton(tileButtonID, texture, Vector2(iconSize, iconSize), 0, FL::GetColor("imageButton"), FL::GetColor("imageButtonTint"), FL::GetColor("tileSetIconHovered"), FL::GetColor("imageButtonActive"), uvStart, uvEnd))
				{
					std::pair<std::string, int> tileBrushPair = { selectedTileSet->GetName(), index };
					FL::F_tileSetAndIndexOnBrush = tileBrushPair;
					FL::F_CursorMode = FL::F_CURSOR_MODE::TILE_BRUSH;
				}

				if (iconsThisRow < maxIconsPerRow)
				{
					ImGui::SetCursorScreenPos(Vector2(currentPos.x + iconSize + horizontalSpacing, currentPos.y)); // Ready to draw the next button
					iconsThisRow++;
				}
				else
				{
					iconsThisRow = 0;
				}

				FL::MoveScreenCursor(0, iconSize + verticalSpacing + 5);
			}
		
			ImGui::GetWindowDrawList()->AddRect(tileSetTilesStart, Vector2(tileSetTilesStart.x + regionAvailable.x - 5, ImGui::GetCursorScreenPos().y), FL::GetColor32("componentSectionBorder"), 0, 0, 2);
		}

		ImGui::SetCursorScreenPos(Vector2(tileSetTilesStart.x, ImGui::GetCursorScreenPos().y + 2));
		ImGui::Separator();
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 4);


		FL::RenderSubTitle("Collision Areas");

		// Collision Areas
		std::map<std::string, std::vector<FL::CollisionAreaData>> &collisionAreas = tileMap->GetCollisionAreas();
		static std::string selectedCollisionArea = "";
		static int currentSelectedColArea = 0;
		static std::string collisionAreaLabel = "";
		std::vector<std::string> areaNames;
		for (std::pair<std::string, std::vector<FL::CollisionAreaData>> collisionArea : collisionAreas)
		{
			areaNames.push_back(collisionArea.first);
		}

		// BUG INTRODUCED WHEN CLOSING THE GAME VIA Quit button (possibly) after adding this tree to the end of this section.
		// Create new Collision Area
		FL::RenderInput("##CollisionAreaLabel" + std::to_string(id), "Collision Area Name", collisionAreaLabel, false);

		FL::RenderSeparator(3, 3);

		if (FL::RenderButton("Add Collision Area"))
		{
			if (!tileMap->ContainsCollisionAreaLabel(collisionAreaLabel))
			{
				std::vector<FL::CollisionAreaData> newData = std::vector<FL::CollisionAreaData>();				
				collisionAreas.emplace(collisionAreaLabel, newData);					
			}
			else
			{
				FL::LogError("Collision area label already being used in this TileMap, please choose a different one.");
			}
		}


		if (areaNames.size() > 0)
		{
			ImGui::PushStyleColor(ImGuiCol_FrameBg, FL::GetColor("innerWindow"));
			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, Vector2(0, 0));
			FL::PushMenuStyles();
			if (ImGui::BeginTable("#CollisionAreasTable", 1, FL::F_tableFlags))
			{
				ImGui::TableSetupColumn("##CollisionArea", 0, ImGui::GetContentRegionAvail().x);

				for (std::string areaName : areaNames)
				{
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);

					ImGuiTreeNodeFlags nodeFlags;
					std::string treeID = "##SelectActiveTileSetTree";

					// If node selected
					if (selectedCollisionArea == areaName)
						nodeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_Selected;
					else
						nodeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;

					// render a leaf
					ImGui::TreeNodeEx((void*)(intptr_t)treeID.c_str(), nodeFlags, areaName.c_str());
					if (ImGui::IsItemClicked())
					{
						tileMap->SetSelectedCollisionArea(areaName);
						selectedCollisionArea = areaName;
					}
				}

				ImGui::EndTable();
			}
			FL::PopMenuStyles();
			ImGui::PopStyleVar();
			ImGui::PopStyleColor();
		}

		if (selectedCollisionArea != "")
		{
			if (FL::RenderButton("Create Collisions"))
			{
				FL::F_CursorMode = FL::F_CURSOR_MODE::TILE_COLLIDER_DRAW;
				FG_collisionAreasBuffer.clear();
			}

			if (FL::RenderButton("Save Collisions"))
			{
				FL::F_CursorMode = FL::F_CURSOR_MODE::TILE_COLLIDER_DRAW;
				tileMap->SetCollisionAreaValues(selectedCollisionArea, FG_collisionAreasBuffer);
				FG_collisionAreasBuffer.clear();
			}
		}

		if (areaNames.size() > 0)
		{			
			FL::MoveScreenCursor(0, 5);
		}
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