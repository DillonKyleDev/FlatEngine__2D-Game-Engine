#pragma once
#include <string>
#include <vector>
#include "Component.h"
#include "Transform.h"
#include "Sprite.h"



namespace FlatEngine
{
	class GameObject
	{
	public:
		GameObject(GameObject* parent = nullptr);
		~GameObject();
		int GetID();
		void SetName(std::string name);
		std::string GetName();
		Component* AddComponent(Component::ComponentTypes type);
		void RemoveComponent(Component* component);
		Component* GetComponent(Component::ComponentTypes type);
		std::vector<Component*> &GetComponents();
		void SetParent(GameObject* parent);
		GameObject* GetParent();
		void AddChild(GameObject* child);
		void RemoveChild(GameObject child);
		std::vector<GameObject*> GetChildren();
		bool HasChildren();

	private:
		long ID;
		std::string name;

		std::vector<Component*> components;
		GameObject* parent;
		std::vector<GameObject*> children;
	};
}
