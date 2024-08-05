#pragma once
#include "Vector2.h"
#include "Vector4.h"
#include "Animation.h"

#include <sstream>
#include <map>
#include "imgui.h"
#include "SDL.h"

/*
######################################
######							######
######     Gui Declarations     ######
######                          ######
######################################
*/

namespace FlatEngine {
	class GameObject;
	class TagList;
	class Texture;
	class Project;
	class SceneManager;
	class Scene;
	class Process;
	class Component;
	class Transform;
	class Sprite;
	class Camera;
	class ScriptComponent;
	class Button;
	class Canvas;
	class Animation;
	class Audio;
	class Text;
	class CharacterController;
	class BoxCollider;
	class CircleCollider;
	class RigidBody;
}

using GameObject = FlatEngine::GameObject;
using TagList = FlatEngine::TagList;
using Texture = FlatEngine::Texture;
using Animation = FlatEngine::Animation;
using Project = FlatEngine::Project;
using SceneManager = FlatEngine::SceneManager;
using Scene = FlatEngine::Scene;
using Process = FlatEngine::Process;
using Component = FlatEngine::Component;
using Transform = FlatEngine::Transform;
using Sprite = FlatEngine::Sprite;
using Camera = FlatEngine::Camera;
using ScriptComponent = FlatEngine::ScriptComponent;
using Button = FlatEngine::Button;
using Canvas = FlatEngine::Canvas;
using Animation = FlatEngine::Animation;
using Audio = FlatEngine::Audio;
using Text = FlatEngine::Text;
using CharacterController = FlatEngine::CharacterController;
using BoxCollider = FlatEngine::BoxCollider;
using CircleCollider = FlatEngine::CircleCollider;
using RigidBody = FlatEngine::RigidBody;

using ComponentTypes = FlatEngine::Component::ComponentTypes;

namespace FlatGui
{
	class WidgetsManager;


	extern bool _editingValue;
	extern ImDrawList* drawList;
	extern WidgetsManager widgetsManager;
	extern int maxSpriteLayers;
	extern float spriteScaleMultiplier;

	// Project Management
	extern std::shared_ptr<Project> loadedProject;
	extern void OpenProject(std::string path);
	extern void SaveProject(std::shared_ptr<Project> project, std::string path);

	extern std::shared_ptr<GameObject> GetPlayerObject();
	extern std::shared_ptr<GameObject> playerObject;

	// Focused GameObject
	extern long FocusedGameObjectID;
	extern void SetFocusedGameObjectID(long ID);
	extern long GetFocusedGameObjectID();

	// Focused Animation
	extern std::shared_ptr<Animation::S_AnimationProperties> FocusedAnimation;
	extern GameObject objectForFocusedAnimation;
	extern std::shared_ptr<Animation::S_Property> selectedKeyFrameToEdit;
	extern int previewAnimationStartTime;
	extern int previewAnimationTime;
	extern bool _playPreviewAnimation;
	extern void SetFocusedAnimation(std::shared_ptr<Animation::S_AnimationProperties> animation);
	extern std::shared_ptr<Animation::S_AnimationProperties> GetFocusedAnimation();
	// Global Animation Event Functions
	extern void DestroySelf(std::shared_ptr<FlatEngine::GameObject> thisObject);

	// Scene view
	extern float SCENE_VIEWPORT_WIDTH;
	extern float SCENE_VIEWPORT_HEIGHT;
	extern float DYNAMIC_VIEWPORT_WIDTH;
	extern float DYNAMIC_VIEWPORT_HEIGHT;
	extern bool _firstSceneRenderPass;
	extern bool _sceneHasBeenSet;
	extern Vector2 sceneViewGridStep;
	extern Vector2 sceneViewScrolling;
	extern Vector2 sceneViewCenter;
	extern bool _sceneViewLockedOnObject;
	extern GameObject *sceneViewLockedObject;

	// Settings
	extern int iconTransparency;
	extern bool _clearBufferEveryFrame;

	// Hierarchy
	extern std::map<long, bool> leafExpandedTracker;

	// Window Visibility
	extern bool _showDemoWindow;
	extern bool _showSceneView;
	extern bool _showGameView;
	extern bool _showHierarchy;
	extern bool _showInspector;
	extern bool _showAnimator;
	extern bool _showAnimationPreview;
	extern bool _showKeyFrameEditor;
	extern bool _showLogger;
	extern bool _showProfiler;
	extern bool _showMappingContextEditor;
	extern bool _showSettings;

