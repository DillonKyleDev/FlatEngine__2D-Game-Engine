#pragma once
#include <string>
#include <vector>
#include "Component.h"


enum ComponentTypes {
	Transform,
	Sprite,
};

class GameObject
{
public:
	GameObject();
	~GameObject();
	void SetName(std::string name);
	std::string GetName();
	void AddComponent(Component component);
	void RemoveComponent(Component component);
	Component GetComponent();

private:
	long ID;
	std::string name;
	std::vector<Component> components;
};