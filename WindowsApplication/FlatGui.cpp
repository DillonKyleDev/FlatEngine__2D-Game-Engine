#include "FlatEngine.h"
#include "TextureManager.h"
#include "SceneManager.h"
#include "Logger.h"
#include "ScriptComponent.h"
#include <cmath>


/*
######################################
######							######
######   ImGui implimentation   ######
######                          ######
######################################
*/


namespace FlatEngine { namespace FlatGui {

	// For window styles
	float childPadding = 8;
	ImVec4 outerWindowColor = ImVec4(float(0.15), float(0.15), float(0.15), float(1));
	ImVec4 innerWindowColor = ImVec4(float(0.19), float(0.19), float(0.19), float(1));
	ImVec4 singleItemColor = ImVec4(float(0.15), float(0.15), float(0.15), float(1));
	ImVec4 singleItemDark = ImVec4(float(0.09), float(0.09), float(0.13), float(1));
	
	// Icons
	std::unique_ptr<Texture> expandIcon(new Texture());
	std::unique_ptr<Texture> expandFlippedIcon(new Texture());
	std::unique_ptr<Texture> trashIcon(new Texture());
	std::unique_ptr<Texture> transformArrow(new Texture());
	std::unique_ptr<Texture> cameraTexture(new Texture());


	int maxSpriteLayers = 55;


	// Frame Counter
	int framesDrawn = 0;

	// Scene view
	// The multiplier for gridstep. Used to convert grid space values to pixel values. ie. 2 grid squares = 2 * 10 = 20px.
	float gridStep = 10;
	float SCENE_VIEWPORT_WIDTH = 600;
	float SCENE_VIEWPORT_HEIGHT = 400;
	float DYNAMIC_VIEWPORT_WIDTH = 600;
	float DYNAMIC_VIEWPORT_HEIGHT = 400;
	bool _firstSceneRenderPass = true;
	bool _sceneHasBeenSet = false;

	// Settings
	// Eventually move these over to Settings() once there is enough to save between sessions //
	int iconTransparency = 100;
	bool _clearBufferEveryFrame = true;

	float spriteScaleMultiplier = 0.2f;

	// Game view default values
	float GAME_VIEWPORT_WIDTH = 600;
	float GAME_VIEWPORT_HEIGHT = 400;
	float xGameCenter = 600/2;
	float yGameCenter = 400/2;

	ImVec2 worldCenterPoint = ImVec2(0, 0);


	using ComponentTypes = Component::ComponentTypes;


	void FlatEngine::FlatGui::SetupImGui()
	{
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsLight();

		ImGuiStyle& style = ImGui::GetStyle();

		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		style.WindowPadding = { 5.0f, 5.0f };
		style.DockingSeparatorSize = 1;
		style.SeparatorTextAlign = ImVec2(0.5f, 0.0f);
		style.SeparatorTextBorderSize = 1;

		ImGui_ImplSDL2_InitForSDLRenderer(Window::window, Window::renderer);
		ImGui_ImplSDLRenderer2_Init(Window::renderer);


		// If Release
		if (FlatEngine::_isDebugMode == false)
		{
			// Load in what is currently in SavedScenes.json
			FlatEngine::LoadScene("MainMenu.json");

			// Remove the reference to the imgui.ini file for layout since we only need that in Engine mode and
			// we don't want to have to include it in the final release build anyway.
			io.IniFilename = NULL;

			// Set fullscreen here for now
			Window::SetFullscreen(true);
		}
		else 
			FlatEngine::CreateNewScene();
		//FlatEngine::sceneManager->LoadScene("SavedScenes.json");
	}


