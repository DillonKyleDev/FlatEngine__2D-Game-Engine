#include "FlatEngine.h"
#include "UIManager.h"
#include "Logger.h"
#include "Scene.h"
#include "Transform.h"
#include "Vector2.h"

/*
######################################
######							######
######         Engine           ######
######                          ######
######################################
*/


namespace FlatEngine
{
	bool _isDebugMode = true;
	bool _closeProgram = false;

	std::shared_ptr<GameManager> FlatEngine::gameManager = nullptr;
	std::shared_ptr<Sound> soundController = std::make_shared<Sound>();
	long FlatEngine::FocusedGameObjectID = -1;

	FlatEngine::SceneManager* FlatEngine::sceneManager = new FlatEngine::SceneManager();
	FlatEngine::Logger* FlatEngine::logger = new FlatEngine::Logger();
	FlatEngine::GameLoop* FlatEngine::gameLoop = new FlatEngine::GameLoop();
	std::shared_ptr<FlatEngine::FlatGui::WidgetsManager> widgetsManager(new FlatEngine::FlatGui::WidgetsManager());
	std::shared_ptr<FlatEngine::FlatGui::UIManager> uiManager(new FlatEngine::FlatGui::UIManager());

	// Animator
	std::shared_ptr<Animation::S_AnimationProperties> FocusedAnimation = std::make_shared<Animation::S_AnimationProperties>();
	std::shared_ptr<GameObject> objectForFocusedAnimation = nullptr;
	std::shared_ptr<Animation::S_Property> selectedKeyFrameToEdit = nullptr;
	int previewAnimationTime = 0;
	bool _playPreviewAnimation = true;

	// Profiler
	std::vector<std::shared_ptr<Process>> profilerProcesses = std::vector<std::shared_ptr<Process>>();

	// Colors
	ImU32 White = IM_COL32(255, 255, 255, 255);
	ImU32 Green = IM_COL32(0, 255, 0, 255);
	ImU32 Blue = IM_COL32(0, 0, 255, 255);
	ImU32 Red = IM_COL32(255, 0, 0, 255);
	ImU32 ActiveButtonColor = IM_COL32(50, 230, 50, 130);
	ImU32 InactiveButtonColor = IM_COL32(230, 50, 50, 130);
	ImU32 CanvasBorder = IM_COL32(195, 107, 1, 130);

	// FlatEngine
	void Run(bool& _hasQuit)
	{
		// Save a copy of the old process map values
		//AddProfilerProcess("Run Start", 0);

		_hasQuit = FlatEngine::_closeProgram;
		FlatEngine::FlatGui::Render(_hasQuit);

		// If Release - Start the Game Loop
		if (FlatEngine::_isDebugMode == false && FlatEngine::GameLoopStarted() == false)
		{
			FlatEngine::StartGameLoop();
		}

		// Keeping track of the frames passed since we started the GameLoop
		if (FlatEngine::GameLoopStarted() && FlatEngine::GameLoopPaused() == false)
		{
			FlatEngine::GameLoopUpdate();
		}
	}

	void CloseProgram()
	{
		FlatEngine::_closeProgram = true;
	}

	void SetFocusedGameObjectID(long ID)
	{
		FocusedGameObjectID = ID;
		std::shared_ptr<GameObject> focusedObject = GetObjectById(ID);
		std::shared_ptr<Animation> animationComponent = focusedObject->GetAnimationComponent();
		std::string animationPath = "";

		if (animationComponent != nullptr)
			animationPath = animationComponent->GetAnimationPath();
	

		// If applicable to the current animation, create a copy of the focused GameObject to be used for the animator window.
		if (FlatGui::_showAnimator && ID != -1 && FocusedAnimation != nullptr &&
			animationComponent != nullptr && animationPath == FocusedAnimation->animationPath)
		{
			std::vector<std::shared_ptr<GameObject>> animatorObjects = std::vector<std::shared_ptr<GameObject>>();
			objectForFocusedAnimation = std::make_shared<GameObject>(GetObjectById(ID), animatorObjects, -1);
			std::shared_ptr<Transform> transform = objectForFocusedAnimation->GetTransformComponent();
			transform->SetPosition(Vector2(0,0));
			animatorObjects.push_back(objectForFocusedAnimation);
			GetLoadedScene()->SetAnimatorPreviewObjects(animatorObjects);
		}
	}

