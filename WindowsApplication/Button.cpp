#include "Button.h"
#include "FlatEngine.h"


namespace FlatEngine
{
	Button::Button(long myID, long parentID)
	{
		this->SetType(ComponentTypes::Button);
		this->SetID(myID);
		this->SetParentID(parentID);
		this->_mouseIsOver = false;
		this->_hasMouseOverFired = false;
		this->_active = true;
		this->activeWidth = 5;
		this->activeHeight = 3;
		this->activeOffset = Vector2(0, 0);
		this->activeLayer = 0;

		// Initialize callback functions to nullptr
		this->OnMouseOverFunction = nullptr;
		this->OnMouseLeaveFunction = nullptr;
		this->OnLeftClickFunction = nullptr;
		this->OnRightClickFunction = nullptr;

		this->_mouseOverSet = false;
		this->_mouseEnterSet = false;
		this->_mouseLeaveSet = false;
		this->_leftClickSet = false;
		this->_rightClickSet = false;
	}

	Button::~Button()
	{
	}

	void Button::OnMouseOver(std::function<void(std::shared_ptr<GameObject>)> callback)
	{	
		this->OnMouseOverFunction = callback;
		this->_mouseOverSet = true;
	}

	void Button::OnMouseEnter(std::function<void(std::shared_ptr<GameObject>)> callback)
	{
		this->OnMouseEnterFunction = callback;
		this->_mouseEnterSet = true;
	}

	void Button::OnMouseLeave(std::function<void(std::shared_ptr<GameObject>)> callback)
	{
		this->OnMouseLeaveFunction = callback;
		this->_mouseLeaveSet = true;
	}

	void Button::OnMouseLeftClick(std::function<void(std::shared_ptr<GameObject>)> callback)
	{
		this->OnLeftClickFunction = callback;
		this->_leftClickSet = true;
	}

	void Button::OnMouseRightClick(std::function<void(std::shared_ptr<GameObject>)> callback)
	{
		this->OnRightClickFunction = callback;
		this->_rightClickSet = true;
	}

	void Button::SetActive(bool _active)
	{
		this->_active = _active;
	}

	void Button::SetActiveDimensions(float width, float height)
	{
		if (width >= 0 && height >= 0)
		{
			this->activeWidth = width;
			this->activeHeight = height;
		}
		else
			FlatEngine::LogString("The active width or height you tried to set to Button component was < 0. Try again.");
	}

	void Button::SetActiveOffset(Vector2 offset)
	{
		this->activeOffset = offset;
	}

	bool Button::IsActive()
	{
		return this->_active;
	}

	void Button::SetActiveLayer(int layer)
	{
		if (layer >= 0)
			this->activeLayer = layer;
		else
			FlatEngine::LogString("Button active layer must be an integer greater than 0.");
	}

	int Button::GetActiveLayer()
	{
		return this->activeLayer;
	}

	float Button::GetActiveWidth()
	{
		return this->activeWidth;
	}

	float Button::GetActiveHeight()
	{
		return this->activeHeight;
	}

	Vector2 Button::GetActiveOffset()
	{
		return this->activeOffset;
	}

	void Button::SetMouseIsOver(bool _isOver)
	{
		this->_mouseIsOver = _isOver;
	}
	void Button::SetIsOverFired(bool _fired)
	{
		this->_hasMouseOverFired = _fired;
	}

	bool Button::MouseIsOver()
	{
		return this->_mouseIsOver;
	}

	void Button::SetActiveEdges(ImVec4 edges)
	{
		this->activeEdges = edges;
	}

	ImVec4 Button::GetActiveEdges()
	{
		return this->activeEdges;
	}

	std::string Button::GetData()
	{
		json jsonData = {
			{ "type", "Button" },
			{ "id", this->GetID() },
			{ "_isCollapsed", this->IsCollapsed() },
			{ "_isActive", this->_active },
			{ "activeWidth", this->activeWidth },
			{ "activeHeight", this->activeHeight },
			{ "activeOffsetX", this->activeOffset.x },
			{ "activeOffsetY", this->activeOffset.y },
			{ "activeLayer", this->activeLayer },
		};

		std::string data = jsonData.dump();
		// Return dumped json object with required data for saving
		return data;
	}
	bool Button::MouseOverSet()
	{
		return this->_mouseOverSet;
	}
	bool Button::MouseEnterSet()
	{
		return this->_mouseEnterSet;
	}
	bool Button::MouseLeaveSet()
	{
		return this->_mouseLeaveSet;
	}
	bool Button::LeftClickSet()
	{
		return this->_leftClickSet;
	}
	bool Button::RightClickSet()
	{
		return this->_rightClickSet;
	}
}
