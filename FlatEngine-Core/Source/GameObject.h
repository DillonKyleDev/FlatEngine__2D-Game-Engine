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
	class Script;
	class Animation;
	class Sprite;
	class Camera;
	class Audio;
	class Transform;
	class Text;	
	class CharacterController;
	class RigidBody;
	class BoxCollider;
	class CircleCollider;
	class CompositeCollider;
	class TileMap;

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
		Transform* AddTransform(long id = -1, bool _active = true, bool _collapsed = false);
		Sprite* AddSprite(long id = -1, bool _active = true, bool _collapsed = false);
		Camera* AddCamera(long id = -1, bool _active = true, bool _collapsed = false);
		Script* AddScript(long id = -1, bool _active = true, bool _collapsed = false);
		Button* AddButton(long id = -1, bool _active = true, bool _collapsed = false);
		Canvas* AddCanvas(long id = -1, bool _active = true, bool _collapsed = false);
		Animation* AddAnimation(long id = -1, bool _active = true, bool _collapsed = false);
		Audio* AddAudio(long id = -1, bool _active = true, bool _collapsed = false);
		Text* AddText(long id = -1, bool _active = true, bool _collapsed = false);
		BoxCollider* AddBoxCollider(long id = -1, bool _active = true, bool _collapsed = false);
		CircleCollider* AddCircleCollider(long id = -1, bool _active = true, bool _collapsed = false);
		CompositeCollider* AddCompositeCollider(long id = -1, bool _active = true, bool _collapsed = false);
		RigidBody* AddRigidBody(long id = -1, bool _active = true, bool _collapsed = false);
		CharacterController* AddCharacterController(long id = -1, bool _active = true, bool _collapsed = false);
		TileMap* AddTileMap(long id = -1, bool _active = true, bool _collapsed = false);

		Component* GetComponent(ComponentTypes type);
		bool HasComponent(ComponentTypes type);
		bool HasComponent(std::string type);
		Transform* GetTransform();
		Sprite* GetSprite();
		Camera* GetCamera();
		Animation* GetAnimation();
		Audio* GetAudio();
		Button* GetButton();
		Canvas* GetCanvas();
		std::vector<Script*> GetScripts();
		Text* GetText();
		CharacterController* GetCharacterController();
		RigidBody* GetRigidBody();
		std::vector<BoxCollider*> GetBoxColliders();		
		std::vector<CircleCollider*> GetCircleColliders();
		CompositeCollider* GetCompositeCollider();	
		TileMap* GetTileMap();
		std::vector<Component*> GetComponents();		

		void SetParentID(long parentID);
		long GetParentID();
		GameObject *GetParent();
		void AddChild(long childID);
		void RemoveChild(long childID);
		GameObject GetFirstChild();
		GameObject FindChildByName(std::string name);
		std::vector<long> GetChildren();
		bool HasChildren();
		void SetActive(bool _active);
		bool IsActive();

	private:
		std::string name;
		bool _isValid;
		bool _isPrefab;
		std::string prefabName;
		Vector2 prefabSpawnLocation;
		TagList m_tagList;
		long ID;
		long parentID;
		bool _isActive;
		std::vector<Component*> components;
		std::vector<long> childrenIDs;
	};
}