	long GetFocusedGameObjectID()
	{
		return FlatEngine::FocusedGameObjectID;
	}

	void SetFocusedAnimation(std::shared_ptr<Animation::S_AnimationProperties> animation)
	{
		FocusedAnimation = animation;
	}

	std::shared_ptr<Animation::S_AnimationProperties> GetFocusedAnimation()
	{
		return FocusedAnimation;
	}


	// Scene Manager Prettification
	std::shared_ptr<Scene> GetLoadedScene()
	{
		return sceneManager->GetLoadedScene();
	}

	std::shared_ptr<Scene> CreateNewScene()
	{
		return FlatEngine::sceneManager->CreateNewScene();
	}

	std::string GetLoadedScenePath()
	{
		return sceneManager->GetLoadedScenePath();
	}

	long GetNextComponentID()
	{
		long nextID = -1;
		std::shared_ptr<Scene> loadedScene = GetLoadedScene();
		if (loadedScene != nullptr)
		{
			nextID = loadedScene->GetNextComponentID();
			GetLoadedScene()->IncrementComponentID();
		}

		return nextID;
	}

	long GetNextGameObjectID()
	{
		long nextID = -1;
		std::shared_ptr<Scene> loadedScene = GetLoadedScene();
		if (loadedScene != nullptr)
		{
			nextID = loadedScene->GetNextGameObjectID();
			GetLoadedScene()->IncrementGameObjectID();
		}

		return nextID;
	}

	void SaveScene(std::shared_ptr<Scene> scene, std::string filename)
	{
		FlatEngine::sceneManager->SaveScene(scene, filename);
	}

	void LoadScene(std::string name)
	{
		// Stop any playing music
		soundController->StopMusic();
		// Reset buttons in UIManager
		uiManager->ResetButtons();
		sceneManager->SaveAnimationPreviewObjects();
		sceneManager->LoadScene(name);
		sceneManager->LoadAnimationPreviewObjects();

		// If the GameLoop is running, reinitialize the new scene's GameObjects
		if (GameLoopStarted())
			gameLoop->InitializeScriptObjects();
	}
	

	// Scene Prettification
	std::vector<std::shared_ptr<GameObject>> GetSceneObjects()
	{
		if (FlatEngine::GetLoadedScene() != nullptr)
			return FlatEngine::GetLoadedScene()->GetSceneObjects();
		else
			return std::vector<std::shared_ptr<GameObject>>();
	}

	std::shared_ptr<GameObject> CreateGameObject(long parentID)
	{
		return FlatEngine::GetLoadedScene()->CreateGameObject(parentID);
	}

	void DeleteGameObject(int sceneObjectID)
	{
		FlatEngine::GetLoadedScene()->DeleteGameObject(sceneObjectID);
	}

	std::shared_ptr<Component> GetObjectComponent(long objectID, ComponentTypes type)
	{
		return FlatEngine::GetLoadedScene()->GetObjectById(objectID)->GetComponent(type);
	}

	std::shared_ptr<GameObject> GetObjectById(long objectID)
	{
		return GetLoadedScene()->GetObjectById(objectID);
	}

	std::shared_ptr<GameObject> GetObjectByName(std::string name)
	{
		return FlatEngine::GetLoadedScene()->GetObjectByName(name);
	}


	// Logging Abstraction
	void LogString(std::string line)
	{
		logger->LogString(line);
	}

	void LogFloat(float var, std::string line)
	{
		logger->LogFloat(var, line);
	}

	void LogInt(int var, std::string line)
	{
		logger->LogInt(var, line);
	}

	void LogVector2(Vector2 vector, std::string line)
	{
		logger->LogVector2(vector, line);
	}

	void DrawRectangle(Vector2 startingPoint, Vector2 endingPoint, ImVec2 canvas_p0, ImVec2 canvas_sz, ImU32 color, float thickness, ImDrawList* drawList)
	{
		if (startingPoint.x < canvas_p0.x)
			startingPoint.x = canvas_p0.x;
		if (endingPoint.x > canvas_p0.x + canvas_sz.x)
			endingPoint.x = canvas_p0.x + canvas_sz.x;
		if (startingPoint.y < canvas_p0.y)
			startingPoint.y = canvas_p0.y;
		if (endingPoint.y > canvas_p0.y + canvas_sz.y)
			endingPoint.y = canvas_p0.y + canvas_sz.y;

		logger->DrawRectangle(startingPoint, endingPoint, color, thickness, drawList);
	}

