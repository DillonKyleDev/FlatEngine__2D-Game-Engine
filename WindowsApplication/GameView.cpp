#include "FlatEngine.h"
#include "Transform.h"
#include "Camera.h"
#include "Scene.h"
#include "Sprite.h"
#include "Text.h"
#include "Animation.h"


namespace FlatEngine { namespace FlatGui {

	// Game view default values
	float GAME_VIEWPORT_WIDTH = 600;
	float GAME_VIEWPORT_HEIGHT = 400;
	float xGameCenter = 600 / 2;
	float yGameCenter = 400 / 2;
	Vector2 gameViewCenter = Vector2(0, 0);
	Vector2 gameViewGridStep = Vector2(50, 50);


	void Game_RenderView()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, Vector2(0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

		ImGuiWindowFlags flags = ImGuiWindowFlags_None;

		// If Release - Make GameView full screen and disable tab decoration and resizing
		if (!FlatEngine::_isDebugMode)
		{
			// Get InputOutput
			ImGuiIO& inputOutput = ImGui::GetIO();
			float xSize = inputOutput.DisplaySize.x;
			float ySize = inputOutput.DisplaySize.y;
			ImGui::SetNextWindowSize(Vector2(xSize, ySize));
			ImGui::SetNextWindowPos(Vector2(0, 0));
			flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize;
		}

		PushWindowStyles();
		ImGui::Begin("Game View", 0, flags);
		PopWindowStyles();

		if (ImGui::IsWindowFocused())
			if (ImGui::IsKeyPressed(ImGuiKey_Escape))
				FlatEngine::gameManager->PauseGame();

		static bool opt_enable_context_menu = true;

		Vector2 canvas_p0 = ImGui::GetCursorScreenPos();
		Vector2 canvas_sz = ImGui::GetContentRegionAvail();
		if (canvas_sz.x < 50.0f) canvas_sz.x = 50.0f;
		if (canvas_sz.y < 50.0f) canvas_sz.y = 50.0f;
		Vector2 canvas_p1 = Vector2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);

		// Set viewport dimensions for rendering objects in game view. We want this to always be centered in game view so we can get it every frame.
		GAME_VIEWPORT_WIDTH = canvas_p1.x - canvas_p0.x + 1;
		GAME_VIEWPORT_HEIGHT = canvas_p1.y - canvas_p0.y + 1;

		// This will catch our interactions
		ImGui::InvisibleButton("GameViewCanvas", canvas_sz, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
		const bool is_hovered = ImGui::IsItemHovered(); // Hovered
		const bool is_active = ImGui::IsItemActive();   // Held

		// Render GameObjects in game view
		Game_RenderObjects(canvas_p0, canvas_sz);

		ImGui::End();

		// Reset WindowRounding
		ImGui::PopStyleVar();
		// Reset WindowPadding
		ImGui::PopStyleVar();
		// Reset WindowBorder
		ImGui::PopStyleVar();
	}

	void Game_RenderObjects(Vector2 canvas_p0, Vector2 canvas_sz)
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
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		// Create the splitter for the draw_list
		ImDrawListSplitter* drawSplitter = new ImDrawListSplitter();
		// 3 channels for now in this scene view. 0 = scene objects, 1 = other UI (camera icon, etc), 2 = transform arrow
		drawSplitter->Split(draw_list, maxSpriteLayers + 5);

		Vector2 cameraPosition(0, 0);
		float cameraWidth = 50;
		float cameraHeight = 30;
		// Used to convert grid space values to pixel values.ie. 2 grid squares = 2 * 10 = 20px.
		ImVec4 frustrumColor = ImVec4(1, 1, 1, 1);


		// For Profiler
		float cameraStartTime = (float)FlatEngine::GetEngineTime();

		// If the primaryCamera is found and not nullptr, set the cameraPosition accordingly, else it remains at {0,0} above
		if (primaryCamera != nullptr)
		{
			cameraTransform = std::static_pointer_cast<FlatEngine::Transform>(FlatEngine::GetObjectComponent(primaryCamera->GetParentID(), ComponentTypes::Transform));
			cameraWidth = primaryCamera->GetWidth();
			cameraHeight = primaryCamera->GetHeight();
			gameViewGridStep.x = primaryCamera->GetZoom();
			gameViewGridStep.y = primaryCamera->GetZoom();
			frustrumColor = primaryCamera->GetFrustrumColor();

			// Get the cameras position including all of its parents transforms offsets using the recusive Game_GetTotalCameraOffset();
			cameraPosition = cameraTransform->GetTruePosition();
		}

