#include "BoxCollider.h"
#include "FlatEngine.h"
#include "GameObject.h"

namespace FlatEngine
{
	BoxCollider::BoxCollider(long myID, long parentID)
	{
		this->SetType(ComponentTypes::BoxCollider);
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
		this->OnActiveCollisionFunction = nullptr;
		this->OnCollisionEnterFunction = nullptr;
		this->OnCollisionLeaveFunction = nullptr;

		this->_mouseOverSet = false;
		this->_mouseEnterSet = false;
		this->_mouseLeaveSet = false;
		this->_leftClickSet = false;
		this->_rightClickSet = false;
	}

	BoxCollider::BoxCollider(std::shared_ptr<BoxCollider> toCopy)
	{
		this->SetType(ComponentTypes::Button);
		this->SetID(GetNextComponentID());
		this->SetParentID(toCopy->GetParentID());
		this->_mouseIsOver = false;
		this->_hasMouseOverFired = false;
		this->_active = toCopy->IsActive();
		this->activeWidth = toCopy->GetActiveWidth();
		this->activeHeight = toCopy->GetActiveHeight();
		this->activeOffset = toCopy->GetActiveOffset();
		this->activeLayer = toCopy->GetActiveLayer();

		// Initialize callback functions to nullptr
		this->OnActiveCollisionFunction = nullptr;
		this->OnCollisionEnterFunction = nullptr;
		this->OnCollisionLeaveFunction = nullptr;

		this->_mouseOverSet = false;
		this->_mouseEnterSet = false;
		this->_mouseLeaveSet = false;
		this->_leftClickSet = false;
		this->_rightClickSet = false;
	}

	BoxCollider::~BoxCollider()
	{
	}

	void BoxCollider::SetOnOverlapping(std::function<void(std::shared_ptr<GameObject>, std::shared_ptr<GameObject>)> callback)
	{
		this->OnActiveCollisionFunction = callback;
		this->_mouseOverSet = true;
	}

	void BoxCollider::SetOnCollisionEnter(std::function<void(std::shared_ptr<GameObject>, std::shared_ptr<GameObject>)> callback)
	{
		this->OnCollisionEnterFunction = callback;
		this->_mouseEnterSet = true;
	}

	void BoxCollider::SetOnCollisionLeave(std::function<void(std::shared_ptr<GameObject>, std::shared_ptr<GameObject>)> callback)
	{
		this->OnCollisionLeaveFunction = callback;
		this->_mouseLeaveSet = true;
	}

	void BoxCollider::SetActive(bool _active)
	{
		this->_active = _active;
	}

	void BoxCollider::SetActiveDimensions(float width, float height)
	{
		if (width >= 0 && height >= 0)
		{
			this->activeWidth = width;
			this->activeHeight = height;
		}
		else
			FlatEngine::LogString("The active width or height you tried to set to BoxCollider component was < 0. Try again.");
	}

	void BoxCollider::SetActiveOffset(Vector2 offset)
	{
		this->activeOffset = offset;
	}

	bool BoxCollider::IsActive()
	{
		return this->_active;
	}

	void BoxCollider::SetActiveLayer(int layer)
	{
		if (layer >= 0)
			this->activeLayer = layer;
		else
			FlatEngine::LogString("BoxCollider active layer must be an integer greater than 0.");
	}

	int BoxCollider::GetActiveLayer()
	{
		return this->activeLayer;
	}

	float BoxCollider::GetActiveWidth()
	{
		return this->activeWidth;
	}

	float BoxCollider::GetActiveHeight()
	{
		return this->activeHeight;
	}

	Vector2 BoxCollider::GetActiveOffset()
	{
		return this->activeOffset;
	}

	void BoxCollider::SetMouseIsOver(bool _isOver)
	{
		this->_mouseIsOver = _isOver;
	}
	void BoxCollider::SetIsOverFired(bool _fired)
	{
		this->_hasMouseOverFired = _fired;
	}

	bool BoxCollider::MouseIsOver()
	{
		return this->_mouseIsOver;
	}

	void BoxCollider::SetActiveEdges(ImVec4 edges)
	{
		this->activeEdges = edges;
	}

	ImVec4 BoxCollider::GetActiveEdges()
	{
		return this->activeEdges;
	}

	std::string BoxCollider::GetData()
	{
		json jsonData = {
			{ "type", "BoxCollider" },
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
	void BoxCollider::SetConnectedScript(std::string scriptName)
	{
		this->connectedScript = scriptName;
	}
	std::string BoxCollider::GetConnectedScript()
	{
		return this->connectedScript;
	}
	bool BoxCollider::MouseOverSet()
	{
		return this->_mouseOverSet;
	}
	bool BoxCollider::MouseEnterSet()
	{
		return this->_mouseEnterSet;
	}
	bool BoxCollider::MouseLeaveSet()
	{
		return this->_mouseLeaveSet;
	}
	bool BoxCollider::LeftClickSet()
	{
		return this->_leftClickSet;
	}
	bool BoxCollider::RightClickSet()
	{
		return this->_rightClickSet;
	}
}
