#include "FlatEngine.h"
#include "Transform.h"
#include "Camera.h"
#include "Scene.h"
#include "Sprite.h"
#include "Text.h"
#include "BoxCollider.h"
#include "imgui_internal.h"

namespace FlatEngine { namespace FlatGui {

	// Scene view
	// The multiplier for sceneViewGridStep. Used to convert grid space values to pixel values. ie. 2 grid squares = 2 * 10 = 20px.
	Vector2 sceneViewGridStep = Vector2(50,50);
	float SCENE_VIEWPORT_WIDTH = 600;
	float SCENE_VIEWPORT_HEIGHT = 400;
	float DYNAMIC_VIEWPORT_WIDTH = 600;
	float DYNAMIC_VIEWPORT_HEIGHT = 400;
	bool _firstSceneRenderPass = true;
	bool _sceneHasBeenSet = false;
	Vector2 sceneViewScrolling = Vector2(0,0);
	Vector2 sceneViewCenter = Vector2(0, 0);
	bool _sceneViewLockedOnObject = false;
	std::shared_ptr<GameObject> sceneViewLockedObject = nullptr;

	void Scene_RenderView()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, Vector2(0, 0));

		PushWindowStyles();
		ImGui::Begin("Scene View", 0, 16 | 8);
		PopWindowStyles();