		// For Profiler
		if (FlatEngine::_isDebugMode)
		{
			float cameraHangTime = (float)FlatEngine::GetEngineTime() - cameraStartTime;
			AddProcessData("##Game_RenderView_Camera", cameraHangTime);
		}

		// Get the "center point" of the games view. This will appear to move around when we move the camera
		gameViewCenter = Vector2((GAME_VIEWPORT_WIDTH / 2) - (cameraPosition.x * gameViewGridStep.x) + canvas_p0.x, (GAME_VIEWPORT_HEIGHT / 2) + (cameraPosition.y * gameViewGridStep.x) + canvas_p0.y);
		// Get the center point of the viewport
		Vector2 viewportCenterPoint = Vector2((GAME_VIEWPORT_WIDTH / 2) + canvas_p0.x, (GAME_VIEWPORT_HEIGHT / 2) + canvas_p0.y);

		// For Profiler
		float renderStartTime = 0;
		if (FlatEngine::_isDebugMode)
			renderStartTime = (float)FlatEngine::GetEngineTime();

		// Render Game Objects
		for (std::shared_ptr<GameObject> sceneObject : sceneObjects)
		{
			if (sceneObject->IsActive())
				Game_RenderObject(sceneObject, canvas_p0, canvas_sz, draw_list, drawSplitter, cameraPosition, cameraWidth, cameraHeight);
		}

		// For Profiler
		if (FlatEngine::_isDebugMode)
		{
			float renderHangTime = (float)FlatEngine::GetEngineTime() - renderStartTime;
			AddProcessData("##Game_RenderView_RenderObjects", renderHangTime);
		}

		// Render Primary Camera Frustrum
		//{
		//	Texture* frustrum = new Texture();
		//	frustrum->loadFromFile("assets/images/frustrum.png");
		//	float h_FrustWidth = GAME_VIEWPORT_WIDTH;
		//	float h_FrustHeight = (GAME_VIEWPORT_HEIGHT - cameraHeight * gameViewGridStep.x) / 2;
		//	float v_FrustWidth = (GAME_VIEWPORT_WIDTH - cameraWidth * gameViewGridStep.x) / 2;
		//	float v_FrustHeight = (cameraHeight * gameViewGridStep.x) + GAME_VIEWPORT_HEIGHT;
		//	SDL_Texture* frustrumTexture = frustrum->getTexture();
		//	bool _frustScales = false;
		//	Vector2 frustrumScale = { 1, 1 };
		//	// Offsets in px
		//	Vector2 topFrustOffset = Vector2(v_FrustWidth, h_FrustHeight);
		//	Vector2 botFrustOffset = Vector2(v_FrustWidth, 0);
		//	Vector2 leftFrustOffset = Vector2(v_FrustWidth, v_FrustHeight);
		//	Vector2 rightFrustOffset = Vector2(0, v_FrustHeight);
		//	float redValue = trunc(frustrumColor.x);
		//	float greenValue = trunc(frustrumColor.y);
		//	float blueValue = trunc(frustrumColor.z);
		//	float alphaValue = trunc(frustrumColor.w);
		//	ImU32 frustrumColorU32 = (((ImU32)(alphaValue) << 24) | ((ImU32)(blueValue) << 16) | ((ImU32)(greenValue) << 8) | ((ImU32)(redValue)));

		//	// Set drawing channel to maxSpriteLayers + 2 for top layer camera UI
		//	drawSplitter->SetCurrentChannel(draw_list, maxSpriteLayers + 2);

		//	// Top frustrum
		//	//AddImageToDrawList(frustrumTexture, Vector2(-cameraWidth / 2, cameraHeight / 2), viewportCenterPoint, h_FrustWidth, h_FrustHeight, topFrustOffset, frustrumScale, _frustScales, gameViewGridStep.x, draw_list, frustrumColorU32);
		//	// Bottom frustrum
		//	//AddImageToDrawList(frustrumTexture, Vector2(-cameraWidth / 2, -cameraHeight / 2), viewportCenterPoint, h_FrustWidth, h_FrustHeight, botFrustOffset, frustrumScale, _frustScales, gameViewGridStep.x, draw_list, frustrumColorU32);
		//	// Left frustrum
		//	//AddImageToDrawList(frustrumTexture, Vector2(-cameraWidth / 2, -cameraHeight / 2), viewportCenterPoint, v_FrustWidth, v_FrustHeight, leftFrustOffset, frustrumScale, _frustScales, gameViewGridStep.x, draw_list, frustrumColorU32);
		//	// Left frustrum
		//	//AddImageToDrawList(frustrumTexture, Vector2(cameraWidth / 2, -cameraHeight / 2), viewportCenterPoint, v_FrustWidth, v_FrustHeight, rightFrustOffset, frustrumScale, _frustScales, gameViewGridStep.x, draw_list, frustrumColorU32);

