#pragma once
#include <string>
#include <vector>
#include "Component.h"
#include "TagList.h"

namespace FlatEngine
{
	class TagList;
	class Button;
	class Canvas;
	class ScriptComponent;
	class Animation;
	class Sprite;
	class Camera;
	class Audio;
	class Transform;
	class Text;
	class GameScript;
	class CharacterController;
	class RigidBody;
	class BoxCollider;
	class CircleCollider;
	class CompositeCollider;

	using ComponentTypes = Component::ComponentTypes;

	class GameObject
	{
	public:
		
		GameObject(long parentID = -1, long myID = -1);
		// Copy Constructor
		GameObject(GameObject* toCopy);
		GameObject(GameObject* toCopy, std::vector<GameObject> &objectVector, std::vector<GameObject> objectPool, long parentID = -1, long ID = -1);
		~GameObject();
		bool operator==(const GameObject toCompare);
	
		bool IsValid();
		void SetIsPrefab(bool _newIsPrefab);
		bool IsPrefab();
		void SetPrefabName(std::string newPrefabName);
		std::string GetPrefabName();
		void SetPrefabSpawnLocation(Vector2 newSpawnLocation);
		Vector2 GetPrefabSpawnLocation();
		void SetID(long ID);
		long GetID();
		void SetName(std::string name);
		std::string GetName();
		TagList &GetTagList();
		void SetTagList(TagList tagList);
		bool HasTag(std::string tagName);
		void SetTag(std::string tagName, bool _value);


		// Components
		void RemoveComponent(long componentID);
		void DeleteComponents();
		Transform* AddTransformComponent(long id = -1, bool _active = true, bool _collapsed = false);
		Sprite* AddSpriteComponent(long id = -1, bool _active = true, bool _collapsed = false);
		Camera* AddCameraComponent(long id = -1, bool _active = true, bool _collapsed = false);
		ScriptComponent* AddScriptComponent(long id = -1, bool _active = true, bool _collapsed = false);
		Button* AddButtonComponent(long id = -1, bool _active = true, bool _collapsed = false);
		Canvas* AddCanvasComponent(long id = -1, bool _active = true, bool _collapsed = false);
		Animation* AddAnimationComponent(long id = -1, bool _active = true, bool _collapsed = false);
		Audio* AddAudioComponent(long id = -1, bool _active = true, bool _collapsed = false);
		Text* AddTextComponent(long id = -1, bool _active = true, bool _collapsed = false);
		BoxCollider* AddBoxColliderComponent(long id = -1, bool _active = true, bool _collapsed = false);
		CircleCollider* AddCircleColliderComponent(long id = -1, bool _active = true, bool _collapsed = false);
		CompositeCollider* AddCompositeColliderComponent(long id = -1, bool _active = true, bool _collapsed = false);
		RigidBody* AddRigidBodyComponent(long id = -1, bool _active = true, bool _collapsed = false);
		CharacterController* AddCharacterControllerComponent(long id = -1, bool _active = true, bool _collapsed = false);
		Component* GetComponent(ComponentTypes type);
		bool HasComponent(ComponentTypes type);
		bool HasComponent(std::string type);
		Transform* GetTransformComponent();
		Sprite* GetSpriteComponent();
		Camera* GetCameraComponent();
		Animation* GetAnimationComponent();
		Audio* GetAudioComponent();
		Button* GetButtonComponent();
		Canvas* GetCanvasComponent();
		ScriptComponent* GetScriptComponent();
		std::vector<ScriptComponent*> GetScriptComponents();
		Text* GetTextComponent();
		CharacterController* GetCharacterController();
		RigidBody* GetRigidBody();
		BoxCollider* GetBoxCollider();
		std::vector<BoxCollider*> GetBoxColliders();
		CircleCollider* GetCircleCollider();
		std::vector<CircleCollider*> GetCircleColliders();
		CompositeCollider* GetCompositeCollider();
		GameScript* GetGameScriptByName(std::string scriptName);
		std::vector<Component*> &GetComponents();
		std::vector<GameScript*> GetScriptInstances();


		void SetParentID(long parentID);
		long GetParentID();
		GameObject GetParent();
		void AddChild(long childID);
		void RemoveChild(long childID);
		GameObject GetFirstChild();
		GameObject FindChildByName(std::string name);
		std::vector<long> GetChildren();
		bool HasChildren();
		void SetActive(bool _active);
		bool IsActive();

	private:
		bool _isValid;
		bool _isPrefab;
		std::string prefabName;
		Vector2 prefabSpawnLocation;
		TagList m_tagList;
		long ID;
		long parentID;
		bool _isActive;
		std::string name;
		std::vector<Component*> components;
		std::vector<long> childrenIDs;
	};
}