	void FlatEngine::FlatGui::Render(bool& quit)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			ImGui_ImplSDL2_ProcessEvent(&event);
			if (event.type == SDL_QUIT)
				quit = true;
			if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(Window::window))
				quit = true;
		}

		// Start the Dear ImGui frame
		ImGui_ImplSDLRenderer2_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		//Create dockable background space for all viewports
		ImGui::DockSpaceOverViewport();

		//Add viewport(s)
		// 
		// If Release
		if (FlatEngine::_isDebugMode == false)
		{
			// Just Add GameView
			FlatEngine::FlatGui::Game_RenderView();
		}
		// Else add FlatEngine viewports
		else
			FlatEngine::FlatGui::AddViewports();

		// Rendering
		ImVec4 clear_color = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		ImGui::Render();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		SDL_RenderSetScale(Window::renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
		SDL_SetRenderDrawColor(Window::renderer, (Uint8)(clear_color.x * 255), (Uint8)(clear_color.y * 255), (Uint8)(clear_color.z * 255), (Uint8)(clear_color.w * 255));
		SDL_RenderClear(Window::renderer);
		ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
		SDL_RenderPresent(Window::renderer);
	}


	void FlatEngine::FlatGui::AddViewports()
	{
		FlatEngine::FlatGui::RenderToolbar();
		FlatEngine::FlatGui::RenderHierarchy();
		FlatEngine::FlatGui::RenderInspector();
		FlatEngine::FlatGui::Game_RenderView();
		FlatEngine::FlatGui::Scene_RenderView();
		FlatEngine::FlatGui::RenderLog();
	}


	void FlatEngine::FlatGui::RenderToolbar()
	{
		ImGui::Begin("Toolbar");

		if (ImGui::Button("Start"))
		{
			FlatEngine::StartGameLoop();
		}
		ImGui::SameLine(0, 5);
		if (ImGui::Button("Pause"))
		{
			FlatEngine::PauseGameLoop();
		}
		ImGui::SameLine(0, 5);
		if (ImGui::Button("Stop"))
		{
			FlatEngine::StopGameLoop();
		}

		if (FlatEngine::GameLoopStarted())
		{
			framesDrawn++;
			int ellapsedTime = FlatEngine::GetEllapsedGameTime();
			std::string timeString = "Ellapsed Time: " + std::to_string(ellapsedTime / 1000);
			std::string fpsString = "Average FPS: " + std::to_string(FlatEngine::GetAverageFps());
			std::string deltaTimeString = "deltaTime: " + std::to_string(FlatEngine::GetDeltaTime());

			ImGui::SameLine(0, 5);
			ImGui::Text(timeString.c_str());
			ImGui::SameLine(0, 5);
			ImGui::Text(fpsString.c_str());
			ImGui::SameLine(0, 5);
			ImGui::Text(deltaTimeString.c_str());
		}

		// Icon transparency slider
		ImGui::SliderInt("Scene View Icon Transparency", &iconTransparency, 0, 255, "%d");

		ImGui::End();
	}


	void FlatEngine::FlatGui::RenderHierarchy()
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

		std::string sceneText = "Scene: ";
		static char filename[1024] = "Test.json";
		ImGuiInputTextFlags flags = ImGuiInputTextFlags_::ImGuiInputTextFlags_AutoSelectAll;

		// Edit field
		ImGui::Text(sceneText.c_str());
		ImGui::SameLine(0, 5);
		ImGui::InputText("##SceneName", filename, IM_ARRAYSIZE(filename), flags);

		// MOVE THESE OUT INTO A FILE TOP BAR EVENTUALLY
		if (ImGui::Button("Save Scene"))
		{
			FlatEngine::SaveScene(FlatEngine::GetLoadedScene(), filename);
		}
		ImGui::SameLine(0, 5);
		if (ImGui::Button("Load Scene"))
		{
			FlatEngine::sceneManager->LoadScene(filename);
		}
		if (ImGui::Button("Create New Game Object"))
		{
			FlatEngine::CreateGameObject();
		}

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
						FlatEngine::FlatGui::AddObjectWithChild(currentObject, charName, node_clicked, queuedForDelete);
					else
						FlatEngine::FlatGui::AddObjectWithoutChild(currentObject, charName, node_clicked, queuedForDelete);

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
	void FlatEngine::FlatGui::AddObjectWithChild(std::shared_ptr<GameObject> currentObject, const char* charName, int& node_clicked, long &queuedForDelete)
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
					FlatEngine::FlatGui::AddObjectWithChild(child, childName, node_clicked, queuedForDelete);
				else
					FlatEngine::FlatGui::AddObjectWithoutChild(child, childName, node_clicked, queuedForDelete);
			}

			ImGui::TreePop(); // TreeNode Closer
		}

		// Pop Item Spacing
		ImGui::PopStyleVar();
	}

	// Helper function for Hierarchy child rendering
	void FlatEngine::FlatGui::AddObjectWithoutChild(std::shared_ptr<GameObject> currentObject, const char* charName, int& node_clicked, long &queuedForDelete)
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


	void FlatEngine::FlatGui::RenderInspector()
	{
		ImGui::Begin("Inspector Window");
		long focusedObjectID = FlatEngine::GetFocusedGameObjectID();

		ImGui::PushStyleColor(ImGuiCol_ChildBg, outerWindowColor);
		ImGuiChildFlags padding_child_flags = ImGuiChildFlags_::ImGuiChildFlags_AlwaysUseWindowPadding;
		ImGui::BeginChild("Inspector Background", ImVec2(0,0), padding_child_flags);

		if (focusedObjectID != -1)
		{
			// Get focused GameObject
			std::shared_ptr<GameObject> focusedObject = FlatEngine::GetObjectById(focusedObjectID);
			// Name editing
			std::string nameLabel = "Name: ";
			char newName[1024];
			strcpy_s(newName, focusedObject->GetName().c_str());
			ImGuiInputTextFlags flags = ImGuiInputTextFlags_::ImGuiInputTextFlags_AutoSelectAll;

			// Edit field
			ImGui::Text(nameLabel.c_str());
			ImGui::SameLine(0, 5);
			if (ImGui::InputText("##GameObject Name", newName, IM_ARRAYSIZE(newName), flags))
				focusedObject->SetName(newName);

			bool _isActive = focusedObject->IsActive();

			if (ImGui::Checkbox("Active", &_isActive))
				focusedObject->SetActive(_isActive);

			// Components section
			ImGui::Text("Components:");

			std::vector<std::shared_ptr<Component>> components = focusedObject->GetComponents();

			if (components.size() > 0)
			{
				// Get Expander and Trashcan icons for components
				// Expanders
				expandIcon->loadFromFile("assets/images/Expand.png");
				SDL_Texture* expandTexture = expandIcon->getTexture();
				expandFlippedIcon->loadFromFile("assets/images/ExpandFlipped.png");
				SDL_Texture* expandFlippedTexture = expandFlippedIcon->getTexture();
				float expandWidth = (float)expandIcon->getWidth();
				float expandHeight = (float)expandIcon->getHeight();
				// Trashcan
				trashIcon->loadFromFile("assets/images/Trashcan.png");
				SDL_Texture* trashTexture = trashIcon->getTexture();
				float trashWidth = (float)trashIcon->getWidth();
				float trashHeight = (float)trashIcon->getHeight();
				long queuedForDelete = -1;

				// Flags for child padding and dimensions
				ImGuiChildFlags child_flags = ImGuiChildFlags_::ImGuiChildFlags_AlwaysUseWindowPadding | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_::ImGuiChildFlags_AlwaysAutoResize;

				for (int i = 0; i < components.size(); i++)
				{
					if (components[i] != nullptr)
					{
						// Is Collapsed
						bool _isCollapsed = components[i]->IsCollapsed();

						// Component Name
						ImGui::Separator();
						std::string componentType = components[i]->GetTypeString();
						// Get Component ID in to keep the child unique
						std::string componentID = componentType + std::to_string(components[i]->GetID());

						// Begin Component Child
						ImGui::PushStyleColor(ImGuiCol_ChildBg, innerWindowColor);
						ImGui::BeginChild(componentID.c_str(), ImVec2(0, 0), child_flags);
						ImGui::PopStyleColor();
						ImGui::Text(componentType.c_str());

						// Same Line
						ImGui::SameLine(ImGui::GetContentRegionMax().x - (expandWidth + childPadding + 20), 5); // Add the expander icon on the same line

						// Pushes	
						ImGui::PushItemWidth(-1.0f);
						ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1.0f, 1.0f));
						ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 1, 1, 0));

						// Expand Icon Button
						ImVec2 expandSize = ImVec2(expandWidth, expandHeight);
						ImVec2 uv0 = ImVec2(0.0f, 0.0f);
						ImVec2 uv1 = ImVec2(1.0f, 1.0f);
						ImVec4 bg_col = ImVec4(1.0f, 1.0f, 1.0f, 0.0f);
						ImVec4 expand_tint_col = ImVec4(1.0, 1.0, 1.0, 1.0f);
						std::string expandID = "##expandIcon-" + i;
						// Trashcan Icon Button
						ImVec2 trashSize = ImVec2(trashWidth, trashHeight);
						ImVec4 trash_tint_col = ImVec4(1.0, 1.0, 1.0, 1.0f);
						std::string trashcanID = "##trashIcon-" + i;

						// Trash Can Icon for removing Component from Focused Object
						if (ImGui::ImageButton(trashcanID.c_str(), trashTexture, trashSize, uv0, uv1, bg_col, trash_tint_col))
							queuedForDelete = components[i]->GetID();
								
						// Set Mouse Cursor
						if (ImGui::IsItemHovered())
							ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_Hand);

						ImGui::SameLine(0, 5);

						// Draw Expand Icon for expanding/collapsing current component information
						if (_isCollapsed)
						{
							if (ImGui::ImageButton(expandID.c_str(), expandTexture, expandSize, uv0, uv1, bg_col, expand_tint_col))
								components[i]->SetCollapsed(!_isCollapsed);
							// Set Mouse Cursor
							if (ImGui::IsItemHovered())
								ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_Hand);
						}
						else
						{
							if (ImGui::ImageButton(expandID.c_str(), expandFlippedTexture, expandSize, uv0, uv1, bg_col, expand_tint_col))
								components[i]->SetCollapsed(!_isCollapsed);
							// Set Mouse Cursor
							if (ImGui::IsItemHovered())
								ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_Hand);
						}


						// Pops
						ImGui::PopStyleColor();
						ImGui::PopStyleVar();
						ImGui::PopItemWidth();

						
						// Render Component
						//
						if (!_isCollapsed)
						{
							// Component Data - Give it background color and padding
							// Push
							ImGui::PushStyleColor(ImGuiCol_ChildBg, singleItemColor);
							ImGui::BeginChild(componentType.c_str(), ImVec2(0, 0), child_flags);

							// Make full width Push
							ImGui::PushItemWidth(-1.0f);

							if (componentType == "Transform")
							{
								// Position, scale, and rotation of transform
								std::shared_ptr<FlatEngine::Transform> transform = std::static_pointer_cast<FlatEngine::Transform>(components[i]);
								Vector2 position = transform->GetPosition();
								float xPos = position.x;
								float yPos = position.y;
								Vector2 scale = transform->GetScale();
								float scaleX = scale.x;
								float scaleY = scale.y;
								float rotation = transform->GetRotation();
								static ImGuiSliderFlags flags = ImGuiSliderFlags_::ImGuiSliderFlags_None;

								// Push Item Width
								ImGui::PushItemWidth(ImGui::GetContentRegionMax().x / 3 - 5);

								// Drags for position editing
								//
								// Render Text for Positions + Rotation
								ImGui::Text("xPos:");
								ImGui::SameLine(ImGui::GetContentRegionMax().x / 3 + 5, 0);
								ImGui::Text("yPos:");
								ImGui::SameLine((ImGui::GetContentRegionMax().x / 3 * 2) + 5, 0);
								ImGui::Text("Rotation:");

								// Render Drags for Positions + Rotation
								ImGui::DragFloat("##xPos", &xPos, 0.5f, -FLT_MAX, -FLT_MAX, "%.3f", flags);
								if (ImGui::IsItemHovered())
									ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_ResizeEW);
								ImGui::SameLine(0, 5);
								ImGui::DragFloat("##yPos", &yPos, 0.5f, -FLT_MAX, -FLT_MAX, "%.3f", flags);
								// Set cursor type
								if (ImGui::IsItemHovered())
									ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_ResizeEW);
								ImGui::SameLine(0, 5);
								ImGui::DragFloat("##rotation", &rotation, 1.0f, -360, 360, "%.3f", flags);
								// Set cursor type
								if (ImGui::IsItemHovered())
									ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_ResizeEW);

								// Assign the new slider values to the transforms position
								transform->SetPosition(Vector2(xPos, yPos));
								transform->SetRotation(rotation);

								// Render Drags for scale of transform
								// Push
								ImGui::PushItemWidth(ImGui::GetContentRegionMax().x / 2 - 5);

								// Render text for scales
								ImGui::Text("Scale x:");
								ImGui::SameLine(ImGui::GetContentRegionMax().x / 2 + 5, 0);
								ImGui::Text("Scale y:");

								// Render Drags for scales
								ImGui::DragFloat("##xScale", &scaleX, 0.05f, 0, -FLT_MAX, "%.3f", flags);
								// Set cursor type
								if (ImGui::IsItemHovered())
									ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_ResizeEW);
								ImGui::SameLine(0, 5);
								ImGui::DragFloat("##yScale", &scaleY, 0.05f, -FLT_MAX, -FLT_MAX, "%.3f", flags);
								// Set cursor type
								if (ImGui::IsItemHovered())
									ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_ResizeEW);

								// Pop Width Setting
								ImGui::PopItemWidth();

								// Assign the new slider values to the sprites pivotPoint
								transform->SetScale(Vector2(scaleX, scaleY));
							}
							else if (componentType == "Sprite")
							{
								// Sprite path and texture dimension variables
								std::shared_ptr<FlatEngine::Sprite> sprite = std::static_pointer_cast<FlatEngine::Sprite>(components[i]);
								std::string path = sprite->GetPath();
								char newPath[1024];
								strcpy_s(newPath, path.c_str());
								float textureWidth = sprite->GetTextureWidth();
								float textureHeight = sprite->GetTextureHeight();
								int renderOrder = sprite->GetRenderOrder();

								// Sprite Path Strings
								std::string pathString = "Path: ";
								std::string textureWidthString = "Texture width: " + std::to_string(textureWidth);
								std::string textureHeightString = "Texture height: " + std::to_string(textureHeight);

								// Render Sprite Path
								ImGui::Text(pathString.c_str());
								ImGui::SameLine(0, 5);
								if (ImGui::InputText("##spritePath", newPath, IM_ARRAYSIZE(newPath), flags))
									sprite->SetTexture(newPath);
								ImGui::Text(textureWidthString.c_str());
								ImGui::Text(textureHeightString.c_str());

								// Render Order
								ImGui::Text("Render order: ");
								if (ImGui::SliderInt("Sprite Render Order", &renderOrder, 0, maxSpriteLayers, "%d"))
									sprite->SetRenderOrder(renderOrder);

								// Push Item Width Setting
								ImGui::PushItemWidth(ImGui::GetContentRegionMax().x / 2 - 5);

								// Sprite offset variables
								Vector2 offset = sprite->GetOffset();
								float xOffset = offset.x;
								float yOffset = offset.y;
								ImGuiSliderFlags flags = ImGuiSliderFlags_::ImGuiSliderFlags_None;

								// Render Drags for offset of texture editing
								ImGui::Text("xOffset:");
								ImGui::SameLine(ImGui::GetContentRegionMax().x / 2 + 5, 0);
								ImGui::Text("yOffset:");
								ImGui::DragFloat("##xOffset", &xOffset, 0.5f, -FLT_MAX, -FLT_MAX, "%.3f", flags);
								// Set cursor type
								if (ImGui::IsItemHovered())
									ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_ResizeEW);
								ImGui::SameLine(0, 5);
								ImGui::DragFloat("##yOffset", &yOffset, 0.5f, -FLT_MAX, -FLT_MAX, "%.3f", flags);
								// Set cursor type
								if (ImGui::IsItemHovered())
									ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_ResizeEW);

								// Pop Width Setting
								ImGui::PopItemWidth();

								// Assign the new slider values to the sprites pivotPoint
								sprite->SetOffset(Vector2(xOffset, yOffset));
							}
							else if (componentType == "Camera")
							{
								// Camera position and aspect ratio
								std::shared_ptr<FlatEngine::Camera> camera = std::static_pointer_cast<FlatEngine::Camera>(components[i]);
								float width = camera->GetWidth();
								float height = camera->GetHeight();
								bool _isPrimary = camera->IsPrimary();
								float zoom = camera->GetZoom();
								ImVec4 frustrumColor = camera->GetFrustrumColor();

								// Push Item Width Setting
								ImGui::PushItemWidth(ImGui::GetContentRegionMax().x / 2 - 5);

								// Drags for camera width and height editing
								ImGui::Text("Camera width:");
								ImGui::SameLine(ImGui::GetContentRegionMax().x / 2 + 5, 0);
								ImGui::Text("Camera height:");
								ImGui::DragFloat("##CameraWidth", &width, 0.5f, 0, -FLT_MAX, "%.3f", flags);
								// Set cursor type
								if (ImGui::IsItemHovered())
									ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_ResizeEW);
								ImGui::SameLine(0, 5);
								ImGui::DragFloat("##CameraHeight", &height, 0.5f, 0, -FLT_MAX, "%.3f", flags);
								// Set cursor type
								if (ImGui::IsItemHovered())
									ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_ResizeEW);

								// Pop Width Setting
								ImGui::PopItemWidth();

								// Assign the new slider values
								camera->SetDimensions(width, height);

								// Zoom slider
								ImGui::Text("Camera zoom:");
								ImGui::SliderFloat("##Zoom", &zoom, 1.0f, 100.0f, "%.3f");
								camera->SetZoom(zoom);

								// Frustrum color picker
								ImVec4 color = ImVec4(frustrumColor.x / 255.0f, frustrumColor.y / 255.0f, frustrumColor.z / 255.0f, frustrumColor.w / 255.0f);

								ImGui::ColorEdit4("##FrustrumColor", (float*)&color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
								ImGui::SameLine(0, 5);
								ImGui::Text("Frustrum color");

								// Set frustrum color
								camera->SetFrustrumColor(ImVec4(color.x * 255.0f, color.y * 255.0f, color.z * 255.0f, color.w * 255.0f));

								// _isPrimaryCamera checkbox
								// Before allowing this camera to be set as primary, we need to ensure it has a transform component
								if (focusedObject->GetComponent(ComponentTypes::Transform) != nullptr)
								{
									if (ImGui::Checkbox("Is Primary Camera", &_isPrimary))
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
									if (ImGui::Checkbox("Is Primary Camera", &temp))
										FlatEngine::LogString("FlatGui::RenderInspector() - Attempt to set Camera component as primary failed: No Transform component found...");
									temp = false;
									ImGui::TextWrapped("*A Camera Component must be coupled with a Transform Component to be set as the primary camera.*");
								}

								camera->SetPrimaryCamera(_isPrimary);
							}
							else if (componentType == "Script")
							{
								std::shared_ptr<FlatEngine::ScriptComponent> script = std::static_pointer_cast<FlatEngine::ScriptComponent>(components[i]);
								std::string path = script->GetAttachedScript();
								bool _isActive = script->IsActive();

								// For path editing
								char newPath[1024];
								strcpy_s(newPath, path.c_str());
								std::string pathString = "Name: ";
								ImGui::Text(pathString.c_str());
								ImGui::SameLine(0, 5);
								ImGuiSliderFlags flags = ImGuiSliderFlags_::ImGuiSliderFlags_None;
								std::string inputId = "##scriptName_" + std::to_string(script->GetID());
								if (ImGui::InputText(inputId.c_str(), newPath, IM_ARRAYSIZE(newPath), flags))
									script->SetAttachedScript(newPath);

								// _isActive checkbox
								std::string checkboxId = "Active##" + std::to_string(script->GetID());
								ImGui::Checkbox(checkboxId.c_str(), &_isActive);
								script->SetActive(_isActive);
							}
							else if (componentType == "Button")
							{
								std::shared_ptr<FlatEngine::Button> button = std::static_pointer_cast<FlatEngine::Button>(components[i]);			
								bool _isActive = button->IsActive();
								float activeWidth = button->GetActiveWidth();
								float activeHeight = button->GetActiveHeight();
								Vector2 activeOffset = button->GetActiveOffset();
								int activeLayer = button->GetActiveLayer();

								// Active Checkbox
								ImGui::Checkbox("Active:", &_isActive);
								button->SetActive(_isActive);

								// Push Item Width Setting
								ImGui::PushItemWidth(ImGui::GetContentRegionMax().x / 2 - 5);

								// Drags for active width, height and offsets
								ImGui::Text("Active width:");
								ImGui::SameLine(ImGui::GetContentRegionMax().x / 2, 5);
								ImGui::Text("Active height:");

								ImGui::DragFloat("##activeWidth", &activeWidth, 0.5f, 0.1f, -FLT_MAX, "%.3f", flags);
								// Set cursor type
								if (ImGui::IsItemHovered())
									ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_ResizeEW);
								ImGui::SameLine(0, 5);
								ImGui::DragFloat("##activeHeight", &activeHeight, 0.5f, 0.1f, -FLT_MAX, "%.3f", flags);
								// Set cursor type
								if (ImGui::IsItemHovered())
									ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_ResizeEW);

								ImGui::Text("Active offset x:");
								ImGui::SameLine(ImGui::GetContentRegionMax().x / 2, 5);
								ImGui::Text("Active offset y:");

								ImGui::DragFloat("##activeoffsetx", &activeOffset.x, 0.1f, -FLT_MAX, -FLT_MAX, "%.3f", flags);
								// Set cursor type
								if (ImGui::IsItemHovered())
									ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_ResizeEW);
								ImGui::SameLine(0, 5);
								ImGui::DragFloat("##activeoffsety", &activeOffset.y, 0.1f, -FLT_MAX, -FLT_MAX, "%.3f", flags);
								// Set cursor type
								if (ImGui::IsItemHovered())
									ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_ResizeEW);

								// Assign the new slider values
								button->SetActiveDimensions(activeWidth, activeHeight);
								button->SetActiveOffset(activeOffset);

								// Pop Width Setting
								ImGui::PopItemWidth();
							}
							else if (componentType == "Canvas")
							{
								std::shared_ptr<FlatEngine::Canvas> canvas = std::static_pointer_cast<FlatEngine::Canvas>(components[i]);

								// Retrieve Canvas values
								float canvasWidth = canvas->GetWidth();
								float canvasHeight = canvas->GetHeight();
								int layerNumber = canvas->GetLayerNumber();
								bool _blocksLayers = canvas->GetBlocksLayers();
								std::vector<std::shared_ptr<Button>> canvasButtons = canvas->GetButtons();

								// _BlocksLayers Checkbox
								ImGui::Checkbox("Blocks Layers:", &_blocksLayers);
								canvas->SetBlocksLayers(_blocksLayers);

								// Text
								ImGui::Text("Canvas Layer: ");

								// Set Layer Number Slider
								ImGui::SliderInt("##layerNumber", &layerNumber, 0, 20, "%d");
								// Set cursor type
								if (ImGui::IsItemHovered())
									ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_Hand);
								
								// Set Canvas Layer to the new value
								canvas->SetLayerNumber(layerNumber);

								// Text
								ImGui::Text("Canvas width: ");
								ImGui::SameLine(ImGui::GetContentRegionMax().x / 2, 5);
								ImGui::Text("Canvas height: ");

								// Push Item Width Setting
								ImGui::PushItemWidth(ImGui::GetContentRegionMax().x / 2 - 5);

								ImGui::DragFloat("##canvasWidth", &canvasWidth, 0.5f, 0.1f, -FLT_MAX, "%.3f", flags);
								// Set cursor type
								if (ImGui::IsItemHovered())
									ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_ResizeEW);
								ImGui::SameLine(0, 5);
								ImGui::DragFloat("##canvasHeight", &canvasHeight, 0.5f, 0.1f, -FLT_MAX, "%.3f", flags);
								// Set cursor type
								if (ImGui::IsItemHovered())
									ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_ResizeEW);

								// Assign new values to Canvas
								canvas->SetDimensions(canvasWidth, canvasHeight);


								ImGui::PopItemWidth();
							}
							else if (componentType == "Animation")
							{
								std::shared_ptr<FlatEngine::Animation> animation = std::static_pointer_cast<FlatEngine::Animation>(components[i]);

								// Retrieve Animation values
								float ticksPerFrame = animation->GetTicksPerFrame();
								std::vector<std::shared_ptr<GameObject>> frames = animation->GetFrames();

								// Text
								ImGui::Text("Ticks per frame: ");

								// Set Ticks Per Frame
								ImGui::SliderFloat("##ticksPerFrame", &ticksPerFrame, 0.5f, -FLT_MAX, "%.3f");
								animation->SetTicksPerFrame(ticksPerFrame);

								// Set cursor type
								if (ImGui::IsItemHovered())
									ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_Hand);

								// Total Frames Text
								std::string totalFrames = "Total frames: " + std::to_string(frames.size());
								ImGui::Text(totalFrames.c_str());

								// Add Frame Button
								if (ImGui::Button("Add Frame"))
									animation->AddFrame();

								// Add Frame Button
								if (ImGui::Button("Play Animation"))
									animation->Play();

								// Set cursor type
								if (ImGui::IsItemHovered())
									ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_Hand);
							}
							else if (componentType == "Audio")
							{
								std::shared_ptr<FlatEngine::Audio> audio = std::static_pointer_cast<FlatEngine::Audio>(components[i]);

								// Retrieve Audio values
								std::string path = audio->GetPath();
								bool _isMusic = audio->IsMusic();

								// For path editing
								char newPath[1024];
								strcpy_s(newPath, path.c_str());
								std::string pathString = "Path: ";
								ImGui::Text(pathString.c_str());
								ImGui::SameLine(0, 5);

								// On Path editing, reload the effect
								ImGuiSliderFlags flags = ImGuiSliderFlags_::ImGuiSliderFlags_None;
								std::string inputId = "##audioPath_" + std::to_string(audio->GetID());
								if (ImGui::InputText(inputId.c_str(), newPath, IM_ARRAYSIZE(newPath), flags))
								{
									if (_isMusic)
										audio->LoadMusic(newPath);
									else
										audio->LoadEffect(newPath);

									audio->SetPath(newPath);
								}
								
								// _isMusic checkbox, also reload the effect as music or chunk
								std::string checkboxId = "Is Music##" + std::to_string(audio->GetID());
								if (ImGui::Checkbox(checkboxId.c_str(), &_isMusic))
								{
									if (_isMusic)
										audio->LoadMusic(newPath);
									else
										audio->LoadEffect(newPath);

									audio->SetIsMusic(_isMusic);
								}
								
								// Set cursor type
								if (ImGui::IsItemHovered())
									ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_Hand);

								// Play Audio
								if (ImGui::Button("Play"))
									audio->Play();
								ImGui::SameLine(0, 5);
								// Pause Audio
								if (ImGui::Button("Pause"))
									audio->Pause();
								ImGui::SameLine(0, 5);
								// Stop Audio
								if (ImGui::Button("Stop"))
									audio->Stop();
							}
							else if (componentType == "Text")
							{
								// Sprite path and texture dimension variables
								std::shared_ptr<FlatEngine::Text> text = std::static_pointer_cast<FlatEngine::Text>(components[i]);

	
								// Text String Variables
								std::string textText = text->GetText();
								char newTextString[1024];
								strcpy_s(newTextString, textText.c_str());
								// Text Strings
								std::string textString = "Text: ";
								// Render Text String
								ImGui::Text(textString.c_str());
								ImGui::SameLine(0, 5);
								if (ImGui::InputText("##textString", newTextString, IM_ARRAYSIZE(newTextString), flags))
								{
									text->SetText(newTextString);
									text->LoadText();
								}
									

								// Other variables
								std::shared_ptr<Texture> texture = text->GetTexture();
								float textureWidth = (float)texture->getWidth();
								float textureHeight = (float)texture->getHeight();
								int renderOrder = text->GetRenderOrder();
								SDL_Color color = text->GetColor();

								// Font Path Variables
								std::string fontPath = text->GetFontPath();
								char newPath[1024];
								strcpy_s(newPath, fontPath.c_str());
								// Font Path Strings
								std::string pathString = "Font path: ";
								// Render Font Path
								ImGui::Text(pathString.c_str());
								ImGui::SameLine(0, 5);
								if (ImGui::InputText("##fontPath", newPath, IM_ARRAYSIZE(newPath), flags))
									text->SetFontPath(newPath);

								// Text Width and Height
								std::string textureWidthString = "Text width: " + std::to_string(textureWidth);
								std::string textureHeightString = "Text height: " + std::to_string(textureHeight);
								ImGui::Text(textureWidthString.c_str());
								ImGui::Text(textureHeightString.c_str());

								// Push Item Width Setting
								ImGui::PushItemWidth(ImGui::GetContentRegionMax().x / 2 - 5);

								// Text offset variables
								Vector2 offset = text->GetOffset();
								float xOffset = offset.x;
								float yOffset = offset.y;
								ImGuiSliderFlags flags = ImGuiSliderFlags_::ImGuiSliderFlags_None;

								// Render Drags for offset of texture editing
								ImGui::Text("xOffset:");
								ImGui::SameLine(ImGui::GetContentRegionMax().x / 2 + 5, 0);
								ImGui::Text("yOffset:");
								ImGui::DragFloat("##xOffset", &xOffset, 0.5f, -FLT_MAX, -FLT_MAX, "%.3f", flags);
								// Set cursor type
								if (ImGui::IsItemHovered())
									ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_ResizeEW);
								ImGui::SameLine(0, 5);
								ImGui::DragFloat("##yOffset", &yOffset, 0.5f, -FLT_MAX, -FLT_MAX, "%.3f", flags);
								// Set cursor type
								if (ImGui::IsItemHovered())
									ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_ResizeEW);
								// Set new offset values
								text->SetOffset(Vector2(xOffset, yOffset));

								// Pop Width Setting
								ImGui::PopItemWidth();

								// Render Order
								ImGui::Text("Render order: ");
								if (ImGui::SliderInt("Text Render Order", &renderOrder, 0, maxSpriteLayers, "%d"))
									text->SetRenderOrder(renderOrder);

								// Push Item Width Setting
								ImGui::PushItemWidth(ImGui::GetContentRegionMax().x / 2 - 5);

								// Pop Width Setting
								ImGui::PopItemWidth();
							}

							// Pops
							ImGui::PopItemWidth();
							ImGui::PopStyleColor();

							ImGui::EndChild();
						}
					}

					ImGui::EndChild();

					if (i == components.size() - 1)
						ImGui::Separator();
				}

				if (queuedForDelete != -1)
				{
					focusedObject->RemoveComponent(queuedForDelete);
					queuedForDelete = -1;
				}
			}

			// For checking if SceneObject has these components yet
			std::shared_ptr<FlatEngine::Component> transformComponent = focusedObject->GetComponent(ComponentTypes::Transform);
			std::shared_ptr<FlatEngine::Component> spriteComponent = focusedObject->GetComponent(ComponentTypes::Sprite);
			std::shared_ptr<FlatEngine::Component> cameraComponent = focusedObject->GetComponent(ComponentTypes::Camera);
			std::shared_ptr<FlatEngine::Component> canvasComponent = focusedObject->GetComponent(ComponentTypes::Canvas);
			std::shared_ptr<FlatEngine::Component> animationComponent = focusedObject->GetComponent(ComponentTypes::Animation);

			// Render the Adding Components button
			ImGui::Button("Add Component", ImVec2(ImGui::GetContentRegionMax().x, 0));
			if (ImGui::BeginPopupContextItem("##AddComponent", ImGuiPopupFlags_MouseButtonLeft)) // <-- use last item id as popup id
			{
				ImGui::PushStyleColor(ImGuiCol_FrameBg, outerWindowColor);
				ImGui::BeginListBox("##SceneObjects", ImVec2(220, 100));
				ImGui::PopStyleColor();

				// Push button bg color style
				ImGui::PushStyleColor(ImGuiCol_Button, innerWindowColor);

				// Add all the component types you can add to this GameObject
				//
				if (transformComponent == nullptr)
					if (ImGui::Button("Transform", ImVec2(ImGui::GetContentRegionMax().x, 0)))
					{
						focusedObject->AddComponent(ComponentTypes::Transform);
						ImGui::CloseCurrentPopup();
					}

				if (spriteComponent == nullptr)
					if (ImGui::Button("Sprite", ImVec2(ImGui::GetContentRegionMax().x, 0)))
					{
						focusedObject->AddComponent(ComponentTypes::Sprite);
						ImGui::CloseCurrentPopup();
					}

				if (ImGui::Button("Button", ImVec2(ImGui::GetContentRegionMax().x, 0)))
				{
					focusedObject->AddComponent(ComponentTypes::Button);
					ImGui::CloseCurrentPopup();
				}

				if (cameraComponent == nullptr)
					if (ImGui::Button("Camera", ImVec2(ImGui::GetContentRegionMax().x, 0)))
					{
						focusedObject->AddComponent(ComponentTypes::Camera);
						ImGui::CloseCurrentPopup();
					}

				if (canvasComponent == nullptr)
					if (ImGui::Button("Canvas", ImVec2(ImGui::GetContentRegionMax().x, 0)))
					{
						focusedObject->AddComponent(ComponentTypes::Canvas);
						ImGui::CloseCurrentPopup();
					}
				
				if (animationComponent == nullptr)
					if (ImGui::Button("Animation", ImVec2(ImGui::GetContentRegionMax().x, 0)))
					{
						focusedObject->AddComponent(ComponentTypes::Animation);
						ImGui::CloseCurrentPopup();
					}
				
				if (ImGui::Button("Audio", ImVec2(ImGui::GetContentRegionMax().x, 0)))
				{
					focusedObject->AddComponent(ComponentTypes::Audio);
					ImGui::CloseCurrentPopup();
				}

				if (ImGui::Button("Text", ImVec2(ImGui::GetContentRegionMax().x, 0)))
				{
					focusedObject->AddComponent(ComponentTypes::Text);
					ImGui::CloseCurrentPopup();
				}

				if (ImGui::Button("Script", ImVec2(ImGui::GetContentRegionMax().x, 0)))
				{
					focusedObject->AddComponent(ComponentTypes::Script);
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndListBox();

				// Pop button bg color styles
				ImGui::PopStyleColor();
				ImGui::EndPopup();
			}

			if (ImGui::Button("Delete GameObject"))
			{
				// Unfocus GameObject first
				int tempID = focusedObjectID;
				FlatEngine::SetFocusedGameObjectID(-1);
				FlatEngine::DeleteGameObject(tempID);
			}
		}
		ImGui::PopStyleColor();
		ImGui::EndChild();

		ImGui::End();
	}


	void FlatEngine::FlatGui::Game_RenderView()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

		ImGuiWindowFlags flags = ImGuiWindowFlags_None;


		// If Release - Make GameView full screen and disable tab decoration and resizing
		if (FlatEngine::_isDebugMode == false)
		{
			// Get InputOutput
			ImGuiIO& inputOutput = ImGui::GetIO();
			float xSize = inputOutput.DisplaySize.x;
			float ySize = inputOutput.DisplaySize.y;
			ImGui::SetNextWindowSize(ImVec2(xSize, ySize));
			ImGui::SetNextWindowPos(ImVec2(0, 0));
			flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize;
		}

		bool _open = true;

		ImGui::Begin("Game View", &_open, flags);

		static bool opt_enable_context_menu = true;

		ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();  
		ImVec2 canvas_sz = ImGui::GetContentRegionAvail();
		if (canvas_sz.x < 50.0f) canvas_sz.x = 50.0f;
		if (canvas_sz.y < 50.0f) canvas_sz.y = 50.0f;
		ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);

		// Set viewport dimensions for rendering objects in game view. We want this to always be centered in game view so we can get it every frame.
		GAME_VIEWPORT_WIDTH = canvas_p1.x - canvas_p0.x + 1;
		GAME_VIEWPORT_HEIGHT = canvas_p1.y - canvas_p0.y + 1;

		// This will catch our interactions
		ImGui::InvisibleButton("GameViewCanvas", canvas_sz, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
		const bool is_hovered = ImGui::IsItemHovered(); // Hovered
		const bool is_active = ImGui::IsItemActive();   // Held

		// Render GameObjects in game view
		FlatEngine::FlatGui::Game_RenderObjects(canvas_p0, canvas_sz);

		ImGui::End();

		// Reset WindowRounding
		ImGui::PopStyleVar();
		// Reset WindowPadding
		ImGui::PopStyleVar();
		// Reset WindowBorder
		ImGui::PopStyleVar();
	}


	void FlatEngine::FlatGui::Game_RenderObjects(ImVec2 canvas_p0, ImVec2 canvas_sz)
	{
		// Get loaded scene if it's not a nullptr and initialize necessary entities
		std::shared_ptr<Scene> loadedScene = FlatEngine::GetLoadedScene();
		std::vector<std::shared_ptr<GameObject>> sceneObjects;
		std::shared_ptr<FlatEngine::Camera> primaryCamera;
		std::shared_ptr<FlatEngine::Transform> cameraTransform;

		if (loadedScene != nullptr)
		{
			sceneObjects = loadedScene->GetSceneObjects();
			// Get Primary Camera and set default values for if no camera is set
			primaryCamera = loadedScene->GetPrimaryCamera();
		}
		else
		{
			sceneObjects = std::vector <std::shared_ptr<GameObject>>();
			primaryCamera = nullptr;
		}
			
		// Create Draw List
		ImDrawList *draw_list = ImGui::GetWindowDrawList();
		// Create the splitter for the draw_list
		ImDrawListSplitter* drawSplitter = new ImDrawListSplitter();
		// 3 channels for now in this scene view. 0 = scene objects, 1 = other UI (camera icon, etc), 2 = transform arrow
		drawSplitter->Split(draw_list, maxSpriteLayers + 5);

		ImVec2 cameraPosition(0, 0);
		float cameraWidth = 50;
		float cameraHeight = 30;
		// Used to convert grid space values to pixel values.ie. 2 grid squares = 2 * 10 = 20px.
		float cameraZoom = 10;
		ImVec4 frustrumColor = ImVec4(1, 1, 1, 1);
		

		// If the primaryCamera is found and not nullptr, set the cameraPosition accordingly, else it remains at {0,0} above
		if (primaryCamera != nullptr)
		{
			cameraTransform = std::static_pointer_cast<FlatEngine::Transform>(FlatEngine::GetObjectComponent(primaryCamera->GetParentID(), ComponentTypes::Transform));
			cameraWidth = primaryCamera->GetWidth();
			cameraHeight = primaryCamera->GetHeight();
			cameraZoom = primaryCamera->GetZoom();
			frustrumColor = primaryCamera->GetFrustrumColor();
			
			// Get the cameras position including all of its parents transforms offsets using the recusive Game_GetTotalCameraOffset();
			cameraPosition = FlatEngine::FlatGui::Game_GetTotalCameraOffset(primaryCamera);
		}

		// Get the "center point" of the games view. This will appear to move around when we move the camera
		worldCenterPoint = ImVec2((GAME_VIEWPORT_WIDTH / 2) - (cameraPosition.x * cameraZoom) + canvas_p0.x, (GAME_VIEWPORT_HEIGHT / 2) + (cameraPosition.y * cameraZoom) + canvas_p0.y);
		// Get the "center point of the viewport
		ImVec2 viewportCenterPoint = ImVec2((GAME_VIEWPORT_WIDTH / 2) + canvas_p0.x, (GAME_VIEWPORT_HEIGHT / 2) + canvas_p0.y);

		// Start off with a 0,0 parentOffset because this is the top level object to be rendered.
		Vector2 parentOffset(0, 0);
		Vector2 parentScale(1, 1);

		// Render Game Objects
		for (int i = 0; i < sceneObjects.size(); i++)
		{
			// If this object doesn't have a parent, render it and then its children recursively
			if (sceneObjects[i]->GetParentID() == -1 && sceneObjects[i]->IsActive())
				FlatEngine::FlatGui::Game_RenderSelfThenChildren(sceneObjects[i], parentOffset, parentScale, worldCenterPoint, canvas_p0, canvas_sz, draw_list, drawSplitter,
					cameraPosition, cameraWidth, cameraHeight, cameraZoom);
		}


		// Render Primary Camera Frustrum
		{
			Texture* frustrum = new Texture();
			frustrum->loadFromFile("assets/images/frustrum.png");
			float h_FrustWidth = GAME_VIEWPORT_WIDTH;
			float h_FrustHeight = (GAME_VIEWPORT_HEIGHT - cameraHeight * cameraZoom) / 2;
			float v_FrustWidth = (GAME_VIEWPORT_WIDTH - cameraWidth * cameraZoom) / 2;
			float v_FrustHeight = (cameraHeight * cameraZoom) + GAME_VIEWPORT_HEIGHT;
			SDL_Texture* frustrumTexture = frustrum->getTexture();
			bool _frustScales = false;
			Vector2 frustrumScale = { 1, 1 };
			// Offsets in px
			Vector2 topFrustOffset = Vector2(v_FrustWidth, h_FrustHeight);
			Vector2 botFrustOffset = Vector2(v_FrustWidth, 0);
			Vector2 leftFrustOffset = Vector2(v_FrustWidth, v_FrustHeight);
			Vector2 rightFrustOffset = Vector2(0, v_FrustHeight);
			float redValue = trunc(frustrumColor.x);
			float greenValue = trunc(frustrumColor.y);
			float blueValue = trunc(frustrumColor.z);
			float alphaValue = trunc(frustrumColor.w);
			ImU32 frustrumColorU32 = (((ImU32)(alphaValue) << 24) | ((ImU32)(blueValue) << 16) | ((ImU32)(greenValue) << 8) | ((ImU32)(redValue)));

			// Set drawing channel to maxSpriteLayers + 2 for top layer camera UI
			drawSplitter->SetCurrentChannel(draw_list, maxSpriteLayers + 2);

			// Top frustrum
			//FlatEngine::FlatGui::AddImageToDrawList(frustrumTexture, Vector2(-cameraWidth / 2, cameraHeight / 2), viewportCenterPoint, h_FrustWidth, h_FrustHeight, topFrustOffset, frustrumScale, _frustScales, cameraZoom, draw_list, frustrumColorU32);
			// Bottom frustrum
			//FlatEngine::FlatGui::AddImageToDrawList(frustrumTexture, Vector2(-cameraWidth / 2, -cameraHeight / 2), viewportCenterPoint, h_FrustWidth, h_FrustHeight, botFrustOffset, frustrumScale, _frustScales, cameraZoom, draw_list, frustrumColorU32);
			// Left frustrum
			//FlatEngine::FlatGui::AddImageToDrawList(frustrumTexture, Vector2(-cameraWidth / 2, -cameraHeight / 2), viewportCenterPoint, v_FrustWidth, v_FrustHeight, leftFrustOffset, frustrumScale, _frustScales, cameraZoom, draw_list, frustrumColorU32);
			// Left frustrum
			//FlatEngine::FlatGui::AddImageToDrawList(frustrumTexture, Vector2(cameraWidth / 2, -cameraHeight / 2), viewportCenterPoint, v_FrustWidth, v_FrustHeight, rightFrustOffset, frustrumScale, _frustScales, cameraZoom, draw_list, frustrumColorU32);

			drawSplitter->Merge(draw_list);
		}
	}


	ImVec2 FlatEngine::FlatGui::Game_GetTotalCameraOffset(std::shared_ptr<Camera> primaryCamera)
	{
		std::shared_ptr<GameObject> parent = FlatEngine::GetObjectById(primaryCamera->GetParentID());
		std::shared_ptr<Transform> transform = std::static_pointer_cast<Transform>(parent->GetComponent(ComponentTypes::Transform));
		Vector2 offset = transform->GetPosition();
		Vector2 scale = transform->GetScale();

		// If the Primary Camera has a parent, get its offset recursively
		if (parent->GetParentID() != -1)
			FlatEngine::FlatGui::Game_GetTotalOffsetAndScale(parent, offset, scale);

		return ImVec2(offset.x, offset.y);
	}


	void FlatEngine::FlatGui::Game_GetTotalOffsetAndScale(std::shared_ptr<GameObject> child, Vector2 &offset, Vector2 &scale)
	{
		// Check if the child actually has a parent object
		if (child->GetParentID() != -1)
		{
			std::shared_ptr<GameObject> parent = FlatEngine::GetObjectById(child->GetParentID());
			std::shared_ptr<Transform> parentTransform = std::static_pointer_cast<Transform>(parent->GetComponent(ComponentTypes::Transform));
			Vector2 parentPosition = Vector2(0, 0);
			Vector2 parentScale = Vector2(1, 1);

			if (parentTransform != nullptr)
			{
				parentPosition = parentTransform->GetPosition();
				parentScale = parentTransform->GetScale();
			}

			offset.x += parentPosition.x;
			offset.y += parentPosition.y;
			scale.x *= parentScale.x;
			scale.y *= parentScale.y;

			// Check if the childs parent has a parent
			if (parent->GetParentID() != -1)
				FlatEngine::FlatGui::Game_GetTotalOffsetAndScale(parent, offset, scale);
		}
	}


	void FlatEngine::FlatGui::Scene_RenderView()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

		ImGui::Begin("Scene View");

		// Using InvisibleButton() as a convenience 1) it will advance the layout cursor and 2) allows us to use IsItemHovered()/IsItemActive()
		ImVec2 canvas_p0 = ImGui::GetCursorScreenPos(); 
		ImVec2 canvas_sz = ImGui::GetContentRegionAvail();
		if (canvas_sz.x < 50.0f) canvas_sz.x = 50.0f;
		if (canvas_sz.y < 50.0f) canvas_sz.y = 50.0f;
		ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);

		static ImVec2 scrolling(0,0);

		// Set initial viewport dimensions for rendering scene view grid and objects
		if (!_firstSceneRenderPass)
		{
			if (!_sceneHasBeenSet)
			{
				SCENE_VIEWPORT_WIDTH = canvas_sz.x;
				SCENE_VIEWPORT_HEIGHT = canvas_sz.y;

				// We offset this from the beginning so our canvas shows our axis in the middle of the screen.
				scrolling = ImVec2((SCENE_VIEWPORT_WIDTH / 2) + canvas_p0.x, (SCENE_VIEWPORT_HEIGHT / 2) + canvas_p0.y);

				_sceneHasBeenSet = true;
			}
		}

		_firstSceneRenderPass = false;

		// For calculating scrolling mouse position and what vector to zoom to
		DYNAMIC_VIEWPORT_WIDTH = trunc(canvas_p1.x - canvas_p0.x);
		DYNAMIC_VIEWPORT_HEIGHT = trunc(canvas_p1.y - canvas_p0.y);

		// Get Input and Output
		ImGuiIO& inputOutput = ImGui::GetIO();

		// This will catch our interactions
		ImGui::InvisibleButton("SceneViewCanvas", canvas_sz, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
		const bool is_hovered = ImGui::IsItemHovered(); // Hovered
		const bool is_active = ImGui::IsItemActive();   // Held

		// Get scroll amount for changing zoom level of scene view
		Vector2 mousePos = Vector2(inputOutput.MousePos.x, inputOutput.MousePos.y);
		float scrollInput = inputOutput.MouseWheel;
		float weight = 0.08f;
		float signedMousePosX = mousePos.x - canvas_p0.x - (DYNAMIC_VIEWPORT_WIDTH / 2);
		float signedMousePosY = mousePos.y - canvas_p0.y - (DYNAMIC_VIEWPORT_HEIGHT / 2);

		// Change scrolling offset based on mouse position and weight
		if (is_hovered)
		{
			if (scrollInput > 0)
			{
				scrolling.x -= trunc(signedMousePosX * weight);
				scrolling.y -= trunc(signedMousePosY * weight);
				gridStep += 1;
			}
			else if (scrollInput < 0 && gridStep > 2)
			{
				scrolling.x += trunc(signedMousePosX * weight);
				scrolling.y += trunc(signedMousePosY * weight);
				gridStep -= 1;
			}
		}

		// For panning the scene view
		const float mouse_threshold_for_pan = 0.0f;
		if (is_active && ImGui::IsMouseDragging(ImGuiMouseButton_Right, mouse_threshold_for_pan))
		{
			scrolling.x += inputOutput.MouseDelta.x;
			scrolling.y += inputOutput.MouseDelta.y;
		}

		FlatEngine::FlatGui::Scene_RenderGrid(scrolling, canvas_p0, canvas_p1, canvas_sz, gridStep);
	
		FlatEngine::FlatGui::Scene_RenderObjects(scrolling, canvas_p0, canvas_sz);

		// Reset WindowPadding
		ImGui::PopStyleVar();
		// Reset WindowBorder
		ImGui::PopStyleVar();

		ImGui::End();
	}


	void FlatEngine::FlatGui::Scene_RenderObjects(ImVec2 scrolling, ImVec2 canvas_p0, ImVec2 canvas_sz)
	{
		// Get currently loade scene
		std::shared_ptr<Scene> loadedScene = FlatEngine::sceneManager->GetLoadedScene();
		std::vector<std::shared_ptr<GameObject>> sceneObjects;
		
		if (loadedScene != nullptr)
			sceneObjects = loadedScene->GetSceneObjects();
		else
			sceneObjects = std::vector<std::shared_ptr<GameObject>>();

		// Split our drawlist into multiple channels for different rendering orders
		ImDrawList *draw_list = ImGui::GetWindowDrawList();
		ImDrawListSplitter* drawSplitter = new ImDrawListSplitter();

		// 4 channels for now in this scene view. 0 = scene objects, 1 & 2 = other UI (camera icon, etc), 4 = transform arrow
		drawSplitter->Split(draw_list, maxSpriteLayers + 5);

		// Loop through scene objects
		for (int i = 0; i < sceneObjects.size(); i++)
		{
			// If this Scene Object doesn't have a parent, render it and all of its children
			if (sceneObjects[i]->GetParentID() == -1 && sceneObjects[i]->IsActive())
			{
				// Start off with a 0,0 parentOffset because this is the top level object to be rendered.
				Vector2 parentOffset(0, 0);
				Vector2 parentScale(1, 1);

				// Render self and children recursively
				FlatEngine::FlatGui::Scene_RenderSelfThenChildren(sceneObjects[i], parentOffset, parentScale, scrolling, canvas_p0, canvas_sz, draw_list, drawSplitter);
			}
		}
	
		drawSplitter->Merge(draw_list);
		delete drawSplitter;
		drawSplitter = nullptr;
	}


	// Helper - Recursively draws scene objects and their children to the scene view
	void FlatEngine::FlatGui::Scene_RenderSelfThenChildren(std::shared_ptr<GameObject> self, Vector2 parentOffset, Vector2 parentScale, ImVec2 scrolling, ImVec2 canvas_p0, ImVec2 canvas_sz, ImDrawList *draw_list, ImDrawListSplitter* drawSplitter)
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
			long focusedObjectID = FlatEngine::GetFocusedGameObjectID();
			std::shared_ptr<Transform> transformCasted = std::static_pointer_cast<Transform>(transformComponent);
			Vector2 position = Vector2(transformCasted->GetPosition().x + parentOffset.x, transformCasted->GetPosition().y + parentOffset.y);
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
					FlatEngine::FlatGui::AddImageToDrawList(spriteTexture, position, scrolling, spriteTextureWidth, spriteTextureHeight, spriteOffset, transformScale, _spriteScalesWithZoom, gridStep, draw_list);
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
					FlatEngine::FlatGui::AddImageToDrawList(textTexture->getTexture(), position, scrolling, textWidth, textHeight, offset, transformScale, _spriteScalesWithZoom, gridStep, draw_list);
				}
			}

			// Renders the camera
			if (cameraComponent != nullptr)
			{
				std::shared_ptr<Camera> camera = std::static_pointer_cast<Camera>(cameraComponent);
				float cameraWidth = camera->GetWidth();
				float cameraHeight = camera->GetHeight();

				float cameraLeftEdge = WorldToViewport(scrolling.x, position.x - cameraWidth / 2 * transformScale.x, gridStep);
				float cameraRightEdge = WorldToViewport(scrolling.x, position.x + cameraWidth / 2 * transformScale.x, gridStep);
				float cameraTopEdge = WorldToViewport(scrolling.y, -(position.y + cameraHeight / 2 * transformScale.y), gridStep);
				float cameraBottomEdge = WorldToViewport(scrolling.y, -(position.y - cameraHeight / 2 * transformScale.y), gridStep);

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
				FlatEngine::FlatGui::AddImageToDrawList(texture, position, scrolling, cameraTextureWidth, cameraTextureHeight, cameraTextureOffset, cameraTextureScale, _scalesWithZoom, gridStep, draw_list, IM_COL32(255, 255, 255, iconTransparency));
			}


			// Renders Canvas Component
			if (canvasComponent != nullptr)
			{
				std::shared_ptr<Canvas> canvas = std::static_pointer_cast<Canvas>(canvasComponent);

				float activeWidth = canvas->GetWidth();
				float activeHeight = canvas->GetHeight();
				int layerNumber = canvas->GetLayerNumber();
				bool _blocksLayers = canvas->GetBlocksLayers();

				float canvasLeft = WorldToViewport(scrolling.x, position.x - activeWidth / 2, gridStep, false);
				float canvasRight = WorldToViewport(scrolling.x, position.x + activeWidth / 2, gridStep, false);
				float canvasTop = WorldToViewport(scrolling.y, position.y + activeHeight / 2, gridStep, true);
				float canvasBottom = WorldToViewport(scrolling.y, position.y - activeHeight / 2, gridStep, true);

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

				float activeLeft = WorldToViewport(scrolling.x, position.x + activeOffset.x - (activeWidth / 2 * transformScale.x), gridStep, false);
				float activeRight = WorldToViewport(scrolling.x, position.x + activeOffset.x + (activeWidth / 2 * transformScale.x), gridStep, false);
				float activeTop = WorldToViewport(scrolling.y, position.y + activeOffset.y + (activeHeight / 2 * transformScale.y), gridStep, true);
				float activeBottom = WorldToViewport(scrolling.y, position.y + activeOffset.y - (activeHeight / 2 * transformScale.y), gridStep, true);

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
				FlatEngine::FlatGui::AddImageToDrawList(texture, position, scrolling, arrowWidth, arrowHeight, arrowOffset, arrowScale, _scalesWithZoom, gridStep, draw_list, IM_COL32(255, 255, 255, 255));
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
					FlatEngine::FlatGui::Scene_RenderSelfThenChildren(child, parentOffset, parentScale, scrolling, canvas_p0, canvas_sz, draw_list, drawSplitter);
			}
		}
	}

	// Helper - Recursively draws scene objects and their children to the game view
	void FlatEngine::FlatGui::Game_RenderSelfThenChildren(std::shared_ptr<GameObject> self, Vector2 parentOffset, Vector2 parentScale, ImVec2 worldCenterPoint, ImVec2 canvas_p0,
		ImVec2 canvas_sz, ImDrawList* draw_list, ImDrawListSplitter* drawSplitter, ImVec2 cameraPosition, float cameraWidth, float cameraHeight, float cameraZoom)
	{
		// Get Components
		std::shared_ptr<Component> transformComponent = self->GetComponent(Component::ComponentTypes::Transform);
		std::shared_ptr<Component> spriteComponent = self->GetComponent(Component::ComponentTypes::Sprite);
		std::shared_ptr<Component> animationComponent = self->GetComponent(Component::ComponentTypes::Animation);
		std::shared_ptr<Component> textComponent = self->GetComponent(ComponentTypes::Text);
		std::shared_ptr<Component> buttonComponent = self->GetComponent(ComponentTypes::Button);

		// Animation component handling
		if (animationComponent != nullptr)
		{
			// Cast Animation component
			std::shared_ptr<Animation> animationCasted = std::static_pointer_cast<Animation>(animationComponent);
			
			// If animation component is playing, play the animation
			if (animationCasted != nullptr && animationCasted->IsPlaying())
				animationCasted->LerpToCenter();
		}


		// Check if each object has a Transform component
		if (transformComponent != nullptr)
		{
			long focusedObjectID = FlatEngine::GetFocusedGameObjectID();
			std::shared_ptr<Transform> transformCasted = std::static_pointer_cast<Transform>(transformComponent);
			Vector2 position = Vector2(transformCasted->GetPosition().x + parentOffset.x, transformCasted->GetPosition().y + parentOffset.y);
			Vector2 scale = Vector2(transformCasted->GetScale().x * parentScale.x, transformCasted->GetScale().y * parentScale.y);

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
					FlatEngine::FlatGui::AddImageToDrawList(textTexture->getTexture(), position, worldCenterPoint, textWidth, textHeight, offset, scale, _spriteScalesWithZoom, cameraZoom, draw_list);
				}
			}

			// If it has a sprite component, render that sprite texture at the objects transform position
			if (spriteComponent != nullptr)
			{
				// Cast the components to their respective types
				std::shared_ptr<Sprite> spriteCasted = std::static_pointer_cast<Sprite>(spriteComponent);

				SDL_Texture* spriteTexture = spriteCasted->GetTexture();
				float textureWidth = (float)spriteCasted->GetTextureWidth();
				float textureHeight = (float)spriteCasted->GetTextureHeight();
				Vector2 offset = spriteCasted->GetOffset();
				bool _scalesWithZoom = true;
				int renderOrder = spriteCasted->GetRenderOrder();

				// Changing the scale here because things are rendering too large and I want them to start off smaller
				Vector2 newScale = Vector2(scale.x * spriteScaleMultiplier, scale.y * spriteScaleMultiplier);

				float spriteLeftEdge = position.x - offset.x * newScale.x;
				float spriteRightEdge = position.x + offset.x * newScale.x;
				float spriteTopEdge = position.y + offset.y * newScale.y;
				float spriteBottomEdge = position.y - offset.y * newScale.y;

				float cameraLeftEdge = cameraPosition.x - cameraWidth / 2;
				float cameraRightEdge = cameraPosition.x + cameraWidth / 2;
				float cameraTopEdge = cameraPosition.y + cameraHeight / 2;
				float cameraBottomEdge = cameraPosition.y - cameraHeight / 2;

				bool _isIntersecting = false;

				if (spriteLeftEdge < cameraRightEdge && spriteRightEdge > cameraLeftEdge &&
					spriteTopEdge > cameraBottomEdge && spriteBottomEdge < cameraTopEdge)
					_isIntersecting = true;

				if (_isIntersecting)
				{
					if (renderOrder <= maxSpriteLayers && renderOrder >= 0)
						drawSplitter->SetCurrentChannel(draw_list, renderOrder);
					else
						drawSplitter->SetCurrentChannel(draw_list, 0);
					FlatEngine::FlatGui::AddImageToDrawList(spriteTexture, position, worldCenterPoint, textureWidth, textureHeight, offset, scale, _scalesWithZoom, cameraZoom, draw_list);
				}
			}

			// Renders Button Component
			if (buttonComponent != nullptr)
			{
				std::shared_ptr<Button> button = std::static_pointer_cast<Button>(buttonComponent);

				float activeWidth = button->GetActiveWidth();
				float activeHeight = button->GetActiveHeight();
				Vector2 activeOffset = button->GetActiveOffset();
				bool _isActive = button->IsActive();

				// Get Active Button bounds to check against later for mouse events
				float activeLeft = WorldToViewport(worldCenterPoint.x, position.x + activeOffset.x - (activeWidth / 2 * scale.x), cameraZoom, false);
				float activeRight = WorldToViewport(worldCenterPoint.x, position.x + activeOffset.x + (activeWidth / 2 * scale.x), cameraZoom, false);
				float activeTop = WorldToViewport(worldCenterPoint.y, position.y + activeOffset.y + (activeHeight / 2 * scale.y), cameraZoom, true);
				float activeBottom = WorldToViewport(worldCenterPoint.y, position.y + activeOffset.y - (activeHeight / 2 * scale.y), cameraZoom, true);

				button->SetActiveEdges(ImVec4(activeTop, activeRight, activeBottom, activeLeft));

				// FOR DRAWING ACTIVE BUTTON RECTANGLE IN GAME VIEW
				// 
				//Vector2 topLeft = { activeLeft, activeTop };
				//Vector2 bottomRight = { activeRight, activeBottom };

				//drawSplitter->SetCurrentChannel(draw_list, maxSpriteLayers + 2);

				//if (_isActive)
				//	FlatEngine::DrawRectangle(topLeft, bottomRight, canvas_p0, canvas_sz, FlatEngine::ActiveButtonColor, 3.0f, draw_list);
				//else
				//	FlatEngine::DrawRectangle(topLeft, bottomRight, canvas_p0, canvas_sz, FlatEngine::InactiveButtonColor, 3.0f, draw_list);
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
					FlatEngine::FlatGui::Game_RenderSelfThenChildren(child, parentOffset, parentScale, worldCenterPoint, canvas_p0, canvas_sz, draw_list, drawSplitter,
						cameraPosition, cameraWidth, cameraHeight, cameraZoom);
			}
		}
	}


	// Helper - Get a value from world/grid position converted into viewport position. Just add - canvas_p0 to get Window coordinates
	float FlatEngine::FlatGui::WorldToViewport(float centerPoint, float worldPosition, float zoomFactor, bool _isYCoord)
	{
		std::shared_ptr<Camera> primaryCamera = FlatEngine::GetLoadedScene()->GetPrimaryCamera();
		float scaleToScreenSizeBy = 1;

		// If we're in Release mode, scale the Game Viewport to the window size
		if (FlatEngine::_isDebugMode == true && primaryCamera != nullptr)
			scaleToScreenSizeBy = GAME_VIEWPORT_HEIGHT / primaryCamera->GetHeight() * .05f;

		if (_isYCoord)
			return centerPoint + (-worldPosition * zoomFactor * scaleToScreenSizeBy);
		else
			return centerPoint + (worldPosition * zoomFactor * scaleToScreenSizeBy);
	}
	// Helper - Get a value from viewport position converted into world/grid position.
	ImVec2 FlatEngine::FlatGui::ViewportToWorld(ImVec2 viewportPosition)
	{
		std::shared_ptr<Camera> primaryCamera = FlatEngine::GetLoadedScene()->GetPrimaryCamera();
		float zoom = 10;

		if (primaryCamera != nullptr)
			zoom = primaryCamera->GetZoom();

		float xPos = (viewportPosition.x - worldCenterPoint.x) / zoom;
		float yPos = -(viewportPosition.y - worldCenterPoint.y) / zoom;

		return ImVec2(xPos, yPos);
	}


	void FlatEngine::FlatGui::Scene_RenderGrid(ImVec2 scrolling, ImVec2 canvas_p0, ImVec2 canvas_p1, ImVec2 canvas_sz, float gridStep)
	{
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(50, 50, 50, 255));

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

		// Draw horizontal grid lines
		for (float x = trunc(fmodf(scrolling.x, gridStep)); x < canvas_p0.x + canvas_sz.x; x += gridStep)
		{
			FlatEngine::DrawLine(ImVec2(x, canvas_p0.y), ImVec2(x, canvas_p1.y), IM_COL32(200, 200, 200, 40), 1.0f, draw_list);
		}
		// Draw vertical grid lines
		for (float y = trunc(fmodf(scrolling.y, gridStep)); y < canvas_p0.y + canvas_sz.y; y += gridStep)
		{
			FlatEngine::DrawLine(ImVec2(canvas_p0.x, y), ImVec2(canvas_p1.x, y), IM_COL32(200, 200, 200, 40), 1.0f, draw_list);
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

		// Draw the axis and center point
		FlatEngine::DrawLine(ImVec2(offsetX, canvas_p0.y), ImVec2(offsetX, canvas_p1.y), xColor, 1.0f, draw_list);
		FlatEngine::DrawLine(ImVec2(canvas_p0.x, offsetY), ImVec2(canvas_p1.x, offsetY), yColor, 1.0f, draw_list);
		FlatEngine::DrawPoint(ImVec2(scrolling.x, scrolling.y), centerColor, draw_list);
	}


	void FlatEngine::FlatGui::AddImageToDrawList(SDL_Texture *texture, Vector2 positionInGrid, ImVec2 relativeCenterPoint, float textureWidthPx, float textureHeightPx, Vector2 offsetPx, Vector2 scale, bool _scalesWithZoom, float zoomMultiplier, ImDrawList *draw_list, ImU32 addColor)
	{
		// Changing the scale here because sprites are rendering too large and I want them to start off smaller and also keep the default scale value to 1.0f
		Vector2 newScale = Vector2(scale.x * spriteScaleMultiplier, scale.y * spriteScaleMultiplier);

		float scalingXStart = WorldToViewport(relativeCenterPoint.x, positionInGrid.x - (offsetPx.x * newScale.x), zoomMultiplier);
		float scalingYStart = WorldToViewport(relativeCenterPoint.y, -positionInGrid.y - (offsetPx.y * newScale.y), zoomMultiplier);
		float scalingXEnd = WorldToViewport(scalingXStart, textureWidthPx * newScale.x, zoomMultiplier);
		float scalingYEnd = WorldToViewport(scalingYStart, textureHeightPx * newScale.y, zoomMultiplier);

		float unscaledXStart = WorldToViewport(relativeCenterPoint.x, positionInGrid.x, zoomMultiplier) - offsetPx.x * scale.x;
		float unscaledYStart = WorldToViewport(relativeCenterPoint.y, -positionInGrid.y, zoomMultiplier) - offsetPx.y * scale.y;

		ImVec2 renderStart;
		ImVec2 renderEnd;
		ImVec2 UvStart = { 0, 0 };
		ImVec2 UvEnd = { 1, 1 };

		if (_scalesWithZoom)
		{
			renderStart = ImVec2(scalingXStart, scalingYStart);
			renderEnd = ImVec2(scalingXEnd, scalingYEnd);
		}
		else
		{
			renderStart = ImVec2(unscaledXStart, unscaledYStart);
			renderEnd = ImVec2(renderStart.x + textureWidthPx * scale.x, renderStart.y + textureHeightPx * scale.y);
		}

		// Render sprite to viewport
		draw_list->AddImage((void*)texture, renderStart, renderEnd, UvStart, UvEnd, addColor);
	}


	void FlatEngine::FlatGui::RenderLog()
	{
		ImGui::Begin("Debug Log");
		
		if (ImGui::Checkbox("Clear buffer after every frame", &_clearBufferEveryFrame))
			FlatEngine::logger->ClearBuffer();

		ImGuiChildFlags padding_child_flags = ImGuiChildFlags_::ImGuiChildFlags_AlwaysUseWindowPadding;

		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ChildBg, outerWindowColor);
		ImGui::BeginChild("Log Container", ImVec2(0,0), padding_child_flags);
		ImGui::PopStyleColor();

		static ImGuiTextBuffer *log = FlatEngine::logger->GetBuffer();
		static int lines = 0;

		ImGui::Text("Log buffer contents : % d bytes", log->size());
		ImGui::SameLine(0, 10);
		if (ImGui::Button("Clear"))
		{ 
			log->clear(); lines = 0; 
		}

		ImGui::PushStyleColor(ImGuiCol_ChildBg, innerWindowColor);
		ImGui::BeginChild("Log", ImVec2(0,0), padding_child_flags);
		ImGui::TextUnformatted(log->begin(), log->end());
		ImGui::PopStyleColor();

		ImGui::EndChild(); // Log
		ImGui::EndChild(); // Log Container
		ImGui::End(); // Debug Log

		// For keeping the log from filling up when logging repeating values
		if (_clearBufferEveryFrame)
		{
			FlatEngine::logger->ClearBuffer();
			FlatEngine::LogString("Log buffer is being cleared...");
		}
	}


	void FlatEngine::FlatGui::Cleanup()
	{
		ImGui_ImplSDLRenderer2_Shutdown();
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext();
	}
}
}










