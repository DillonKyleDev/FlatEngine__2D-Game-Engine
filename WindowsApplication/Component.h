#pragma once
#include <string>
#include "json.hpp"
#include "Vector2.h"
using json = nlohmann::json;
using namespace nlohmann::literals;




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
	virtual std::string GetData();

	//Vector2 position = { 0, 0 };

private:
	ComponentTypes type = Component::ComponentTypes::Null;

};

