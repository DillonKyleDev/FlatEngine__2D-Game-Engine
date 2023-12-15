#pragma once
#include <string>

class GameObject
{
public:
	GameObject();
	~GameObject();

	void AddComponent();

	void RemoveComponent();

private:
	std::string name;

};

