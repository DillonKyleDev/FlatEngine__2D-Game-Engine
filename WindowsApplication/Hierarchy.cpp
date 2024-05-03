#include "FlatEngine.h"

namespace FlatEngine { namespace FlatGui {

	void RenderHierarchy()
	{
		ImGuiWindowFlags window_flags = ImGuiChildFlags_AutoResizeX;
		ImGuiStyle& style = ImGui::GetStyle();

		PushWindowStyles();
		ImGui::Begin("Scene Hierarchy");
		PopWindowStyles();

		ImGui::PushStyleColor(ImGuiCol_ChildBg, outerWindowColor);
		float availableHorizontalSpace = ImGui::GetContentRegionAvail().x;
		float availableVerticalSpace = ImGui::GetContentRegionAvail().y;

		ImGuiChildFlags padding_child_flags = ImGuiChildFlags_::ImGuiChildFlags_AlwaysUseWindowPadding;
		ImGui::BeginChild("Hierarchy Background", ImVec2(0, 0), padding_child_flags);

		// Variables for viewport dimensions
		ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();      // ImDrawList API uses screen coordinates!
		ImVec2 canvas_sz = ImGui::GetContentRegionAvail();   // Resize canvas to what's available
		if (canvas_sz.x < 50.0f) canvas_sz.x = 50.0f;
		if (canvas_sz.y < 50.0f) canvas_sz.y = 50.0f;
		ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);

		ImGuiInputTextFlags flags = ImGuiInputTextFlags_::ImGuiInputTextFlags_AutoSelectAll;

		if (RenderButton("Add New GameObject"))
			CreateGameObject(-1);

		// Scene Objects in Hierarchy
		{
			static ImGuiTableFlags tableFlags = ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchSame;
			float objectColumnWidth = ImGui::GetContentRegionAvail().x - 10;
			float visibleIconColumnWidth = 24;
			static float currentIndent = 10;

			ImGui::PushStyleColor(ImGuiCol_FrameBg, innerWindowColor);
			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0, -5));
			ImGui::BeginTable("##HierarchyTable", 2, tableFlags);
			ImGui::PopStyleVar();
			ImGui::PopStyleColor();

			ImGui::TableSetupColumn("##VISIBLE", 0, visibleIconColumnWidth);
			ImGui::TableSetupColumn("##OBJECT", 0, objectColumnWidth);

			static int node_clicked = -1;
			long queuedForDelete = -1;

			std::vector<std::shared_ptr<GameObject>> sceneObjects = FlatEngine::GetSceneObjects();

			for (std::vector<std::shared_ptr<GameObject>>::iterator object = sceneObjects.begin(); object != sceneObjects.end(); object++)
			{
				// If this object does not have a parent we render it and all of its children.
				if ((*object)->GetParentID() == -1)
				{
					// Get Object name
					std::shared_ptr<GameObject> currentObject = (*object);
					std::string name = currentObject->GetName();
					const char* charName = name.c_str();
					float indent = 0;

					// If the object has children call the recursive AddObjectWithChild();
					if (currentObject->HasChildren())
						AddObjectWithChild(currentObject, charName, node_clicked, queuedForDelete, indent);
					else
						AddObjectWithoutChild(currentObject, charName, node_clicked, queuedForDelete, -15);
				}
			}
			if (node_clicked != -1)
			{
				// Update selection state
				// (process outside of tree loop to avoid visual inconsistencies during the clicking frame)
				if (ImGui::GetIO().KeyCtrl)
				{

				}
				else
				{

				}
			}

			// Add empty table rows so the table goes all the way to the bottom of the screen
			float availableVerticalSpace = ImGui::GetContentRegionAvail().y;
			if (availableVerticalSpace > 22)
			{
				for (int i = 0; i < availableVerticalSpace / 22 - 1; i++)
				{
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(1);
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
					ImGui::Text("");
				}
			}

			ImGui::EndTable();

