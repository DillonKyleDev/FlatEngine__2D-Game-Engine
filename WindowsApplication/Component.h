#pragma once
#include <string>
#include "json.hpp"
#include "Vector2.h"

using json = nlohmann::json;
using namespace nlohmann::literals;



namespace FlatEngine
{
	class GameObject;

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
			Audio,
			Text,
			BoxCollider,
			CircleCollider,
			RigidBody,
			CharacterController,
		};

		Component();
		~Component();

		void SetType(ComponentTypes type);
		void SetID(long ID);
		long GetID();
		void SetParentID(long ID);
		long GetParentID();
		std::shared_ptr<GameObject> GetParent();
		bool IsCollapsed();
		void SetCollapsed(bool collapsed);
		void SetActive(bool _active);
		bool IsActive();

		ComponentTypes GetType();
		std::string GetTypeString();
		virtual std::string GetData();

	private:
		ComponentTypes type = Component::ComponentTypes::Null;
		long ID;
		long parentID;
		bool _isCollapsed;
		bool _isActive;
	};
}