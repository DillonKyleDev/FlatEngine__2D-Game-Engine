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
			Canvas,
			Animation,
		};

		Component();
		~Component();

		void SetType(ComponentTypes type);
		void SetID(long ID);
		long GetID();
		void SetParentID(long ID);
		long GetParentID();
		bool IsCollapsed();
		void SetCollapsed(bool collapsed);

		ComponentTypes GetType();
		std::string GetTypeString();
		virtual std::string GetData();

	private:
		ComponentTypes type = Component::ComponentTypes::Null;
		long ID;
		long parentID;
		bool _isCollapsed;
	};
}