			// Delete queued GameObject
			if (queuedForDelete != -1)
			{
				long focusedObjectID = FlatEngine::GetFocusedGameObjectID();
				long saveFocusedObject = focusedObjectID;

				// Unfocus focused GameObject first
				SetFocusedGameObjectID(-1);
				DeleteGameObject(queuedForDelete);

				// If previous focused object still exists, set it to focused object again
				if (GetObjectById(saveFocusedObject) != nullptr)
					SetFocusedGameObjectID(saveFocusedObject);
			}
		}

		ImGui::EndChild();
		ImGui::PopStyleColor();
		ImGui::End();
	}

	// Helper function for Hierarchy child rendering (Recursive)
	void AddObjectWithChild(std::shared_ptr<GameObject> currentObject, const char* charName, int& node_clicked, long& queuedForDelete, float indent)
	{
		indent += 15;
		ImGuiTreeNodeFlags node_flags;
		long focusedObjectID = FlatEngine::GetFocusedGameObjectID();

		// If this node is selected, use the nodeFlag_selected to highlight it
		if (focusedObjectID == currentObject->GetID())
			node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_Selected;
		else
			node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;

		// Go to next row and column
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(1);
		if (currentObject->GetParentID() != -1)
		{
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + indent);
			// Set table cell bg color for child object						
			ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(ImVec4(hierarchyChildObjectColor.x, hierarchyChildObjectColor.y, hierarchyChildObjectColor.z, hierarchyChildObjectColor.w * .03f * indent)));
		}


		ImGui::PushStyleColor(ImGuiCol_Header, treeSelectableSelectedColor);
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, treeSelectableHoveredColor);
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, treeSelectableActiveColor);
		// TreeNode Opener. This tag renders the name of the node already so all we have to put in content-wise is it's children and interaction
		bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)currentObject->GetID(), node_flags, charName);
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();

		
		// Right click context menu for GameObject
		if (ImGui::BeginPopupContextItem()) // <-- use last item id as popup id
		{
			ImGui::PushStyleColor(ImGuiCol_HeaderHovered, treeSelectableHoveredColor);
			ImGui::PushStyleColor(ImGuiCol_HeaderActive, treeSelectableActiveColor);
			if (ImGui::MenuItem("Create Child"))
			{
				std::shared_ptr<GameObject> childObject = FlatEngine::CreateGameObject(currentObject->GetID());
				currentObject->AddChild(childObject->GetID());
				ImGui::CloseCurrentPopup();
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Delete GameObject"))
			{
				queuedForDelete = currentObject->GetID();
				ImGui::CloseCurrentPopup();
			}
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::EndPopup();
		}
		// For whether the object is the currentlySelected GameObject
		if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
		{
			node_clicked = currentObject->GetID();
			FlatEngine::SetFocusedGameObjectID(currentObject->GetID());
		}
		// Drag and Drop functionality
		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("_TREENODE", NULL, 0);
			ImGui::Text("This is a drag and drop source");
			ImGui::EndDragDropSource();
		}

		// If the node is open, render it's children
		if (node_open)
		{
			std::vector<long> childrenIDs = currentObject->GetChildren();

			// Render SceneObject children
			for (long childID : childrenIDs)
			{
				std::shared_ptr<GameObject> child = FlatEngine::GetObjectById(childID);
				std::string name = child->GetName();
				const char* childName = name.c_str();

				if (child->HasChildren())
					AddObjectWithChild(child, childName, node_clicked, queuedForDelete, indent);
				else
					AddObjectWithoutChild(child, childName, node_clicked, queuedForDelete, indent);
			}

			ImGui::TreePop(); // TreeNode Closer
		}
	}

	// Helper function for Hierarchy child rendering
	void AddObjectWithoutChild(std::shared_ptr<GameObject> currentObject, const char* charName, int& node_clicked, long& queuedForDelete, float indent)
	{
		indent += 15;
		ImGuiTreeNodeFlags node_flags;
		long focusedObjectID = FlatEngine::GetFocusedGameObjectID();

		// If this node is selected, use the nodeFlag_selected to highlight it
		if (focusedObjectID == currentObject->GetID())
			node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_Selected;
		else
			node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;

		// Go to next row and column
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(1);
		if (currentObject->GetParentID() != -1)
		{
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + indent);
			// Set table cell bg color for child object						
			ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(ImVec4(hierarchyChildObjectColor.x, hierarchyChildObjectColor.y, hierarchyChildObjectColor.z, hierarchyChildObjectColor.w * .03f * indent)));
		}

		ImGui::PushStyleColor(ImGuiCol_Header, treeSelectableSelectedColor);
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, treeSelectableHoveredColor);
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, treeSelectableActiveColor);
		// TreeNode Opener - No TreePop because it's a leaf
		ImGui::TreeNodeEx((void*)(intptr_t)currentObject->GetID(), node_flags, charName);
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();

		if (ImGui::BeginPopupContextItem()) // <-- use last item id as popup id
		{
			ImGui::PushStyleColor(ImGuiCol_HeaderHovered, treeSelectableHoveredColor);
			ImGui::PushStyleColor(ImGuiCol_HeaderActive, treeSelectableActiveColor);
			if (ImGui::MenuItem("Create Child"))
			{
				std::shared_ptr<GameObject> childObject = FlatEngine::CreateGameObject(currentObject->GetID());
				currentObject->AddChild(childObject->GetID());
				ImGui::CloseCurrentPopup();
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Delete GameObject"))
			{
				queuedForDelete = currentObject->GetID();
				ImGui::CloseCurrentPopup();
			}
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			
			ImGui::EndPopup();
		}

		// Setting the focus to this GameObject
		if (ImGui::IsItemClicked())
		{
			node_clicked = currentObject->GetID();
			FlatEngine::SetFocusedGameObjectID(currentObject->GetID());
		}
		// Drag and Drop Functionality
		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("_TREENODE", NULL, 0);
			ImGui::Text("This is a drag and drop source");
			ImGui::EndDragDropSource();
		}
	}
}
}