#include "Button.h"
#include "FlatEngine.h"
#include "GameObject.h"
#include "Transform.h"


namespace FlatEngine
{
	Button::Button(long myID, long parentID)
	{
		SetType(T_Button);
		SetID(myID);
		SetParentID(parentID);
		m_b_mouseIsOver = false;
		m_b_hasMouseOverFired = false;
		m_activeWidth = 5;
		m_activeHeight = 3;
		m_activeOffset = Vector2(0, 0);
		m_activeLayer = 0;
	}

	Button::~Button()
	{
	}

	std::string Button::GetData()
	{
		json jsonData = {
			{ "type", "Button" },
			{ "id", GetID() },
			{ "_isCollapsed", IsCollapsed() },
			{ "_isActive", IsActive() },
			{ "activeWidth", m_activeWidth },
			{ "activeHeight", m_activeHeight },
			{ "activeOffsetX", m_activeOffset.x },
			{ "activeOffsetY", m_activeOffset.y },
			{ "activeLayer", m_activeLayer },
		};

		std::string data = jsonData.dump();
		// Return dumped json object with required data for saving
		return data;
	}

	void Button::SetActiveDimensions(float width, float height)
	{
		if (width >= 0 && height >= 0)
		{
			m_activeWidth = width;
			m_activeHeight = height;
		}
		else
		{
			LogError("The active width and height must be larger than 0.");
		}
	}

	void Button::SetActiveOffset(Vector2 offset)
	{
		m_activeOffset = offset;
	}

	void Button::SetActiveLayer(int layer)
	{
		if (layer >= 0)
		{
			m_activeLayer = layer;
		}
		else
		{
			LogError("Button active layer must be an larger than 0.");
		}
	}

	int Button::GetActiveLayer()
	{
		return m_activeLayer;
	}

	float Button::GetActiveWidth()
	{
		return m_activeWidth;
	}

	float Button::GetActiveHeight()
	{
		return m_activeHeight;
	}

	Vector2 Button::GetActiveOffset()
	{
		return m_activeOffset;
	}

	void Button::SetMouseIsOver(bool b_isOver)
	{
		m_b_mouseIsOver = b_isOver;
	}

	void Button::SetIsOverFired(bool b_fired)
	{
		m_b_hasMouseOverFired = b_fired;
	}

	bool Button::MouseIsOver()
	{
		return m_b_mouseIsOver;
	}

	void Button::CalculateActiveEdges()
	{
		Transform* transform = GetParent()->GetTransform();
		
		if (transform != nullptr)
		{
			Vector2 position = transform->GetTruePosition();
			Vector2 scale = transform->GetScale();

			float activeLeft = F_gameViewCenter.x + ((position.x - (m_activeWidth / 2 * scale.x) + m_activeOffset.x * scale.x) * F_gameViewGridStep.x);
			float activeRight = F_gameViewCenter.x + ((position.x + (m_activeWidth / 2 * scale.x) + m_activeOffset.x * scale.x) * F_gameViewGridStep.x);
			float activeTop = F_gameViewCenter.y - ((position.y + (m_activeHeight / 2 * scale.y) + m_activeOffset.y * scale.y) * F_gameViewGridStep.x);
			float activeBottom = F_gameViewCenter.y - ((position.y - (m_activeHeight / 2 * scale.y) + m_activeOffset.y * scale.y) * F_gameViewGridStep.x);

			m_activeEdges = Vector4(activeTop, activeRight, activeBottom, activeLeft);
		}
	}

	Vector4 Button::GetActiveEdges()
	{
		return m_activeEdges;
	}
}
