#include "FlatEngine.h"
#include "FlatGui.h"
#include "GameObject.h"
#include "Scene.h"
#include "Transform.h"

using Transform = FlatEngine::Transform;

namespace FlatGui 
{
	void RenderHierarchy()
	{
		ImGuiWindowFlags window_flags = ImGuiChildFlags_AutoResizeX;
		ImGuiStyle& style = ImGui::GetStyle();

		FlatEngine::BeginWindow("Hierarchy");

		// Variables for viewport dimensions
		Vector2 canvas_p0 = ImGui::GetCursorScreenPos();      // ImDrawList API uses screen coordinates!
		Vector2 canvas_sz = ImGui::GetContentRegionAvail();   // Resize canvas to what's available
		if (canvas_sz.x < 50.0f) canvas_sz.x = 50.0f;
		if (canvas_sz.y < 50.0f) canvas_sz.y = 50.0f;
		Vector2 canvas_p1 = Vector2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);

		ImGuiInputTextFlags flags = ImGuiInputTextFlags_::ImGuiInputTextFlags_AutoSelectAll;

		// Render Loaded Scene text and threeDots more menu button
		std::string sceneName = FlatEngine::GetLoadedScene()->GetName();
		std::string loadedSceneString = "Loaded Scene: " + sceneName;