//// DirectX12 Helper functions
//bool CreateDeviceD3D(HWND hWnd)
//{
//	// Setup swap chain
//	DXGI_SWAP_CHAIN_DESC1 sd;
//	{
//		ZeroMemory(&sd, sizeof(sd));
//		sd.BufferCount = NUM_BACK_BUFFERS;
//		sd.Width = 0;
//		sd.Height = 0;
//		sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
//		sd.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
//		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
//		sd.SampleDesc.Count = 1;
//		sd.SampleDesc.Quality = 0;
//		sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
//		sd.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
//		sd.Scaling = DXGI_SCALING_STRETCH;
//		sd.Stereo = FALSE;
//	}
//
//	// [DEBUG] Enable debug interface
//	#ifdef DX12_ENABLE_DEBUG_LAYER
//	ID3D12Debug* pdx12Debug = nullptr;
//	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&pdx12Debug))))
//		pdx12Debug->EnableDebugLayer();
//	#endif
//
//	// Create device
//	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
//	if (D3D12CreateDevice(nullptr, featureLevel, IID_PPV_ARGS(&g_pd3dDevice)) != S_OK)
//		return false;
//
//	// [DEBUG] Setup debug interface to break on any warnings/errors
//	#ifdef DX12_ENABLE_DEBUG_LAYER
//	if (pdx12Debug != nullptr)
//	{
//		ID3D12InfoQueue* pInfoQueue = nullptr;
//		g_pd3dDevice->QueryInterface(IID_PPV_ARGS(&pInfoQueue));
//		pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
//		pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
//		pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
//		pInfoQueue->Release();
//		pdx12Debug->Release();
//	}
//	#endif
//
//	{
//		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
//		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
//		desc.NumDescriptors = NUM_BACK_BUFFERS;
//		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
//		desc.NodeMask = 1;
//		if (g_pd3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&g_pd3dRtvDescHeap)) != S_OK)
//			return false;
//
//		SIZE_T rtvDescriptorSize = g_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
//		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = g_pd3dRtvDescHeap->GetCPUDescriptorHandleForHeapStart();
//		for (UINT i = 0; i < NUM_BACK_BUFFERS; i++)
//		{
//			g_mainRenderTargetDescriptor[i] = rtvHandle;
//			rtvHandle.ptr += rtvDescriptorSize;
//		}
//	}
//
//	{
//		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
//		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
//		desc.NumDescriptors = 1;
//		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
//		if (g_pd3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&g_pd3dSrvDescHeap)) != S_OK)
//			return false;
//	}
//
//	{
//		D3D12_COMMAND_QUEUE_DESC desc = {};
//		desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
//		desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
//		desc.NodeMask = 1;
//		if (g_pd3dDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(&g_pd3dCommandQueue)) != S_OK)
//			return false;
//	}
//
//	for (UINT i = 0; i < NUM_FRAMES_IN_FLIGHT; i++)
//		if (g_pd3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&g_frameContext[i].CommandAllocator)) != S_OK)
//			return false;
//
//	if (g_pd3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, g_frameContext[0].CommandAllocator, nullptr, IID_PPV_ARGS(&g_pd3dCommandList)) != S_OK ||
//		g_pd3dCommandList->Close() != S_OK)
//		return false;
//
//	if (g_pd3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&g_fence)) != S_OK)
//		return false;
//
//	g_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
//	if (g_fenceEvent == nullptr)
//		return false;
//
//	{
//		IDXGIFactory4* dxgiFactory = nullptr;
//		IDXGISwapChain1* swapChain1 = nullptr;
//		if (CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory)) != S_OK)
//			return false;
//		if (dxgiFactory->CreateSwapChainForHwnd(g_pd3dCommandQueue, hWnd, &sd, nullptr, nullptr, &swapChain1) != S_OK)
//			return false;
//		if (swapChain1->QueryInterface(IID_PPV_ARGS(&g_pSwapChain)) != S_OK)
//			return false;
//		swapChain1->Release();
//		dxgiFactory->Release();
//		g_pSwapChain->SetMaximumFrameLatency(NUM_BACK_BUFFERS);
//		g_hSwapChainWaitableObject = g_pSwapChain->GetFrameLatencyWaitableObject();
//	}
//
//	CreateRenderTarget();
//	return true;
//}
//
//void CleanupDeviceD3D()
//{
//	CleanupRenderTarget();
//	if (g_pSwapChain) { g_pSwapChain->SetFullscreenState(false, nullptr); g_pSwapChain->Release(); g_pSwapChain = nullptr; }
//	if (g_hSwapChainWaitableObject != nullptr) { CloseHandle(g_hSwapChainWaitableObject); }
//	for (UINT i = 0; i < NUM_FRAMES_IN_FLIGHT; i++)
//		if (g_frameContext[i].CommandAllocator) { g_frameContext[i].CommandAllocator->Release(); g_frameContext[i].CommandAllocator = nullptr; }
//	if (g_pd3dCommandQueue) { g_pd3dCommandQueue->Release(); g_pd3dCommandQueue = nullptr; }
//	if (g_pd3dCommandList) { g_pd3dCommandList->Release(); g_pd3dCommandList = nullptr; }
//	if (g_pd3dRtvDescHeap) { g_pd3dRtvDescHeap->Release(); g_pd3dRtvDescHeap = nullptr; }
//	if (g_pd3dSrvDescHeap) { g_pd3dSrvDescHeap->Release(); g_pd3dSrvDescHeap = nullptr; }
//	if (g_fence) { g_fence->Release(); g_fence = nullptr; }
//	if (g_fenceEvent) { CloseHandle(g_fenceEvent); g_fenceEvent = nullptr; }
//	if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
//
//#ifdef DX12_ENABLE_DEBUG_LAYER
//	IDXGIDebug1* pDebug = nullptr;
//	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&pDebug))))
//	{
//		pDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_SUMMARY);
//		pDebug->Release();
//	}
//#endif
//}
//
//void CreateRenderTarget()
//{
//	for (UINT i = 0; i < NUM_BACK_BUFFERS; i++)
//	{
//		ID3D12Resource* pBackBuffer = nullptr;
//		g_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&pBackBuffer));
//		g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, g_mainRenderTargetDescriptor[i]);
//		g_mainRenderTargetResource[i] = pBackBuffer;
//	}
//}
//
//void CleanupRenderTarget()
//{
//	WaitForLastSubmittedFrame();
//
//	for (UINT i = 0; i < NUM_BACK_BUFFERS; i++)
//		if (g_mainRenderTargetResource[i]) { g_mainRenderTargetResource[i]->Release(); g_mainRenderTargetResource[i] = nullptr; }
//}
//
//void WaitForLastSubmittedFrame()
//{
//	FrameContext* frameCtx = &g_frameContext[g_frameIndex % NUM_FRAMES_IN_FLIGHT];
//
//	UINT64 fenceValue = frameCtx->FenceValue;
//	if (fenceValue == 0)
//		return; // No fence was signaled
//
//	frameCtx->FenceValue = 0;
//	if (g_fence->GetCompletedValue() >= fenceValue)
//		return;
//
//	g_fence->SetEventOnCompletion(fenceValue, g_fenceEvent);
//	WaitForSingleObject(g_fenceEvent, INFINITE);
//}
//
//FrameContext* WaitForNextFrameResources()
//{
//	UINT nextFrameIndex = g_frameIndex + 1;
//	g_frameIndex = nextFrameIndex;
//
//	HANDLE waitableObjects[] = { g_hSwapChainWaitableObject, nullptr };
//	DWORD numWaitableObjects = 1;
//
//	FrameContext* frameCtx = &g_frameContext[nextFrameIndex % NUM_FRAMES_IN_FLIGHT];
//	UINT64 fenceValue = frameCtx->FenceValue;
//	if (fenceValue != 0) // means no fence was signaled
//	{
//		frameCtx->FenceValue = 0;
//		g_fence->SetEventOnCompletion(fenceValue, g_fenceEvent);
//		waitableObjects[1] = g_fenceEvent;
//		numWaitableObjects = 2;
//	}
//
//	WaitForMultipleObjects(numWaitableObjects, waitableObjects, TRUE, INFINITE);
//
//	return frameCtx;
//}
//
//}}
//
//
//
////DirectX12 Variables
//
//struct FrameContext
//{
//	ID3D12CommandAllocator* CommandAllocator;
//	UINT64                  FenceValue;
//};
//
//// Data
//static int const                    NUM_FRAMES_IN_FLIGHT = 3;
//static FrameContext                 g_frameContext[NUM_FRAMES_IN_FLIGHT] = {};
//static UINT                         g_frameIndex = 0;
//
//static int const                    NUM_BACK_BUFFERS = 3;
//static ID3D12Device* g_pd3dDevice = nullptr;
//static ID3D12DescriptorHeap* g_pd3dRtvDescHeap = nullptr;
//static ID3D12DescriptorHeap* g_pd3dSrvDescHeap = nullptr;
//static ID3D12CommandQueue* g_pd3dCommandQueue = nullptr;
//static ID3D12GraphicsCommandList* g_pd3dCommandList = nullptr;
//static ID3D12Fence* g_fence = nullptr;
//static HANDLE                       g_fenceEvent = nullptr;
//static UINT64                       g_fenceLastSignaledValue = 0;
//static IDXGISwapChain3* g_pSwapChain = nullptr;
//static HANDLE                       g_hSwapChainWaitableObject = nullptr;
//static ID3D12Resource* g_mainRenderTargetResource[NUM_BACK_BUFFERS] = {};
//static D3D12_CPU_DESCRIPTOR_HANDLE  g_mainRenderTargetDescriptor[NUM_BACK_BUFFERS] = {};
//
//// Forward declarations of helper functions
//bool CreateDeviceD3D(HWND hWnd);
//void CleanupDeviceD3D();
//void CreateRenderTarget();
//void CleanupRenderTarget();
//void WaitForLastSubmittedFrame();
//FrameContext* WaitForNextFrameResources();








