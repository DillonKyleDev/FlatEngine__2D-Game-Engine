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
	void BeginWindow(std::string name)
	{
		PushWindowStyles();
		ImGui::Begin(name.c_str());
		PopWindowStyles();

		BeginWindowChild(name);
	}

	void BeginWindow(std::string name, bool& _isOpen)
	{
		PushWindowStyles();
		ImGui::Begin(name.c_str(), &_isOpen);
		PopWindowStyles();

		BeginWindowChild(name);
	}

	void EndWindow()
	{
		EndWindowChild();
		ImGui::End();
	}

	void BeginWindowChild(std::string title)
	{
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ChildBg, outerWindowColor);
		ImGui::BeginChild(title.c_str(), Vector2(0, 0), childFlags);
		ImGui::PopStyleColor();
	}

	void BeginResizeWindowChild(std::string title)
	{
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ChildBg, outerWindowColor);
		ImGui::BeginChild(title.c_str(), Vector2(0, 0), resizeChildFlags);
		ImGui::PopStyleColor();
	}

	void EndWindowChild()
	{
		ImGui::EndChild();
	}

	void PushWindowStyles()
	{
		ImGui::PushStyleColor(ImGuiCol_WindowBg, windowBgColor);
		ImGui::PushStyleColor(ImGuiCol_FrameBg, frameBgColor);
		ImGui::PushStyleColor(ImGuiCol_FrameBgActive, frameBgActiveColor);
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, frameBgHoveredColor);
		ImGui::PushStyleColor(ImGuiCol_Tab, tabColor);
		ImGui::PushStyleColor(ImGuiCol_TabActive, tabActiveColor);
		ImGui::PushStyleColor(ImGuiCol_TabHovered, tabHoveredColor);
		ImGui::PushStyleColor(ImGuiCol_TabUnfocused, tabUnfocusedColor);
		ImGui::PushStyleColor(ImGuiCol_TabUnfocusedActive, tabUnfocusedActiveColor);
		ImGui::PushStyleColor(ImGuiCol_TitleBg, titleBgColor);
		ImGui::PushStyleColor(ImGuiCol_TitleBgActive, titleBgActiveColor);
		ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed, titleBgCollapsedColor);
		ImGui::PushStyleColor(ImGuiCol_TextSelectedBg, textSelectedBgColor);
		ImGui::PushStyleColor(ImGuiCol_ResizeGrip, resizeGripColor);
		ImGui::PushStyleColor(ImGuiCol_ResizeGripActive, resizeGripActiveColor);
		ImGui::PushStyleColor(ImGuiCol_ResizeGripHovered, resizeGripHoveredColor);
		ImGui::PushStyleColor(ImGuiCol_PopupBg, popupBgColor);
		ImGui::PushStyleColor(ImGuiCol_NavWindowingHighlight, navWindowHighlightColor);
		ImGui::PushStyleColor(ImGuiCol_NavHighlight, navHighlightColor);
		ImGui::PushStyleColor(ImGuiCol_NavWindowingDimBg, navWindowDimBgColor);
		ImGui::PushStyleColor(ImGuiCol_ModalWindowDimBg, modalWindowDimBgColor);
		ImGui::PushStyleColor(ImGuiCol_DockingPreview, dockingPreviewColor);
		ImGui::PushStyleColor(ImGuiCol_DockingEmptyBg, dockingPreviewEmptyColor);
	}

	void PopWindowStyles()
	{
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
	}

	void PushComboStyles()
	{
		ImGui::PushStyleColor(ImGuiCol_Button, comboArrowColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, comboArrowHoveredColor);
		ImGui::PushStyleColor(ImGuiCol_FrameBg, comboBgColor);
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, comboHoveredColor);
		// For Selectables
		ImGui::PushStyleColor(ImGuiCol_Header, comboSelectableColor);
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, comboSelectedColor);
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, comboHighlightedColor);
	}

	void PopComboStyles()
	{
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		// For Selectables
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
	}

	void PushMenuStyles()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 10, 5 });
		//ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, Vector2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_Header, treeSelectableSelectedColor);
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, treeSelectableHoveredColor);
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, treeSelectableActiveColor);
	}

	void PopMenuStyles()
	{
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		//ImGui::PopStyleVar();
		ImGui::PopStyleVar();
	}

	void PushTableStyles()
	{
		ImGui::PushStyleColor(ImGuiCol_TableRowBg, tableCellDarkColor);
		ImGui::PushStyleColor(ImGuiCol_TableRowBgAlt, tableCellLightColor);
	}

	void PopTableStyles()
	{
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
	}

	bool PushTable(std::string id, int columns, ImGuiTableFlags flags)
	{
		float columnWidth = ImGui::GetContentRegionAvail().x / columns;
		PushTableStyles();
		bool _beginTable = ImGui::BeginTable(id.c_str(), columns, flags);
		if (_beginTable)
		{
			for (int i = 0; i < columns; i++)
			{
				std::string columnLabel = id + std::to_string(i);
				ImGui::TableSetupColumn(columnLabel.c_str());
			}
		}
		else
			PopTableStyles();

		return _beginTable;
	}

	bool RenderFloatDragTableRow(std::string id, std::string fieldName, float& value, float increment, float min, float max)
	{
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
		ImGui::Text(fieldName.c_str());
		ImGui::TableSetColumnIndex(1);
		bool _isChanged = RenderDragFloat(id.c_str(), 0, value, increment, min, max);
		ImGui::PushID(id.c_str());
		ImGui::PopID();

		return _isChanged;
	}

	bool RenderTagListTableRow(std::string id, std::string fieldName, FlatEngine::TagList &tagList)
	{
		bool _changed = false;
		bool _hasTag = tagList.HasTag(fieldName);
		bool _ignoreTag = tagList.IgnoresTag(fieldName);
		std::string hasTagID = "##" + fieldName + "CheckboxHasTagID";
		std::string ignoreTagID = "##" + fieldName + "CheckboxIgnoreTagID";

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
		ImGui::Text(fieldName.c_str());

		ImGui::TableSetColumnIndex(1);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
		if (RenderCheckbox(hasTagID.c_str(), _hasTag))
		{
			tagList.ToggleTag(fieldName);
			_changed = true;
		}

		ImGui::TableSetColumnIndex(2);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
		if (RenderCheckbox(ignoreTagID.c_str(), _ignoreTag))
		{
			tagList.ToggleIgnore(fieldName);
			_changed = true;
		}

		ImGui::PushID(id.c_str());
		ImGui::PopID();

		return _changed;
	}

	bool RenderIntDragTableRow(std::string id, std::string fieldName, int& value, float speed, int min, int max)
	{
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
		ImGui::Text(fieldName.c_str());
		ImGui::TableSetColumnIndex(1);
		bool _isChanged = RenderDragInt(id.c_str(), 0, value, speed, min, max);
		ImGui::PushID(id.c_str());
		ImGui::PopID();

		return _isChanged;
	}

	bool RenderCheckboxTableRow(std::string id, std::string fieldName, bool& _value)
	{
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
		ImGui::Text(fieldName.c_str());
		ImGui::TableSetColumnIndex(1);
		bool _checked = RenderCheckbox("", _value);
		ImGui::PushID(id.c_str());
		ImGui::PopID();

		return _checked;
	}

	void RenderSelectableTableRow(std::string id, std::string fieldName, std::vector<std::string> options, int& current_option)
	{
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
		ImGui::Text(fieldName.c_str());
		ImGui::TableSetColumnIndex(1);
		RenderSelectable(id, options, current_option);
		ImGui::PushID(id.c_str());
		ImGui::PopID();
	}

	void RenderTextTableRow(std::string id, std::string fieldName, std::string value, std::string value2)
	{
		// Push uneditableTableTextColor text color
		ImGui::PushStyleColor(ImGuiCol_Text, noEditTableTextColor);

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		// Set table cell bg color
		ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(noEditTableRowFieldBgColor));
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
		ImGui::Text(fieldName.c_str());
		ImGui::TableSetColumnIndex(1);
		// Set table cell bg color
		ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(noEditTableRowValueBgColor));
		ImGui::Text(value.c_str());
		if (value2 != "")
		{
			ImGui::TableSetColumnIndex(2);
			// Set table cell bg color
			ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(noEditTableRowValueBgColor));
			ImGui::Text(value2.c_str());
		}
		ImGui::PushID(id.c_str());
		ImGui::PopID();

		// Pop text color
		ImGui::PopStyleColor();
	}

	void PopTable()
	{
		ImGui::EndTable();
		PopTableStyles();
	}

	bool RenderInput(std::string id, std::string label, std::string& value, bool _canOpenFiles, ImGuiInputTextFlags flags)
	{
		bool _editedButton = false;
		bool _editedInput = false;
		char newPath[1024];
		strcpy_s(newPath, value.c_str());
		std::string pathString = label;

		if (pathString != "")
		{
			ImGui::Text(pathString.c_str());
			ImGui::SameLine(0, 5);
		}

		if (_canOpenFiles)
		{
			std::string buttonId = id + "openFileButton";
			if (RenderImageButton(buttonId.c_str(), openFileIcon.GetTexture()))
			{
				std::string assetPath = FlatEngine::OpenLoadFileExplorer();
				strcpy_s(newPath, assetPath.c_str());
				_editedButton = true;
			}
			ImGui::SameLine();
		}

		ImGui::PushStyleColor(ImGuiCol_FrameBg, inputColor);
		_editedInput = ImGui::InputText(id.c_str(), newPath, IM_ARRAYSIZE(newPath), flags);
		ImGui::PopStyleColor();

		if (newPath != " ")
			value = newPath;
		return _editedButton || _editedInput;
	}

	void RenderSelectable(std::string id, std::vector<std::string> options, int& current_option)
	{
		PushComboStyles();
		PushMenuStyles();
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		if (ImGui::BeginCombo(id.c_str(), options[current_option].c_str()))
		{
			for (int n = 0; n < options.size(); n++)
			{
				bool is_selected = (options[current_option] == options[n]); // You can store your selection however you want, outside or inside your objects
				if (ImGui::Selectable(options[n].c_str(), is_selected))
					current_option = n;
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		PopMenuStyles();
		PopComboStyles();
	}

	void PushTreeList(std::string id)
	{
		ImGui::PushStyleColor(ImGuiCol_FrameBg, innerWindowColor);
		ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, Vector2(0, 0));
		PushMenuStyles();
		ImGui::BeginTable(id.c_str(), 1, tableFlags);
		ImGui::TableSetupColumn("##PROPERTY", 0, ImGui::GetContentRegionAvail().x + 1);
	}

	void PopTreeList()
	{
		ImGui::EndTable();
		PopMenuStyles();
		ImGui::PopStyleVar();
		ImGui::PopStyleColor();
	}

	void RenderTreeLeaf(std::string name, std::string& node_clicked)
	{
		ImGuiTreeNodeFlags node_flags;

		std::string treeID = name + "_node";
		if (node_clicked == name)
			node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_Selected;
		else
			node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;

		//// TreeNode Opener - No TreePop because it's a leaf
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);

		ImGui::TreeNodeEx((void*)(intptr_t)treeID.c_str(), node_flags, name.c_str());
		if (ImGui::IsItemClicked())
			node_clicked = name;

		ImGui::PushID(treeID.c_str());
		ImGui::PopID();
	}

	bool RenderButton(std::string text, Vector2 size, float rounding, Vector4 color, Vector4 hoverColor, Vector4 activeColor)
	{
		bool _isClicked;

		ImGui::PushStyleColor(ImGuiCol_Button, color);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hoverColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, activeColor);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, rounding);

		if (size.x != 0 || size.y != 0)
			_isClicked = ImGui::Button(text.c_str(), size);
		else
			_isClicked = ImGui::Button(text.c_str());

		// Set Mouse Cursor
		if (ImGui::IsItemHovered())
			ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_Hand);

		ImGui::PopStyleVar();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();

		return _isClicked;
	}

	bool RenderImageButton(std::string id, SDL_Texture* texture, Vector2 size, float rounding, Vector4 bgColor, Vector4 tint, Vector4 hoverColor, Vector4 activeColor)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, bgColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hoverColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, activeColor);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, Vector2(2, 2));
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, rounding);

		bool _isClicked = ImGui::ImageButton(id.c_str(), texture, size, uv0, uv1, transparentColor, tint);

		// Set Mouse Cursor
		if (ImGui::IsItemHovered())
			ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_Hand);

		ImGui::PopStyleVar();
		ImGui::PopStyleVar();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();

		return _isClicked;
	}

	bool RenderDragFloat(std::string text, float width, float& value, float increment, float min, float max, ImGuiSliderFlags flags)
	{
		ImGui::PushStyleColor(ImGuiCol_FrameBg, dragColor);
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, dragHoveredColor);
		ImGui::PushStyleColor(ImGuiCol_FrameBgActive, dragActiveColor);

		if (width != 0)
			ImGui::SetNextItemWidth(width);
		else
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		bool _sliderChanged = ImGui::DragFloat(text.c_str(), &value, increment, min, max, "%.3f", flags);

		// Set cursor type
		if (ImGui::IsItemHovered())
			ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_ResizeEW);

		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();

		return _sliderChanged;
	}

	bool RenderDragInt(std::string text, float width, int& value, float speed, int min, int max, ImGuiSliderFlags flags)
	{
		ImGui::PushStyleColor(ImGuiCol_FrameBg, dragColor);
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, dragHoveredColor);
		ImGui::PushStyleColor(ImGuiCol_FrameBgActive, dragActiveColor);

		if (width != 0)
			ImGui::SetNextItemWidth(width);
		else
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		bool _sliderChanged = ImGui::DragInt(text.c_str(), &value, speed, min, max, "%d", flags);
		// Set cursor type
		if (ImGui::IsItemHovered())
			ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_ResizeEW);

		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();

		return _sliderChanged;
	}

	bool RenderCheckbox(std::string text, bool& _toCheck)
	{
		ImGui::PushStyleColor(ImGuiCol_FrameBg, checkboxBgColor);
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, checkboxHoveredColor);
		ImGui::PushStyleColor(ImGuiCol_FrameBgActive, checkboxActiveColor);
		ImGui::PushStyleColor(ImGuiCol_CheckMark, checkboxCheckColor);

		bool _checked = ImGui::Checkbox(text.c_str(), &_toCheck);

		if (ImGui::IsItemHovered())
			ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_Hand);

		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();

		return _checked;
	}

	void RenderSectionHeader(std::string headerText, float height)
	{
		auto cursorPos = ImGui::GetCursorScreenPos();
		auto regionAvailable = ImGui::GetContentRegionAvail();

		ImGui::Separator();
		ImGui::Separator();
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 3);
		Vector2 screenCursor = ImGui::GetCursorScreenPos();
		ImGui::GetWindowDrawList()->AddRectFilled(screenCursor, Vector2(screenCursor.x + regionAvailable.x, screenCursor.y + 30 + height), ImGui::GetColorU32(innerWindowColor));
		ImGui::SetCursorPos(Vector2(ImGui::GetCursorPosX() + 8, ImGui::GetCursorPosY() + 8));
		ImGui::Text(headerText.c_str());
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10 + height);
		ImGui::Separator();
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 4);

		ImGui::GetWindowDrawList()->AddRect(Vector2(cursorPos.x, cursorPos.y + 4), Vector2(cursorPos.x + regionAvailable.x, cursorPos.y + height + 37), ImGui::GetColorU32(componentBorderColor));
	}

	// *** Sets CursorScreenPos to the starting point! ***
	bool RenderInvisibleButton(std::string id, Vector2 startingPoint, Vector2 size, bool _allowOverlap, bool _showRect)
	{
		if (_showRect)
			FlatEngine::DebugRectangle(startingPoint, Vector2(startingPoint.x + size.x, startingPoint.y + size.y), FlatGui::whiteColor, 1.0f, ImGui::GetWindowDrawList());

		ImGuiButtonFlags flags = ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight;
		if (_allowOverlap)
		{
			ImGui::SetNextItemAllowOverlap();
			flags += ImGuiButtonFlags_AllowOverlap;
		}

		ImGui::SetCursorScreenPos(startingPoint);
		return ImGui::InvisibleButton(id.c_str(), size, flags);
	}

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
		ImGui::PushStyleColor(ImGuiCol_ChildBg, innerWindowColor);
		ImGui::PushStyleColor(ImGuiCol_Border, componentBorderColor);
		ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f);

		ImGui::BeginChild(componentID.c_str(), Vector2(0, 0), autoResizeChildFlags);

		ImGui::PopStyleVar();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();

		// Border around each component
		auto wPos = ImGui::GetWindowPos();
		auto wSize = ImGui::GetWindowSize();
		ImGui::GetWindowDrawList()->AddRect({ wPos.x + 2, wPos.y + 2 }, { wPos.x + wSize.x - 2, wPos.y + wSize.y - 2 }, ImColor(componentBorderColor.x, componentBorderColor.y, componentBorderColor.z, componentBorderColor.w));

		// Component Name
		ImGui::SetCursorPos(Vector2(ImGui::GetCursorPosX() + 5, ImGui::GetCursorPosY() + 5));
		ImGui::Text(componentType.c_str());

		ImGui::SameLine(ImGui::GetContentRegionAvail().x - (childPadding + 42), 5);

		// Pushes	
		ImGui::PushItemWidth(-1.0f);
		ImGui::PushStyleColor(ImGuiCol_Border, componentBorderColor);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, Vector2(1.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 1, 1, 0));

		std::string expandID = "##expandIcon-" + std::to_string(id);
		std::string trashcanID = "##trashIcon-" + std::to_string(id);
		std::string openFileID = "##openFileIcon-" + std::to_string(id);

		ImGui::SetCursorPos(Vector2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() - 3));
		// Trash Can Icon for removing Component from Focused Object
		if (RenderImageButton(trashcanID.c_str(), trashIcon.GetTexture()))
			queuedForDelete = component->GetID();

		ImGui::SameLine(0, 5);

		ImGui::SetCursorPos(Vector2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() - 3));
		// Draw Expand Icon for expanding/collapsing current component information
		if (_isCollapsed)
		{
			if (RenderImageButton(expandID.c_str(), expandIcon.GetTexture()))
				component->SetCollapsed(!_isCollapsed);
		}
		else
			if (RenderImageButton(expandID.c_str(), expandFlippedIcon.GetTexture()))
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
		ImGui::PushStyleColor(ImGuiCol_ChildBg, singleItemColor);
		// Make full width Push

		if (!component->IsCollapsed())
			ImGui::BeginChild(componentItemID.c_str(), Vector2(0, 0), autoResizeChildFlags);
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
		bool _checked = RenderCheckbox("Active", _isActive);
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
		if (PushTable("##TransformProperties" + std::to_string(id), 2))
		{
			RenderFloatDragTableRow("##xPosition" + std::to_string(id), "X Position", xPos, 0.1f, -FLT_MAX, -FLT_MAX);
			RenderFloatDragTableRow("##yPosition" + std::to_string(id), "Y Position", yPos, 0.1f, -FLT_MAX, -FLT_MAX);
			transform->SetPosition(Vector2(xPos, yPos));
			RenderFloatDragTableRow("##rotation" + std::to_string(id), "Rotation", rotation, 0.1f, -360, 360);
			transform->SetRotation(rotation);
			RenderFloatDragTableRow("##xScaleDrag" + std::to_string(id), "X Scale", scaleX, 0.1f, 0.001f, 1000);
			RenderFloatDragTableRow("##yScaleDrag" + std::to_string(id), "Y Scale", scaleY, 0.1f, 0.001f, 1000);
			transform->SetScale(Vector2(scaleX, scaleY));
			PopTable();
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
		if (RenderImageButton(openFileID.c_str(), openFileIcon.GetTexture()))
		{
			std::string assetPath = FlatEngine::OpenLoadFileExplorer();
			strcpy_s(newPath, assetPath.c_str());
			sprite->SetTexture(newPath);
		}
		ImGui::SameLine(0, 5);
								
		// Sprite Path Edit
		std::string spriteID = "##spritePath" + std::to_string(id);
		ImGui::PushStyleColor(ImGuiCol_FrameBg, inputColor);
		if (ImGui::InputText(spriteID.c_str(), newPath, IM_ARRAYSIZE(newPath), inputFlags))
			sprite->SetTexture(newPath);
		ImGui::PopStyleColor();

		// Render Table
		if (PushTable("##SpriteProperties" + std::to_string(id), 2))
		{
			if (RenderFloatDragTableRow("##xSpriteScaleDrag" + std::to_string(id), "X Scale", xScale, 0.1f, 0.001f, 1000))
				sprite->SetScale(Vector2(xScale, yScale));
			if (RenderFloatDragTableRow("##ySpriteScaleDrag" + std::to_string(id), "Y Scale", yScale, 0.1f, 0.001f, 1000))
				sprite->SetScale(Vector2(xScale, yScale));																
			if (RenderFloatDragTableRow("##xSpriteOffsetDrag" + std::to_string(id), "X Offset", xOffset, 0.1f, -FLT_MAX, -FLT_MAX))
				sprite->SetOffset(Vector2(xOffset, yOffset));
			if (RenderFloatDragTableRow("##ySpriteOffsetDrag" + std::to_string(id), "Y Offset", yOffset, 0.1f, -FLT_MAX, -FLT_MAX))
				sprite->SetOffset(Vector2(xOffset, yOffset));										
			if (RenderIntDragTableRow("##renderOrder" + std::to_string(id), "Render Order", renderOrder, 1, 0, (int)maxSpriteLayers))
				sprite->SetRenderOrder(renderOrder);
			RenderTextTableRow("##textureWidth" + std::to_string(id), "Texture width", textureWidthString);
			RenderTextTableRow("##textureHeight" + std::to_string(id), "Texture height", textureHeightString);
			PopTable();
		}

		// Pivot Point Buttons							
		ImGui::SetCursorScreenPos(Vector2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y + 5));
		Vector2 cellSize = Vector2(76, 78);
		Vector2 cursorScreen = Vector2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y);

		// TopLeft, Top, TopRight
		ImGui::GetWindowDrawList()->AddRectFilled(cursorScreen, Vector2(cursorScreen.x + cellSize.x, cursorScreen.y + cellSize.y), ImGui::GetColorU32(logBgColor));
		ImGui::SetCursorScreenPos(Vector2(cursorScreen.x + 5, cursorScreen.y + 5));
		if (RenderImageButton("##PivotTopLeftButton", upLeftIcon.GetTexture(), Vector2(16, 16), 1, imageButtonDarkColor))
			sprite->SetPivotPoint(Sprite::PivotPoint::TopLeft);
		ImGui::SameLine(0, 3);
		if (RenderImageButton("##PivotTopButton", upIcon.GetTexture(), Vector2(16, 16), 1, imageButtonDarkColor))
			sprite->SetPivotPoint(Sprite::PivotPoint::Top);
		ImGui::SameLine(0, 3);
		if (RenderImageButton("##PivotTopRightButton", upRightIcon.GetTexture(), Vector2(16, 16), 1, imageButtonDarkColor))
			sprite->SetPivotPoint(Sprite::PivotPoint::TopRight);
								
		ImGui::SameLine(0, 17);
		ImGui::Text("Pivot Point:");

		// Left, Center, Right
		ImGui::SetCursorScreenPos(Vector2(ImGui::GetCursorScreenPos().x + 5, ImGui::GetCursorScreenPos().y));
		if (RenderImageButton("##PivotLeftButton", leftIcon.GetTexture(), Vector2(16, 16), 1, imageButtonDarkColor))
			sprite->SetPivotPoint(Sprite::PivotPoint::Left);
		ImGui::SameLine(0, 3);
		if (RenderImageButton("##PivotCenterButton", centerIcon.GetTexture(), Vector2(16, 16), 1, imageButtonDarkColor))
			sprite->SetPivotPoint(Sprite::PivotPoint::Center);
		ImGui::SameLine(0, 3);
		if (RenderImageButton("##PivotRightButton", rightIcon.GetTexture(), Vector2(16, 16), 1, imageButtonDarkColor))
			sprite->SetPivotPoint(Sprite::PivotPoint::Right);
								
		ImGui::SameLine(0, 17);
		ImGui::Text(pivotString.c_str());

		// BottomLeft, Bottom, BottomRight
		ImGui::SetCursorScreenPos(Vector2(ImGui::GetCursorScreenPos().x + 5, ImGui::GetCursorScreenPos().y));
		if (RenderImageButton("##PivotBottomLeftButton", downLeftIcon.GetTexture(), Vector2(16, 16), 1, imageButtonDarkColor))
			sprite->SetPivotPoint(Sprite::PivotPoint::BottomLeft);
		ImGui::SameLine(0, 3);
		if (RenderImageButton("##PivotBottomButton", downIcon.GetTexture(), Vector2(16, 16), 1, imageButtonDarkColor))
			sprite->SetPivotPoint(Sprite::PivotPoint::Bottom);
		ImGui::SameLine(0, 3);
		if (RenderImageButton("##PivotBottomRightButton", downRightIcon.GetTexture(), Vector2(16, 16), 1, imageButtonDarkColor))
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
		if (PushTable("##CameraProperties" + std::to_string(id), 2))
		{
			if (RenderFloatDragTableRow("##cameraWidth" + std::to_string(id), "Camera width", width, 0.1f, 0, 1000))
				camera->SetDimensions(width, height);
			if (RenderFloatDragTableRow("##cameraHeight" + std::to_string(id), "Camera height", height, 0.1f, 0, 1000))
				camera->SetDimensions(width, height);
			if (RenderFloatDragTableRow("##cameraZoom" + std::to_string(id), "Camera zoom", zoom, 0.1f, 1, 100))
				camera->SetZoom(zoom);
			if (RenderCheckboxTableRow("##CameraShouldFollowTargetCheckbox", "Follow", _follow))
				camera->SetShouldFollow(_follow);
			if (RenderFloatDragTableRow("##cameraFollowSmoothing" + std::to_string(id), "Follow Smoothing", followSmoothing, 0.01f, 0, 1))
				camera->SetFollowSmoothing(followSmoothing);
			PopTable();
		}

		if (RenderInput("##FollowInput", "To Follow", following))
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
			if (RenderCheckbox("Is Primary Camera", _isPrimary))
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
			if (RenderCheckbox("Is Primary Camera", temp))
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
		if (RenderInput(inputId, "Name: ", path))
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
		if (PushTable("##ButtonProperties" + std::to_string(id), 2))
		{
			RenderFloatDragTableRow("##activeWidth" + std::to_string(id), "Active width", activeWidth, 0.1f, 0, 1000);
			RenderFloatDragTableRow("##activeHeight" + std::to_string(id), "Active height", activeHeight, 0.1f, 0, 1000);
			button->SetActiveDimensions(activeWidth, activeHeight);
			RenderFloatDragTableRow("##activeoffsetx" + std::to_string(id), "X Offset", activeOffset.x, 0.1f, -FLT_MAX, -FLT_MAX);
			RenderFloatDragTableRow("##activeoffsety" + std::to_string(id), "Y Offset", activeOffset.y, 0.1f, -FLT_MAX, -FLT_MAX);
			button->SetActiveOffset(activeOffset);
			PopTable();
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
		if (PushTable("##CanvasProperties" + std::to_string(id), 2))
		{
			RenderIntDragTableRow("##layerNumber" + std::to_string(id), "Canvas layer", layerNumber, 1, 20, 20);
			canvas->SetLayerNumber(layerNumber);
			RenderFloatDragTableRow("##Canvas width" + std::to_string(id), "Width", canvasWidth, 0.1f, 0.1f, -FLT_MAX);
			RenderFloatDragTableRow("##Canvas height" + std::to_string(id), "Height", canvasHeight, 0.1f, 0.1f, -FLT_MAX);
			canvas->SetDimensions(canvasWidth, canvasHeight);
			PopTable();
		}

		// _BlocksLayers Checkbox
		if (RenderCheckbox("Blocks Layers:", _blocksLayers))
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
		RenderInput("##animationPath" + std::to_string(id), "Path: ", path, _canOpenFiles);
		animation->SetAnimationPath(path);

		if (FlatEngine::GameLoopStarted() && !FlatEngine::GameLoopPaused())
		{
			if (RenderButton("Play Animation"))
				animation->Play(FlatEngine::GetEllapsedGameTimeInMs());

			if (animation->GetAnimationPath() != "")
				ImGui::SameLine(0, 5);
		}
		
		if (animation->GetAnimationPath() != "")
		{									
			if (RenderButton("Edit Animation"))
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
		bool _pathChanged = RenderInput(inputId, "Path: ", path, _canOpenFiles);
		audio->SetPath(path);						
		bool _soundTypeChanged = RenderCheckbox("Is Music", _isMusic);

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
		if (RenderInput("##TextContent" + std::to_string(id), "Text: ", textText))
		{
			text->SetText(textText);
			text->LoadText();
		}
		bool _canOpenFiles = true;
		std::string fontPath = text->GetFontPath();
		if (RenderInput("##FontPath" + std::to_string(id), "Font path: ", fontPath, _canOpenFiles))
			text->SetFontPath(fontPath);

		// Render Table
		if (PushTable("##TextProperties" + std::to_string(id), 2))
		{
			RenderTextTableRow("##textWidth" + std::to_string(id), "Text width", std::to_string(textureWidth));
			RenderTextTableRow("##textHeight" + std::to_string(id), "Text height", std::to_string(textureHeight));
			RenderTextTableRow("##xTextOffset" + std::to_string(id), "X offset", std::to_string(xOffset));
			RenderTextTableRow("##yTextOffset" + std::to_string(id), "Y offset", std::to_string(yOffset));
			RenderIntDragTableRow("##TextRenderOrder" + std::to_string(id), "Render Order", renderOrder, 1, 0, (int)maxSpriteLayers);
			text->SetRenderOrder(renderOrder);
			PopTable();
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
		if (PushTable("##CharacterControllerProps" + std::to_string(id), 2))
		{
			if (RenderFloatDragTableRow("##MaxAccelerationDrag" + std::to_string(id), "Max Acceleration", maxAcceleration, 0.01f, 0.0f, 20.0f))
				characterController->SetMaxAcceleration(maxAcceleration);
			if (RenderFloatDragTableRow("##MaxSpeedDrag" + std::to_string(id), "Max Speed", maxSpeed, 0.01f, 0.0f, 1000.0f))
				characterController->SetMaxSpeed(maxSpeed);
			if (RenderFloatDragTableRow("##AirControlDrag" + std::to_string(id), "Air Control", airControl, 0.01f, 0.0f, 1000.0f))
				characterController->SetAirControl(airControl);
			RenderTextTableRow("##IsMoving" + std::to_string(id), "Is Moving", isMoving);
			PopTable();
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
		if (PushTable("##BoxColliderProps" + std::to_string(id), 2))
		{
			if (RenderFloatDragTableRow("##BoxColliderWidth" + std::to_string(id), "Width", activeWidth, 0.01f, 0.0f, 20.0f))
				boxCollider->SetActiveDimensions(activeWidth, activeHeight);
			if (RenderFloatDragTableRow("##BoxColliderHeight" + std::to_string(id), "Height", activeHeight, 0.01f, 0.0f, 20.0f))
				boxCollider->SetActiveDimensions(activeWidth, activeHeight);
			if (RenderFloatDragTableRow("##ActiveOffsetBoxColliderX" + std::to_string(id), "X Offset", activeOffset.x, 0.01f, -FLT_MAX, -FLT_MAX))
				boxCollider->SetActiveOffset(activeOffset);
			if (RenderFloatDragTableRow("##ActiveOffsetBoxColliderY" + std::to_string(id), "Y Offset", activeOffset.y, 0.01f, -FLT_MAX, -FLT_MAX))
				boxCollider->SetActiveOffset(activeOffset);
			if (RenderIntDragTableRow("##BoxColliderActiveLayer" + std::to_string(id), "Active layer", activeLayer, 1, 0, 100))
				boxCollider->SetActiveLayer(activeLayer);
			RenderTextTableRow("##BoxColliderIsColliding" + std::to_string(id), "Is Colliding", isCollidingString);
			PopTable();
		}

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);							
		if (RenderCheckbox(" Is Continuous", _isContinuous))
			boxCollider->SetIsContinuous(_isContinuous);
		if (RenderCheckbox(" Is Static", _isStatic))
			boxCollider->SetIsStatic(_isStatic);
		if (RenderCheckbox(" Is Solid", _isSolid))
			boxCollider->SetIsSolid(_isSolid);
		if (RenderCheckbox(" Show Active Radius", _showActiveRadius))
			boxCollider->SetShowActiveRadius(_showActiveRadius);
		if (RenderCheckbox(" Is Composite", _isComposite))
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
		if (PushTable("##CircleColliderProps" + std::to_string(id), 2))
		{
			if (RenderFloatDragTableRow("##CircleColliderActiveRadius" + std::to_string(id), "Radius", activeRadius, 0.01f, 0.0f, 20.0f))
				circleCollider->SetActiveRadiusGrid(activeRadius);
			if (RenderFloatDragTableRow("##ActiveOffsetCircleColliderX" + std::to_string(id), "X Offset", activeOffset.x, 0.01f, -FLT_MAX, -FLT_MAX))
				circleCollider->SetActiveOffset(activeOffset);
			if (RenderFloatDragTableRow("##ActiveOffsetCircleColliderY" + std::to_string(id), "Y Offset", activeOffset.y, 0.01f, -FLT_MAX, -FLT_MAX))
				circleCollider->SetActiveOffset(activeOffset);
			if (RenderIntDragTableRow("##CircleColliderActiveLayer" + std::to_string(id), "Active layer", activeLayer, 1, 0, 100))
				circleCollider->SetActiveLayer(activeLayer);
			RenderTextTableRow("##CircleColliderIsColliding" + std::to_string(id), "Is Colliding", isCollidingString);
			PopTable();
		}

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
		if (RenderCheckbox(" Is Continuous", _isContinuous))
			circleCollider->SetIsContinuous(_isContinuous);
		if (RenderCheckbox(" Is Static", _isStatic))
			circleCollider->SetIsStatic(_isStatic);
		if (RenderCheckbox(" Is Solid", _isSolid))
			circleCollider->SetIsSolid(_isSolid);
		if (RenderCheckbox(" Is Composite", _isComposite))
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
		if (PushTable("##RigidBodyProps" + std::to_string(id), 2))
		{
			if (RenderFloatDragTableRow("##Mass" + std::to_string(id), "Mass", mass, 0.01f, 0.0f, -FLT_MAX))
				rigidBody->SetMass(mass);
			if (RenderFloatDragTableRow("##GravityScale" + std::to_string(id), "Gravity Scale", gravity, 0.01f, -FLT_MAX, -FLT_MAX))
				rigidBody->SetGravity(gravity);
			if (RenderFloatDragTableRow("##FallingGravityScale" + std::to_string(id), "Falling Gravity", fallingGravity, 0.01f, -FLT_MAX, -FLT_MAX))
				rigidBody->SetFallingGravity(fallingGravity);
			if (RenderFloatDragTableRow("##TerminalVelocity" + std::to_string(id), "Terminal Velocity", terminalVelocity, 0.01f, 0.001f, 1000))
				rigidBody->SetTerminalVelocity(terminalVelocity);
			if (RenderFloatDragTableRow("##WindResistance" + std::to_string(id), "Wind Resistance", windResistance, 0.01f, 0, 1))
				rigidBody->SetWindResistance(windResistance);
			if (RenderFloatDragTableRow("##Friction" + std::to_string(id), "Friction", friction, 0.01f, 0, 1))
				rigidBody->SetFriction(friction);
			if (RenderFloatDragTableRow("##EquilibriumForce" + std::to_string(id), "Equilibrium Force", equilibriumForce, 0.01f, 0, 1000))
				rigidBody->SetEquilibriumForce(equilibriumForce);
			RenderTextTableRow("##VelocityX" + std::to_string(id), "X Velocity", std::to_string(velocity.x));
			RenderTextTableRow("##VelocityY" + std::to_string(id), "Y Velocity", std::to_string(velocity.y));
			RenderTextTableRow("##AccelerationX" + std::to_string(id), "X Acceleration", std::to_string(acceleration.x));
			RenderTextTableRow("##AccelerationY" + std::to_string(id), "Y Acceleration", std::to_string(acceleration.y));
			RenderTextTableRow("##PendingForcesX" + std::to_string(id), "X Pending Forces", std::to_string(pendingForces.x));
			RenderTextTableRow("##PendingForcesY" + std::to_string(id), "Y Pending Forces", std::to_string(pendingForces.y));									
			RenderTextTableRow("##RigidBodyGrounded" + std::to_string(id), "Is Grounded", isGroundedString);
			PopTable();
		}								

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
		// Kinematic Checkbox
		RenderCheckbox(" Is Kinematic", _isKinematic);
		rigidBody->SetIsKinematic(_isKinematic);

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
		// Static Checkbox
		RenderCheckbox(" Is Static", _isStatic);
		rigidBody->SetIsStatic(_isStatic);
	}
}