		FlatEngine::RenderSectionHeader(loadedSceneString);
		ImGui::SameLine(ImGui::GetContentRegionAvail().x - 26, 0);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 29);

		FlatEngine::RenderImageButton("##AddGameObjectHierarchy", FlatEngine::F_threeDotsIcon.GetTexture(), Vector2(16, 16), 1, FlatEngine::F_transparentColor);
		FlatEngine::PushMenuStyles();
		if (ImGui::BeginPopupContextItem("##InspectorMoreContext", ImGuiPopupFlags_MouseButtonLeft)) // <-- use last item id as popup id
		{
			if (ImGui::MenuItem("Quick save"))
			{
				FlatEngine::SaveCurrentScene();
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::MenuItem("Reload scene"))
			{
				FlatEngine::LoadScene(FlatEngine::GetLoadedScenePath());
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
		FlatEngine::PopMenuStyles();

		// Hides bottom separator in the RenderSectionHeader under hierarchy
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 4);

		// Scene Objects in Hierarchy
		{
			float objectColumnWidth = ImGui::GetContentRegionAvail().x;
			float visibleIconColumnWidth = 20;
			float isPrefabIconColumnWidth = 20;
			static float currentIndent = 10;
			static bool _allAreVisible = false;
			std::map<long, GameObject> &sceneObjects = FlatEngine::GetSceneObjects();

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, 0.0f });			
			ImGui::PushStyleColor(ImGuiCol_FrameBg, FlatEngine::F_innerWindowColor);
			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, Vector2(0, 0));
			ImGui::BeginTable("##HierarchyTable", 3, FlatEngine::F_tableFlags);

			ImGui::TableSetupColumn("##VISIBLE", 0, visibleIconColumnWidth);
			ImGui::TableSetupColumn("##OBJECT", 0, objectColumnWidth);
			ImGui::TableSetupColumn("##ISPREFAB", 0, isPrefabIconColumnWidth);
			ImGui::TableNextRow();

			// Visible/Invisible all gameObjects at once
			ImGui::TableSetColumnIndex(0);
			ImGui::SetCursorPos(Vector2(ImGui::GetCursorPosX() - 1, ImGui::GetCursorPosY() + 1));
			if (_allAreVisible)
			{
				if (FlatEngine::RenderImageButton("##SetAllInvisible", FlatEngine::F_showIcon.GetTexture(), Vector2(16, 16), 0, FlatEngine::F_buttonColor, FlatEngine::F_whiteColor, FlatEngine::F_buttonHoveredColor, FlatEngine::F_buttonActiveColor))
				{
					for (std::map<long, GameObject>::iterator iter = sceneObjects.begin(); iter != sceneObjects.end();)
					{
						iter->second.SetActive(false);
						iter++;
					}
					_allAreVisible = false;
				}
			}
			else
			{
				if (FlatEngine::RenderImageButton("##SetAllVisible", FlatEngine::F_hideIcon.GetTexture(), Vector2(16, 16), 0, FlatEngine::F_buttonColor, FlatEngine::F_whiteColor, FlatEngine::F_buttonHoveredColor, FlatEngine::F_buttonActiveColor))
				{
					for (std::map<long, GameObject>::iterator iter = sceneObjects.begin(); iter != sceneObjects.end();)
					{
						iter->second.SetActive(true);
						iter++;
					}
					_allAreVisible = true;
				}
			}

			ImGui::TableSetColumnIndex(1);
			Vector2 cellSize = Vector2(ImGui::GetContentRegionAvail().x + 1, 20);
			Vector2 cursorScreen = Vector2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y + 1);
			ImGui::PushStyleColor(ImGuiCol_Text, FlatEngine::F_logTextColor);
			ImGui::SetCursorPos(Vector2(ImGui::GetCursorPosX() + 7, ImGui::GetCursorPosY() + 4));
			ImGui::Text("ALL SCENE OBJECTS");
			ImGui::PopStyleColor();

			ImGui::TableSetColumnIndex(2);
			ImGui::SetCursorPos(Vector2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 1));
			if (FlatEngine::RenderImageButton("##PrefabCubes", FlatEngine::F_prefabCubeIcon.GetTexture(), Vector2(16, 16), 0, FlatEngine::F_buttonColor, FlatEngine::F_whiteColor, FlatEngine::F_buttonHoveredColor, FlatEngine::F_buttonActiveColor))
			{
				// Doesn't do anything, should just be an icon
			}

			ImGui::TableNextRow();
			ImGui::TableNextRow();
			ImGui::TableNextRow();
			ImGui::GetWindowDrawList()->AddRectFilled(cursorScreen, Vector2(cursorScreen.x + cellSize.x, cursorScreen.y + cellSize.y), ImGui::GetColorU32(FlatEngine::F_buttonActiveColor));

			static int node_clicked = -1;
			long queuedForDelete = -1;
			/*FlatEngine::LogFloat(FlatEngine::GetEngineTime(), "Start: ");*/
			for (std::map<long, GameObject>::iterator object = sceneObjects.begin(); object != sceneObjects.end(); object++)
			{
				// Add new scene Objects to the tracker as they appear
				if (leafExpandedTracker.count(object->second.GetID()) == 0)
					leafExpandedTracker.emplace(object->second.GetID(), true);

				// If this object does not have a parent we render it and all of its children.
				if (object->second.GetParentID() == -1)
				{
					// Get Object name
					GameObject &currentObject = object->second;
					std::string name = currentObject.GetName();
					const char* charName = name.c_str();
					float indent = 0;

					AddObjectToHierarchy(currentObject, charName, node_clicked, queuedForDelete, indent);
				}
			}
			//FlatEngine::LogFloat(FlatEngine::GetEngineTime(), "End: ");
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
				long focusedObjectID = GetFocusedGameObjectID();
				long saveFocusedObject = focusedObjectID;

				// Unfocus focused GameObject first
				SetFocusedGameObjectID(-1);
				FlatEngine::DeleteGameObject(queuedForDelete);

				// If previous focused object still exists, set it to focused object again
				if (FlatEngine::GetObjectById(saveFocusedObject) != nullptr)
					SetFocusedGameObjectID(saveFocusedObject);
			}
		}

		FlatEngine::EndWindow();
	}

	// Add GameObject to Hierarchy viewport
	void AddObjectToHierarchy(GameObject& currentObject, const char* charName, int& node_clicked, long& queuedForDelete, float indent)
	{
		ImGuiTreeNodeFlags node_flags;
		long focusedObjectID = GetFocusedGameObjectID();

		// If this node is selected, use the nodeFlag_selected to highlight it
		if (focusedObjectID == currentObject.GetID())
		{
			if (currentObject.HasChildren())
			{
				node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Selected;
			}
			else
			{
				node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_Selected;
			}
		}
		// Not focused
		else
		{
			if (currentObject.HasChildren())
			{
				node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_DefaultOpen;
			}
			else
			{
				node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;
			}
		}

		// Go to next row and column
		ImGui::TableNextRow();

		// Draw visibility eye icon
		ImGui::TableSetColumnIndex(0);
		std::string visibleID = "VisibleID" + std::to_string(currentObject.GetID());

		float extraIndent = 0;
		if (indent > 0)
			extraIndent = 6;
		float indentMultiplier = indent / 15;
		// Moves the cursor to account for the visible icon maybe?
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - indent - (extraIndent * indentMultiplier) - 1);
		indent += 15;
		// Show Visible/Invisible Icons
		if (currentObject.IsActive())
		{
			if (FlatEngine::RenderImageButton(visibleID.c_str(), FlatEngine::F_showIcon.GetTexture(), Vector2(16, 16), 0, FlatEngine::F_transparentColor))
				currentObject.SetActive(false);
		}
		else
		{
			if (FlatEngine::RenderImageButton(visibleID.c_str(), FlatEngine::F_hideIcon.GetTexture(), Vector2(16, 16), 0, FlatEngine::F_transparentColor))
				currentObject.SetActive(true);
		}

		// Render actual gameObject
		ImGui::TableSetColumnIndex(1);

		int index = -1;

		Vector2 cursorPos = ImGui::GetCursorPos();
		Vector2 availSpace = ImGui::GetContentRegionAvail();
		Vector2 size = Vector2(availSpace.x + 30 - cursorPos.x, 2);
		if (size.x < 30)
			size.x = 30;
		std::string id = "##SwapDropSource" + std::to_string(index);

		ImGui::PushStyleColor(ImGuiCol_DragDropTarget, FlatEngine::F_buttonHoveredColor);
		ImGui::InvisibleButton(id.c_str(), size);
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_HIERARCHY_OBJECT"))
			{
				IM_ASSERT(payload->DataSize == sizeof(int));
				int ID = *(const int*)payload->Data;

				// Save Dropped Object
				GameObject *dropped = FlatEngine::GetObjectById(ID);
				// Remove dropped object from its previous parents children
				if (dropped->GetParentID() != -1)
				{
					GameObject parent = FlatEngine::GetObjectById(dropped->GetParentID());
					parent.RemoveChild(dropped->GetID());
				}
				// Set parent ID of dropped object to -1
				dropped->SetParentID(-1);
				dropped->GetTransform()->SetOrigin(Vector2(0, 0));
			}
			ImGui::EndDragDropTarget();
		}


		bool node_open = false;

		ImGui::PushStyleColor(ImGuiCol_Header, FlatEngine::F_treeSelectableSelectedColor);
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, FlatEngine::F_treeSelectableHoveredColor);
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, FlatEngine::F_treeSelectableActiveColor);
		// Indent for the GameObject name
		if (currentObject.GetParentID() != -1)
		{
			ImGui::SetCursorScreenPos(Vector2(ImGui::GetCursorPos().x + indent, ImGui::GetCursorScreenPos().y));
			// Set table cell bg color for child object						
			ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(ImVec4(FlatEngine::F_hierarchyChildObjectColor.x, FlatEngine::F_hierarchyChildObjectColor.y, FlatEngine::F_hierarchyChildObjectColor.z, FlatEngine::F_hierarchyChildObjectColor.w * .03f * indent)));
		}
		if (currentObject.HasChildren())
		{
			ImGui::SetNextItemOpen(leafExpandedTracker.at(currentObject.GetID()));
			// TreeNode Opener. This tag renders the name of the node already so all we have to put in content-wise is it's children and interaction
			node_open = ImGui::TreeNodeEx((void*)(intptr_t)currentObject.GetID(), node_flags, charName);
			leafExpandedTracker.at(currentObject.GetID()) = node_open;
		}
		else 
		{
			// TreeNode Opener - No TreePop because it's a leaf
			ImGui::TreeNodeEx((void*)(intptr_t)currentObject.GetID(), node_flags, charName);
		}
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();


		// Get Scene View Dimensions from its ImGui window
		Vector2 sceneViewDimensions;
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, Vector2(0, 0));
		FlatEngine::PushWindowStyles();
		ImGui::Begin("Scene View", 0, 16 | 8);
		sceneViewDimensions = Vector2(ImGui::GetWindowWidth(), ImGui::GetWindowHeight());
		ImGui::End();
		FlatEngine::PopWindowStyles();
		ImGui::PopStyleVar();
		ImGui::PopStyleVar();

		// Control click a Hierarchy item to focus on it in the Scene View
		if (ImGui::GetIO().KeyCtrl && ImGui::IsItemClicked())
		{
			FlatEngine::Transform* transform = currentObject.GetTransform();
			Vector2 position = transform->GetTruePosition();
			FG_sceneViewScrolling = Vector2(position.x * -FG_sceneViewGridStep.x + (sceneViewDimensions.x / 2), position.y * FG_sceneViewGridStep.y + (sceneViewDimensions.y / 2));
		}

		// Hold Alt key and hover object in Hierarchy for ToolTip with information about that GameObject
		if (ImGui::IsItemHovered() && ImGui::GetIO().KeyAlt)
		{	
			FlatEngine::BeginToolTip("GameObject Data");
			FlatEngine::RenderToolTipLong("Object ID", currentObject.GetID());
			FlatEngine::RenderToolTipLong("Parent ID", currentObject.GetParentID());
			FlatEngine::RenderToolTipLongVector("Children IDs", currentObject.GetChildren());
			FlatEngine::EndToolTip();
		}

		Vector2 savedCursorPos = ImGui::GetCursorPos();

		// Right click menu
		if (ImGui::BeginPopupContextItem()) // <-- use last item id as popup id
		{
			FlatEngine::PushMenuStyles();
			if (ImGui::MenuItem("Create Child"))
			{
				GameObject childObject = FlatEngine::CreateGameObject(currentObject.GetID());
				SetFocusedGameObjectID(childObject.GetID());
				ImGui::CloseCurrentPopup();
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Create Prefab"))
			{
				std::string prefabPath = FlatEngine::OpenSaveFileExplorer();
				if (prefabPath != "")
				{
					std::string prefabName = FlatEngine::GetFilenameFromPath(prefabPath);
					CreatePrefab(prefabPath, currentObject);
					currentObject.SetIsPrefab(true);
					currentObject.SetPrefabName(prefabName);
					currentObject.SetPrefabSpawnLocation(currentObject.GetTransform()->GetPosition());
				}
				ImGui::CloseCurrentPopup();
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Delete GameObject"))
			{
				queuedForDelete = currentObject.GetID();
				ImGui::CloseCurrentPopup();
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Lock in view"))
			{
				if (FG_b_sceneViewLockedOnObject && FG_sceneViewLockedObject->GetID() == currentObject.GetID())
				{
					FG_b_sceneViewLockedOnObject = false;
					FG_sceneViewLockedObject = &currentObject;
				}
				else if (!FG_b_sceneViewLockedOnObject)
				{
					FG_sceneViewLockedObject = &currentObject;
					FG_b_sceneViewLockedOnObject = true;
				}

				ImGui::CloseCurrentPopup();
			}
			FlatEngine::PopMenuStyles();

			ImGui::EndPopup();
		}

		// Setting the focus to this GameObject
		if (ImGui::IsItemClicked())
		{
			node_clicked = currentObject.GetID();
			SetFocusedGameObjectID(currentObject.GetID());
		}

		// Add As Child Drag and Drop
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceNoHoldToOpenOthers))
		{
			int ID = currentObject.GetID();
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
				GameObject *dropped = FlatEngine::GetObjectById(ID);
				// Remove dropped object from its previous parents children
				if (dropped->GetParentID() != -1)
				{
					GameObject parent = dropped->GetParent();
					parent.RemoveChild(dropped->GetID());
				}
				// Add dropped object to this object as a child
				currentObject.AddChild(dropped->GetID());
				dropped->SetParentID(currentObject.GetID());
				dropped->GetTransform()->SetOrigin(currentObject.GetTransform()->GetTruePosition());
			}
			ImGui::EndDragDropTarget();
		}
		ImGui::PopStyleColor();

		// If the node is open, render it's children if it has any
		if (currentObject.HasChildren() && node_open)
		{
			std::vector<long> childrenIDs = currentObject.GetChildren();

			// Render SceneObject children
			for (long childID : childrenIDs)
			{
				GameObject* child = FlatEngine::GetObjectById(childID);

				if (child != nullptr)
				{
					std::string name = child->GetName();
					const char* childName = name.c_str();

					AddObjectToHierarchy(*child, childName, node_clicked, queuedForDelete, indent);
				}
			}

			ImGui::TreePop(); // TreeNode Closer
		}

		// Render Prefab Cube if it is a prefab object
		if (currentObject.IsPrefab())
		{
			std::string prefabIDImageButton = "PrefabID" + std::to_string(currentObject.GetID());
			std::string prefabIDContextMenu = "PrefabID" + std::to_string(currentObject.GetID());
			ImGui::TableSetColumnIndex(2);
			ImGui::SetCursorPos(Vector2(ImGui::GetCursorPosX() - 1, ImGui::GetCursorPosY()));
			FlatEngine::RenderImageButton(prefabIDImageButton.c_str(), FlatEngine::F_prefabCubeIcon.GetTexture(), Vector2(16, 16), 0, FlatEngine::F_transparentColor, FlatEngine::F_whiteColor, FlatEngine::F_buttonHoveredColor, FlatEngine::F_buttonActiveColor);
			FlatEngine::PushMenuStyles();
			if (ImGui::BeginPopupContextItem(prefabIDContextMenu.c_str(), ImGuiPopupFlags_MouseButtonLeft)) // <-- use last item id as popup id
			{
				if (ImGui::MenuItem("Disassemble prefab"))
				{
					currentObject.SetIsPrefab(false);
					currentObject.SetPrefabName("");
					currentObject.SetPrefabSpawnLocation(Vector2(0, 0));
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}
			FlatEngine::PopMenuStyles();
		}
	}

	void ResetHierarchyExpanderTracker()
	{
		// Initialize Hierarchy scene object expanded tracker
		leafExpandedTracker.clear();
		std::map<long, GameObject> sceneObjects = FlatEngine::GetSceneObjects();
		for (std::map<long, GameObject>::iterator object = sceneObjects.begin(); object != sceneObjects.end(); object++)
		{
			if (leafExpandedTracker.count(object->second.GetID()) == 0)
			{
				leafExpandedTracker.emplace(object->second.GetID(), true);
			}
		}
	}
}