//DirectX12 Setup

	//SDL_SysWMinfo wmInfo;
	//SDL_VERSION(&wmInfo.version);
	//SDL_GetWindowWMInfo(Window::window, &wmInfo);
	//HWND hwnd = (HWND)wmInfo.info.win.window;


	//// Initialize Direct3D
	//if (!CreateDeviceD3D(hwnd))
	//{
	//	CleanupDeviceD3D();
	//		
	//}

	// Setup Dear ImGui context
//IMGUI_CHECKVERSION();
//ImGui::CreateContext();
//io = ImGui::GetIO(); (void)io;
//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
//io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
//io.ConfigViewportsNoAutoMerge = true;
//io.ConfigViewportsNoTaskBarIcon = true;

// Setup Dear ImGui style
//ImGui::StyleColorsDark();
//ImGui::StyleColorsLight();

// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
//ImGuiStyle& style = ImGui::GetStyle();
//if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
//{
//	style.WindowRounding = 0.0f;
//	style.Colors[ImGuiCol_WindowBg].w = 1.0f;
//}

// Setup Platform/Renderer backends
//ImGui_ImplSDL2_InitForD3D(Window::window);
//ImGui_ImplDX12_Init(g_pd3dDevice, NUM_FRAMES_IN_FLIGHT,
//	DXGI_FORMAT_R8G8B8A8_UNORM, g_pd3dSrvDescHeap,
//	g_pd3dSrvDescHeap->GetCPUDescriptorHandleForHeapStart(),
//	g_pd3dSrvDescHeap->GetGPUDescriptorHandleForHeapStart());






