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
			T_Null,
			T_Transform,
			T_Sprite,
			T_Camera,
			T_Script,
			T_Button,
			T_Canvas,
			T_Animation,
			T_Audio,
			T_Text,
			T_BoxCollider,
			T_CircleCollider,
			T_CompositeCollider,
			T_RigidBody,
			T_CharacterController,
		};

		Component();
		~Component();

		void SetType(ComponentTypes type);
		void SetID(long ID);
		long GetID();
		void SetParentID(long ID);
		long GetParentID();
		GameObject* GetParent();
		bool IsCollapsed();
		void SetCollapsed(bool collapsed);
		void SetActive(bool _active);
		bool IsActive();

		ComponentTypes GetType();
		std::string GetTypeString();
		virtual std::string GetData();

	private:
		ComponentTypes type = Component::ComponentTypes::T_Null;
		long ID;
		long parentID;
		bool _isCollapsed;
		bool _isActive;
	};
}