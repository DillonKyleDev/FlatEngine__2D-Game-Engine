#include "Transform.h"
#include "FlatEngine.h"
#include "BoxCollider.h"
#include "Project.h"


namespace FlatEngine
{
	Transform::Transform(long myID, long parentID)
	{
		SetType(ComponentTypes::T_Transform);
		SetID(myID);
		SetParentID(parentID);
		m_origin = Vector2(0, 0);
		m_position = Vector2(0, 0);
		m_scale = Vector2(1, 1);
		m_rotation = 0;
	}

	Transform::~Transform()
	{
	}

	void Transform::SetInitialPosition(Vector2 initialPos)
	{
		m_position = initialPos;
	}

	void Transform::SetOrigin(Vector2 newOrigin)
	{
		m_origin = newOrigin;
	}

	Vector2 Transform::GetOrigin()
	{
		return m_origin;
	}

	Vector2 Transform::GetTruePosition()
	{
		return Vector2(m_origin.x + m_position.x, m_origin.y + m_position.y);
	}

	std::string Transform::GetData()
	{
		json jsonData = { 
			{ "type", "Transform" },
			{ "id", GetID() },
			{ "_isCollapsed", IsCollapsed() },
			{ "_isActive", IsActive() },
			{ "xOrigin", m_origin.x },
			{ "yOrigin", m_origin.y },
			{ "xPos", m_position.x },
			{ "yPos", m_position.y },
			{ "rotation", m_rotation },
			{ "xScale", m_scale.x },
			{ "yScale", m_scale.y }
		};

		std::string data = jsonData.dump();
		// Return dumped json object with required data for saving
		return data;
	}

	void Transform::SetPosition(Vector2 newPosition)
	{
		m_position = newPosition;

		if (GetParent() != nullptr && GetParent()->HasChildren())
		{
			for (long id : GetParent()->GetChildren())
			{
				GameObject *child = GetObjectById(id);
				if (child->HasComponent("Transform"))
					child->GetTransform()->UpdateOrigin(GetTruePosition());
			}
		}
	}

	void Transform::UpdateOrigin(Vector2 newOrigin)
	{
		m_origin = newOrigin;
		UpdateChildOrigins(GetTruePosition());
	}

	void Transform::UpdateChildOrigins(Vector2 newOrigin)
	{
		if (GetParent()->HasChildren())
		{
			for (long id : GetParent()->GetChildren())
			{
				GameObject *child = GetObjectById(id);
				if (child->HasComponent("Transform"))
				{
					FlatEngine::Transform* childTransform = child->GetTransform();
					childTransform->SetOrigin(newOrigin);
					childTransform->UpdateChildOrigins(childTransform->GetTruePosition());
				}
			}
		}
	}

	void Transform::SetScale(Vector2 newScale)
	{
		m_scale = newScale;
	}

	void Transform::SetRotation(float newRotation)
	{
		m_rotation = newRotation;
	}

	Vector2 Transform::GetPosition()
	{
		return m_position;
	}

	Vector2 Transform::GetScale()
	{
		return m_scale;
	}

	float Transform::GetRotation()
	{
		return m_rotation;
	}

	void Transform::LookAt(Vector2 lookAt)
	{
		Vector2 slope = Vector2(lookAt.x - GetTruePosition().x, lookAt.y - GetTruePosition().y);
		float angle = atan(slope.y / slope.x) * 180.0f / (float)M_PI;
		LogFloat(angle, "Angle: ");
		m_rotation = angle;
	}
}