	void DrawRectangle(ImVec2 startingPoint, ImVec2 endingPoint, ImVec2 canvas_p0, ImVec2 canvas_sz, ImU32 color, float thickness, ImDrawList* drawList)
	{
		if (startingPoint.x < canvas_p0.x)
			startingPoint.x = canvas_p0.x;
		if (endingPoint.x > canvas_p0.x + canvas_sz.x)
			endingPoint.x = canvas_p0.x + canvas_sz.x;
		if (startingPoint.y < canvas_p0.y)
			startingPoint.y = canvas_p0.y;
		if (endingPoint.y > canvas_p0.y + canvas_sz.y)
			endingPoint.y = canvas_p0.y + canvas_sz.y;

		logger->DrawRectangle(startingPoint, endingPoint, color, thickness, drawList);
	}

	void DrawLine(ImVec2 startingPoint, ImVec2 endingPoint, ImU32 color, float thickness, ImDrawList* drawList)
	{
		logger->DrawLine(startingPoint, endingPoint, color, thickness, drawList);
	}

	void DrawLine(Vector2 startingPoint, Vector2 endingPoint, ImU32 color, float thickness, ImDrawList* drawList)
	{
		logger->DrawLine(startingPoint, endingPoint, color, thickness, drawList);
	}

	void DrawPoint(ImVec2 point, ImU32 color, ImDrawList* drawList)
	{
		logger->DrawPoint(point, color, drawList);
	}

	void DrawPoint(Vector2 point, ImU32 color, ImDrawList* drawList)
	{
		logger->DrawPoint(point, color, drawList);
	}


	// Game Loop prettification
	void StartGameLoop()
	{
		gameLoop->Start();
	}

	void GameLoopUpdate()
	{
		gameLoop->Update();
	}

	void PauseGameLoop()
	{
		if (gameLoop->IsPaused())
			gameLoop->Unpause();
		else
			gameLoop->Pause();
	}

	void StopGameLoop()
	{
		gameLoop->Stop();
	}

	int GetEllapsedGameTime()
	{
		return gameLoop->TimeEllapsed();
	}

	bool GameLoopStarted()
	{
		return gameLoop->IsStarted();
	}

	bool GameLoopPaused()
	{
		return gameLoop->IsPaused();
	}

	float GetAverageFps()
	{
		return gameLoop->GetAverageFps();
	}

	float GetDeltaTime()
	{
		return gameLoop->GetDeltaTime();
	}


	// Helper
	//ImVec4 objectA(top, right, bottom, left), ImVec4 objectB(top, right, bottom, left)
	bool AreCollidingWorld(ImVec4 ObjectA, ImVec4 ObjectB)
	{
		float A_TopEdge = ObjectA.x;
		float A_RightEdge = ObjectA.y;
		float A_BottomEdge = ObjectA.z;
		float A_LeftEdge = ObjectA.w;

		float B_TopEdge = ObjectB.x;
		float B_RightEdge = ObjectB.y;
		float B_BottomEdge = ObjectB.z;
		float B_LeftEdge = ObjectB.w;

		return (A_LeftEdge < B_RightEdge && A_RightEdge > B_LeftEdge && A_TopEdge > B_BottomEdge && A_BottomEdge < B_TopEdge);
	}

	bool AreCollidingViewport(ImVec4 ObjectA, ImVec4 ObjectB)
	{
		float A_TopEdge = ObjectA.z;
		float A_RightEdge = ObjectA.y;
		float A_BottomEdge = ObjectA.x;
		float A_LeftEdge = ObjectA.w;

		float B_TopEdge = ObjectB.x;
		float B_RightEdge = ObjectB.y;
		float B_BottomEdge = ObjectB.z;
		float B_LeftEdge = ObjectB.w;

		return (A_LeftEdge < B_RightEdge && A_RightEdge > B_LeftEdge && A_TopEdge > B_BottomEdge && A_BottomEdge < B_TopEdge);
	}

	Vector2 Lerp(Vector2 startPos, Vector2 endPos, float ease)
	{
		Vector2 difference = Vector2(endPos.x - startPos.x, endPos.y - startPos.y);
		Vector2 easedDiff = Vector2(difference.x * ease, difference.y * ease);
		return Vector2(startPos.x + easedDiff.x, startPos.y + easedDiff.y);
	}
}