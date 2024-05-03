#include "Camera.h"
#include "FlatEngine.h"
#include "Transform.h"


namespace FlatEngine 
{
	using ComponentTypes = Component::ComponentTypes;

	Camera::Camera(long myID, long parentID)
	{
		SetType(ComponentTypes::Camera);
		SetID(myID);
		SetParentID(parentID);
		_isPrimaryCamera = false;
		width = 50;
		height = 30;
		zoom = 10;
		frustrumColor = ImVec4(255,255,255,255);
	}

	Camera::Camera(std::shared_ptr<Camera> toCopy, long newParentID)
	{
		SetType(ComponentTypes::Camera);
		SetID(GetNextComponentID());
		SetParentID(newParentID);
		SetActive(toCopy->IsActive());
		_isPrimaryCamera = toCopy->IsPrimary();
		width = toCopy->GetWidth();
		height = toCopy->GetHeight();
		zoom = toCopy->GetZoom();
		frustrumColor = toCopy->GetFrustrumColor();
	}

	Camera::~Camera()
	{
	}

	void Camera::FollowTarget(FlatEngine::Transform transform, float ease)
	{

	}

	void Camera::SetPrimaryCamera(bool _isPrimary)
	{
		_isPrimaryCamera = _isPrimary;
	}

	bool Camera::IsPrimary()
	{
		return _isPrimaryCamera;
	}

	void Camera::SetFrustrumColor(ImVec4 color)
	{
		frustrumColor = color;
	}

	ImVec4 Camera::GetFrustrumColor()
	{
		return frustrumColor;
	}

	void Camera::SetZoom(float newZoom)
	{
		if (newZoom >= 1 && newZoom <= 100)
			zoom = newZoom;
	}

	float Camera::GetZoom()
	{
		return zoom;
	}

	void Camera::SetDimensions(float newWidth, float newHeight)
	{
		width = newWidth;
		height = newHeight;
	}

	float Camera::GetWidth()
	{
		return width;
	}

	float Camera::GetHeight()
	{
		return height;
	}

	std::string Camera::GetData()
	{
		json jsonData = {
			{ "type", "Camera" },
			{ "id", GetID() },
			{ "_isCollapsed", IsCollapsed() },
			{ "_isActive", IsActive() },
			{ "width", width },
			{ "height", height },
			{ "_isPrimaryCamera", _isPrimaryCamera },
			{ "zoom", zoom },
			{ "frustrumRed", frustrumColor.x },
			{ "frustrumGreen", frustrumColor.y },
			{ "frustrumBlue", frustrumColor.z },
			{ "frustrumAlpha", frustrumColor.w },
		};

		std::string data = jsonData.dump();
		// Return dumped json object with required data for saving
		return data;
	}
}