		Vector2 canvas_p0 = ImGui::GetCursorScreenPos();
		Vector2 canvas_sz = ImGui::GetContentRegionAvail();
		if (canvas_sz.x < 50.0f) canvas_sz.x = 50.0f;
		if (canvas_sz.y < 50.0f) canvas_sz.y = 50.0f;
		Vector2 canvas_p1 = Vector2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);

		// Set initial viewport dimensions for rendering scene view grid and objects
		if (!_firstSceneRenderPass)
		{
			if (!_sceneHasBeenSet)
			{
				SCENE_VIEWPORT_WIDTH = canvas_sz.x;
				SCENE_VIEWPORT_HEIGHT = canvas_sz.y;
				_sceneHasBeenSet = true;
			}
		}
		_firstSceneRenderPass = false;

		// For calculating scrolling mouse position and what vector to zoom to
		DYNAMIC_VIEWPORT_WIDTH = trunc(canvas_p1.x - canvas_p0.x);
		DYNAMIC_VIEWPORT_HEIGHT = trunc(canvas_p1.y - canvas_p0.y);

		// Get Input and Output
		ImGuiIO& inputOutput = ImGui::GetIO();
		Vector2 currentPos = ImGui::GetCursorScreenPos();
		Vector2 centerOffset = Vector2(SCENE_VIEWPORT_WIDTH / 2, SCENE_VIEWPORT_HEIGHT / 2);
		bool _weightedScroll = true;

		// This will catch our interactions
		ImGui::SetCursorScreenPos(currentPos);
		ImGui::SetNextItemAllowOverlap();
		ImGui::InvisibleButton("SceneViewCanvas", canvas_sz, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight | 4096);
		const bool is_hovered = ImGui::IsItemHovered(); // Hovered
		const bool is_active = ImGui::IsItemActive();   // Held

		RenderGridView(sceneViewCenter, sceneViewScrolling, _weightedScroll, canvas_p0, canvas_p1, canvas_sz, sceneViewGridStep, centerOffset);

		// Get currently loaded scene objects
		std::shared_ptr<Scene> loadedScene = FlatEngine::sceneManager->GetLoadedScene();
		std::vector<std::shared_ptr<GameObject>> sceneObjects;

		if (loadedScene != nullptr)
			sceneObjects = loadedScene->GetSceneObjects();
		else
			sceneObjects = std::vector<std::shared_ptr<GameObject>>();

		RenderViewObjects(sceneObjects, sceneViewCenter, canvas_p0, canvas_sz, sceneViewGridStep.x);

		// For panning the scene view
		const float mouse_threshold_for_pan = 0.0f;
		if (is_active && ImGui::IsMouseDragging(ImGuiMouseButton_Right, mouse_threshold_for_pan))
		{
			sceneViewScrolling.x += inputOutput.MouseDelta.x;
			sceneViewScrolling.y += inputOutput.MouseDelta.y;
		}

		// Show cursor position in scene view when pressing Alt
		if (is_hovered && inputOutput.KeyAlt)
			RenderSceneViewTooltip();

		if (_sceneViewLockedOnObject && sceneViewLockedObject != nullptr)
		{
			std::shared_ptr<Transform> transform = sceneViewLockedObject->GetTransformComponent();
			Vector2 position = transform->GetTruePosition();
			sceneViewScrolling = Vector2(position.x * -sceneViewGridStep.x + (ImGui::GetWindowWidth() / 2), position.y * sceneViewGridStep.y + (ImGui::GetWindowHeight() / 2));
		}

		Vector2 adjustedScrolling = Vector2(sceneViewScrolling.x + centerOffset.x, sceneViewScrolling.y + centerOffset.y);

		// Get scroll amount for changing zoom level of scene view
		Vector2 mousePos = Vector2(inputOutput.MousePos.x, inputOutput.MousePos.y);
		float scrollInput = inputOutput.MouseWheel;
		float weight = 0.01f;
		float signedMousePosX = mousePos.x - canvas_p0.x - (DYNAMIC_VIEWPORT_WIDTH / 2);
		float signedMousePosY = mousePos.y - canvas_p0.y - (DYNAMIC_VIEWPORT_HEIGHT / 2);
		float zoomSpeed = 0.1f;
		float zoomMultiplier = 10;
		float finalZoomSpeed = zoomSpeed;
		
		if (inputOutput.KeyCtrl)
			finalZoomSpeed *= zoomMultiplier;

		// Change scrolling offset based on mouse position and weight
		if (is_hovered)
		{
			if (scrollInput > 0)
			{
				if (_weightedScroll)
				{
					sceneViewScrolling.x -= trunc(signedMousePosX * weight);
					sceneViewScrolling.y -= trunc(signedMousePosY * weight);
				}
				sceneViewGridStep.x += finalZoomSpeed;
				sceneViewGridStep.y += finalZoomSpeed;
			}
			else if (scrollInput < 0 && sceneViewGridStep.x > 2 && sceneViewGridStep.y > 2)
			{
				if (_weightedScroll)
				{
					sceneViewScrolling.x += trunc(signedMousePosX * weight);
					sceneViewScrolling.y += trunc(signedMousePosY * weight);
				}
				sceneViewGridStep.x -= finalZoomSpeed;
				sceneViewGridStep.y -= finalZoomSpeed;
			}
		}

		// Reset WindowPadding
		ImGui::PopStyleVar();
		// Reset WindowBorder
		ImGui::PopStyleVar();

		ImGui::End();
	}

	void Scene_RenderObject(std::shared_ptr<GameObject> self, Vector2 scrolling, Vector2 canvas_p0, Vector2 canvas_sz, float step, ImDrawList* draw_list, ImDrawListSplitter* drawSplitter)
	{
		std::shared_ptr<Transform> transform = self->GetTransformComponent();
		std::shared_ptr<Sprite> sprite = self->GetSpriteComponent();
		std::shared_ptr<Camera> camera = self->GetCameraComponent();
		std::shared_ptr<Button> button = self->GetButtonComponent();
		std::shared_ptr<Canvas> canvas = self->GetCanvasComponent();
		std::shared_ptr<Text> text = self->GetTextComponent();
		std::shared_ptr<BoxCollider> boxCollider = self->GetBoxCollider();

		// Check if each object has a Transform component
		if (transform != nullptr)
		{
			long focusedObjectID = FlatEngine::GetFocusedGameObjectID();
			Vector2 position = transform->GetTruePosition();
			Vector2 transformScale = transform->GetScale();
			float rotation = transform->GetRotation();
			Vector2 scale = transform->GetScale();

			// If it has a sprite component, render that sprite texture at the objects transform position with offsets
			if (sprite != nullptr)
			{
				SDL_Texture* spriteTexture = sprite->GetTexture();
				float spriteTextureWidth = (float)sprite->GetTextureWidth();
				float spriteTextureHeight = (float)sprite->GetTextureHeight();
				Vector2 spriteOffset = sprite->GetOffset();
				bool _spriteScalesWithZoom = true;
				int renderOrder = sprite->GetRenderOrder();
				std::string invisibleButtonID = "GameObjectSelectorButton_" + std::to_string(sprite->GetID());

				if (spriteTexture != nullptr)
				{
					// Get Input and Output
					ImGuiIO& inputOutput = ImGui::GetIO();

					Vector2 positionOnScreen = Vector2(sceneViewCenter.x - canvas_p0.x + (position.x * step) - ((spriteOffset.x * spriteScaleMultiplier * step) * scale.x), sceneViewCenter.y - canvas_p0.y - (position.y * step - 20) - ((spriteOffset.y * spriteScaleMultiplier * step) * scale.y));
					ImGui::SetCursorPos(positionOnScreen);
					//// This will catch our interactions  - 4096 for overlap or keyword if it works
					ImGui::SetNextItemAllowOverlap();
					ImGui::InvisibleButton(invisibleButtonID.c_str(), Vector2(spriteTextureWidth * spriteScaleMultiplier * step * scale.x, spriteTextureHeight * spriteScaleMultiplier * step * scale.y), ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
					const bool is_hovered = ImGui::IsItemHovered(); // Hovered
					const bool is_active = ImGui::IsItemActive();   // Held
					const bool is_clicked = ImGui::IsItemClicked();

					if (is_hovered || is_active)
						ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

					if (is_clicked)
						SetFocusedGameObjectID(sprite->GetParentID());

					// Show cursor position in scene view when pressing Alt
					if (is_hovered && inputOutput.KeyAlt)
						RenderSceneViewTooltip();

					// Add the same behavior as the sceneview grid so pan and zoom behaviors are not disabled when view entirely obstructed by sprite
					////////////////////////
					if (is_active && ImGui::IsMouseDragging(ImGuiMouseButton_Right))
					{
						sceneViewScrolling.x += inputOutput.MouseDelta.x;
						sceneViewScrolling.y += inputOutput.MouseDelta.y;
					}
					// Get scroll amount for changing zoom level of scene view
					Vector2 mousePos = Vector2(inputOutput.MousePos.x, inputOutput.MousePos.y);
					float scrollInput = inputOutput.MouseWheel;
					float weight = 0.01f;
					float signedMousePosX = mousePos.x - canvas_p0.x - (DYNAMIC_VIEWPORT_WIDTH / 2);
					float signedMousePosY = mousePos.y - canvas_p0.y - (DYNAMIC_VIEWPORT_HEIGHT / 2);
					bool _weightedScroll = true;
					float zoomSpeed = 0.1f;
					float zoomMultiplier = 10;
					float finalZoomSpeed = zoomSpeed;

					if (ImGui::GetIO().KeyCtrl)
						finalZoomSpeed *= zoomMultiplier;

					// Change scrolling offset based on mouse position and weight
					if (is_hovered)
					{
						if (scrollInput > 0)
						{
							if (_weightedScroll)
							{
								sceneViewScrolling.x -= trunc(signedMousePosX * weight);
								sceneViewScrolling.y -= trunc(signedMousePosY * weight);
							}
							sceneViewGridStep.x += finalZoomSpeed;
							sceneViewGridStep.y += finalZoomSpeed;
						}
						else if (scrollInput < 0 && sceneViewGridStep.x > 2 && sceneViewGridStep.y > 2)
						{
							if (_weightedScroll)
							{
								sceneViewScrolling.x += trunc(signedMousePosX * weight);
								sceneViewScrolling.y += trunc(signedMousePosY * weight);
							}
							sceneViewGridStep.x -= finalZoomSpeed;
							sceneViewGridStep.y -= finalZoomSpeed;
						}
					}
					//////////////////

					// Change the draw channel for the scene object
					if (renderOrder <= maxSpriteLayers && renderOrder >= 0)
						drawSplitter->SetCurrentChannel(draw_list, renderOrder);
					else
						drawSplitter->SetCurrentChannel(draw_list, 0);

					// Draw the texture
					AddImageToDrawList(spriteTexture, position, scrolling, spriteTextureWidth, spriteTextureHeight, spriteOffset, transformScale, _spriteScalesWithZoom, step, draw_list, rotation);
				}
			}

			// If it has a text component, render that text texture at the objects transform position
			if (text != nullptr)
			{
				std::shared_ptr<Texture> textTexture = text->GetTexture();
				text->LoadText();
				SDL_Texture* texture = textTexture->getTexture();
				float textWidth = (float)textTexture->getWidth();
				float textHeight = (float)textTexture->getHeight();
				int renderOrder = text->GetRenderOrder();
				Vector2 offset = text->GetOffset();
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
					AddImageToDrawList(textTexture->getTexture(), position, scrolling, textWidth, textHeight, offset, transformScale, _spriteScalesWithZoom, step, draw_list, rotation);
				}
			}

			// Renders the camera
			if (camera != nullptr)
			{
				float cameraWidth = camera->GetWidth();
				float cameraHeight = camera->GetHeight();

				float cameraLeftEdge = scrolling.x + (position.x * sceneViewGridStep.x) - (cameraWidth * sceneViewGridStep.x / 2 * scale.x);
				float cameraTopEdge = scrolling.y + (-position.y * sceneViewGridStep.y) - (cameraHeight * sceneViewGridStep.y / 2 * scale.y);
				float cameraRightEdge = scrolling.x + (position.x * sceneViewGridStep.x) + (cameraWidth * sceneViewGridStep.x / 2 * scale.x);
				float cameraBottomEdge = scrolling.y + (-position.y * sceneViewGridStep.y) + (cameraHeight * sceneViewGridStep.y / 2 * scale.y);

				Vector2 topLeftCorner = Vector2(cameraLeftEdge, cameraTopEdge);
				Vector2 bottomRightCorner = Vector2(cameraRightEdge, cameraBottomEdge);
				Vector2 topRightCorner = Vector2(cameraRightEdge, cameraTopEdge);
				Vector2 bottomLeftCorner = Vector2(cameraLeftEdge, cameraBottomEdge);

				float cameraTextureWidth = (float)cameraIcon->getWidth() / 4;
				float cameraTextureHeight = (float)cameraIcon->getHeight() / 4;
				bool _scalesWithZoom = false;
				Vector2 cameraTextureOffset = { cameraTextureWidth / 2, cameraTextureHeight / 2 };
				Vector2 cameraTextureScale = { 1, 1 };
				Vector2 offsetPosition = Vector2(position.x - cameraTextureWidth / 2, position.y + cameraTextureHeight / 2);

				// Draw channel 2 for Lower UI
				drawSplitter->SetCurrentChannel(draw_list, maxSpriteLayers + 2);

				// Draw a rectangle to the scene view to represent the camera frustrum
				FlatEngine::DrawRectangle(topLeftCorner, bottomRightCorner, canvas_p0, canvas_sz, cameraBoxColor, 2.0f, draw_list);
				FlatEngine::DrawLine(topLeftCorner, bottomRightCorner, cameraBoxColor, 2.0f, draw_list);
				FlatEngine::DrawLine(topRightCorner, bottomLeftCorner, cameraBoxColor, 2.0f, draw_list);

				// Draw actual camera icon
				AddImageToDrawList(cameraTexture, position, scrolling, cameraTextureWidth, cameraTextureHeight, cameraTextureOffset, cameraTextureScale, _scalesWithZoom, step, draw_list, 0, IM_COL32(255, 255, 255, iconTransparency));
			}

			// Renders Canvas Component
			if (canvas != nullptr)
			{
				float activeWidth = canvas->GetWidth();
				float activeHeight = canvas->GetHeight();
				int layerNumber = canvas->GetLayerNumber();
				bool _blocksLayers = canvas->GetBlocksLayers();

				float unscaledXStart = scrolling.x + (position.x * sceneViewGridStep.x) - (activeWidth / 2 * scale.x);
				float unscaledYStart = scrolling.y + (-position.y * sceneViewGridStep.y) - (activeHeight / 2 * scale.y);
				Vector2 renderStart = Vector2(unscaledXStart, unscaledYStart);
				Vector2 renderEnd = Vector2(unscaledXStart + (activeWidth * scale.x), unscaledYStart + (activeHeight * scale.y));

				drawSplitter->SetCurrentChannel(draw_list, maxSpriteLayers + 2);

				FlatEngine::DrawRectangle(renderStart, renderEnd, canvas_p0, canvas_sz, canvasBorderColor, 3.0f, draw_list);
			}


			// Renders Button Component
			if (button != nullptr)
			{
				float activeWidth = button->GetActiveWidth();
				float activeHeight = button->GetActiveHeight();
				Vector2 activeOffset = button->GetActiveOffset();
				bool _isActive = button->IsActive();

				float activeLeft = WorldToViewport(scrolling.x, position.x + activeOffset.x - (activeWidth / 2 * transformScale.x), step, false);
				float activeRight = WorldToViewport(scrolling.x, position.x + activeOffset.x + (activeWidth / 2 * transformScale.x), step, false);
				float activeTop = WorldToViewport(scrolling.y, position.y + activeOffset.y + (activeHeight / 2 * transformScale.y), step, true);
				float activeBottom = WorldToViewport(scrolling.y, position.y + activeOffset.y - (activeHeight / 2 * transformScale.y), step, true);

				Vector2 center = Vector2(activeLeft + (activeRight - activeLeft) / 2, activeTop + (activeBottom - activeTop) / 2);

				Vector2 topLeft = { activeLeft, activeTop };
				Vector2 bottomRight = { activeRight, activeBottom };
				Vector2 topRight = { activeRight, activeTop };
				Vector2 bottomLeft = { activeLeft, activeBottom };

				drawSplitter->SetCurrentChannel(draw_list, maxSpriteLayers + 2);

				if (rotation != 0)
				{
					float cos_a = cosf(rotation * 2.0f * (float)M_PI / 360.0f); // Convert degrees into radians
					float sin_a = sinf(rotation * 2.0f * (float)M_PI / 360.0f);

					topLeft = ImRotate(Vector2(-activeWidth * step / 2 * transformScale.x, -activeHeight * step / 2 * transformScale.y), cos_a, sin_a);
					topRight = ImRotate(Vector2(+activeWidth * step / 2 * transformScale.x, -activeHeight * step / 2 * transformScale.y), cos_a, sin_a);
					bottomRight = ImRotate(Vector2(+activeWidth * step / 2 * transformScale.x, +activeHeight * step / 2 * transformScale.y), cos_a, sin_a);
					bottomLeft = ImRotate(Vector2(-activeWidth * step / 2 * transformScale.x, +activeHeight * step / 2 * transformScale.y), cos_a, sin_a);

					Vector2 pos[4] =
					{
						Vector2(center.x + topLeft.x, center.y + topLeft.y),
						Vector2(center.x + topRight.x, center.y + topRight.y),
						Vector2(center.x + bottomRight.x, center.y + bottomRight.y),
						Vector2(center.x + bottomLeft.x, center.y + bottomLeft.y),
					};

					if (_isActive)
					{
						DrawLine(pos[0], pos[1], buttonComponentActiveColor, 2.0f, draw_list);
						DrawLine(pos[1], pos[2], buttonComponentActiveColor, 2.0f, draw_list);
						DrawLine(pos[2], pos[3], buttonComponentActiveColor, 2.0f, draw_list);
						DrawLine(pos[3], pos[0], buttonComponentActiveColor, 2.0f, draw_list);
					}
					else
						DrawRectangle(topLeft, bottomRight, canvas_p0, canvas_sz, buttonComponentInctiveColor, 1.0f, draw_list);
				}
				else
				{
					if (_isActive)
						DrawRectangle(topLeft, bottomRight, canvas_p0, canvas_sz, buttonComponentActiveColor, 1.0f, draw_list);
					else
						DrawRectangle(topLeft, bottomRight, canvas_p0, canvas_sz, buttonComponentInctiveColor, 1.0f, draw_list);
				}
			}

			// Renders BoxCollider Component
			if (boxCollider != nullptr)
			{
				float activeWidth = boxCollider->GetActiveWidth();
				float activeHeight = boxCollider->GetActiveHeight();
				Vector2 activeOffset = boxCollider->GetActiveOffset();
				int activeLayer = boxCollider->GetActiveLayer();
				bool _isActive = boxCollider->IsActive();
				bool _isColliding = boxCollider->IsColliding();
				float activeRadius = boxCollider->GetActiveRadiusScreen();
				bool _showActiveRadius = boxCollider->GetShowActiveRadius();
				Vector2 center = boxCollider->GetCenter();

				boxCollider->UpdateActiveEdges();

				Vector2 corners[4] = {
					boxCollider->GetCorners()[0],
					boxCollider->GetCorners()[1],
					boxCollider->GetCorners()[2],
					boxCollider->GetCorners()[3],
				};

				drawSplitter->SetCurrentChannel(draw_list, maxSpriteLayers + 2);

				if (loadedProject->GetCollisionDetection() == "Simple Box")
				{
					if (_isActive && !_isColliding)
						DrawRectangleFromLines(corners, boxColliderActiveColor, 1.0f, draw_list);
					else if (!_isActive)
						DrawRectangleFromLines(corners, boxColliderInactiveColor, 1.0f, draw_list);
					else if (_isColliding)
						DrawRectangleFromLines(corners, boxColliderCollidingColor, 1.0f, draw_list);
				}
				else if (loadedProject->GetCollisionDetection() == "Separating Axis (Rotational)")
				{
					Vector2 corners[4] = {
						boxCollider->GetCorners()[0],
						boxCollider->GetCorners()[1],
						boxCollider->GetCorners()[2],
						boxCollider->GetCorners()[3],
					};
					Vector2 normals[4] =
					{
						boxCollider->GetNormals()[0],
						boxCollider->GetNormals()[1],
						boxCollider->GetNormals()[2],
						boxCollider->GetNormals()[3],
					};

					// Draw Normals
					DrawLine(center, normals[0], boxColliderInactiveColor, 2.0f, draw_list);
					DrawLine(center, normals[1], boxColliderInactiveColor, 2.0f, draw_list);
					DrawLine(center, normals[2], boxColliderInactiveColor, 2.0f, draw_list);
					DrawLine(center, normals[3], boxColliderInactiveColor, 2.0f, draw_list);

					if (_isActive && !_isColliding)
						DrawRectangleFromLines(corners, boxColliderActiveColor, 1.0f, draw_list);
					else if (!_isActive)
						DrawRectangleFromLines(corners, boxColliderInactiveColor, 1.0f, draw_list);
					else if (_isColliding)
						DrawRectangleFromLines(corners, boxColliderCollidingColor, 1.0f, draw_list);
				}

				// Draw activeRadius circle
				if (_showActiveRadius)
					draw_list->AddCircle(center, activeRadius, ImGui::GetColorU32(boxColliderActiveColor));
			}


			// Renders Transform Arrow // 
			//
			// Should be last in line here to be rendered top-most -- If this obect is focused
			if (focusedObjectID != -1 && focusedObjectID == self->GetID())
			{
				std::shared_ptr<GameObject> focusedObject = FlatEngine::GetObjectById(focusedObjectID);
				SDL_Texture* arrowToRender = transformArrowTexture;
				// * 3 because the texture is so small. If we change the scale, it will change the render starting position. We only want to change the render ending position so we adjust dimensions only
				float arrowWidth = (float)transformArrow->getWidth() * 3;
				float arrowHeight = (float)transformArrow->getHeight() * 3;
				Vector2 arrowScale = { 1, 1 };
				Vector2 arrowOffset = { 3, arrowHeight - 3 };
				bool _scalesWithZoom = false;
				float transformMoveModifier = 0.02f;
				ImGuiIO& inputOutput = ImGui::GetIO();
				Vector2 positionOnScreen = Vector2(sceneViewCenter.x + (position.x * step), sceneViewCenter.y - (position.y * step));

				// Invisible button for Transform Arrow Move X and Y
				Vector2 moveAllStartPos = Vector2(positionOnScreen.x - 4, positionOnScreen.y - 23);
				RenderInvisibleButton("TransformBaseArrowButton", moveAllStartPos, Vector2(28, 28), false);
				const bool _baseHovered = ImGui::IsItemHovered();
				const bool _baseActive = ImGui::IsItemActive();
				const bool _baseClicked = ImGui::IsItemClicked();

				if (_baseHovered || _baseActive)
				{
					arrowToRender = transformArrowAllWhiteTexture;
					ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
				}

				// Invisible button for X arrow
				Vector2 moveXStartPos = Vector2(positionOnScreen.x + 24, positionOnScreen.y - 30);
				RenderInvisibleButton("TransformBaseArrowXButton", moveXStartPos, Vector2(63, 35), false);
				const bool _xHovered = ImGui::IsItemHovered();
				const bool _xActive = ImGui::IsItemActive();
				const bool _xClicked = ImGui::IsItemClicked();
				if (_xHovered || _xActive)
				{
					arrowToRender = transformArrowXWhiteTexture;
					ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
				}

				// Invisible button for Y arrow
				Vector2 moveYStartPos = Vector2(positionOnScreen.x - 4, positionOnScreen.y - 86);
				RenderInvisibleButton("TransformBaseArrowYButton", moveYStartPos, Vector2(35, 63), false);
				const bool _yHovered = ImGui::IsItemHovered();
				const bool _yActive = ImGui::IsItemActive();
				const bool _yClicked = ImGui::IsItemClicked();
				if (_yHovered || _yActive)
				{
					arrowToRender = transformArrowYWhiteTexture;
					ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
				}

				static Vector2 transformScreenPos = Vector2(0, 0);
				static Vector2 cursorPosAtClick = inputOutput.MousePos;

				if (_baseClicked || _xClicked || _yClicked)
				{
					cursorPosAtClick = inputOutput.MousePos;
					transformScreenPos = Vector2(sceneViewCenter.x + (position.x * step), sceneViewCenter.y - (position.y * step));
				}

				Vector2 transformPosOffsetFromMouse = Vector2((cursorPosAtClick.x - transformScreenPos.x) / step, (cursorPosAtClick.y - transformScreenPos.y) / step);
				Vector2 mousePosInGrid = Vector2((inputOutput.MousePos.x - sceneViewCenter.x) / step, (sceneViewCenter.y - inputOutput.MousePos.y) / step);
				Vector2 newTransformPos = Vector2(mousePosInGrid.x - transformPosOffsetFromMouse.x, mousePosInGrid.y + transformPosOffsetFromMouse.y);

				if (_baseActive && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
					transform->SetPosition(newTransformPos);
				else if (_xActive && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
					transform->SetPosition(Vector2(newTransformPos.x, position.y));
				else if (_yActive && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
					transform->SetPosition(Vector2(position.x, newTransformPos.y));


				// Draw channel maxSpriteLayers + 3 for Upper UI Transform Arrow
				drawSplitter->SetCurrentChannel(draw_list, maxSpriteLayers + 3);
				AddImageToDrawList(arrowToRender, position, scrolling, arrowWidth, arrowHeight, arrowOffset, arrowScale, _scalesWithZoom, step, draw_list);
			}
		}
	}


	// Show cursor position in scene view when pressing Alt
	void RenderSceneViewTooltip()
	{
		ImGuiIO& inputOutput = ImGui::GetIO();
		Vector2 positionInGrid = Vector2((inputOutput.MousePos.x - sceneViewCenter.x) / sceneViewGridStep.x, -(inputOutput.MousePos.y - sceneViewCenter.y) / sceneViewGridStep.y);
		std::string cursorXPos = "x: " + std::to_string(positionInGrid.x);
		std::string cursorYPos = "y: " + std::to_string(positionInGrid.y);
		// Mouse Hover Tooltip - Scene View Tooltip			
		ImGui::BeginTooltip();
		ImGui::SetCursorPos(Vector2(ImGui::GetCursorPosX() + 5, ImGui::GetCursorPosY() + 5));
		ImGui::Text("Scene View Data ");
		ImGui::Separator();
		ImGui::SetCursorPos(Vector2(ImGui::GetCursorPosX() + 5, ImGui::GetCursorPosY() + 5));
		// Cursor Position
		ImGui::Text("Cursor Position: ");
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5);
		ImGui::Text(cursorXPos.c_str());
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5);
		ImGui::Text(cursorYPos.c_str());
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
		ImGui::Separator();
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
		ImGui::EndTooltip();
	}
}
}