#pragma once
#include <string>
#include "json.hpp"
#include "Vector2.h"

using json = nlohmann::json;
using namespace nlohmann::literals;



namespace FlatEngine
{
	class Component
	{
	public:
		enum ComponentTypes {
			Null,
			Transform,
			Sprite,
			Camera,
			Script,
		};

		Component();
		~Component();

		void SetType(ComponentTypes type);

		ComponentTypes GetType();
		std::string GetTypeString();
		virtual std::string GetData();

	private:
		ComponentTypes type = Component::ComponentTypes::Null;
		// Maybe use an id system here instead of passing actual gameobjects so you can keep track of the 
		// parents of each component without causing cyclical dependencies. ie. GameObject uses component and component can't use gameobject.
	};
}