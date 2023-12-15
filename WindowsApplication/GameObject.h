#pragma once
#include <string>
#include <vector>
#include "Component.h"


class GameObject
{
public:
	GameObject();
	~GameObject();
	void SetName(std::string name);
	std::string GetName();
	void AddComponent(Component component);
	void RemoveComponent(Component component);
	void GetComponent();

private:
	long ID;
	std::string name;
	std::vector<Component> components;
};