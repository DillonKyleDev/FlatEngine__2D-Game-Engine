#pragma once
#include "FlatEngine.h"
#include "ECSManager.h"
#include <vector>
#include <string>
#include <memory>
#include <list>
#include <utility>


namespace FlatEngine
{
	class GameObject;
	class Transform;
	class Sprite;
	class ScriptComponent;
	class GameScript;
	class Canvas;
	class Audio;
	class Text;
	class Camera;
	class Collider;
	class BoxCollider;
	class CircleCollider;
	class Animation;
	class Button;
	class RigidBody;
	class CompositeCollider;
	class CharacterController;

	class Scene
	{
		friend class SceneManager;
	public:
		Scene();
		~Scene();

		void SetName(std::string name);
		std::string GetName();
		void SetPath(std::string path);
		std::string GetPath();
		GameObject* AddSceneObject(GameObject sceneObject);
		std::vector<GameObject> &GetSceneObjects();
		void SetAnimatorPreviewObjects(std::vector<GameObject*> previewObjects);
		std::vector<GameObject*> GetAnimatorPreviewObjects();
		GameObject* GetObjectById(long ID);
		GameObject* GetObjectByName(std::string name);
		GameObject* CreateGameObject(long parentID = -1);
		void DeleteGameObject(long sceneObjectID);
		void DeleteChildrenAndSelf(GameObject *objectToDelete);
		void IncrementGameObjectID();
		long GetNextGameObjectID();
		void IncrementComponentID();
		long GetNextComponentID();
		void SetPrimaryCamera(Camera* camera);
		void RemovePrimaryCamera();
		Camera *GetPrimaryCamera();		
		void OnPrefabInstantiated(std::vector<GameObject> children);
		void UpdateColliderPairs(); // TODO

		// ECS Wrappers
		void InitializeScriptObjects(); // May not need
		Transform* AddTransform(Transform transform, long ownerID);
		Sprite* AddSprite(Sprite sprite, long ownerID);
		Camera* AddCamera(Camera camera, long ownerID);
		ScriptComponent* AddScriptComponent(ScriptComponent script, long ownerID);
		GameScript* AddScript(GameObject owner, ScriptComponent scriptComponent, GameScript scriptInstance);
		Canvas* AddCanvas(Canvas canvas, long ownerID);
		Audio* AddAudio(Audio audio, long ownerID);
		Text* AddText(Text text, long ownerID);
		Collider* AddCollider(Collider collider, long ownerID);
		CompositeCollider* AddCompositeCollider(CompositeCollider collider, long ownerID);
		BoxCollider* AddBoxCollider(BoxCollider collider, long ownerID);
		CircleCollider* AddCircleCollider(CircleCollider collider, long ownerID);
		Animation* AddAnimation(Animation animation, long ownerID);
		Button* AddButton(Button button, long ownerID);
		RigidBody* AddRigidBody(RigidBody rigidBody, long ownerID);
		CharacterController* AddCharacterController(CharacterController characterController, long ownerID);

		Transform* GetTransform(long ownerID);
		Sprite* GetSprite(long ownerID);
		Camera* GetCamera(long ownerID);
		ScriptComponent* GetScriptComponent(long ownerID);
		GameScript* GetScript(long ownerID, std::string name);
		Canvas* GetCanvas(long ownerID);
		Audio* GetAudio(long ownerID);
		Text* GetText(long ownerID);
		Collider* GetCollider(long ownerID);
		CompositeCollider* GetCompositeCollider(long ownerID);
		BoxCollider* GetBoxCollider(long ownerID);
		CircleCollider* GetCircleCollider(long ownerID);
		Animation* GetAnimation(long ownerID);
		Button* GetButton(long ownerID);
		RigidBody* GetRigidBody(long ownerID);
		CharacterController* GetCharacterController(long ownerID);

		std::vector<std::pair<Transform, long>> GetTransforms();
		std::vector<std::pair<Sprite, long>> GetSprites();
		std::vector<std::pair<Camera, long>> GetCameras();
		std::vector<std::pair<ScriptComponent, long>> GetScriptComponents();
		std::vector<std::pair<GameScript, long>> GetScripts();
		std::vector<std::pair<Button, long>> GetButtons();
		std::vector<std::pair<Canvas, long>> GetCanvases();
		std::vector<std::pair<Animation, long>> GetAnimations();
		std::vector<std::pair<Audio, long>> GetAudios();
		std::vector<std::pair<Text, long>> GetTexts();
		std::vector<std::pair<Collider, long>> GetColliders();
		std::vector<std::pair<CompositeCollider, long>> GetCompositeColliders();
		std::vector<std::pair<BoxCollider, long>> GetBoxColliders();
		std::vector<std::pair<CircleCollider, long>> GetCircleColliders();
		std::vector<std::pair<RigidBody, long>> GetRigidBodies();
		std::vector<std::pair<CharacterController, long>> GetCharacterControllers();
		std::vector<std::pair<Collider*, Collider*>> GetColliderPairs();

	private:
		std::string name;
		std::string path;
		std::vector<GameObject> sceneObjects;
		std::vector<GameObject*> animatorPreviewObjects;
		ECSManager m_ECSManager;
		Camera *primaryCamera;
		long nextGameObjectID;
		long nextComponentID;
	};
}