	// Game view
	extern float GAME_VIEWPORT_WIDTH;
	extern 	float GAME_VIEWPORT_HEIGHT;
	extern float xGameCenter;
	extern float yGameCenter;
	extern Vector2 gameViewCenter;
	extern Vector2 gameViewGridStep;

	extern void Init();

	// Profiler
	extern std::vector<std::shared_ptr<Process>> profilerProcesses;
	extern bool _logProfilerOutput;
	extern void AddProfilerProcess(std::string name);
	extern void AddProcessData(std::string processName, float data);
	extern void RemoveProfilerProcess(std::string toRemove);

	// Gui Rendering
	extern void SetupProfilerProcesses();
	extern void RunOnceAfterInitialization();

	// Viewports
	extern void RenderProjectHub(bool & b_projectSelected);
	extern void MainMenuBar();
	extern void AddViewports();
	extern void RenderToolbar();
	extern void RenderHierarchy();
	extern void RenderInspector();
	extern void Game_RenderView();
	extern void Game_RenderObjects(Vector2 canvas_p0, Vector2 canvas_sz);
	extern void Game_RenderObject(GameObject self, Vector2 canvas_p0, Vector2 canvas_sz, ImDrawList* draw_list, ImDrawListSplitter* drawSplitter, Vector2 cameraPosition, float cameraWidth, float cameraHeight);
	extern void Scene_RenderView();
	extern void RenderAnimator();
	extern void RenderAnimationPreview();
	extern void RenderAnimationTimelineGrid(Vector2& zeroPoint, Vector2 scrolling, Vector2 canvas_p0, Vector2 canvas_p1, Vector2 canvas_sz, float gridStep);
	extern void RenderKeyFrameEditor();
	extern void RenderLog();
	extern void RenderProfiler();
	extern void RenderMappingContextEditor();
	extern void RenderSettings();
	extern void Cleanup();
	extern void RenderGridView(Vector2& centerPoint, Vector2& scrolling, bool _weightedScroll, Vector2 canvas_p0, Vector2 canvas_p1, Vector2 canvas_sz, Vector2& step, Vector2 centerOffset);
	extern void RenderViewObjects(std::vector<GameObject> objects, Vector2 centerPoint, Vector2 canvas_p0, Vector2 canvas_sz, float step);
	extern void RenderViewObject(GameObject self, Vector2 scrolling, Vector2 canvas_p0, Vector2 canvas_sz, float step, ImDrawList* draw_list, ImDrawListSplitter* drawSplitter);

	// Component Wrappers
	extern bool RenderIsActiveCheckbox(bool& _isActive);
	extern void BeginComponent(Component* component, long& queuedForDelete);
	extern void EndComponent(Component* component);
	extern void RenderTransformComponent(Transform* transform);
	extern void RenderSpriteComponent(Sprite* sprite);
	extern void RenderCameraComponent(Camera* camera);
	extern void RenderScriptComponent(ScriptComponent* script);
	extern void RenderButtonComponent(Button* button);
	extern void RenderCanvasComponent(Canvas* canvas);
	extern void RenderAnimationComponent(Animation* animation);
	extern void RenderAudioComponent(Audio* audio);
	extern void RenderTextComponent(Text* text);
	extern void RenderCharacterControllerComponent(CharacterController* characterController);
	extern void RenderBoxColliderComponent(BoxCollider* boxCollider);
	extern void RenderCircleColliderComponent(CircleCollider* circleCollider);
	extern void RenderRigidBodyComponent(RigidBody* rigidBody);

	// SDL
	extern Vector2 AddImageToDrawList(SDL_Texture* texture, Vector2 position, Vector2 centerPoint, float textureWidth, float textureHeight, Vector2 pivotPoint, Vector2 scale, bool _scalesWithZoom, float zoomMultiplier, ImDrawList* draw_list, float rotation = 0, ImU32 addColor = (((ImU32)(255) << 24) | ((ImU32)(255) << 16) | ((ImU32)(255) << 8) | ((ImU32)(255) << 0)));
	// SceneView
	extern void RenderSceneViewTooltip();
	// Hierarchy
	extern void AddObjectToHierarchy(GameObject& currentObject, const char* charName, int& node_clicked, long& queuedForDelete, float indent);
	extern void ResetHierarchyExpanderTracker();
	// Profiler
	void Sparkline(const char* id, const float* values, int count, float min_v, float max_v, int offset, const Vector4& col, const Vector2& size);
	// Casts
	extern std::vector<std::shared_ptr<GameObject>> RayCast(Vector2 origin, Vector2 direction, float distance);
	extern std::vector<std::shared_ptr<GameObject>> SphereCast(Vector2 origin, Vector2 direction, float distance);
}