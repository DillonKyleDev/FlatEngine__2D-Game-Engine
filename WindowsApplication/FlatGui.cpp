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
	ImVec4 outerWindowColor = ImVec4(float(0.9), float(0.9), 1, float(0.05));
	ImVec4 innerWindowColor = ImVec4(float(0.9), float(0.9), 1, float(0.1));
	ImVec4 singleItemColor = ImVec4(float(0.9), float(0.9), 1, float(0.1));

	ImU32 white = IM_COL32(255,255,255,255);
	
	// Frame Counter
	int framesDrawn = 0;

	// Scene view
	float gridStep = 10;
	float SCENE_VIEWPORT_WIDTH = 600;
	float SCENE_VIEWPORT_HEIGHT = 400;
	float DYNAMIC_VIEWPORT_WIDTH = 600;
	float DYNAMIC_VIEWPORT_HEIGHT = 400;
	bool _firstSceneRenderPass = true;
	bool _sceneHasBeenSet = false;
	int iconTransparency = 100;

	Texture* transformArrow;
	Texture* cameraTexture;

	// Game view default values
	float GAME_VIEWPORT_WIDTH = 1920;
	float GAME_VIEWPORT_HEIGHT = 1080;
	float xGameCenter = 600/2;
	float yGameCenter = 400/2;


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
		//style.ItemSpacing = { 0, 0 };
		//style.FramePadding = { 0.0f, 0.0f };
		//style.DisplayWindowPadding = { 0.0f, 0.0f };

		ImGui_ImplSDL2_InitForSDLRenderer(Window::window, Window::renderer);
		ImGui_ImplSDLRenderer2_Init(Window::renderer);


		FlatEngine::sceneManager->LoadScene("SavedScenes.json");
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

		//Add viewports
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
		FlatEngine::FlatGui::RenderGameView();
		FlatEngine::FlatGui::RenderSceneView();
		FlatEngine::FlatGui::RenderLog();

		// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
		//if (show_demo_window)
		//	ImGui::ShowDemoWindow(&show_demo_window);
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
		ImGui::SameLine(0, 5);
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

		if (ImGui::Button("Save Scene"))
		{
			FlatEngine::sceneManager->SaveScene(FlatEngine::sceneManager->GetLoadedScene());
		}
		ImGui::SameLine(0, 5);
		if (ImGui::Button("Load Scene"))
		{
			FlatEngine::sceneManager->LoadScene("SavedScenes.json");
		}

		if (ImGui::Button("Create New Game Object"))
		{
			FlatEngine::sceneManager->GetLoadedScene()->CreateGameObject();
		}


		// Scene Objects
		{
			ImGui::PushStyleColor(ImGuiCol_FrameBg, innerWindowColor);
			ImGui::BeginListBox("##SceneObjects", ImVec2(-FLT_MIN, -FLT_MIN));
			ImGui::PopStyleColor();
				
			static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
			static bool align_label_with_current_x_position = false;
			static bool test_drag_and_drop = true;
			if (align_label_with_current_x_position)
				ImGui::Unindent(ImGui::GetTreeNodeToLabelSpacing());

			// 'selection_mask' is dumb representation of what may be user-side selection state.
			//  You may retain selection state inside or outside your objects in whatever format you see fit.
			// 'node_clicked' is temporary storage of what node we have clicked to process selection at the end
			/// of the loop. May be a pointer to your own node type, etc.
			//static int selection_mask = (1 << 2);
			static int selection_mask = -1;
			int node_clicked = -1;

			std::vector<GameObject*> sceneObjects = FlatEngine::sceneManager->GetLoadedScene()->GetSceneObjects();

			for (int i = 0; i < sceneObjects.size(); i++)
			{
				// Disable the default "open on single-click behavior" + set Selected flag according to our selection.
				// To alter selection we use IsItemClicked() && !IsItemToggledOpen(), so clicking on an arrow doesn't alter selection.
				ImGuiTreeNodeFlags node_flags = base_flags;
				//const bool is_selected = (selection_mask & (1 << i)) != 0;
				const bool is_selected = selection_mask == i != 0;
				// If this node is selected, use the nodeFlag_selected to highlight it
				if (is_selected)
					node_flags |= ImGuiTreeNodeFlags_Selected;

				FlatEngine::GameObject* currentObject = sceneObjects[i];
				std::string name = currentObject->GetName();
				const char* charName = name.c_str();

				if (currentObject->HasChildren())
				{
					bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, charName, i);
					if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
					{
						node_clicked = i;
						FlatEngine::SetFocusedGameObjectIndex(i);
					}

					if (test_drag_and_drop && ImGui::BeginDragDropSource())
					{
						ImGui::SetDragDropPayload("_TREENODE", NULL, 0);
						ImGui::Text("This is a drag and drop source");
						ImGui::EndDragDropSource();
					}
					if (node_open)
					{
						// Render SceneObject children
						ImGui::BulletText("Blah blah\nBlah Blah");
						ImGui::TreePop();
					}
				}
				else
				{
					node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; // ImGuiTreeNodeFlags_Bullet
					ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, charName, i);
					if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
					{
						node_clicked = i;
						FlatEngine::SetFocusedGameObjectIndex(i);
					}

					if (test_drag_and_drop && ImGui::BeginDragDropSource())
					{
						ImGui::SetDragDropPayload("_TREENODE", NULL, 0);
						ImGui::Text("This is a drag and drop source");
						ImGui::EndDragDropSource();
					}
				}
			}
			if (node_clicked != -1)
			{
				// Update selection state
				// (process outside of tree loop to avoid visual inconsistencies during the clicking frame)
				if (ImGui::GetIO().KeyCtrl)
				{
					//selection_mask ^= (1 << node_clicked);          // CTRL+click to toggle
					selection_mask = node_clicked;
				}
					
				else //if (!(selection_mask & (1 << node_clicked))) // Depending on selection behavior you want, may want to preserve selection when clicking on item that is part of the selection
				{
					//selection_mask = (1 << node_clicked);  // Click to single-select
					selection_mask = node_clicked;
				}
			}
			if (align_label_with_current_x_position)
				ImGui::Indent(ImGui::GetTreeNodeToLabelSpacing());

			ImGui::EndListBox();
		}
		
		ImGui::EndChild();
		ImGui::PopStyleColor();
		ImGui::End();
	}
	
	
	void FlatEngine::FlatGui::RenderInspector()
	{
		ImGui::Begin("Inspector Window");
		int focusedObjectIndex = FlatEngine::GetFocusedGameObjectIndex();

		ImGui::PushStyleColor(ImGuiCol_ChildBg, innerWindowColor);
		ImGuiChildFlags padding_child_flags = ImGuiChildFlags_::ImGuiChildFlags_AlwaysUseWindowPadding;
		ImGui::BeginChild("Inspector Background", ImVec2(0,0), padding_child_flags);

		if (focusedObjectIndex != -1)
		{
			// Get focused GameObject
			GameObject* focusedObject = FlatEngine::sceneManager->GetLoadedScene()->GetSceneObjects()[focusedObjectIndex];
			// Name variables and editing
			std::string nameLabel = "Name: ";
			char newName[1024];
			strcpy_s(newName, focusedObject->GetName().c_str());
			ImGuiInputTextFlags flags = ImGuiInputTextFlags_::ImGuiInputTextFlags_AutoSelectAll;

			ImGui::Text(nameLabel.c_str());
			ImGui::SameLine(0, 5);
			if (ImGui::InputText("##GameObject Name", newName, IM_ARRAYSIZE(newName), flags))
				focusedObject->SetName(newName);

			ImGui::Text("Components:");
			
			std::vector<Component*> components = focusedObject->GetComponents();
			if (components.size() > 0)
			{
				for (int i = 0; i < components.size(); i++)
				{
					std::string componentType = components[i]->GetTypeString();
					ImGui::SeparatorText(componentType.c_str());

					ImGuiChildFlags child_flags = ImGuiChildFlags_::ImGuiChildFlags_AlwaysUseWindowPadding | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_::ImGuiChildFlags_AlwaysAutoResize;
					ImGui::PushStyleColor(ImGuiCol_ChildBg, singleItemColor);

					ImGui::BeginChild(componentType.c_str(), ImVec2(0, 0), child_flags);

					if (componentType == "Transform")
					{
						// Position, scale, and rotation of transform
						FlatEngine::Transform *transform = static_cast<FlatEngine::Transform*>(components[i]);
						Vector2 position = transform->GetPosition();
						float xPos = position.x;
						float yPos = position.y;
						Vector2 scale = transform->GetScale();
						float scaleX = scale.x;
						float scaleY = scale.y;
						float rotation = transform->GetRotation();
						std::string rotationString = "rotation: " + std::to_string(rotation);
						static ImGuiSliderFlags flags = ImGuiSliderFlags_::ImGuiSliderFlags_None;

						// Drags for position editing
						ImGui::Text("xPos:");
						ImGui::DragFloat("##xPos", &xPos, 0.5f, -FLT_MAX, -FLT_MAX, "%.3f", flags);
						ImGui::Text("yPos:");
						ImGui::DragFloat("##yPos", &yPos, 0.5f, -FLT_MAX, -FLT_MAX, "%.3f", flags);
						ImGui::Text(rotationString.c_str());
						// Assign the new slider values to the transforms position
						transform->SetPosition(Vector2(xPos, yPos));

						// Drags for scale of transform
						ImGui::Text("Scale x:");
						ImGui::DragFloat("##xScale", &scaleX, 0.5f, 0, -FLT_MAX, "%.3f", flags);
						ImGui::Text("Scale y:");
						ImGui::DragFloat("##yScale", &scaleY, 0.5f, -FLT_MAX, -FLT_MAX, "%.3f", flags);
						// Assign the new slider values to the sprites pivotPoint
						transform->SetScale(Vector2(scaleX, scaleY));
					}
					else if (componentType == "Sprite")
					{
						// Sprite path and texture dimensions
						FlatEngine::Sprite *sprite = static_cast<FlatEngine::Sprite*>(components[i]);
						std::string path = sprite->GetPath();
						char newPath[1024];
						strcpy_s(newPath, path.c_str());
						float textureWidth = sprite->GetTextureWidth();
						float textureHeight = sprite->GetTextureHeight();
						std::string pathString = "Path: ";
						std::string textureWidthString = "Texture width: " + std::to_string(textureWidth);
						std::string textureHeightString = "Texture height: " + std::to_string(textureHeight);

						ImGui::Text(pathString.c_str());
						ImGui::SameLine(0,5);
						if (ImGui::InputText("##spritePath", newPath, IM_ARRAYSIZE(newPath), flags))
							sprite->SetTexture(newPath);
						ImGui::Text(textureWidthString.c_str());
						ImGui::Text(textureHeightString.c_str());

						// Sprite offset
						Vector2 offset = sprite->GetOffset();
						float xOffset = offset.x;
						float yOffset = offset.y;
						ImGuiSliderFlags flags = ImGuiSliderFlags_::ImGuiSliderFlags_None;

						// Drags for offset of texture editing
						ImGui::Text("xOffset:");
						ImGui::DragFloat("x Offset", &xOffset, 0.5f, -FLT_MAX, -FLT_MAX, "%.3f", flags);
						ImGui::Text("yOffset:");
						ImGui::DragFloat("y Offset", &yOffset, 0.5f, -FLT_MAX, -FLT_MAX, "%.3f", flags);
						// Assign the new slider values to the sprites pivotPoint
						sprite->SetOffset(Vector2(xOffset, yOffset));
					}
					else if (componentType == "Camera")
					{
						// Camera position and aspect ratio
						FlatEngine::Camera* camera = static_cast<FlatEngine::Camera*>(components[i]);
						float width = camera->GetWidth();
						float height = camera->GetHeight();
						bool _isPrimary = camera->IsPrimary();

						// Drags for camera width and height editing
						ImGui::Text("Camera width:");
						ImGui::DragFloat("##CameraWidth", &width, 0.5f, 0, -FLT_MAX, "%.3f", flags);
						ImGui::Text("Camera height:");
						ImGui::DragFloat("##CameraHeight", &height, 0.5f, 0, -FLT_MAX, "%.3f", flags);

						// Assign the new slider values
						camera->SetDimensions(width, height);

						// Aspect ratio and _isPrimaryCamera checkbox
						float truncatedWidth = trunc(width);
						float truncatedHeight = trunc(height);
						std::string aspectRatioString = "Aspect Ratio: " + std::to_string(truncatedWidth) + ":" + std::to_string(truncatedHeight);
						ImGui::Text(aspectRatioString.c_str());

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
						FlatEngine::ScriptComponent* script = static_cast<FlatEngine::ScriptComponent*>(components[i]);
						std::string path = script->GetAttachedScript();
						bool _isActive = script->IsActive();

						// For path editing
						char newPath[1024];
						strcpy_s(newPath, path.c_str());
						std::string pathString = "Name: ";
						ImGui::Text(pathString.c_str());
						ImGui::SameLine(0, 5);
						ImGuiSliderFlags flags = ImGuiSliderFlags_::ImGuiSliderFlags_None;
						std::string inputId = "##scriptName_" + std::to_string(i);
						if (ImGui::InputText(inputId.c_str(), newPath, IM_ARRAYSIZE(newPath), flags))
							script->SetAttachedScript(newPath);

						// _isActive checkbox
						std::string checkboxId = "Active##" + std::to_string(i);
						ImGui::Checkbox(checkboxId.c_str(), &_isActive);
						script->SetActive(_isActive);
					}

					ImGui::PopStyleColor();
					ImGui::EndChild();
				}
			}

			// For checking if SceneObject has these components yet
			FlatEngine::Component* transformComponent = focusedObject->GetComponent(Component::ComponentTypes::Transform);
			FlatEngine::Component* spriteComponent = focusedObject->GetComponent(Component::ComponentTypes::Sprite);
			FlatEngine::Component* cameraComponent = focusedObject->GetComponent(Component::ComponentTypes::Camera);

			if (transformComponent == nullptr)
			{
				if (ImGui::Button("Add Transform Component"))
				{
					focusedObject->AddComponent(Component::ComponentTypes::Transform);
				}
			}

			if (spriteComponent == nullptr)
			{
				if (ImGui::Button("Add Sprite Component"))
				{
					focusedObject->AddComponent(Component::ComponentTypes::Sprite);
				}
			}

			if (cameraComponent == nullptr)
			{
				if (ImGui::Button("Add Camera Component"))
				{
					focusedObject->AddComponent(Component::ComponentTypes::Camera);
				}
			}

			if (ImGui::Button("Add Script Component"))
			{
				focusedObject->AddComponent(Component::ComponentTypes::Script);
			}


			if (ImGui::Button("Delete GameObject"))
			{
				// Unfocus GameObject first
				int tempIndex = focusedObjectIndex;
				FlatEngine::SetFocusedGameObjectIndex(-1);
				FlatEngine::sceneManager->GetLoadedScene()->DeleteGameObject(tempIndex);
			}
		}
		ImGui::PopStyleColor();
		ImGui::EndChild();

		ImGui::End();
	}


	void FlatEngine::FlatGui::RenderGameView()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

		ImGui::Begin("Game");

		FlatEngine::Camera* primaryCamera = FlatEngine::GetLoadedScene()->GetPrimaryCamera();
		FlatEngine::Transform* cameraTransform = nullptr;
		ImVec2 cameraPosition(0, 0);
		float cameraWidth = 50;
		float cameraHeight = 30;

		// If the primaryCamera is found and not nullptr, set the cameraPosition accordingly, else it remains at {0,0} above
		if (primaryCamera != nullptr)
		{
			cameraTransform = static_cast<FlatEngine::Transform*>(FlatEngine::GetObjectComponent(primaryCamera->GetParentID(), ComponentTypes::Transform));
			cameraWidth = primaryCamera->GetWidth();
			cameraHeight = primaryCamera->GetHeight();
		}
		if (cameraTransform != nullptr)
		{
			cameraPosition = { cameraTransform->GetPosition().x, cameraTransform->GetPosition().y };
		}

		static bool opt_enable_context_menu = true;

		ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();  
		ImVec2 canvas_sz = ImGui::GetContentRegionAvail();
		if (canvas_sz.x < 50.0f) canvas_sz.x = 50.0f;
		if (canvas_sz.y < 50.0f) canvas_sz.y = 50.0f;
		ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);

		// Set viewport dimensions for rendering objects in game view. We want this to always be centered in game view so we can get it every frame.
		GAME_VIEWPORT_WIDTH = canvas_p1.x - canvas_p0.x + 1;
		GAME_VIEWPORT_HEIGHT = canvas_p1.y - canvas_p0.y + 1;

		// Get InputOutput
		ImGuiIO& inputOutput = ImGui::GetIO();

		// This will catch our interactions
		ImGui::InvisibleButton("canvas", canvas_sz, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
		const bool is_hovered = ImGui::IsItemHovered(); // Hovered
		const bool is_active = ImGui::IsItemActive();   // Held

		// Render GameObjects in game view
		FlatEngine::FlatGui::RenderGameObjects(cameraPosition, cameraWidth, cameraHeight, canvas_p0, canvas_sz);

		ImGui::End();

		// Reset WindowPadding
		ImGui::PopStyleVar();
		// Reset WindowBorder
		ImGui::PopStyleVar();
	}


	void FlatEngine::FlatGui::RenderGameObjects(ImVec2 cameraPosition, float cameraWidth, float cameraHeight, ImVec2 canvas_p0, ImVec2 canvas_sz)
	{
		Scene* loadedScene = FlatEngine::sceneManager->GetLoadedScene();
		std::vector<GameObject*> sceneObjects = loadedScene->GetSceneObjects();
		ImDrawList *draw_list = ImGui::GetWindowDrawList();

		// Create the splitter for the draw_list
		ImDrawListSplitter* drawSplitter = new ImDrawListSplitter();
		// 3 channels for now in this scene view. 0 = scene objects, 1 = other UI (camera icon, etc), 2 = transform arrow
		drawSplitter->Split(draw_list, 3);

		// To be implemented with cameras
		float cameraZoom = 10;

		// Get the "center point" of the games view. This will appear to move around when we move the camera
		ImVec2 centerPoint = ImVec2((GAME_VIEWPORT_WIDTH / 2) - cameraPosition.x + canvas_p0.x, (GAME_VIEWPORT_HEIGHT / 2) + cameraPosition.y + canvas_p0.y);

		// Go through scene objects
		for (int i = 0; i < sceneObjects.size(); i++)
		{
			// Get Components
			FlatEngine::Component* transformComponent = sceneObjects[i]->GetComponent(Component::ComponentTypes::Transform);
			FlatEngine::Component* spriteComponent = sceneObjects[i]->GetComponent(Component::ComponentTypes::Sprite);

			// Check if each object has a Transform component
			if (transformComponent != nullptr)
			{
				int focusedObjectIndex = FlatEngine::GetFocusedGameObjectIndex();
				FlatEngine::Transform* transformCasted = static_cast<Transform*>(transformComponent);
				Vector2 position = transformCasted->GetPosition();
				Vector2 scale = transformCasted->GetScale();				

				// If it has a sprite component, render that sprite texture at the objects transform position
				if (spriteComponent != nullptr)
				{
					// Cast the components to their respective types
					FlatEngine::Sprite* spriteCasted = static_cast<Sprite*>(spriteComponent);

					SDL_Texture* spriteTexture = spriteCasted->GetTexture();
					float textureWidth = (float)spriteCasted->GetTextureWidth();
					float textureHeight = (float)spriteCasted->GetTextureHeight();
					Vector2 offset = spriteCasted->GetOffset();
					bool _scalesWithZoom = true;
	

					// Think about how we actually want to check if things are within the camera view. 
					// Going to have to probably use the center point of the x and y axis. Even if we don't use the same center point, which I think we could maybe??
					// the relative values will be the same and can still be used to calculate the overlaps.
					// can probably just save the values of the primary camera in FlatEngine from rendering cameras in the scene view
					// and use that here. Just then have to check if the game objects are within that view. Probably not going to use any
					// math from the game view, just use it for visually checking that it works. Can at least copy the methods used to get it.
					// 
					// 
					// 
					// For checking if scene objects are within the frustrum
					float spriteLeftEdge = centerPoint.x + ((position.x - offset.x * scale.x) * cameraZoom);
					float spriteRightEdge = centerPoint.x + ((position.x + offset.x * scale.x) * cameraZoom);
					float spriteTopEdge = centerPoint.y - ((position.y + offset.y * scale.y) * cameraZoom);
					float spriteBottomEdge = centerPoint.y - ((position.y - offset.y * scale.y) * cameraZoom);

					float cameraLeftEdge = WorldToViewport(centerPoint.x, cameraPosition.x - cameraWidth / 2, cameraZoom);
					float cameraRightEdge = WorldToViewport(centerPoint.x, cameraPosition.x + cameraWidth / 2, cameraZoom);
					float cameraTopEdge = WorldToViewport(centerPoint.y, cameraPosition.y + cameraHeight / 2, cameraZoom); 
					float cameraBottomEdge = WorldToViewport(centerPoint.y, cameraPosition.y - cameraHeight / 2, cameraZoom);

					//float cameraLeftEdge = cameraPosition.x - cameraWidth / 2;
					//float cameraRightEdge = cameraPosition.x + cameraWidth / 2;
					//float cameraTopEdge = cameraPosition.y + cameraHeight / 2;
					//float cameraBottomEdge = cameraPosition.y - cameraHeight / 2;


					FlatEngine::LogFloat(spriteLeftEdge, "Sprite left edge: ");
					FlatEngine::LogFloat(cameraLeftEdge, "Camera left edge: ");
					drawSplitter->SetCurrentChannel(draw_list, 2);
					FlatEngine::DrawRectangle(Vector2(cameraLeftEdge, cameraTopEdge), Vector2(cameraRightEdge, cameraBottomEdge), canvas_p0, canvas_sz, white, 2.0f, draw_list);
					FlatEngine::DrawRectangle(Vector2(spriteLeftEdge, spriteTopEdge), Vector2(spriteRightEdge, spriteBottomEdge), canvas_p0, canvas_sz, white, 2.0f, draw_list);

					bool _isIntersecting = false;

					if (spriteLeftEdge < cameraRightEdge && spriteRightEdge > cameraLeftEdge &&
						spriteTopEdge > cameraBottomEdge && spriteBottomEdge < cameraTopEdge)
						_isIntersecting = true;

					if (true)
					{
						drawSplitter->SetCurrentChannel(draw_list, 0);
						FlatEngine::FlatGui::AddImageToDrawList(spriteTexture, position, centerPoint, textureWidth, textureHeight, offset, scale, _scalesWithZoom, cameraZoom, draw_list);
					}
				}
			}
		}

		// Render primary camera frustrum
		Texture* frustrum = new Texture();
		frustrum->loadFromFile("assets/images/frustrum.png");
		float horizontalFrustrumWidth = GAME_VIEWPORT_WIDTH;
		float horizontalFrustrumHeight = (GAME_VIEWPORT_HEIGHT - cameraHeight) / 2;
		float verticalFrustrumWidth = (GAME_VIEWPORT_WIDTH - cameraWidth) / 2;
		float verticalFrustrumHeight = GAME_VIEWPORT_HEIGHT;
		SDL_Texture* frustrumTexture = frustrum->getTexture();
		Vector2 topFrustrumOffset = Vector2(GAME_VIEWPORT_WIDTH / 2, 0);
		Vector2 bottomFrustrumOffset = Vector2(GAME_VIEWPORT_WIDTH / 2, (cameraHeight / 2 + horizontalFrustrumHeight));
		Vector2 leftFrustrumOffset = Vector2(0, verticalFrustrumHeight / 2);
		Vector2 rightFrustrumOffset = Vector2(0, verticalFrustrumHeight / 2);

		// Set drawing channel to 2 for top layer camera UI
		drawSplitter->SetCurrentChannel(draw_list, 2);

		// Horizontal Frustrums
		//FlatEngine::FlatGui::AddImageToDrawList(frustrumTexture, Vector2(cameraPosition.x, cameraPosition.y + (cameraHeight / 2)), centerPoint, horizontalFrustrumWidth, horizontalFrustrumHeight, topFrustrumOffset, Vector2(1, 1), false, draw_list);
		//FlatEngine::FlatGui::AddImageToDrawList(frustrumTexture, Vector2(cameraPosition.x, cameraPosition.y), centerPoint, horizontalFrustrumWidth, horizontalFrustrumHeight, bottomFrustrumOffset, Vector2(1, 1), false, draw_list);
		//// Vertical Frustrums
		//FlatEngine::FlatGui::AddImageToDrawList(frustrumTexture, Vector2(cameraPosition.x + cameraWidth / 2, cameraPosition.y), centerPoint, verticalFrustrumWidth, verticalFrustrumHeight, leftFrustrumOffset, Vector2(1, 1), false, draw_list);
		//FlatEngine::FlatGui::AddImageToDrawList(frustrumTexture, Vector2(cameraPosition.x - cameraWidth / 2 - verticalFrustrumWidth, cameraPosition.y), centerPoint, verticalFrustrumWidth, verticalFrustrumHeight, rightFrustrumOffset, Vector2(1, 1), false, draw_list);

		drawSplitter->Merge(draw_list);
	}


	void FlatEngine::FlatGui::RenderSceneView()
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
		ImGui::InvisibleButton("canvas", canvas_sz, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
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

		FlatEngine::FlatGui::RenderSceneGrid(scrolling, canvas_p0, canvas_p1, canvas_sz, gridStep);
	
		FlatEngine::FlatGui::RenderSceneObjects(scrolling, canvas_p0, canvas_sz);

		// Reset WindowPadding
		ImGui::PopStyleVar();
		// Reset WindowBorder
		ImGui::PopStyleVar();

		ImGui::End();
	}


	void FlatEngine::FlatGui::RenderSceneObjects(ImVec2 scrolling, ImVec2 canvas_p0, ImVec2 canvas_sz)
	{
		Scene* loadedScene = FlatEngine::sceneManager->GetLoadedScene();
		std::vector<GameObject*> sceneObjects = loadedScene->GetSceneObjects();

		// Split our drawlist into multiple channels for different rendering orders
		ImDrawList *draw_list = ImGui::GetWindowDrawList();
		ImDrawListSplitter* drawSplitter = new ImDrawListSplitter();

		// 4 channels for now in this scene view. 0 = scene objects, 1 & 2 = other UI (camera icon, etc), 4 = transform arrow
		drawSplitter->Split(draw_list, 4);

		// Loop through scene objects
		for (int i = 0; i < sceneObjects.size(); i++)
		{
			FlatEngine::Component* transformComponent = sceneObjects[i]->GetComponent(ComponentTypes::Transform);
			FlatEngine::Component* spriteComponent = sceneObjects[i]->GetComponent(ComponentTypes::Sprite);
			FlatEngine::Component* cameraComponent = sceneObjects[i]->GetComponent(ComponentTypes::Camera);

			// Check if each object has a Transform component
			if (transformComponent != nullptr)
			{
				int focusedObjectIndex = FlatEngine::GetFocusedGameObjectIndex();
				FlatEngine::Transform* transformCasted = static_cast<Transform*>(transformComponent);
				Vector2 position = transformCasted->GetPosition();
				Vector2 transformScale = transformCasted->GetScale();

				// If it has a sprite component, render that sprite texture at the objects transform position
				if (spriteComponent != nullptr)
				{
					// Cast the component to sprite*
					FlatEngine::Sprite* spriteCasted = static_cast<Sprite*>(spriteComponent);
					SDL_Texture* spriteTexture = spriteCasted->GetTexture();
					float spriteTextureWidth = (float)spriteCasted->GetTextureWidth();
					float spriteTextureHeight = (float)spriteCasted->GetTextureHeight();
					Vector2 spriteOffset = spriteCasted->GetOffset();
					bool _spriteScalesWithZoom = true;

					// Channel 0 for scene objects
					drawSplitter->SetCurrentChannel(draw_list, 0);
					// Draw the texture
					FlatEngine::FlatGui::AddImageToDrawList(spriteTexture, position, scrolling, spriteTextureWidth, spriteTextureHeight, spriteOffset, transformScale, _spriteScalesWithZoom, gridStep, draw_list);
				}

				// Renders Transform Arrow //
				// If a sceneObject is focused and the currently focused object is the same as this loop iteration,
				// render the focused objects TransformArrow for moving it within the scene view
				if (focusedObjectIndex != -1 && focusedObjectIndex == i)
				{
					// Get focused GameObject and transformArrow png
					GameObject* focusedObject = FlatEngine::sceneManager->GetLoadedScene()->GetSceneObjects()[focusedObjectIndex];
					transformArrow = new Texture("assets/images/transformArrow.png");

					SDL_Texture* texture = transformArrow->getTexture();
					// * 3 because the texture is so small. If we change the scale, it will change the render starting
					// position, which we don't want. We only want to change the render ending position so we adjust dimensions only
					float arrowWidth = (float)transformArrow->getWidth() * 3;
					float arrowHeight = (float)transformArrow->getHeight() * 3;
					Vector2 arrowScale = { 1, 1 };
					Vector2 arrowOffset = { 0, arrowHeight };
					bool _scalesWithZoom = false;

					// Channel 2 for Upper UI Transform Arrow
					drawSplitter->SetCurrentChannel(draw_list, 3);
					FlatEngine::FlatGui::AddImageToDrawList(texture, position, scrolling, arrowWidth, arrowHeight, arrowOffset, arrowScale, _scalesWithZoom, gridStep, draw_list, IM_COL32(255, 255, 255, 255));
				}

				// Renders the camera
				if (cameraComponent != nullptr)
				{
					FlatEngine::Camera* camera = static_cast<FlatEngine::Camera*>(cameraComponent);
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

					cameraTexture = new Texture("assets/images/camera.png");
					SDL_Texture* texture = cameraTexture->getTexture();
					float cameraTextureWidth = (float)cameraTexture->getWidth() * 3;
					float cameraTextureHeight = (float)cameraTexture->getHeight() * 3;
					bool _scalesWithZoom = false;
					Vector2 cameraTextureOffset = { cameraTextureWidth / 2, cameraTextureHeight / 2 };
					Vector2 cameraTextureScale = { 1, 1 };
					Vector2 offsetPosition = Vector2(position.x - cameraTextureWidth / 2, position.y + cameraTextureHeight / 2);

					// Channel 2 for Lower UI
					drawSplitter->SetCurrentChannel(draw_list, 2);

					// Draw a rectangle to the scene view to represent the camera frustrum
					FlatEngine::DrawRectangle(topLeftCorner, bottomRightCorner, canvas_p0, canvas_sz, IM_COL32(255, 30, 30, 70), 2.0f, draw_list);
					FlatEngine::DrawLine(topLeftCorner, bottomRightCorner, IM_COL32(255, 30, 30, 70), 2.0f, draw_list);
					FlatEngine::DrawLine(topRightCorner, bottomLeftCorner, IM_COL32(255, 30, 30, 70), 2.0f, draw_list);

					// Draw actual camera icon
					FlatEngine::FlatGui::AddImageToDrawList(texture, position, scrolling, cameraTextureWidth, cameraTextureHeight, cameraTextureOffset, cameraTextureScale, _scalesWithZoom, gridStep, draw_list, IM_COL32(255, 255, 255, iconTransparency));
				}
			}
		}
	
		drawSplitter->Merge(draw_list);
		delete drawSplitter;
		drawSplitter = nullptr;
	}


	// Get a value from world/grid position converted into viewport position.
	// Just add - canvas_p0 to get Window coordinates
	float FlatEngine::FlatGui::WorldToViewport(float centerPoint, float worldPosition, float zoomFactor)
	{
		return centerPoint + (worldPosition * zoomFactor);
	}

	//float FlatEngine::FlatGui::WorldToViewport(float centerPoint, )
	//{
	//	return centerPoint.x + ((position.x - offset.x * scale.x));
	//}
	

	void FlatEngine::FlatGui::RenderSceneGrid(ImVec2 scrolling, ImVec2 canvas_p0, ImVec2 canvas_p1, ImVec2 canvas_sz, float gridStep)
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

		// X = 0 starts the drawing at the left most edge of the whole app window.
		for (float x = trunc(fmodf(scrolling.x, gridStep)); x < canvas_p0.x + canvas_sz.x; x += gridStep)
		{
			FlatEngine::DrawLine(ImVec2(x, canvas_p0.y), ImVec2(x, canvas_p1.y), IM_COL32(200, 200, 200, 40), 1.0f, draw_list);
		}

		for (float y = trunc(fmodf(scrolling.y, gridStep)); y < canvas_p0.y + canvas_sz.y; y += gridStep)
		{
			FlatEngine::DrawLine(ImVec2(canvas_p0.x, y), ImVec2(canvas_p1.x, y), IM_COL32(200, 200, 200, 40), 1.0f, draw_list);
		}

		float midPointX = trunc(SCENE_VIEWPORT_WIDTH / gridStep / 2) * gridStep;
		float midPointY = trunc(SCENE_VIEWPORT_HEIGHT / gridStep / 2) * gridStep;

		// Draw our x and y axis
		float divX = trunc(scrolling.x / gridStep);
		float modX = fmodf(scrolling.x, gridStep);
		float offsetX = (gridStep * divX) + modX;
		float divY = trunc(scrolling.y / gridStep);
		float modY = fmodf(scrolling.y, gridStep);
		float offsetY = (gridStep * divY) + modY;

		ImU32 xColor = IM_COL32(1, 210, 35, 255);
		ImU32 yColor = IM_COL32(1, 1, 255, 255);

		// x axis bounds check + color change if out of bounds
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
		// y axis bounds check + color change if out of bounds
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
		FlatEngine::DrawPoint(ImVec2(scrolling.x, scrolling.y), IM_COL32(255, 1, 247, 255), draw_list);
	}


	void FlatEngine::FlatGui::AddImageToDrawList(SDL_Texture *texture, Vector2 position, ImVec2 centerPoint, float textureWidth, float textureHeight, Vector2 offset, Vector2 scale, bool _scalesWithZoom, float zoomMultiplier, ImDrawList *draw_list, ImU32 addColor)
	{
		// Changing the scale here because things are rendering too large and I want them to start off smaller
		Vector2 newScale = Vector2(scale.x * 0.2f, scale.y * 0.2f);

		/*float scalingXStart = centerPoint.x + ((position.x - (offset.x * newScale.x)) * zoomMultiplier);
		float scalingYStart = centerPoint.y + ((-position.y - (offset.y * newScale.y)) * zoomMultiplier);
		float scalingXEnd = scalingXStart + ((textureWidth * newScale.x) * zoomMultiplier);
		float scalingYEnd = scalingYStart + ((textureHeight * newScale.y) * zoomMultiplier);*/

		float scalingXStart = WorldToViewport(centerPoint.x, position.x - (offset.x * newScale.x), zoomMultiplier);
		float scalingYStart = WorldToViewport(centerPoint.y, -position.y - (offset.y * newScale.y), zoomMultiplier);
		float scalingXEnd = WorldToViewport(scalingXStart, textureWidth * newScale.x, zoomMultiplier);
		float scalingYEnd = WorldToViewport(scalingYStart, textureHeight * newScale.y, zoomMultiplier);

		float unscaledXStart = WorldToViewport(centerPoint.x, position.x, zoomMultiplier) - offset.x;
		float unscaledYStart = WorldToViewport(centerPoint.y, -position.y, zoomMultiplier) - offset.y;

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
			renderEnd = ImVec2(renderStart.x + textureWidth, renderStart.y + textureHeight);
		}

		// Render sprite to viewport
		draw_list->AddImage((void*)texture, renderStart, renderEnd, UvStart, UvEnd, addColor);
	}


	void FlatEngine::FlatGui::RenderLog()
	{
		ImGui::Begin("Debug Log");
		
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

		FlatEngine::logger->ClearBuffer();
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