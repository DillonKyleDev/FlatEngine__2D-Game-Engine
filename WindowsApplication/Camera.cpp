#include "Camera.h"
#include "FlatEngine.h"


namespace FlatEngine 
{
	using ComponentTypes = Component::ComponentTypes;

	Camera::Camera(long myID, long parentID)
	{
		this->SetType(ComponentTypes::Camera);
		this->SetID(myID);
		this->SetParentID(parentID);
		this->_isPrimaryCamera = false;
		this->width = 50;
		this->height = 30;
		this->zoom = 10;
		this->frustrumColor = ImVec4(255,255,255,255);
	}

	Camera::~Camera()
	{

	}

	void Camera::FollowTarget(FlatEngine::Transform transform, float ease)
	{

	}

	void Camera::SetPrimaryCamera(bool _isPrimary)
	{
		this->_isPrimaryCamera = _isPrimary;
	}

	bool Camera::IsPrimary()
	{
		return this->_isPrimaryCamera;
	}

	void Camera::SetFrustrumColor(ImVec4 color)
	{
		this->frustrumColor = color;
	}

	ImVec4 Camera::GetFrustrumColor()
	{
		return this->frustrumColor;
	}

	void Camera::SetZoom(float zoom)
	{
		if (zoom >= 1 && zoom <= 100)
			this->zoom = zoom;
	}

	float Camera::GetZoom()
	{
		return this->zoom;
	}

	void Camera::SetDimensions(float width, float height)
	{
		this->width = width;
		this->height = height;
	}

	float Camera::GetWidth()
	{
		return this->width;
	}

	float Camera::GetHeight()
	{
		return this->height;
	}

	std::string Camera::GetData()
	{
		json jsonData = {
			{ "type", "Camera" },
			{ "id", this->GetID() },
			{ "_isCollapsed", this->IsCollapsed() },
			{ "width", this->width },
			{ "height", this->height },
			{ "_isPrimaryCamera", this->_isPrimaryCamera },
			{ "zoom", this->zoom },
			{ "frustrumRed", this->frustrumColor.x },
			{ "frustrumGreen", this->frustrumColor.y },
			{ "frustrumBlue", this->frustrumColor.z },
			{ "frustrumAlpha", this->frustrumColor.w },
		};

		std::string data = jsonData.dump();
		// Return dumped json object with required data for saving
		return data;
	}
}