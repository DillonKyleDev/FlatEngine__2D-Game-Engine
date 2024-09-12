#include "FlatEngine.h"
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

#include "imgui.h"
#include "imgui_internal.h"


namespace FlatEngine 
{
	// Flags
	ImGuiChildFlags F_autoResizeChildFlags = ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_AlwaysUseWindowPadding;
	ImGuiChildFlags F_resizeChildFlags = ImGuiChildFlags_ResizeX | ImGuiChildFlags_AlwaysUseWindowPadding;
	ImGuiChildFlags F_childFlags = ImGuiChildFlags_AlwaysUseWindowPadding;
	ImGuiChildFlags F_headerFlags = ImGuiChildFlags_AlwaysUseWindowPadding | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysAutoResize;
	//ImGuiTableFlags_Resizable
	ImGuiTableFlags F_tableFlags = ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchSame;
	ImGuiTableFlags F_resizeableTableFlags = ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable;
	ImGuiInputTextFlags F_inputFlags = ImGuiInputTextFlags_::ImGuiInputTextFlags_AutoSelectAll;

	float F_childPadding = 8;


	// ImGui Wrappers
	void MoveScreenCursor(float x, float y)
	{
		ImGui::SetCursorScreenPos(Vector2(ImGui::GetCursorScreenPos().x + x, ImGui::GetCursorScreenPos().y + y));
	}

	void RenderSeparator(float topPadding, float bottomPadding)
	{
		MoveScreenCursor(0, topPadding);
		ImGui::Separator();
		MoveScreenCursor(0, bottomPadding);
	}

	void RenderSubTitle(std::string title)
	{
		Vector2 titleStartPos = ImGui::GetCursorScreenPos();
		Vector2 titleEndPos = Vector2(titleStartPos.x + ImGui::GetContentRegionAvail().x, titleStartPos.y + 24);
		ImGui::GetWindowDrawList()->AddRectFilled(titleStartPos, titleEndPos, GetColor32("componentSubTitleBg"));
		MoveScreenCursor(5, 5);
		ImGui::Text(title.c_str());
		MoveScreenCursor(0, 12);
	}

	void BeginWindow(std::string name, Vector4 bgColor)
	{
		PushWindowStyles(bgColor);
		ImGui::Begin(name.c_str());
		PopWindowStyles();

		BeginWindowChild(name);
	}

	void BeginWindow(std::string name, bool& b_isOpen, Vector4 bgColor)
	{
		PushWindowStyles(bgColor);
		ImGui::Begin(name.c_str(), &b_isOpen);
		PopWindowStyles();

		BeginWindowChild(name);
	}

	void BeginWindow(std::string name, bool& b_isOpen, ImGuiWindowFlags flags, Vector4 bgColor)
	{
		PushWindowStyles(bgColor);
		ImGui::Begin(name.c_str(), &b_isOpen, flags);
		PopWindowStyles();

		BeginWindowChild(name);
	}

	void EndWindow()
	{
		EndWindowChild();
		ImGui::End();
	}

