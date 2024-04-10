#include "FlatEngine.h"

namespace FlatEngine { namespace FlatGui {

	void RenderHierarchy()
	{
		ImGuiWindowFlags window_flags = ImGuiChildFlags_AutoResizeX;
		ImGuiStyle& style = ImGui::GetStyle();

		ImGui::Begin("Scene Hierarchy");

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

		if (ImGui::Button("Add New GameObject"))
			CreateGameObject(-1);

		// Scene Objects in Hierarchy
		{
			ImGui::PushStyleColor(ImGuiCol_FrameBg, innerWindowColor);
			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0, -5));
			ImGui::BeginListBox("##SceneObjects", ImVec2(-FLT_MIN, -FLT_MIN));
			ImGui::PopStyleVar();
			ImGui::PopStyleColor();

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

					// If the object has children call the recursive AddObjectWithChild();
					if (currentObject->HasChildren())
						AddObjectWithChild(currentObject, charName, node_clicked, queuedForDelete);
					else
						AddObjectWithoutChild(currentObject, charName, node_clicked, queuedForDelete);

					// Push Item Spacing
					ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, 1.0f });
					ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(1, 1, 1, 0.2f));
					ImGui::Separator();
					ImGui::PopStyleColor();
					// Pop Item Spacing
					ImGui::PopStyleVar();
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

			ImGui::EndListBox();

			// Delete queued GameObject
			if (queuedForDelete != -1)
			{
				long focusedObjectID = FlatEngine::GetFocusedGameObjectID();

				// Unfocus GameObject first if it is focused
				if (queuedForDelete == focusedObjectID)
				{
					FlatEngine::SetFocusedGameObjectID(-1);
				}
				FlatEngine::DeleteGameObject(queuedForDelete);
			}
		}

		ImGui::EndChild();
		ImGui::PopStyleColor();
		ImGui::End();
	}

	// Helper function for Hierarchy child rendering (Recursive)
	void AddObjectWithChild(std::shared_ptr<GameObject> currentObject, const char* charName, int& node_clicked, long& queuedForDelete)
	{
		ImGuiTreeNodeFlags node_flags;
		long focusedObjectID = FlatEngine::GetFocusedGameObjectID();

		// If this node is selected, use the nodeFlag_selected to highlight it
		if (focusedObjectID == currentObject->GetID())
			node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_Selected;
		else
			node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;

		// Push item spacing
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, 0.0f });

		// TreeNode Opener. This tag renders the name of the node already so all we have to put in content-wise is it's children and interaction
		bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)currentObject->GetID(), node_flags, charName);

		// Right click context menu for GameObject
		if (ImGui::BeginPopupContextItem()) // <-- use last item id as popup id
		{
			if (ImGui::Button("Add child"))
			{
				std::shared_ptr<GameObject> childObject = FlatEngine::CreateGameObject(currentObject->GetID());
				currentObject->AddChild(childObject->GetID());
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::Button("Delete GameObject"))
			{
				queuedForDelete = currentObject->GetID();
				ImGui::CloseCurrentPopup();
			}
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

				ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(1, 1, 1, 0.05f));
				ImGui::Separator();
				ImGui::PopStyleColor();

				if (child->HasChildren())
					AddObjectWithChild(child, childName, node_clicked, queuedForDelete);
				else
					AddObjectWithoutChild(child, childName, node_clicked, queuedForDelete);
			}

			ImGui::TreePop(); // TreeNode Closer
		}

		// Pop Item Spacing
		ImGui::PopStyleVar();
	}

	// Helper function for Hierarchy child rendering
	void AddObjectWithoutChild(std::shared_ptr<GameObject> currentObject, const char* charName, int& node_clicked, long& queuedForDelete)
	{
		ImGuiTreeNodeFlags node_flags;
		long focusedObjectID = FlatEngine::GetFocusedGameObjectID();

		// If this node is selected, use the nodeFlag_selected to highlight it
		if (focusedObjectID == currentObject->GetID())
			node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_Selected;
		else
			node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, 0.0f });

		// TreeNode Opener - No TreePop because it's a leaf
		ImGui::TreeNodeEx((void*)(intptr_t)currentObject->GetID(), node_flags, charName);
		if (ImGui::BeginPopupContextItem()) // <-- use last item id as popup id
		{
			if (ImGui::Button("Add child"))
			{
				std::shared_ptr<GameObject> childObject = FlatEngine::CreateGameObject(currentObject->GetID());
				currentObject->AddChild(childObject->GetID());
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::Button("Delete GameObject"))
			{
				queuedForDelete = currentObject->GetID();
				ImGui::CloseCurrentPopup();
			}
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

		ImGui::PopStyleVar();
	}
}
}