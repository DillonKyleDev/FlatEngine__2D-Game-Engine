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
		_follow = false;
		following = -1;
		followSmoothing = 0.9f;
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
		_follow = toCopy->_follow;
		following = toCopy->following;
		followSmoothing = toCopy->followSmoothing;
	}

	Camera::~Camera()
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

	void Camera::Follow()
	{
		std::shared_ptr<GameObject> followTarget = GetObjectById(following);
		if (_follow && GetParent()->HasComponent("Transform") && followTarget != nullptr && followTarget->HasComponent("Transform"))
		{
			std::shared_ptr<FlatEngine::Transform> cameraTransform = GetParent()->GetTransformComponent();
			Vector2 followPos = followTarget->GetTransformComponent()->GetTruePosition();
			Vector2 currentPos = cameraTransform->GetPosition(); // Shouldn't have a parent if following so don't need GetTruePosition()

			cameraTransform->SetPosition(Lerp(currentPos, followPos, followSmoothing));
		}
	}

	void Camera::SetShouldFollow(bool _shouldFollow)
	{
		_follow = _shouldFollow;
	}

	bool Camera::GetShouldFollow()
	{
		return _follow;
	}

	void Camera::SetFollowing(long toFollow)
	{
		following = toFollow;
	}

	long Camera::GetFollowing()
	{
		return following;
	}

	void Camera::SetFollowSmoothing(float smoothing)
	{
		followSmoothing = smoothing;
	}

	float Camera::GetFollowSmoothing()
	{
		return followSmoothing;
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
			{ "_follow", _follow },
			{ "followSmoothing", followSmoothing },
			{ "following", following },
		};

		std::string data = jsonData.dump();
		// Return dumped json object with required data for saving
		return data;
	}
}