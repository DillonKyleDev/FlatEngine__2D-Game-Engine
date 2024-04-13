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
	class Transform;

	class Camera : public Component
	{
	public:
		Camera(long myID = -1, long parentID = -1);
		Camera(std::shared_ptr<Camera> toCopy, long newParentID);
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

