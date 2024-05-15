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
		Transform(long myID = -1, long parentID = -1);
		Transform(std::shared_ptr<Transform> toCopy, long newParentID);
		~Transform();
		void SetInitialPosition(Vector2 initialPos);
		void SetOrigin(Vector2 newOrigin);
		Vector2 GetOrigin();
		void UpdateOrigin(Vector2 newOrigin);
		void UpdateChildOrigins(Vector2 newOrigin);
		Vector2 GetTruePosition();
		void SetPosition(Vector2 position);
		void SetScale(Vector2 scale);
		void SetRotation(float rotation);
		Vector2 GetPosition();
		Vector2 GetScale();
		float GetRotation();
		std::string GetData();

	private:
		Vector2 origin;
		Vector2 position;
		Vector2 scale;
		float rotation;
	};
}