//DirectX12 Cleanup

	//WaitForLastSubmittedFrame();

	//// Cleanup
	//ImGui_ImplDX12_Shutdown();
	//ImGui_ImplSDL2_Shutdown();
	//ImGui::DestroyContext();

	//CleanupDeviceD3D();







//DirectX12 Render

	//SDL_Event event;
	//while (SDL_PollEvent(&event))
	//{
	//	ImGui_ImplSDL2_ProcessEvent(&event);
	//	if (event.type == SDL_QUIT)
	//		quit = true;
	//	if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(Window::window))
	//		quit = true;
	//	if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED && event.window.windowID == SDL_GetWindowID(Window::window))
	//	{
	//		// Release all outstanding references to the swap chain's buffers before resizing.
	//		CleanupRenderTarget();
	//		g_pSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
	//		CreateRenderTarget();
	//	}
	//}

		//// Start the Dear ImGui frame
	//ImGui_ImplDX12_NewFrame();
	//ImGui_ImplSDL2_NewFrame();
	//ImGui::NewFrame();


	//if (show_demo_window)
	//ImGui::ShowDemoWindow(&show_demo_window);


	//// Rendering
	//ImGui::Render();

	//FrameContext* frameCtx = WaitForNextFrameResources();
	//UINT backBufferIdx = g_pSwapChain->GetCurrentBackBufferIndex();
	//frameCtx->CommandAllocator->Reset();

	//D3D12_RESOURCE_BARRIER barrier = {};
	//barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	//barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	//barrier.Transition.pResource = g_mainRenderTargetResource[backBufferIdx];
	//barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	//barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	//barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	//g_pd3dCommandList->Reset(frameCtx->CommandAllocator, nullptr);
	//g_pd3dCommandList->ResourceBarrier(1, &barrier);
	//TextureManager::RenderTextures();
	//// Render Dear ImGui graphics
	//const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
	//g_pd3dCommandList->ClearRenderTargetView(g_mainRenderTargetDescriptor[backBufferIdx], clear_color_with_alpha, 0, nullptr);
	//g_pd3dCommandList->OMSetRenderTargets(1, &g_mainRenderTargetDescriptor[backBufferIdx], FALSE, nullptr);
	//g_pd3dCommandList->SetDescriptorHeaps(1, &g_pd3dSrvDescHeap);
	//ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), g_pd3dCommandList);
	//barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	//barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	//g_pd3dCommandList->ResourceBarrier(1, &barrier);
	//g_pd3dCommandList->Close();

	//g_pd3dCommandQueue->ExecuteCommandLists(1, (ID3D12CommandList* const*)&g_pd3dCommandList);

	// Update and Render additional Platform Windows
	//ImGui::UpdatePlatformWindows();
	//ImGui::RenderPlatformWindowsDefault(nullptr, (void*)g_pd3dCommandList);

	//g_pSwapChain->Present(1, 0); // Present with vsync
	////g_pSwapChain->Present(0, 0); // Present without vsync

	//UINT64 fenceValue = g_fenceLastSignaledValue + 1;
	//g_pd3dCommandQueue->Signal(g_fence, fenceValue);
	//g_fenceLastSignaledValue = fenceValue;
	//frameCtx->FenceValue = fenceValue;




// DirectX12 Handle Events
//void FlatEngine::FlatGui::HandleEvents(SDL_Event event, bool& quit)
//{
	// Poll and handle events (inputs, window resize, etc.)
	// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
	// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
	// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
	// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
	//ImGui_ImplSDL2_ProcessEvent(&event);
	//if (event.type == SDL_QUIT)
	//	quit = true;
	//if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(Window::window))
	//	quit = true;
	//if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED && event.window.windowID == SDL_GetWindowID(Window::window))
	//{
	//	// Release all outstanding references to the swap chain's buffers before resizing.
	//	CleanupRenderTarget();
	//	g_pSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
	//	CreateRenderTarget();
	//}
//}