#pragma once
#include "Vector2.h"
#include "Transform.h"
#include "Component.h"
#include "json.hpp"
#include <imgui.h>


namespace FlatEngine 
{
	class Transform;

	class Camera : public Component
	{
	public:
		Camera(long myID = -1, long parentID = -1);		
		~Camera();

		void SetPrimaryCamera(bool _isPrimary);
		bool IsPrimary();
		void SetDimensions(float width, float height);
		void SetZoom(float zoom);
		float GetZoom();
		float GetWidth();
		float GetHeight();
		void SetFrustrumColor(ImVec4 color);
		ImVec4 GetFrustrumColor();
		void Follow();
		void SetShouldFollow(bool _shouldFollow);
		bool GetShouldFollow();
		void SetFollowing(long toFollow);
		long GetFollowing();
		void SetFollowSmoothing(float smoothing);
		float GetFollowSmoothing();
		std::string GetData();

	private:
		float m_width;
		float m_height;
		float m_zoom;
		ImVec4 m_frustrumColor;
		bool m_b_isPrimaryCamera;
		bool m_b_follow;
		long m_toFollowID;
		float m_followSmoothing;
	};
}

