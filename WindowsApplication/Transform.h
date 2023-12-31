#pragma once
#include "Component.h"
#include "Vector2.h"
#include "json.hpp"
using json = nlohmann::json;
using namespace nlohmann::literals;


namespace FlatEngine
{
	class Transform : public Component
	{
	public:
		Transform();
		~Transform();
		void SetPosition(Vector2 position);
		void SetRotation(float rotation);
		Vector2 GetPosition();
		float GetRotation();
		std::string GetData();


		//Vector2 position = { 0, 0 };

	private:
		Vector2 position;
		float rotation;
	};
}
