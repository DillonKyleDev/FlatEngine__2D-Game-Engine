#include "Camera.h"


namespace FlatEngine 
{
	Camera::Camera()
	{
		this->SetType(Component::ComponentTypes::Camera);
		this->_isPrimaryCamera = false;
		this->width = 400;
		this->height = 300;
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
			{ "width", this->width },
			{ "height", this->height },
			{ "_isPrimaryCamera", this->_isPrimaryCamera }
		};

		std::string data = jsonData.dump();
		// Return dumped json object with required data for saving
		return data;
	}
}