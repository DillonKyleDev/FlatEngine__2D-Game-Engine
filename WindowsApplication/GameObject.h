#pragma once
#include <string>
#include <vector>
#include "Transform.h"
#include "Sprite.h"
#include "Camera.h"




namespace FlatEngine
{
	using ComponentTypes = Component::ComponentTypes;

	class GameObject
	{
	public:
		GameObject(long parentID = -1);
		~GameObject();
		// Copy Constructor
		GameObject(GameObject& original);

		void SetID(long ID);
		long GetID();
		void SetName(std::string name);
		std::string GetName();
		std::shared_ptr<Component> AddComponent(ComponentTypes type);
		void RemoveComponent(long componentID);
		std::shared_ptr<Component> GetComponent(ComponentTypes type);
		std::vector<std::shared_ptr<Component>> &GetComponents();
		void SetParentID(long parentID);
		long GetParentID();
		void AddChild(long childID);
		void RemoveChild(long childID);
		std::vector<long> GetChildren();
		bool HasChildren();
		void SetActive(bool _isActive);
		bool IsActive();
		//void AddScriptInstance(std::shared_ptr<GameScript> script);

	private:
		long ID;
		long parentID;
		bool _isActive;
		std::string name;
		std::vector<std::shared_ptr<Component>> components;
		std::vector<long> childrenIDs;
		//std::vector<std::shared_ptr<GameScript>> scripts;
	};
}