	void BeginWindowChild(std::string title, Vector4 bgColor, ImGuiWindowFlags flags)
	{
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ChildBg, bgColor);
		ImGui::BeginChild(title.c_str(), Vector2(0, 0), F_childFlags, flags);
		ImGui::PopStyleColor();		
	}

	void BeginResizeWindowChild(std::string title, Vector4 bgColor, ImGuiWindowFlags flags)
	{
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ChildBg, bgColor);
		ImGui::BeginChild(title.c_str(), Vector2(0, 0), F_resizeChildFlags, flags);
		ImGui::PopStyleColor();
	}

	void EndWindowChild()
	{
		ImGui::EndChild();
	}

	void PushWindowStyles(Vector4 bgColor)
	{
		ImGui::PushStyleColor(ImGuiCol_WindowBg, bgColor);
		ImGui::PushStyleColor(ImGuiCol_FrameBg, GetColor("frameBg"));
		ImGui::PushStyleColor(ImGuiCol_FrameBgActive, GetColor("frameBgActive"));
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, GetColor("frameBgHovered"));
		ImGui::PushStyleColor(ImGuiCol_Tab, GetColor("tab"));
		ImGui::PushStyleColor(ImGuiCol_TabActive, GetColor("tabActive"));
		ImGui::PushStyleColor(ImGuiCol_TabHovered, GetColor("tabHovered"));
		ImGui::PushStyleColor(ImGuiCol_TabUnfocused, GetColor("tabUnfocused"));
		ImGui::PushStyleColor(ImGuiCol_TabUnfocusedActive, GetColor("tabUnfocusedActive"));
		ImGui::PushStyleColor(ImGuiCol_TitleBg, GetColor("titleBg"));
		ImGui::PushStyleColor(ImGuiCol_TitleBgActive, GetColor("titleBgActive"));
		ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed, GetColor("titleBgCollapsed"));
		ImGui::PushStyleColor(ImGuiCol_TextSelectedBg, GetColor("textSelectedBg"));
		ImGui::PushStyleColor(ImGuiCol_ResizeGrip, GetColor("resizeGrip"));
		ImGui::PushStyleColor(ImGuiCol_ResizeGripActive, GetColor("resizeGripActive"));
		ImGui::PushStyleColor(ImGuiCol_ResizeGripHovered, GetColor("resizeGripHovered"));
		ImGui::PushStyleColor(ImGuiCol_PopupBg, GetColor("popupBg"));
		ImGui::PushStyleColor(ImGuiCol_NavWindowingHighlight, GetColor("navWindowHighlight"));
		ImGui::PushStyleColor(ImGuiCol_NavHighlight, GetColor("navHighlight"));
		ImGui::PushStyleColor(ImGuiCol_NavWindowingDimBg, GetColor("navWindowDimBg"));
		ImGui::PushStyleColor(ImGuiCol_ModalWindowDimBg, GetColor("modalWindowDimBg"));
		ImGui::PushStyleColor(ImGuiCol_DockingPreview, GetColor("dockingPreview"));
		ImGui::PushStyleColor(ImGuiCol_DockingEmptyBg, GetColor("dockingPreviewEmpty"));
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
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, Vector2(8, 8));
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, Vector2(5, 5));
		ImGui::PushStyleColor(ImGuiCol_Button, GetColor("comboArrow"));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, GetColor("comboArrowHovered"));
		ImGui::PushStyleColor(ImGuiCol_FrameBg, GetColor("comboBg"));
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, GetColor("comboHovered"));
		// For Selectables
		ImGui::PushStyleColor(ImGuiCol_Header, GetColor("comboSelectable"));
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, GetColor("comboSelected"));
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, GetColor("comboHighlighted"));
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
		ImGui::PopStyleVar();
		ImGui::PopStyleVar();
	}

	void PushMenuStyles()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, Vector2(8, 8));		
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, Vector2(5, 5));
		ImGui::PushStyleColor(ImGuiCol_Header, GetColor("treeSelectableSelected"));
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, GetColor("treeSelectableHovered"));
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, GetColor("treeSelectableActive"));
	}

	void PopMenuStyles()
	{
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleVar();
		ImGui::PopStyleVar();
	}

	void PushTableStyles()
	{
		ImGui::PushStyleColor(ImGuiCol_TableRowBg, GetColor("tableCellDark"));
		ImGui::PushStyleColor(ImGuiCol_TableRowBgAlt, GetColor("tableCellLight"));
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


	bool RenderInputTableRow(std::string id, std::string fieldName, std::string &value, bool b_canOpenFiles)
	{
		bool b_edited = false;
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
		ImGui::Text(fieldName.c_str());
		ImGui::TableSetColumnIndex(1);
		b_edited = RenderInput(id, "", value, b_canOpenFiles);
		ImGui::PushID(id.c_str());
		ImGui::PopID();

		return b_edited;
	}

	void RenderTextTableRow(std::string id, std::string fieldName, std::string value, std::string value2)
	{
		// Push uneditableTableTextColor text color
		ImGui::PushStyleColor(ImGuiCol_Text, GetColor("noEditTableText"));

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		// Set table cell bg color
		ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(GetColor("noEditTableRowFieldBg")));
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
		ImGui::Text(fieldName.c_str());
		ImGui::TableSetColumnIndex(1);
		// Set table cell bg color
		ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(GetColor("noEditTableRowValueBg")));
		ImGui::Text(value.c_str());
		if (value2 != "")
		{
			ImGui::TableSetColumnIndex(2);
			// Set table cell bg color
			ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(GetColor("noEditTableRowValueBg")));
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

	bool RenderInput(std::string id, std::string label, std::string& value, bool b_canOpenFiles, float inputWidth, ImGuiInputTextFlags flags)
	{
		bool b_editedButton = false;
		bool b_editedInput = false;
		bool b_dragTargeted = false;
		char newPath[1024];
		strcpy_s(newPath, value.c_str());
		std::string pathString = label;
		
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, Vector2(5,3));

		if (pathString != "")
		{
			pathString += ":";
			ImGui::AlignTextToFramePadding();
			ImGui::TextUnformatted(pathString.c_str());
			ImGui::SameLine(0, 5);			
		}

		if (b_canOpenFiles && inputWidth == -1)
		{
			inputWidth = ImGui::GetContentRegionAvail().x - 31;
		}
		else if (b_canOpenFiles)
		{
			inputWidth -= 31;
		}

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, Vector2(5, 4));
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 1);
		Vector2 inputStart = ImGui::GetCursorScreenPos();
		ImGui::PushStyleColor(ImGuiCol_FrameBg, GetColor("input"));
		ImGui::SetNextItemWidth(inputWidth);
		b_editedInput = ImGui::InputText(id.c_str(), newPath, IM_ARRAYSIZE(newPath), flags);
		ImGui::PopStyleColor();
		Vector2 inputSize = Vector2(inputWidth, ImGui::GetCursorScreenPos().y - inputStart.y);
		ImGui::PopStyleVar();
		ImGui::PopStyleVar();

		if (b_canOpenFiles)
		{
			ImGui::SameLine(0, -1);

			std::string buttonId = id + "openFileButton";
			if (RenderImageButton(buttonId.c_str(), GetTexture("openFile"), Vector2(16), 1, GetColor("openFileButtonBg"), GetColor("imageButtonTint"), GetColor("openFileButtonHovered"), GetColor("imageButtonActive"), Vector2(0), Vector2(1), Vector2(3)))
			{
				std::string assetPath = FlatEngine::OpenLoadFileExplorer();
				strcpy_s(newPath, assetPath.c_str());
				b_editedButton = true;
			}
		}

		ImGui::PopStyleVar();

		if (newPath != nullptr)
			value = newPath;
		return b_editedButton || b_editedInput || b_dragTargeted;
	}

	bool DropInput(std::string id, std::string label, std::string displayValue, std::string dropTargetID, int &droppedValue, std::string tooltip, float inputWidth)
	{		
		bool b_dragTargeted = false;

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, Vector2(5, 3));

		if (label != "")
		{
			label += ":";
			ImGui::AlignTextToFramePadding();
			ImGui::Text(label.c_str());
			ImGui::SameLine(0, 5);			
		}

		if (inputWidth == -1)
		{
			inputWidth = ImGui::GetContentRegionAvail().x - 1;
		}

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, Vector2(5, 4));
		Vector2 inputStart = ImGui::GetCursorScreenPos();
		Vector2 inputSize = Vector2(inputWidth, ImGui::GetFontSize() * 1.65f);
		ImGui::GetWindowDrawList()->AddRectFilled(inputStart, Vector2(inputStart.x + inputSize.x, inputStart.y + inputSize.y), GetColor32("input"), 1);
		ImGui::SetCursorScreenPos(Vector2(inputStart.x + 3, inputStart.y + 1));
		ImGui::Text(displayValue.c_str());
		RenderInvisibleButton("##DropInputdropTarget", inputStart, inputSize, true, false, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight | 4096);
		ImGui::PopStyleVar();
		if (tooltip != "" && ImGui::IsItemHovered())
		{
			RenderTextToolTip(tooltip);
		}

		// Drop Target
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(dropTargetID.c_str()))
			{
				IM_ASSERT(payload->DataSize == sizeof(int));
				droppedValue = *(const int*)payload->Data;
				b_dragTargeted = true;
			}
			ImGui::EndDragDropTarget();
		}

		// Right click menu
		if (ImGui::BeginPopupContextItem("##DropInputdropTarget"))
		{
			PushMenuStyles();
			if (ImGui::MenuItem("Remove reference"))
			{
				droppedValue = -1;
				b_dragTargeted = true;
				ImGui::CloseCurrentPopup();
			}
			PopMenuStyles();

			ImGui::EndPopup();
		}

		ImGui::PopStyleVar();

		return b_dragTargeted;
	}

	bool DropInputCanOpenFiles(std::string id, std::string label, std::string displayValue, std::string dropTargetID, int& droppedValue, std::string& openedFileValue, std::string tooltip, float inputWidth)
	{
		bool b_editedButton = false;
		bool b_dragTargeted = false;
		char newPath[1024];
		strcpy_s(newPath, openedFileValue.c_str());

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, Vector2(5, 3));

		if (label != "")
		{
			label += ":";
			ImGui::AlignTextToFramePadding();
			ImGui::Text(label.c_str());
			ImGui::SameLine(0, 5);			
		}

		if (inputWidth == -1)
		{
			inputWidth = ImGui::GetContentRegionAvail().x - 31;
		}
		else
		{
			inputWidth -= 31;
		}

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, Vector2(5, 4));
		Vector2 inputStart = ImGui::GetCursorScreenPos();
		Vector2 inputSize = Vector2(inputWidth, ImGui::GetFontSize() * 1.65f);
		ImGui::GetWindowDrawList()->AddRectFilled(inputStart, Vector2(inputStart.x + inputSize.x, inputStart.y + inputSize.y), GetColor32("input"), 1);
		ImGui::SetCursorScreenPos(Vector2(inputStart.x + 3, inputStart.y + 1));
		ImGui::Text(displayValue.c_str());
		RenderInvisibleButton("##DropInputOpenFilesdropTarget", inputStart, inputSize, true, false, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight | 4096);
		ImGui::PopStyleVar();
		if (tooltip != "" && ImGui::IsItemHovered())
		{
			RenderTextToolTip(tooltip);
		}

		// Drop Target
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(dropTargetID.c_str()))
			{
				IM_ASSERT(payload->DataSize == sizeof(int));
				droppedValue = *(const int*)payload->Data;
				b_dragTargeted = true;
				
			}
			ImGui::EndDragDropTarget();
		}

		// Right click menu
		if (ImGui::BeginPopupContextItem("##DropInputOpenFilesdropTarget"))
		{
			PushMenuStyles();
			if (ImGui::MenuItem("Remove reference"))
			{
				droppedValue = -1;
				b_dragTargeted = true;
				ImGui::CloseCurrentPopup();
			}
			PopMenuStyles();

			ImGui::EndPopup();
		}

		ImGui::SameLine();

		std::string buttonId = id + "openFileButton";		
		if (RenderImageButton(buttonId.c_str(), GetTexture("openFile"), Vector2(16), 1, GetColor("openFileButtonBg"), GetColor("imageButtonTint"), GetColor("openFileButtonHovered"), GetColor("imageButtonActive"), Vector2(0), Vector2(1), Vector2(3)))
		{
			std::string assetPath = FlatEngine::OpenLoadFileExplorer();
			strcpy_s(newPath, assetPath.c_str());
			b_editedButton = true;
		}

		if (newPath != nullptr)
		{
			openedFileValue = newPath;
		}

		ImGui::PopStyleVar();

		return b_editedButton || b_dragTargeted;
	}

	bool RenderSelectable(std::string id, std::vector<std::string> options, int& current_option)
	{
		bool b_selectionMade = false;
		bool b_currentSelectionEmpty = false;
		std::string empty = " - empty -";
		std::string currentlySelected = empty;
		if (options.size() == 0)
			options.push_back(empty);

		if (options.size() <= current_option)
			current_option = 0;

		PushComboStyles();
		PushMenuStyles();
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

		currentlySelected = " " + options[current_option];

		if (options[current_option] == "")
		{
			currentlySelected = empty;
			ImGui::PushStyleColor(ImGuiCol_Text, GetColor("logText"));
			b_currentSelectionEmpty = true;
		}

		if (ImGui::BeginCombo(id.c_str(), currentlySelected.c_str()))
		{
			for (int n = 0; n < options.size(); n++)
			{
				bool is_selected = (options[current_option] == options[n]);
				
				std::string selectableLabel = " " + options[n];
				if (options[n] == "")
				{
					selectableLabel = empty;
				}
					
				if (options[n] == "")
				{
					ImGui::PushStyleColor(ImGuiCol_Text, GetColor("logText"));
				}
				else
				{
					ImGui::PushStyleColor(ImGuiCol_Text, GetColor("white"));
				}

				if (ImGui::Selectable(selectableLabel.c_str(), is_selected))
				{
					current_option = n;
					b_selectionMade = true;
				}
				if (is_selected)
				{
					ImGui::SetItemDefaultFocus();
				}

				ImGui::PopStyleColor();
			}
			ImGui::EndCombo();
		}

		if (b_currentSelectionEmpty)
		{
			ImGui::PopStyleColor();
		}
		PopMenuStyles();
		PopComboStyles();

		return b_selectionMade;
	}

	void PushTreeList(std::string id)
	{
		ImGui::PushStyleColor(ImGuiCol_FrameBg, GetColor("innerWindow"));
		ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, Vector2(0, 0));
		PushMenuStyles();
		ImGui::BeginTable(id.c_str(), 1, F_tableFlags);
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
		{
			node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_Selected;
		}
		else
		{
			node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;
		}

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
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, Vector2(5, 3));

		if (size.x != 0 || size.y != 0)
		{
			_isClicked = ImGui::Button(text.c_str(), size);
		}
		else
		{
			_isClicked = ImGui::Button(text.c_str());
		}

		if (ImGui::IsItemHovered())
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_Hand);
		}
	
		ImGui::PopStyleVar();
		ImGui::PopStyleVar();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();

		return _isClicked;
	}

	bool RenderImageButton(std::string id, SDL_Texture* texture, Vector2 size, float rounding, Vector4 bgColor, Vector4 tint, Vector4 hoverColor, Vector4 activeColor, Vector2 uvStart, Vector2 uvEnd, Vector2 padding)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, bgColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hoverColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, activeColor);		
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, rounding);
		if (padding.x != -1 && padding.y != -1)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, padding);
		}

		bool b_isClicked = ImGui::ImageButton(id.c_str(), texture, size, uvStart, uvEnd, GetColor("transparent"), tint);

		if (ImGui::IsItemHovered())
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_Hand);
		}
		
		if (padding.x != -1 && padding.y != -1)
		{
			ImGui::PopStyleVar();
		}
		ImGui::PopStyleVar();		
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();

		return b_isClicked;
	}

	bool RenderDragFloat(std::string text, float width, float& value, float increment, float min, float max, ImGuiSliderFlags flags)
	{
		ImGui::PushStyleColor(ImGuiCol_FrameBg, GetColor("drag"));
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, GetColor("dragHovered"));
		ImGui::PushStyleColor(ImGuiCol_FrameBgActive, GetColor("dragActive"));

		if (width != 0)
		{
			ImGui::SetNextItemWidth(width);
		}
		else
		{
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		}

		bool b_sliderChanged = ImGui::DragFloat(text.c_str(), &value, increment, min, max, "%.3f", flags);

		if (ImGui::IsItemHovered())
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_ResizeEW);
		}

		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();

		return b_sliderChanged;
	}

	bool RenderDragInt(std::string text, float width, int& value, float speed, int min, int max, ImGuiSliderFlags flags)
	{
		ImGui::PushStyleColor(ImGuiCol_FrameBg, GetColor("drag"));
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, GetColor("dragHovered"));
		ImGui::PushStyleColor(ImGuiCol_FrameBgActive, GetColor("dragActive"));

		if (width != 0)
		{
			ImGui::SetNextItemWidth(width);
		}
		else
		{
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		}
		
		bool b_sliderChanged = ImGui::DragInt(text.c_str(), &value, speed, min, max, "%d", flags);
		
		if (ImGui::IsItemHovered())
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_ResizeEW);
		}

		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();

		return b_sliderChanged;
	}

	void PushSliderStyles()
	{
		ImGui::PushStyleColor(ImGuiCol_FrameBg, GetColor("sliderBg"));
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, GetColor("sliderHovered"));
		ImGui::PushStyleColor(ImGuiCol_FrameBgActive, GetColor("sliderActive"));
		ImGui::PushStyleColor(ImGuiCol_SliderGrab, GetColor("sliderGrab"));
		ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, GetColor("sliderGrabActive"));
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, Vector2(5));
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 1);
		ImGui::PushStyleVar(ImGuiStyleVar_GrabRounding, 1);
		ImGui::PushStyleVar(ImGuiStyleVar_GrabMinSize, 26);
	}

	void PopSliderStyles()
	{
		ImGui::PopStyleVar();
		ImGui::PopStyleVar();
		ImGui::PopStyleVar();
		ImGui::PopStyleVar();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
	}

	// if label starts with ## it will not be included visually
	bool RenderSliderFloat(std::string label, float& value, float increment, float min, float max, float width, int digitsAfterDecimal)
	{
		std::string format = "%." + std::to_string(digitsAfterDecimal) + "f";
		if (width == -1)
		{
			width = ImGui::GetContentRegionAvail().x;
		}

		ImGui::SetNextItemWidth(width);
		PushSliderStyles();
		bool b_sliderChanged = ImGui::SliderFloat(label.c_str(), &value, min, max, format.c_str(), 0);
		PopSliderStyles();
		if (ImGui::IsItemHovered())
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
		}

		return b_sliderChanged;
	}

	// if label starts with ## it will not be included visually
	bool RenderSliderInt(std::string label, int& value, int increment, int min, int max, float width)
	{
		if (width == -1)
		{
			width = ImGui::GetContentRegionAvail().x;
		}

		ImGui::SetNextItemWidth(width);
		PushSliderStyles();
		bool b_sliderChanged = ImGui::SliderInt(label.c_str(), &value, min, max, "%d", 0);
		PopSliderStyles();
		if (ImGui::IsItemHovered())
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
		}

		return b_sliderChanged;
	}

	bool RenderCheckbox(std::string text, bool& _toCheck)
	{
		ImGui::PushStyleColor(ImGuiCol_FrameBg, GetColor("checkboxBg"));
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, GetColor("checkboxHovered"));
		ImGui::PushStyleColor(ImGuiCol_FrameBgActive, GetColor("checkboxActive"));
		ImGui::PushStyleColor(ImGuiCol_CheckMark, GetColor("checkboxCheck"));

		bool b_checked = ImGui::Checkbox(text.c_str(), &_toCheck);

		if (ImGui::IsItemHovered())
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_Hand);
		}

		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();

		return b_checked;
	}

	void RenderSectionHeader(std::string headerText, float height)
	{
		auto cursorPos = ImGui::GetCursorScreenPos();
		auto regionAvailable = ImGui::GetContentRegionAvail();

		Vector2 screenCursor = ImGui::GetCursorScreenPos();
		ImGui::GetWindowDrawList()->AddRectFilled(screenCursor, Vector2(screenCursor.x + regionAvailable.x, screenCursor.y + 30 + height), ImGui::GetColorU32(GetColor("innerWindow")), 1);
		MoveScreenCursor(8, 8);
		ImGui::Text(headerText.c_str());		
		MoveScreenCursor(0, height + 10);
		ImGui::GetWindowDrawList()->AddRect(Vector2(cursorPos.x, cursorPos.y), Vector2(cursorPos.x + regionAvailable.x, cursorPos.y + height + 30), ImGui::GetColorU32(GetColor("componentBorder")), 1);
	}

	// *** SECOND VECTOR IS THE SIZE, **NOT*** THE END POSITION. *** Sets CursorScreenPos to the starting point! *** 
	bool RenderInvisibleButton(std::string id, Vector2 startingPoint, Vector2 size, bool b_allowOverlap, bool b_showRect, ImGuiButtonFlags flags)
	{
		if (b_showRect)
		{
			FlatEngine::DebugRectangle(startingPoint, Vector2(startingPoint.x + size.x, startingPoint.y + size.y), GetColor("white"), 1.0f, ImGui::GetWindowDrawList());
		}
;
		if (b_allowOverlap)
		{
			ImGui::SetNextItemAllowOverlap();
			flags += ImGuiButtonFlags_AllowOverlap; // 4096
		}

		ImGui::SetCursorScreenPos(startingPoint);
		return ImGui::InvisibleButton(id.c_str(), size, flags);
	}

	void RenderTextToolTip(std::string text)
	{
		ImGui::BeginTooltip();
		ImGui::Text(text.c_str());
		ImGui::EndTooltip();
	}

	void BeginToolTip(std::string title)
	{
		ImGui::BeginTooltip();
		if (title != "")
		{
			ImGui::Text(title.c_str());
			RenderSeparator(5, 5);
		}
	}

	void EndToolTip()
	{		
		ImGui::EndTooltip();
	}

	void RenderToolTipText(std::string label, std::string text)
	{
		std::string newLabel = label + "  |  ";
		ImGui::Text(newLabel.c_str());
		ImGui::SameLine();
		ImGui::Text(text.c_str());
		RenderSeparator(5, 5);
	}

	void RenderToolTipFloat(std::string label, float data)
	{
		std::string newLabel = label + "  |  ";
		ImGui::Text(newLabel.c_str());
		ImGui::SameLine();
		ImGui::Text(std::to_string(data).c_str());
		RenderSeparator(5, 5);
	}

	void RenderToolTipLong(std::string label, long data)
	{
		std::string newLabel = label + "  |  ";
		ImGui::Text(newLabel.c_str());
		ImGui::SameLine();
		ImGui::Text(std::to_string(data).c_str());
		RenderSeparator(5, 5);
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

	// Returns true if a valid input was retrieved. ***Remember to use static std::strings for inputValue***
	bool RenderInputModal(std::string label, std::string description, std::string &inputValue, bool &b_openModal)
	{		
		if (b_openModal)
		{
			ImGui::OpenPopup(label.c_str());
			b_openModal = false;
		}

		bool b_validInput = false;

		// Always center this window when appearing
		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));		
		ImGuiStyle& style = ImGui::GetStyle();
		Vector4 currentModalWindowDimBgColor = style.Colors[ImGuiCol_ModalWindowDimBg];
		//style.Colors[ImGuiCol_ModalWindowDimBg] = GetColor("white");
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, Vector2(F_childPadding));
		ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, Vector2(F_childPadding));
		if (ImGui::BeginPopupModal(label.c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			RenderSectionHeader(description + ":");
			
			ImGui::Separator();

			float inputWidth = ImGui::CalcTextSize(description.c_str()).x;
			if (inputWidth < 250)
			{
				inputWidth = 250;
			}

			std::string inputLabel = "##" + label;
			RenderInput(inputLabel.c_str(), "", inputValue, false, inputWidth);

			RenderSeparator(0, 10);

			if (ImGui::IsKeyPressed(ImGuiKey_Enter))
			{
				b_validInput = inputValue != "";
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::IsKeyPressed(ImGuiKey_Escape))
			{
				b_validInput = false;
				ImGui::CloseCurrentPopup();
			}

			MoveScreenCursor(ImGui::GetContentRegionAvail().x - 160, 0);

			if (RenderButton("OK", Vector2(70, 30)))
			{ 
				b_validInput = inputValue != "";
				ImGui::CloseCurrentPopup(); 
			}
			ImGui::SetItemDefaultFocus();
			ImGui::SameLine(0, 10);
			if (RenderButton("Cancel", Vector2(70, 30)))
			{
				ImGui::CloseCurrentPopup();
			}
			MoveScreenCursor(0, 8);
			ImGui::EndPopup();
		}		
		//style.Colors[ImGuiCol_ModalWindowDimBg] = currentModalWindowDimBgColor;
		ImGui::PopStyleVar();
		ImGui::PopStyleVar();
		ImGui::PopStyleVar();

		return b_validInput;
	}

	bool RenderConfirmModal(std::string label, std::string description, bool& b_openModal)
	{
		bool b_confirm = false;

		if (b_openModal)
		{
			ImGui::OpenPopup(label.c_str());
			b_openModal = false;
		}

		// Always center this window when appearing
		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		if (ImGui::BeginPopupModal(label.c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text(description.c_str());

			ImGui::Separator();

			float inputWidth = ImGui::CalcTextSize(description.c_str()).x;
			if (inputWidth < 250)
				inputWidth = 250;

			if (ImGui::IsKeyPressed(ImGuiKey_Enter))
			{
				b_confirm = true;
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::IsKeyPressed(ImGuiKey_Escape))
			{
				b_confirm = false;
				ImGui::CloseCurrentPopup();
			}

			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - 145);
			if (ImGui::Button("Confirm", ImVec2(70, 0)))
			{
				b_confirm = true;
				ImGui::CloseCurrentPopup();
			}
			ImGui::SetItemDefaultFocus();
			ImGui::SameLine(0, 5);
			if (ImGui::Button("Cancel", ImVec2(70, 0)))
			{
				b_confirm = false;
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		return b_confirm;
	}
}