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
		int GetID();
		void SetName(std::string name);
		std::string GetName();
		Component* AddComponent(ComponentTypes type);
		void RemoveComponent(Component* component);
		Component* GetComponent(ComponentTypes type);
		std::vector<Component*> &GetComponents();
		void SetParentID(long parentID);
		long GetParentID();
		void AddChild(long childID);
		void RemoveChild(long childID);
		std::vector<long> GetChildren();
		bool HasChildren();

	private:
		long ID;
		long parentID;
		std::string name;
		std::vector<Component*> components;
		std::vector<long> childrenIDs;
	};
}
