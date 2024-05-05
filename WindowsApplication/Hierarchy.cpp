#include "FlatEngine.h"
#include "Scene.h"

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

		// Render Loaded Scene text and threeDots more menu button
		std::string sceneName = GetLoadedScene()->GetName();
		std::string loadedSceneString = "Loaded Scene: " + sceneName;
		ImGui::PushStyleColor(ImGuiCol_ChildBg, innerWindowColor);
		ImGui::BeginChild(loadedSceneString.c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 30));
		ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 6, ImGui::GetCursorPosY() + 8));
		ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorU32(ImVec4(1,1,1,.8)));
		ImGui::Text(loadedSceneString.c_str());
		ImGui::PopStyleColor();
		ImGui::SameLine(ImGui::GetContentRegionAvail().x - 26, 0);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
		RenderImageButton("##AddGameObjectHierarchy", threeDotsTexture, ImVec2(16, 16), 1, transparentColor);
		PushMenuStyles();
		if (ImGui::BeginPopupContextItem("##InspectorMoreContext", ImGuiPopupFlags_MouseButtonLeft)) // <-- use last item id as popup id
		{
			if (ImGui::BeginMenu("Add New"))
			{
				if (ImGui::MenuItem("GameObject"))
				{
					CreateGameObject(-1);
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndMenu();
			}
			ImGui::Separator();
			if (ImGui::BeginMenu("Manage Scene"))
			{
				if (ImGui::MenuItem("Reload Scene"))
				{
					LoadScene(GetLoadedScenePath());
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndMenu();
			}
			ImGui::EndPopup();
		}
		PopMenuStyles();
		ImGui::EndChild();
		ImGui::PopStyleColor();

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 3);


		// Scene Objects in Hierarchy
		{
			static ImGuiTableFlags tableFlags = ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchSame;
			float objectColumnWidth = ImGui::GetContentRegionAvail().x;
			float visibleIconColumnWidth = 20;
			static float currentIndent = 10;
			static bool _allAreVisible = false;
			std::vector<std::shared_ptr<GameObject>> sceneObjects = FlatEngine::GetSceneObjects();

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, 0.0f });			
			ImGui::PushStyleColor(ImGuiCol_FrameBg, innerWindowColor);
			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0, 0));
			ImGui::BeginTable("##HierarchyTable", 2, tableFlags);		

			ImGui::TableSetupColumn("##VISIBLE", 0, visibleIconColumnWidth);
			ImGui::TableSetupColumn("##OBJECT", 0, objectColumnWidth);
			ImGui::TableNextRow();

			// Visible/Invisible all gameObjects at once
			ImGui::TableSetColumnIndex(0);
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() - 1, ImGui::GetCursorPosY() + 1));
			if (_allAreVisible)
			{
				if (RenderImageButton("##SetAllInVisible", showTexture, ImVec2(16, 16), 0, buttonColor, whiteColor, buttonHoveredColor, buttonActiveColor))
				{
					for (std::shared_ptr<GameObject> currentObject : sceneObjects)
						currentObject->SetActive(false);
					_allAreVisible = false;
				}
			}
			else
			{
				if (RenderImageButton("##SetAllVisible", hideTexture, ImVec2(16, 16), 0, buttonColor, whiteColor, buttonHoveredColor, buttonActiveColor))
				{
					for (std::shared_ptr<GameObject> currentObject : sceneObjects)
						currentObject->SetActive(true);
					_allAreVisible = true;
				}
			}

			ImGui::TableSetColumnIndex(1);
			ImVec2 cellSize = ImVec2(ImGui::GetContentRegionAvail().x + 1, 20);
			ImVec2 cursorScreen = ImVec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y + 1);
			ImGui::PushStyleColor(ImGuiCol_Text, logTextColor);
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 7, ImGui::GetCursorPosY() + 4));
			ImGui::Text("ALL SCENE OBJECTS");
			ImGui::PopStyleColor();
			ImGui::TableNextRow();
			ImGui::TableNextRow();
			ImGui::GetWindowDrawList()->AddRectFilled(cursorScreen, ImVec2(cursorScreen.x + cellSize.x, cursorScreen.y + cellSize.y), ImGui::GetColorU32(buttonActiveColor));

			static int node_clicked = -1;
			long queuedForDelete = -1;

			for (std::vector<std::shared_ptr<GameObject>>::iterator object = sceneObjects.begin(); object != sceneObjects.end(); object++)
			{
				// Add new scene Objects to the tracker as they appear
				if (leafExpandedTracker.count((*object)->GetID()) == 0)
					leafExpandedTracker.emplace((*object)->GetID(), true);

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
						AddObjectWithoutChild(currentObject, charName, node_clicked, queuedForDelete, 0);
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
			if (availableVerticalSpace > 23)
			{
				for (int i = 0; i < availableVerticalSpace / 20 - 1; i++)
				{
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(1);
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 7);
					ImGui::Text("");
				}
			}

			ImGui::EndTable();

			ImGui::PopStyleVar();
			ImGui::PopStyleColor();
			ImGui::PopStyleVar();

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
		ImGuiTreeNodeFlags node_flags;
		long focusedObjectID = FlatEngine::GetFocusedGameObjectID();

		// If this node is selected, use the nodeFlag_selected to highlight it
		if (focusedObjectID == currentObject->GetID())
			node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Selected;
		else
			node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_DefaultOpen;

		// Go to next row and column
		ImGui::TableNextRow();

		// Draw visibility eye icon
		ImGui::TableSetColumnIndex(0);
		std::string visibleID = "VisibleID" + currentObject->GetID();

		float extraIndent = 0;
		if (indent > 0)
			extraIndent = 6;
		float indentMultiplier = indent / 15;
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - indent - (extraIndent * indentMultiplier) - 1);
		indent += 15;
		// Show Visible/Invisible Icons
		if (currentObject->IsActive())
		{
			if (RenderImageButton(visibleID.c_str(), showTexture, ImVec2(16, 16), 0, transparentColor))
				currentObject->SetActive(false);
		}
		else
		{
			if (RenderImageButton(visibleID.c_str(), hideTexture, ImVec2(16, 16), 0, transparentColor))
				currentObject->SetActive(true);
		}

		// Render actual gameObject
		ImGui::TableSetColumnIndex(1);
		if (currentObject->GetParentID() != -1)
		{
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + indent);
			// Set table cell bg color for child object						
			ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(ImVec4(hierarchyChildObjectColor.x, hierarchyChildObjectColor.y, hierarchyChildObjectColor.z, hierarchyChildObjectColor.w * .03f * indent)));
		}
		
		// Swap Drag and Drop 
		std::vector<std::shared_ptr<GameObject>> sceneObjects = GetSceneObjects();
		auto it = std::find(sceneObjects.begin(), sceneObjects.end(), currentObject);
		int index = -1;
		// If element was found 
		if (it != sceneObjects.end())
			index = (int)(it - sceneObjects.begin());

		ImVec2 cursorPos = ImGui::GetCursorPos();
		ImVec2 availSpace = ImGui::GetContentRegionAvail();
		ImVec2 size = ImVec2(availSpace.x + 30 - cursorPos.x, 2);
		if (size.x < 30)
			size.x = 30;
		std::string id = "##SwapDropSource" + index;

		ImGui::InvisibleButton(id.c_str(), size);		
		ImGui::PushStyleColor(ImGuiCol_DragDropTarget, buttonHoveredColor);	
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_HIERARCHY_OBJECT"))
			{
				IM_ASSERT(payload->DataSize == sizeof(int));
				int ID = *(const int*)payload->Data;

				// Save Dropped Object
				std::shared_ptr<GameObject> dropped = GetObjectById(ID);
				// Remove dropped object from its previous parents children
				if (dropped->GetParentID() != -1)
				{
					std::shared_ptr<GameObject> parent = GetObjectById(dropped->GetParentID());
					parent->RemoveChild(dropped->GetID());
				}
				// Set parent ID of dropped object to -1
				dropped->SetParentID(-1);
			}
			ImGui::EndDragDropTarget();
		}

		if (it == sceneObjects.begin())
			ImGui::SetCursorPos(ImVec2(cursorPos.x, cursorPos.y + 1));
		else
			ImGui::SetCursorPos(ImVec2(cursorPos.x, cursorPos.y));

		ImGui::SetNextItemOpen(leafExpandedTracker.at(currentObject->GetID()));

		ImGui::PushStyleColor(ImGuiCol_Header, treeSelectableSelectedColor);
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, treeSelectableHoveredColor);
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, treeSelectableActiveColor);
		// TreeNode Opener. This tag renders the name of the node already so all we have to put in content-wise is it's children and interaction
		bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)currentObject->GetID(), node_flags, charName);
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		//if (ImGui::IsItemClicked())
		//	LogString("Clicked");
		leafExpandedTracker.at(currentObject->GetID()) = node_open;

		if (ImGui::IsItemHovered() && ImGui::GetIO().KeyAlt)
		{
			// Mouse Hover Tooltip - Mouse Over Tooltip			
			ImGui::BeginTooltip();
			ImGui::Text("GameObject Data");
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
			ImGui::Separator();
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
			// ID
			ImGui::Text("Object ID    | ");
			ImGui::SameLine();
			ImGui::Text(std::to_string(currentObject->GetID()).c_str());
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
			ImGui::Separator();
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
			// Children
			ImGui::Text("Children IDs | ");
			for (long child : currentObject->GetChildren())
			{
				std::string idString = std::to_string(child) + "-";
				ImGui::SameLine();
				ImGui::Text(idString.c_str());
			}
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
			ImGui::Separator();
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
			// Indent
			ImGui::Text("Indent       | ");
			std::string indentString = std::to_string(indent);
			ImGui::SameLine();
			ImGui::Text(indentString.c_str());
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
			ImGui::Separator();
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
			// Extra Indent
			ImGui::Text("Extra indent | ");
			std::string extraIndentString = std::to_string(extraIndent);
			ImGui::SameLine();
			ImGui::Text(extraIndentString.c_str());
			ImGui::EndTooltip();
		}


		// Right click context menu for GameObject
		if (ImGui::BeginPopupContextItem()) // <-- use last item id as popup id
		{
			PushMenuStyles();
			if (ImGui::MenuItem("Create Child"))
			{
				std::shared_ptr<GameObject> childObject = CreateGameObject(currentObject->GetID());
				currentObject->AddChild(childObject->GetID());
				ImGui::CloseCurrentPopup();
			}
			
			if (ImGui::MenuItem("Delete GameObject"))
			{
				queuedForDelete = currentObject->GetID();
				ImGui::CloseCurrentPopup();
			}
			PopMenuStyles();
			ImGui::EndPopup();
		}
		// For whether the object is the currentlySelected GameObject
		if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
		{
			node_clicked = currentObject->GetID();
			FlatEngine::SetFocusedGameObjectID(currentObject->GetID());
		}
		
		// Add As Child Drag and Drop
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceNoHoldToOpenOthers))
		{
			int ID = currentObject->GetID();
			leafExpandedTracker.at(currentObject->GetID()) = false;
			ImGui::SetDragDropPayload("DND_HIERARCHY_OBJECT", &ID, sizeof(int));
			ImGui::Text("Set Parent");
			ImGui::EndDragDropSource();
		}
		else
			leafExpandedTracker.at(currentObject->GetID()) = node_open;

		// Separator Button for swapping
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_HIERARCHY_OBJECT"))
			{
				IM_ASSERT(payload->DataSize == sizeof(int));
				int ID = *(const int*)payload->Data;

				// Save Dropped Object
				std::shared_ptr<GameObject> dropped = GetObjectById(ID);
				// Remove dropped object from its previous parents children
				if (dropped->GetParentID() != -1)
				{
					std::shared_ptr<GameObject> parent = GetObjectById(dropped->GetParentID());
					parent->RemoveChild(dropped->GetID());
				}
				// Add dropped object to this object as a child
				currentObject->AddChild(dropped->GetID());
				dropped->SetParentID(currentObject->GetID());
			}
			ImGui::EndDragDropTarget();
		}
		ImGui::PopStyleColor();

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
		ImGuiTreeNodeFlags node_flags;
		long focusedObjectID = FlatEngine::GetFocusedGameObjectID();

		// If this node is selected, use the nodeFlag_selected to highlight it
		if (focusedObjectID == currentObject->GetID())
			node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_Selected;
		else
			node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;

		// Go to next row and column
		ImGui::TableNextRow();

		// Draw visibility eye icon
		ImGui::TableSetColumnIndex(0);
		std::string visibleID = "VisibleID" + currentObject->GetID();
		
		float extraIndent = 0;
		if (indent > 0)
			extraIndent = 6;
		float indentMultiplier = indent / 15;
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - indent - (extraIndent * indentMultiplier) - 1);
		indent += 15;
		// Show Visible/Invisible Icons
		if (currentObject->IsActive())
		{
			if (RenderImageButton(visibleID.c_str(), showTexture, ImVec2(16, 16), 0, transparentColor))
				currentObject->SetActive(false);
		}
		else
		{
			if (RenderImageButton(visibleID.c_str(), hideTexture, ImVec2(16, 16), 0, transparentColor))
				currentObject->SetActive(true);
		}

		// Render actual gameObject
		ImGui::TableSetColumnIndex(1);
		if (currentObject->GetParentID() != -1)
		{
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + indent);
			// Set table cell bg color for child object						
			ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(ImVec4(hierarchyChildObjectColor.x, hierarchyChildObjectColor.y, hierarchyChildObjectColor.z, hierarchyChildObjectColor.w * .03f * indent)));
		}

		// Swap Drag and Drop 
		std::vector<std::shared_ptr<GameObject>> sceneObjects = GetSceneObjects();
		auto it = std::find(sceneObjects.begin(), sceneObjects.end(), currentObject);
		int index = -1;
		// If element was found 
		if (it != sceneObjects.end())
			index = (int)(it - sceneObjects.begin());

		ImVec2 cursorPos = ImGui::GetCursorPos();
		ImVec2 availSpace = ImGui::GetContentRegionAvail();
		ImVec2 size = ImVec2(availSpace.x + 30 - cursorPos.x, 2);
		if (size.x < 30)
			size.x = 30;
		std::string id = "##SwapDropSource" + index;
		
		ImGui::PushStyleColor(ImGuiCol_DragDropTarget, buttonHoveredColor);
		ImGui::InvisibleButton(id.c_str(), size);
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_HIERARCHY_OBJECT"))
			{
				IM_ASSERT(payload->DataSize == sizeof(int));
				int ID = *(const int*)payload->Data;

				// Save Dropped Object
				std::shared_ptr<GameObject> dropped = GetObjectById(ID);
				// Remove dropped object from its previous parents children
				if (dropped->GetParentID() != -1)
				{
					std::shared_ptr<GameObject> parent = GetObjectById(dropped->GetParentID());
					parent->RemoveChild(dropped->GetID());
				}
				// Set parent ID of dropped object to -1
				dropped->SetParentID(-1);
			}
			ImGui::EndDragDropTarget();
		}
		if (it == sceneObjects.begin())
			ImGui::SetCursorPos(ImVec2(cursorPos.x, cursorPos.y + 1));
		else
			ImGui::SetCursorPos(ImVec2(cursorPos.x, cursorPos.y));

		ImGui::PushStyleColor(ImGuiCol_Header, treeSelectableSelectedColor);
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, treeSelectableHoveredColor);
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, treeSelectableActiveColor);
		// TreeNode Opener - No TreePop because it's a leaf
		ImGui::TreeNodeEx((void*)(intptr_t)currentObject->GetID(), node_flags, charName);
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();

		if (ImGui::IsItemHovered() && ImGui::GetIO().KeyAlt)
		{
			// Mouse Hover Tooltip - Mouse Over Tooltip			
			ImGui::BeginTooltip();
			ImGui::Text("GameObject Data");
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
			ImGui::Separator();
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
			// ID
			ImGui::Text("Object ID    | ");
			ImGui::SameLine();
			ImGui::Text(std::to_string(currentObject->GetID()).c_str());
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
			ImGui::Separator();
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
			// Children
			ImGui::Text("Children IDs | ");
			for (long child : currentObject->GetChildren())
			{
				std::string idString = std::to_string(child) + "-";
				ImGui::SameLine();
				ImGui::Text(idString.c_str());
			}
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
			ImGui::Separator();
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
			// Indent
			ImGui::Text("Indent       | ");
			std::string indentString = std::to_string(indent);
			ImGui::SameLine();
			ImGui::Text(indentString.c_str());
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
			ImGui::Separator();
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
			// Extra Indent
			ImGui::Text("Extra indent | ");
			std::string extraIndentString = std::to_string(extraIndent);
			ImGui::SameLine();
			ImGui::Text(extraIndentString.c_str());
			ImGui::EndTooltip();
		}

		ImVec2 savedCursorPos = ImGui::GetCursorPos();

		if (ImGui::BeginPopupContextItem()) // <-- use last item id as popup id
		{
			PushMenuStyles();
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
			PopMenuStyles();
			
			ImGui::EndPopup();
		}

		// Setting the focus to this GameObject
		if (ImGui::IsItemClicked())
		{
			node_clicked = currentObject->GetID();
			FlatEngine::SetFocusedGameObjectID(currentObject->GetID());
		}

		// Add As Child Drag and Drop
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceNoHoldToOpenOthers))
		{
			int ID = currentObject->GetID();
			ImGui::SetDragDropPayload("DND_HIERARCHY_OBJECT", &ID, sizeof(int));
			ImGui::Text("Set Parent");
			ImGui::EndDragDropSource();
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_HIERARCHY_OBJECT"))
			{
				IM_ASSERT(payload->DataSize == sizeof(int));
				int ID = *(const int*)payload->Data;

				// Save Dropped Object
				std::shared_ptr<GameObject> dropped = GetObjectById(ID);
				// Remove dropped object from its previous parents children
				if (dropped->GetParentID() != -1)
				{
					std::shared_ptr<GameObject> parent = GetObjectById(dropped->GetParentID());
					parent->RemoveChild(dropped->GetID());
				}
				// Add dropped object to this object as a child
				currentObject->AddChild(dropped->GetID());
				dropped->SetParentID(currentObject->GetID());
			}
			ImGui::EndDragDropTarget();
		}
		ImGui::PopStyleColor();
	}
}
}