			drawSplitter->Merge(draw_list);
		//}
	}

	void Game_RenderObject(std::shared_ptr<GameObject> self, Vector2 canvas_p0, Vector2 canvas_sz, ImDrawList* draw_list, ImDrawListSplitter* drawSplitter, Vector2 cameraPosition, float cameraWidth, float cameraHeight)
	{
		// Get Components
		std::shared_ptr<Transform> transform = self->GetTransformComponent();
		std::shared_ptr<Sprite> sprite = self->GetSpriteComponent();
		std::shared_ptr<Animation> animation = self->GetAnimationComponent();
		std::shared_ptr<Text> text = self->GetTextComponent();
		std::shared_ptr<Button> button = self->GetButtonComponent();


		// Animation component handling
		if (animation != nullptr)
		{
			// If animation component is playing, play the animation
			if (animation != nullptr && animation->IsPlaying())
				animation->PlayAnimation(GetEllapsedGameTime());
		}


		// Check if each object has a Transform component
		if (transform != nullptr)
		{
			long focusedObjectID = FlatEngine::GetFocusedGameObjectID();
			Vector2 position = transform->GetTruePosition();
			Vector2 scale = transform->GetScale();

			// If it has a text component, render that text texture at the objects transform position
			if (text != nullptr)
			{
				// Cast the component to Text shared_ptr
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
					AddImageToDrawList(textTexture->getTexture(), position, gameViewCenter, textWidth, textHeight, offset, scale, _spriteScalesWithZoom, gameViewGridStep.x, draw_list);
				}
			}

			// If it has a sprite component, render that sprite texture at the objects transform position
			if (sprite != nullptr)
			{
				SDL_Texture* spriteTexture = sprite->GetTexture();
				float textureWidth = sprite->GetTextureWidth();
				float textureHeight = sprite->GetTextureHeight();
				Vector2 spriteScale = sprite->GetScale();
				Vector2 offset = sprite->GetOffset();
				bool _scalesWithZoom = true;
				int renderOrder = sprite->GetRenderOrder();

				// Changing the scale here because things are rendering too large and I want them to start off smaller
				Vector2 newScale = Vector2(scale.x * spriteScale.x * spriteScaleMultiplier, scale.y * spriteScale.y * spriteScaleMultiplier);

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
					AddImageToDrawList(spriteTexture, position, gameViewCenter, textureWidth, textureHeight, offset, Vector2(scale.x * spriteScale.x, scale.y * spriteScale.y), _scalesWithZoom, gameViewGridStep.x, draw_list);
				}
			}

			// Renders Button Component
			if (button != nullptr)
			{
				float activeWidth = button->GetActiveWidth();
				float activeHeight = button->GetActiveHeight();
				Vector2 activeOffset = button->GetActiveOffset();
				bool _isActive = button->IsActive();

				// Get Active Button bounds to check against later for mouse events
				float activeLeft = gameViewCenter.x + ((position.x - (activeWidth / 2 * scale.x) + activeOffset.x * scale.x) * gameViewGridStep.x);
				float activeRight = gameViewCenter.x + ((position.x + (activeWidth / 2 * scale.x) + activeOffset.x * scale.x) * gameViewGridStep.x);
				float activeTop = gameViewCenter.y - ((position.y + (activeHeight / 2 * scale.y) + activeOffset.y * scale.y) * gameViewGridStep.x);
				float activeBottom = gameViewCenter.y - ((position.y - (activeHeight / 2 * scale.y) + activeOffset.y * scale.y) * gameViewGridStep.x);

				button->SetActiveEdges(ImVec4(activeTop, activeRight, activeBottom, activeLeft));

				// FOR DRAWING ACTIVE BUTTON RECTANGLE IN GAME VIEW
				// 
				Vector2 topLeft = { activeLeft, activeTop };
				Vector2 bottomRight = { activeRight, activeBottom };

				drawSplitter->SetCurrentChannel(draw_list, maxSpriteLayers + 2);

				if (_isActive)
					FlatEngine::DrawRectangle(topLeft, bottomRight, canvas_p0, canvas_sz, FlatEngine::FlatGui::buttonActiveColor, 3.0f, draw_list);
				else
					FlatEngine::DrawRectangle(topLeft, bottomRight, canvas_p0, canvas_sz, FlatEngine::FlatGui::buttonColor, 3.0f, draw_list);
			}
		}
	}
}
}