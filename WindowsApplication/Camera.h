#pragma once
#include "Vector2.h"
#include "Transform.h"
#include "Component.h"
#include "json.hpp"
using json = nlohmann::json;
using namespace nlohmann::literals;



namespace FlatEngine 
{
	class Camera : public Component
	{
	public:
		Camera();
		~Camera();

		void FollowTarget(FlatEngine::Transform transform, float ease);
		void SetPrimaryCamera(bool _isPrimary);
		bool IsPrimary();
		void SetDimensions(float width, float height);
		float GetWidth();
		float GetHeight();
		std::string GetData();

	private:
		float width;
		float height;
		bool _isPrimaryCamera;
	};
}

