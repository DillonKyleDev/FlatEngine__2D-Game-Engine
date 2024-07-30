#pragma once
#include <vector>
#include <map>
#include <utility>
#include <string>

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

	class ECSManager
	{
	public:
		ECSManager();
		~ECSManager();

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

		void CollectPhysicsBodies();
		void InitializeScriptObjects(std::vector<GameObject> gameObjects);
		void RemoveScript(long scriptID);
		void RemoveRigidBody(long rigidBodyID);
		void RemoveCollider(long colliderID);
		void UpdateColliderPairs();
		void UpdateActiveRigidBodies();

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
		std::vector<std::pair<Transform, long>> m_Transforms;
		std::vector<std::pair<Sprite, long>> m_Sprites;
		std::vector<std::pair<Camera, long>> m_Cameras;
		std::vector<std::pair<ScriptComponent, long>> m_Scripts;
		std::vector<std::pair<GameScript, long>> m_GameScripts;
		std::vector<std::pair<Button, long>> m_Buttons;
		std::vector<std::pair<Canvas, long>> m_Canvases;
		std::vector<std::pair<Animation, long>> m_Animations;
		std::vector<std::pair<Audio, long>> m_Audios;
		std::vector<std::pair<Text, long>> m_Texts;
		std::vector<std::pair<Collider, long>> m_Colliders;
		std::vector<std::pair<CompositeCollider, long>> m_CompositeColliders;
		std::vector<std::pair<BoxCollider, long>> m_BoxColliders;
		std::vector<std::pair<CircleCollider, long>> m_CircleColliders;
		std::vector<std::pair<RigidBody, long>> m_RigidBodies;
		std::vector<std::pair<CharacterController, long>> m_CharacterControllers;

		std::vector<std::pair<Collider*, Collider*>> m_ColliderPairs;

		std::map<long, long> m_transformMap;
		std::map<long, long> m_spriteMap;
		std::map<long, long> m_cameraMap;
		std::map<long, std::vector<long>> m_scriptMap;
		std::map<long, long> m_gameScriptMap; // To be determined later
		std::map<long, long> m_buttonMap;
		std::map<long, long> m_canvasMap;
		std::map<long, long> m_animationMap;
		std::map<long, long> m_audioMap;
		std::map<long, long> m_textMap;
		std::map<long, long> m_colliderMap;
		std::map<long, long> m_compositeColliderMap;
		std::map<long, long> m_boxColliderMap;
		std::map<long, long> m_circleColliderMap;
		std::map<long, long> m_rigidBodyMap;
		std::map<long, long> m_characterControllerMap;
	};
}