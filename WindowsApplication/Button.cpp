#include "Button.h"
#include "FlatEngine.h"
#include "GameObject.h"
#include "Transform.h"

namespace FlatEngine
{
	Button::Button(long myID, long parentID)
	{
		SetType(ComponentTypes::Button);
		SetID(myID);
		SetParentID(parentID);
		_mouseIsOver = false;
		_hasMouseOverFired = false;
		activeWidth = 5;
		activeHeight = 3;
		activeOffset = Vector2(0, 0);
		activeLayer = 0;

		// Initialize callback functions to nullptr
		OnMouseOverFunction = nullptr;
		OnMouseLeaveFunction = nullptr;
		OnLeftClickFunction = nullptr;
		OnRightClickFunction = nullptr;

		_mouseOverSet = false;
		_mouseEnterSet = false;
		_mouseLeaveSet = false;
		_leftClickSet = false;
		_rightClickSet = false;
	}

	Button::Button(std::shared_ptr<Button> toCopy, long newParentID)
	{
		SetType(ComponentTypes::Button);
		SetID(GetNextComponentID());
		SetParentID(newParentID);
		SetActive(toCopy->IsActive());
		_mouseIsOver = false;
		_hasMouseOverFired = false;
		activeWidth = toCopy->GetActiveWidth();
		activeHeight = toCopy->GetActiveHeight();
		activeOffset = toCopy->GetActiveOffset();
		activeLayer = toCopy->GetActiveLayer();

		// Initialize callback functions to nullptr
		OnMouseOverFunction = nullptr;
		OnMouseLeaveFunction = nullptr;
		OnLeftClickFunction = nullptr;
		OnRightClickFunction = nullptr;

		_mouseOverSet = false;
		_mouseEnterSet = false;
		_mouseLeaveSet = false;
		_leftClickSet = false;
		_rightClickSet = false;
	}

	Button::~Button()
	{
	}

	void Button::SetOnMouseOver(std::function<void(std::shared_ptr<GameObject>)> callback)
	{	
		OnMouseOverFunction = callback;
		_mouseOverSet = true;
	}

	void Button::SetOnMouseEnter(std::function<void(std::shared_ptr<GameObject>)> callback)
	{
		OnMouseEnterFunction = callback;
		_mouseEnterSet = true;
	}

	void Button::SetOnMouseLeave(std::function<void(std::shared_ptr<GameObject>)> callback)
	{
		OnMouseLeaveFunction = callback;
		_mouseLeaveSet = true;
	}

	void Button::SetOnMouseLeftClick(std::function<void(std::shared_ptr<GameObject>)> callback)
	{
		OnLeftClickFunction = callback;
		_leftClickSet = true;
	}

	void Button::SetOnMouseRightClick(std::function<void(std::shared_ptr<GameObject>)> callback)
	{
		OnRightClickFunction = callback;
		_rightClickSet = true;
	}

	void Button::SetActiveDimensions(float width, float height)
	{
		if (width >= 0 && height >= 0)
		{
			activeWidth = width;
			activeHeight = height;
		}
		else
			FlatEngine::LogString("The active width or height you tried to set to Button component was < 0. Try again.");
	}

	void Button::SetActiveOffset(Vector2 offset)
	{
		// Might need this later VVV
		//std::shared_ptr<FlatEngine::GameObject> parent = GetParent();
		//std::shared_ptr<FlatEngine::Transform> transform = nullptr;
		//Vector2 scale = Vector2(1, 1);

		//if (parent != nullptr)
		//	transform = parent->GetTransformComponent();
		//if (transform != nullptr)
		//	scale = transform->GetScale();

		//activeOffset = offset * scale;

		activeOffset = offset;
	}

	void Button::SetActiveLayer(int layer)
	{
		if (layer >= 0)
			activeLayer = layer;
		else
			FlatEngine::LogString("Button active layer must be an integer greater than 0.");
	}

	int Button::GetActiveLayer()
	{
		return activeLayer;
	}

	float Button::GetActiveWidth()
	{
		return activeWidth;
	}

	float Button::GetActiveHeight()
	{
		return activeHeight;
	}

	Vector2 Button::GetActiveOffset()
	{
		return activeOffset;
	}

	void Button::SetMouseIsOver(bool _isOver)
	{
		_mouseIsOver = _isOver;
	}
	void Button::SetIsOverFired(bool _fired)
	{
		_hasMouseOverFired = _fired;
	}

	bool Button::MouseIsOver()
	{
		return _mouseIsOver;
	}

	void Button::SetActiveEdges(ImVec4 edges)
	{
		activeEdges = edges;
	}

	ImVec4 Button::GetActiveEdges()
	{
		return activeEdges;
	}

	std::string Button::GetData()
	{
		json jsonData = {
			{ "type", "Button" },
			{ "id", GetID() },
			{ "_isCollapsed", IsCollapsed() },
			{ "_isActive", IsActive() },
			{ "activeWidth", activeWidth },
			{ "activeHeight", activeHeight },
			{ "activeOffsetX", activeOffset.x },
			{ "activeOffsetY", activeOffset.y },
			{ "activeLayer", activeLayer },
		};

		std::string data = jsonData.dump();
		// Return dumped json object with required data for saving
		return data;
	}
	void Button::SetConnectedScript(std::string scriptName)
	{
		connectedScript = scriptName;
	}
	std::string Button::GetConnectedScript()
	{
		return connectedScript;
	}
	bool Button::MouseOverSet()
	{
		return _mouseOverSet;
	}
	bool Button::MouseEnterSet()
	{
		return _mouseEnterSet;
	}
	bool Button::MouseLeaveSet()
	{
		return _mouseLeaveSet;
	}
	bool Button::LeftClickSet()
	{
		return _leftClickSet;
	}
	bool Button::RightClickSet()
	{
		return _rightClickSet;
	}
}
