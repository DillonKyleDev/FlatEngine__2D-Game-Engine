#pragma once
#include <string>



class Component
{
public:
	enum ComponentTypes {
		Null,
		Transform,
		Sprite,
	};

	Component();
	~Component();

	void SetType(ComponentTypes type);

	ComponentTypes GetType();
	std::string GetTypeString();

private:
	ComponentTypes type;
};

