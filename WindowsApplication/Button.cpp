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

		// Initialize callback functions to nullptr
		this->OnMouseOverFunction = nullptr;
		this->OnMouseLeaveFunction = nullptr;
		this->OnLeftClickFunction = nullptr;
		this->OnRightClickFunction = nullptr;

		this->attachedScript = "";

	}

	Button::~Button()
	{

	}

	void Button::OnMouseOver(CallbackFunction onMouseOver(int params))
	{	
		int mousePos = 233;
		this->OnMouseOverFunction(mousePos);
	}

	void Button::OnMouseLeave(CallbackFunction onMouseLeave(int params))
	{
		this->_mouseIsOver = false;
		int mouseXPosition = 342;

		this->OnMouseLeaveFunction = onMouseLeave(mouseXPosition);
	}

	void Button::OnMouseLeftClick(CallbackFunction onLeftClick(int params))
	{
		int mouseXPosition = 342;

		this->OnLeftClickFunction = onLeftClick(mouseXPosition);
	}

	void Button::OnMouseRightClick(CallbackFunction onRightClick(int params))
	{
		int mouseXPosition = 342;

		this->OnRightClickFunction = onRightClick(mouseXPosition);
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

	void Button::SetAttachedScript(std::string script)
	{
		this->attachedScript = script;
	}

	bool Button::IsActive()
	{
		return this->_active;
	}

	void Button::SetActiveLayer(int layer)
	{
		if (layer > 0)
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

	std::string Button::GetAttachedScript()
	{
		return this->attachedScript;
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

	std::string Button::GetData()
	{
		json jsonData = {
			{ "type", "Button" },
			{ "_isActive", this->_active },
			{ "activeWidth", this->activeWidth },
			{ "activeHeight", this->activeHeight },
			{ "activeOffsetX", this->activeOffset.x },
			{ "activeOffsetY", this->activeOffset.y },
			{ "attachedScript", this->attachedScript },
			{ "activeLayer", this->activeLayer },
		};

		std::string data = jsonData.dump();
		// Return dumped json object with required data for saving
		return data;
	}
}
