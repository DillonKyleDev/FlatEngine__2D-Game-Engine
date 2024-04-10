#include "FlatEngine.h"
#include "Transform.h"
#include "Camera.h"
#include "Scene.h"
#include "Sprite.h"
#include "Text.h"

namespace FlatEngine { namespace FlatGui {

	// Scene view
	// The multiplier for gridstep. Used to convert grid space values to pixel values. ie. 2 grid squares = 2 * 10 = 20px.
	float gridStep = 50;
	float SCENE_VIEWPORT_WIDTH = 600;
	float SCENE_VIEWPORT_HEIGHT = 400;
	float DYNAMIC_VIEWPORT_WIDTH = 600;
	float DYNAMIC_VIEWPORT_HEIGHT = 400;
	bool _firstSceneRenderPass = true;
	bool _sceneHasBeenSet = false;

	void Scene_RenderView()
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

		static ImVec2 scrolling(0, 0);

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

		Scene_RenderGrid(scrolling, canvas_p0, canvas_p1, canvas_sz, gridStep);

		Scene_RenderObjects(scrolling, canvas_p0, canvas_sz);

		// Reset WindowPadding
		ImGui::PopStyleVar();
		// Reset WindowBorder
		ImGui::PopStyleVar();

		ImGui::End();
	}


	void Scene_RenderObjects(ImVec2 scrolling, ImVec2 canvas_p0, ImVec2 canvas_sz)
	{
		// Get currently loade scene
		std::shared_ptr<Scene> loadedScene = FlatEngine::sceneManager->GetLoadedScene();
		std::vector<std::shared_ptr<GameObject>> sceneObjects;

		if (loadedScene != nullptr)
			sceneObjects = loadedScene->GetSceneObjects();
		else
			sceneObjects = std::vector<std::shared_ptr<GameObject>>();

		// Split our drawlist into multiple channels for different rendering orders
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
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
				Scene_RenderSelfThenChildren(sceneObjects[i], parentOffset, parentScale, scrolling, canvas_p0, canvas_sz, draw_list, drawSplitter);
			}
		}

		drawSplitter->Merge(draw_list);
		delete drawSplitter;
		drawSplitter = nullptr;
	}

	// Helper - Recursively draws scene objects and their children to the scene view
	void Scene_RenderSelfThenChildren(std::shared_ptr<GameObject> self, Vector2 parentOffset, Vector2 parentScale, ImVec2 scrolling, ImVec2 canvas_p0, ImVec2 canvas_sz, ImDrawList* draw_list, ImDrawListSplitter* drawSplitter)
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
					AddImageToDrawList(spriteTexture, position, scrolling, spriteTextureWidth, spriteTextureHeight, spriteOffset, transformScale, _spriteScalesWithZoom, gridStep, draw_list);
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
					AddImageToDrawList(textTexture->getTexture(), position, scrolling, textWidth, textHeight, offset, transformScale, _spriteScalesWithZoom, gridStep, draw_list);
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
				AddImageToDrawList(texture, position, scrolling, cameraTextureWidth, cameraTextureHeight, cameraTextureOffset, cameraTextureScale, _scalesWithZoom, gridStep, draw_list, IM_COL32(255, 255, 255, iconTransparency));
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
				AddImageToDrawList(texture, position, scrolling, arrowWidth, arrowHeight, arrowOffset, arrowScale, _scalesWithZoom, gridStep, draw_list, IM_COL32(255, 255, 255, 255));
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
					Scene_RenderSelfThenChildren(child, parentOffset, parentScale, scrolling, canvas_p0, canvas_sz, draw_list, drawSplitter);
			}
		}
	}


	void Scene_RenderGrid(ImVec2 scrolling, ImVec2 canvas_p0, ImVec2 canvas_p1, ImVec2 canvas_sz, float gridStep)
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

		// For making grid go with window, add  + canvas_p0.x and  + canvas_p0.y to trunc(fmodf(scrolling.x ..., gridStep


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

}
}