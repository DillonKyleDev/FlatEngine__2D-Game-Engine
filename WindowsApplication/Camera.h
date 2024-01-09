#pragma once
#include "Vector2.h"
#include "Transform.h"
#include "Component.h"
#include "json.hpp"
#include <imgui.h>
using json = nlohmann::json;
using namespace nlohmann::literals;



namespace FlatEngine 
{
	class Camera : public Component
	{
	public:
		Camera(long myID, long parentID);
		~Camera();

		void FollowTarget(FlatEngine::Transform transform, float ease);
		void SetPrimaryCamera(bool _isPrimary);
		bool IsPrimary();
		void SetDimensions(float width, float height);
		void SetZoom(float zoom);
		float GetZoom();
		float GetWidth();
		float GetHeight();
		void SetFrustrumColor(ImVec4 color);
		ImVec4 GetFrustrumColor();
		std::string GetData();

	private:
		float width;
		float height;
		float zoom;
		ImVec4 frustrumColor;
		bool _isPrimaryCamera;
	};
}

