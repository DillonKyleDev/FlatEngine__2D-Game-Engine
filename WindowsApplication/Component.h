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
			Button,
		};

		Component();
		~Component();

		void SetType(ComponentTypes type);
		void SetParentID(long ID);
		long GetParentID();

		ComponentTypes GetType();
		std::string GetTypeString();
		virtual std::string GetData();

	private:
		ComponentTypes type = Component::ComponentTypes::Null;
		long parentID;
	};
}