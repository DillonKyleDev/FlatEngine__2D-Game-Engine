#pragma once
#include <string>
#include <vector>
#include "Component.h"

namespace FlatEngine
{
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
	//class CircleCollider;

	using ComponentTypes = Component::ComponentTypes;

	class GameObject
	{
	public:
		GameObject(long parentID = -1, long myID = -1);
		// Copy Constructor
		GameObject(std::shared_ptr<GameObject> toCopy, std::vector<std::shared_ptr<GameObject>> &objectVector, long parentID = -1);
		~GameObject();

		void RecursiveChildrenCopy(std::shared_ptr<GameObject>& parent, std::vector<std::shared_ptr<GameObject>> &objectVector, long childID);
	
		void SetID(long ID);
		long GetID();
		void SetName(std::string name);
		std::string GetName();
		std::shared_ptr<Component> AddComponent(ComponentTypes type);
		void RemoveComponent(long componentID);
		std::shared_ptr<Component> GetComponent(ComponentTypes type);
		bool HasComponent(ComponentTypes type);
		bool HasComponent(std::string type);
		std::shared_ptr<Transform> GetTransformComponent();
		std::shared_ptr<Sprite> GetSpriteComponent();
		std::shared_ptr<Camera> GetCameraComponent();
		std::shared_ptr<Animation> GetAnimationComponent();
		std::shared_ptr<Audio> GetAudioComponent();
		std::shared_ptr<Button> GetButtonComponent();
		std::shared_ptr<Canvas> GetCanvasComponent();
		std::shared_ptr<ScriptComponent> GetScriptComponent();
		std::shared_ptr<Text> GetTextComponent();
		std::shared_ptr<CharacterController> GetCharacterController();
		std::shared_ptr<RigidBody> GetRigidBody();
		std::shared_ptr<BoxCollider> GetBoxCollider();
		//std::shared_ptr<GetCircleCollider> GetCircleCollider();
		std::shared_ptr<GameScript> GetGameScriptByName(std::string scriptName);
		std::vector<std::shared_ptr<Component>> GetComponentsOfType(ComponentTypes type);
		std::vector<std::shared_ptr<Component>> &GetComponents();
		std::vector<std::shared_ptr<Component>> GetScriptInstances();
		void SetParentID(long parentID);
		long GetParentID();
		std::shared_ptr<GameObject> GetParent();
		void AddChild(long childID);
		void RemoveChild(long childID);
		std::shared_ptr<GameObject> GetFirstChild();
		std::vector<long> GetChildren();
		bool HasChildren();
		void SetActive(bool _isActive);
		bool IsActive();

	private:
		long ID;
		long parentID;
		bool _isActive;
		std::string name;
		std::vector<std::shared_ptr<Component>> components;
		std::vector<long> childrenIDs;
	};
}