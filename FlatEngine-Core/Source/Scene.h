#pragma once
#include "ECSManager.h"
#include "TileMap.h"
#include <vector>
#include <map>
#include <string>
#include <memory>
#include <list>
#include <utility>


namespace FlatEngine
{
	class GameObject;
	class Transform;
	class Sprite;
	class Script;
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
		void KeepNextGameObjectIDUpToDate(long id);
		std::map<long, GameObject> &GetSceneObjects();
		void SetAnimatorPreviewObjects(std::vector<GameObject*> previewObjects);
		std::vector<GameObject*> GetAnimatorPreviewObjects();
		GameObject* GetObjectById(long ID);
		GameObject* GetObjectByName(std::string name);
		GameObject* GetObjectByTag(std::string tag);
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
		void KeepNextComponentIDUpToDate(long id);
		Transform* AddTransform(Transform transform, long ownerID);
		Sprite* AddSprite(Sprite sprite, long ownerID);
		Camera* AddCamera(Camera camera, long ownerID);
		Script* AddScript(Script script, long ownerID);
		Canvas* AddCanvas(Canvas canvas, long ownerID);
		Audio* AddAudio(Audio audio, long ownerID);
		Text* AddText(Text text, long ownerID);
		CompositeCollider* AddCompositeCollider(CompositeCollider collider, long ownerID);
		BoxCollider* AddBoxCollider(BoxCollider collider, long ownerID);
		CircleCollider* AddCircleCollider(CircleCollider collider, long ownerID);
		Animation* AddAnimation(Animation animation, long ownerID);
		Button* AddButton(Button button, long ownerID);
		RigidBody* AddRigidBody(RigidBody rigidBody, long ownerID);
		CharacterController* AddCharacterController(CharacterController characterController, long ownerID);
		TileMap* AddTileMap(TileMap tileMap, long ownerID);

		void RemoveComponent(Component* component);

		Transform* GetTransformByOwner(long ownerID);
		Sprite* GetSpriteByOwner(long ownerID);
		Camera* GetCameraByOwner(long ownerID);
		std::vector<Script*> GetScriptsByOwner(long ownerID);
		Canvas* GetCanvasByOwner(long ownerID);
		Audio* GetAudioByOwner(long ownerID);
		Text* GetTextByOwner(long ownerID);
		CompositeCollider* GetCompositeColliderByOwner(long ownerID);
		std::vector<BoxCollider*> GetBoxCollidersByOwner(long ownerID);
		std::vector<CircleCollider*> GetCircleCollidersByOwner(long ownerID);
		Animation* GetAnimationByOwner(long ownerID);
		Button* GetButtonByOwner(long ownerID);
		RigidBody* GetRigidBodyByOwner(long ownerID);
		CharacterController* GetCharacterControllerByOwner(long ownerID);
		TileMap* GetTileMapByOwner(long ownerID);

		std::map<long, Transform> &GetTransforms();
		std::map<long, Sprite>& GetSprites();
		std::map<long, Camera>& GetCameras();
		std::map<long, std::map<long, Script>>& GetScripts();
		std::map<long, std::vector<std::string>> &GetLuaScriptsByOwner();
		std::map<long, Button>& GetButtons();
		std::map<long, Canvas>& GetCanvases();
		std::map<long, Animation>& GetAnimations();
		std::map<long, Audio>& GetAudios();
		std::map<long, Text>& GetTexts();
		std::vector<Collider*> GetColliders();
		std::map<long, CompositeCollider>& GetCompositeColliders();
		std::map<long, std::map<long, BoxCollider>>& GetBoxColliders();
		std::map<long, std::map<long, CircleCollider>>& GetCircleColliders();
		std::map<long, RigidBody>& GetRigidBodies();
		std::map<long, CharacterController>& GetCharacterControllers();
		std::map<long, TileMap>& GetTileMaps();
		std::vector<std::pair<Collider*, Collider*>> GetColliderPairs();

	private:
		std::string name;
		std::string path;
		std::map<long, GameObject> m_sceneObjects;
		std::vector<GameObject*> animatorPreviewObjects;
		ECSManager m_ECSManager;
		Camera *m_primaryCamera;
		long m_nextGameObjectID;
		long m_nextComponentID;
		std::vector<long> m_freedComponentIDs;
		std::vector<long> m_freedGameObjectIDs;
	};
}