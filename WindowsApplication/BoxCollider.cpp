#include "BoxCollider.h"
#include "RigidBody.h"
#include "FlatEngine.h"
#include "GameObject.h"
#include "Transform.h"
#include "imgui_internal.h"
#include <cmath>


namespace FlatEngine
{
	BoxCollider::BoxCollider(long myID, long parentID)
	{
		SetType(ComponentTypes::BoxCollider);
		SetID(myID);
		SetParentID(parentID);
		activeWidth = 5;
		activeHeight = 3;
		activeOffset = Vector2(0, 0);
		activeEdges = Vector4(0, 0, 0, 0);
		activeLayer = 0;
		_isContinuous = true;
		_isStatic = false;
		_isColliding = false;
		_activeEdgesSet = false;
		previousPosition = Vector2(0, 0);

		activeLeft = 0;
		activeRight = 0;
		activeBottom = 0;
		activeTop = 0;
		nextActiveLeft = 0;
		nextActiveRight = 0;
		nextActiveBottom = 0;
		nextActiveTop = 0;
		nextCorners;
		centerGrid = Vector2(0, 0);
		nextCenterGrid = Vector2(0, 0);
		centerCoord = Vector2(0, 0);
		nextCenterCoord = Vector2(0, 0);
		activeRadiusScreen = 0;
		activeRadiusGrid = 0;
		_showActiveRadius = false;

		// CREATE A SAVED JSON FIELD THAT WILL LOAD ROTATION EVERY RELOAD
		rotation = 0;

		// Initialize callback functions to nullptr
		OnActiveCollision = nullptr;
		OnCollisionEnter = nullptr;
		OnCollisionLeave = nullptr;

		_onActiveCollidingSet = false;
		_onCollisionEnterSet = false;
		_onCollisionLeaveSet = false;

		_isCollidingRight = false;
		_isCollidingLeft = false;
		_isCollidingBottom = false;
		_isCollidingTop = false;

		_rightCollisionStatic = false;
		_leftCollisionStatic = false;
		_bottomCollisionStatic = false;
		_topCollisionStatic = false;

		rightCollision = 0;
		leftCollision = 0;
		bottomCollision = 0;
		topCollision = 0;
	}

	BoxCollider::BoxCollider(std::shared_ptr<BoxCollider> toCopy, long newParentID)
	{
		SetType(ComponentTypes::BoxCollider);
		SetID(GetNextComponentID());
		SetParentID(newParentID);
		SetActive(toCopy->IsActive());
		activeWidth = toCopy->GetActiveWidth();
		activeHeight = toCopy->GetActiveHeight();
		activeOffset = toCopy->GetActiveOffset();
		activeLayer = toCopy->GetActiveLayer();
		_isContinuous = toCopy->IsContinuous();
		_isStatic = toCopy->IsStatic();
		_isColliding = false;
		_activeEdgesSet = false;
		previousPosition = Vector2(0, 0);
		activeRadiusScreen = 0;
		activeRadiusGrid = 0;
		_showActiveRadius = toCopy->_showActiveRadius;

		activeLeft = 0;
		activeRight = 0;
		activeBottom = 0;
		activeTop = 0;
		nextActiveLeft = 0;
		nextActiveRight = 0;
		nextActiveBottom = 0;
		nextActiveTop = 0;

		// Initialize callback functions to nullptr
		OnActiveCollision = toCopy->OnActiveCollision;
		OnCollisionEnter = toCopy->OnCollisionEnter;
		OnCollisionLeave = toCopy->OnCollisionLeave;

		_onActiveCollidingSet = false;
		_onCollisionEnterSet = false;
		_onCollisionLeaveSet = false;

		_isCollidingRight = false;
		_isCollidingLeft = false;
		_isCollidingBottom = false;
		_isCollidingTop = false;

		_rightCollisionStatic = false;
		_leftCollisionStatic = false;
		_bottomCollisionStatic = false;
		_topCollisionStatic = false;

		rightCollision = 0;
		leftCollision = 0;
		bottomCollision = 0;
		topCollision = 0;
	}

	BoxCollider::~BoxCollider()
	{
	}

	void BoxCollider::SetOnColliding(std::function<void(std::shared_ptr<GameObject>, std::shared_ptr<GameObject>)> callback)
	{
		OnActiveCollision = callback;
		_onActiveCollidingSet = true;
	}

	void BoxCollider::SetOnCollisionEnter(std::function<void(std::shared_ptr<GameObject>, std::shared_ptr<GameObject>)> callback)
	{
		OnCollisionEnter = callback;
		_onCollisionEnterSet = true;
	}

	void BoxCollider::SetOnCollisionLeave(std::function<void(std::shared_ptr<GameObject>, std::shared_ptr<GameObject>)> callback)
	{
		OnCollisionLeave = callback;
		_onCollisionLeaveSet = true;
	}

	bool BoxCollider::CheckForCollision(std::shared_ptr<BoxCollider> other)
	{
		if (loadedProject->GetCollisionDetection() == "Simple Box")
		{
			return SimpleBoxCheckForCollision(other);
		}
		else if (loadedProject->GetCollisionDetection() == "Separating Axis (Rotational)")
		{
			// TODO
			return false;
		}
		return false;
	}

	bool BoxCollider::SimpleBoxCheckForCollision(std::shared_ptr<BoxCollider> other)
	{
		bool _colliding = false;

		// Calculate activeRadius with pythag
		float rise = std::abs(other->centerGrid.y - centerGrid.y);
		float run = std::abs(other->centerGrid.x - centerGrid.x);
		float centerDistance = std::sqrt((rise * rise) + (run * run));

		if (centerDistance < other->activeRadiusGrid + activeRadiusGrid)
		{
			float A_TopEdge = nextActiveTop;
			float A_RightEdge = nextActiveRight;
			float A_BottomEdge = nextActiveBottom;
			float A_LeftEdge = nextActiveLeft;

			float B_TopEdge = other->nextActiveTop;
			float B_RightEdge = other->nextActiveRight;
			float B_BottomEdge = other->nextActiveBottom;
			float B_LeftEdge = other->nextActiveLeft;

			_colliding = ((A_LeftEdge < B_RightEdge) && (A_RightEdge > B_LeftEdge) && (A_BottomEdge < B_TopEdge) && (A_TopEdge > B_BottomEdge));

			if (_colliding)
			{
				AddCollidingObject(other->GetParent());

				// Fire OnActiveCollision while there is a collision happening
				if (OnActiveCollisionSet())
					OnActiveCollision(GetParent(), other->GetParent());

				// Check which direction the collision is happening from //
				// if self is to the right
				if (centerGrid.x > other->centerGrid.x)
				{
					// if self is below other
					if (centerGrid.y < other->centerGrid.y)
					{
						float leftRightOverlap = B_RightEdge - A_LeftEdge;
						float topBottomOverlap = A_TopEdge - B_BottomEdge;
						if (leftRightOverlap < topBottomOverlap)
						{
							_isCollidingLeft = true;
							_leftCollisionStatic = other->_isStatic;
							other->_isCollidingRight = true;
							other->_rightCollisionStatic = _isStatic;
						}
						else {
							_isCollidingTop = true;
							_topCollisionStatic = other->_isStatic;
							other->_isCollidingBottom = true;
							other->_bottomCollisionStatic = _isStatic;
						}
					}
					// if self is above other
					else if (centerGrid.y > other->centerGrid.y)
					{
						float leftRightOverlap = B_RightEdge - A_LeftEdge;
						float topBottomOverlap = B_TopEdge - A_BottomEdge;
						if (leftRightOverlap < topBottomOverlap)
						{
							_isCollidingLeft = true;
							_leftCollisionStatic = other->_isStatic;
							other->_isCollidingRight = true;
							other->_rightCollisionStatic = _isStatic;
						}
						else {
							_isCollidingBottom = true;
							_bottomCollisionStatic = other->_isStatic;
							other->_isCollidingTop = true;
							other->_topCollisionStatic = _isStatic;
						}
					}
				}
				// if self is to the left
				else if (centerGrid.x < other->centerGrid.x)
				{
					// if self is below other
					if (centerGrid.y < other->centerGrid.y)
					{
						float leftRightOverlap = A_RightEdge - B_LeftEdge;
						float topBottomOverlap = A_TopEdge - B_BottomEdge;
						if (leftRightOverlap < topBottomOverlap)
						{
							_isCollidingRight = true;
							_rightCollisionStatic = other->_isStatic;
							other->_isCollidingLeft = true;
							other->_leftCollisionStatic = _isStatic;
						}
						else {
							_isCollidingTop = true;
							_topCollisionStatic = other->_isStatic;
							other->_isCollidingBottom = true;
							other->_bottomCollisionStatic = _isStatic;
						}
					}
					else if (centerGrid.y > other->centerGrid.y)
					{
						float leftRightOverlap = A_RightEdge - B_LeftEdge;
						float topBottomOverlap = B_TopEdge - A_BottomEdge;
						if (leftRightOverlap < topBottomOverlap)
						{
							_isCollidingRight = true;
							_rightCollisionStatic = other->_isStatic;
							other->_isCollidingLeft = true;
							other->_leftCollisionStatic = _isStatic;
						}
						else {
							_isCollidingBottom = true;
							_bottomCollisionStatic = other->_isStatic;
							other->_isCollidingTop = true;
							other->_topCollisionStatic = _isStatic;
						}
					}
				}
			}

			if (GetParent() != nullptr && GetParent()->HasComponent("RigidBody"))
			{
				if (_isCollidingBottom && other->_isStatic)
				{
					GetParent()->GetRigidBody()->SetIsGrounded(true);
					bottomCollision = B_TopEdge;
				}
				if (_isCollidingTop && other->_isStatic)
				{
					topCollision = B_BottomEdge;
				}
				if (_isCollidingRight && other->_isStatic)
				{
					rightCollision = B_LeftEdge;
				}
				if (_isCollidingLeft && other->_isStatic)
				{
					leftCollision = B_RightEdge;
				}
			}
			if (other->GetParent() != nullptr && other->GetParent()->HasComponent("RigidBody"))
			{
				if (other->_isCollidingBottom && _isStatic)
				{
					other->GetParent()->GetRigidBody()->SetIsGrounded(true);
					other->bottomCollision = A_TopEdge;
				}
				if (other->_isCollidingTop && _isStatic)
				{
					other->topCollision = A_BottomEdge;
				}
				if (other->_isCollidingRight && _isStatic)
				{
					other->rightCollision = A_LeftEdge;
				}
				if (other->_isCollidingLeft && _isStatic)
				{
					other->leftCollision = A_RightEdge;
				}
			}

			_isColliding = _colliding;
			other->SetColliding(_colliding);
		}

		return _colliding;
	}

	bool BoxCollider::IsColliding()
	{
		return _isColliding;
	}

	void BoxCollider::SetColliding(bool _colliding)
	{
		_isColliding = _colliding;
	}

	void BoxCollider::UpdatePreviousPosition()
	{
		std::shared_ptr<FlatEngine::Transform> transform = GetParent()->GetTransformComponent();
		previousPosition = transform->GetTruePosition();
	}

	bool BoxCollider::HasMoved()
	{
		std::shared_ptr<FlatEngine::Transform> transform = GetParent()->GetTransformComponent();
		Vector2 position = transform->GetPosition();

		return (previousPosition.x != position.x || previousPosition.y != position.y);
	}

	void BoxCollider::RemoveCollidingObject(std::shared_ptr<GameObject> object)
	{
		for (std::vector<std::shared_ptr<GameObject>>::iterator iterator = collidingObjects.begin(); iterator != collidingObjects.end();)
		{
			if ((*iterator)->GetID() == object->GetID())
				collidingObjects.erase(iterator);

			iterator++;
		}
	}

	void BoxCollider::AddCollidingObject(std::shared_ptr<GameObject> collidedWith)
	{
		for (std::shared_ptr<GameObject> object : collidingObjects)
		{
			// Leave function if the object is already known to be in active collision
			if (object->GetID() == collidedWith->GetID())
				return;
		}
		collidingObjects.push_back(collidedWith);

		// If OnCollisionEnter is set, fire it now. (upon initially adding the object to collidingObjects for the first time)
		if (OnCollisionEnterSet())
			OnCollisionEnter(GetParent(), collidedWith);
	}

	std::vector<std::shared_ptr<GameObject>> BoxCollider::GetCollidingObjects()
	{
		return collidingObjects;
	}

	void BoxCollider::SetActiveDimensions(float width, float height)
	{
		if (width >= 0 && height >= 0)
		{
			activeWidth = width;
			activeHeight = height;
		}
		else
			FlatEngine::LogString("The active width or height you tried to set to BoxCollider component was < 0. Try again.");
	}

	void BoxCollider::SetActiveOffset(Vector2 offset)
	{
		activeOffset = offset;
	}

	void BoxCollider::SetActiveLayer(int layer)
	{
		if (layer >= 0)
			activeLayer = layer;
		else
			FlatEngine::LogString("BoxCollider active layer must be an integer greater than 0.");
	}

	int BoxCollider::GetActiveLayer()
	{
		return activeLayer;
	}

	float BoxCollider::GetActiveWidth()
	{
		return activeWidth;
	}

	float BoxCollider::GetActiveHeight()
	{
		return activeHeight;
	}

	Vector2 BoxCollider::GetActiveOffset()
	{
		return activeOffset;
	}

	void BoxCollider::SetActiveEdges(Vector4 edges)
	{
		activeEdges = edges;
		_activeEdgesSet = true;
	}

	Vector4 BoxCollider::GetActiveEdges()
	{
		return activeEdges;
	}

	// Just based on actual pixel locations (0,0) being the top left of the window
	// You can use it for either game view or scene view, you just need the correct center location of whichever you choose
	Vector4 BoxCollider::UpdateActiveEdges()
	{
		if (loadedProject->GetCollisionDetection() == "Simple Box")
		{
			SimpleBoxUpdateEdges();
		}
		else if(loadedProject->GetCollisionDetection() == "Separating Axis (Rotational)")
		{

		}
		return activeEdges;
	}

	void BoxCollider::SimpleBoxUpdateEdges()
	{
		// Only if the activeEdges has not been set or if the velocity is not 0 do we update the active edges
		bool _shouldUpdate = false;

		std::shared_ptr<FlatEngine::GameObject> parent = GetParent();
		std::shared_ptr<FlatEngine::Transform> transform = nullptr;
		Vector2 scale = Vector2(1, 1);

		if (parent != nullptr)
			transform = parent->GetTransformComponent();
		if (transform != nullptr)
			scale = transform->GetScale();

		std::shared_ptr<FlatEngine::RigidBody> rigidBody;
		if (GetParent() != nullptr && GetParent()->HasComponent("RigidBody"))
		{
			rigidBody = GetParent()->GetRigidBody();
			Vector2 velocity = rigidBody->GetVelocity();

			if (velocity.x != 0 || velocity.y != 0 || !_activeEdgesSet || HasMoved())
				_shouldUpdate = true;
		}
		else
		{
			if (!_activeEdgesSet || HasMoved())
				_shouldUpdate = true;
		}

		if (_shouldUpdate)
		{
			std::shared_ptr<FlatEngine::RigidBody> rigidBody = parent->GetRigidBody();

			float step = FlatGui::sceneViewGridStep.x;
			Vector2 centerPoint = FlatGui::sceneViewCenter;
			std::shared_ptr<FlatEngine::Transform> transform = GetParent()->GetTransformComponent();
			Vector2 scale = transform->GetScale();

			// For visual representation ( screen space values )
			centerGrid = transform->GetTruePosition();

			activeLeft = centerPoint.x + (centerGrid.x - (activeWidth * scale.x / 2) + activeOffset.x) * step;
			activeTop = centerPoint.y + (-centerGrid.y - (activeHeight * scale.y / 2) + activeOffset.y) * step;
			activeRight = centerPoint.x + (centerGrid.x + (activeWidth * scale.x / 2) + activeOffset.x) * step;
			activeBottom = centerPoint.y + (-centerGrid.y + (activeHeight * scale.y / 2) + activeOffset.y) * step;

			centerCoord = Vector2(activeLeft + (activeRight - activeLeft) / 2, activeTop + (activeBottom - activeTop) / 2);

			// For collision detection ( grid space values )
			if (rigidBody != nullptr)
				nextCenterGrid = rigidBody->GetNextPosition();
			else
				nextCenterGrid = transform->GetTruePosition();

			nextActiveLeft = nextCenterGrid.x - (activeWidth * scale.x / 2) + activeOffset.x;
			nextActiveTop = nextCenterGrid.y + (activeHeight * scale.y / 2) + activeOffset.y;
			nextActiveRight = nextCenterGrid.x + (activeWidth * scale.x / 2) + activeOffset.x;
			nextActiveBottom = nextCenterGrid.y - (activeHeight * scale.y / 2) + activeOffset.y;
		
			nextCenterCoord = Vector2(nextActiveLeft + (nextActiveRight - nextActiveLeft) / 2, nextActiveTop + (nextActiveBottom - nextActiveTop) / 2);

			SimpleBoxUpdateCorners();
		}
	}

	float BoxCollider::GetActiveRadiusScreen()
	{
		return activeRadiusScreen;
	}

	float BoxCollider::GetActiveRadiusGrid()
	{
		return activeRadiusGrid;
	}

	void  BoxCollider::SetShowActiveRadius(bool _show)
	{
		_showActiveRadius = _show;
	}

	bool  BoxCollider::GetShowActiveRadius()
	{
		return _showActiveRadius;
	}

	void BoxCollider::UpdateNormals()
	{
		float cos_a = cosf(rotation * 2.0f * (float)M_PI / 360.0f); // Convert degrees into radians
		float sin_a = sinf(rotation * 2.0f * (float)M_PI / 360.0f);
		float step = FlatGui::sceneViewGridStep.x;
		Vector2 centerPoint = FlatGui::sceneViewCenter;	
		Vector2 scale = GetParent()->GetTransformComponent()->GetScale();

		// Normal vectors without rotation
		Vector2 topNormal = Vector2(0, -activeHeight * step * scale.y);
		Vector2 rightNormal = Vector2(+activeWidth * step * scale.x, 0);
		Vector2 bottomNormal = Vector2(0, +activeHeight * step * scale.y);
		Vector2 leftNormal = Vector2(-activeWidth * step * scale.x, 0);

		if (rotation != 0)
		{
			// Normal vectors with rotation
			topNormal = ImRotate(Vector2(0, -activeHeight * step * scale.y), cos_a, sin_a);
			rightNormal = ImRotate(Vector2(+activeWidth * step * scale.x, 0), cos_a, sin_a);
			bottomNormal = ImRotate(Vector2(0, +activeHeight * step * scale.y), cos_a, sin_a);
			leftNormal = ImRotate(Vector2(-activeWidth * step * scale.x, 0), cos_a, sin_a);
		}

		Vector2 newNormals[4] =
		{
			Vector2(center.x + topNormal.x, center.y + topNormal.y),
			Vector2(center.x + rightNormal.x, center.y + rightNormal.y),
			Vector2(center.x + bottomNormal.x, center.y + bottomNormal.y),
			Vector2(center.x + leftNormal.x, center.y + leftNormal.y),
		};
		
		SetNormals(newNormals);
	}

	void BoxCollider::UpdateCorners()
	{
		float cos_a = cosf(rotation * 2.0f * (float)M_PI / 360.0f); // Convert degrees into radians
		float sin_a = sinf(rotation * 2.0f * (float)M_PI / 360.0f);
		float step = FlatGui::sceneViewGridStep.x;
		Vector2 centerPoint = FlatGui::sceneViewCenter;
		std::shared_ptr<FlatEngine::Transform> transform = GetParent()->GetTransformComponent();
		Vector2 scale = transform->GetScale();

		// Corners without rotation
		Vector2 topLeft = { activeLeft, activeTop };
		Vector2 bottomRight = { activeRight, activeBottom };
		Vector2 topRight = { activeRight, activeTop };
		Vector2 bottomLeft = { activeLeft, activeBottom };		

		if (rotation != 0)
		{
			// Corners with rotation
			topLeft = ImRotate(Vector2(-activeWidth * step / 2 * scale.x, -activeHeight * step / 2 * scale.y), cos_a, sin_a);			
			topRight = ImRotate(Vector2(+activeWidth * step / 2 * scale.x, -activeHeight * step / 2 * scale.y), cos_a, sin_a);
			bottomRight = ImRotate(Vector2(+activeWidth * step / 2 * scale.x, +activeHeight * step / 2 * scale.y), cos_a, sin_a);
			bottomLeft = ImRotate(Vector2(-activeWidth * step / 2 * scale.x, +activeHeight * step / 2 * scale.y), cos_a, sin_a);
		}

		Vector2 newCorners[4] =
		{
			Vector2(center.x + topLeft.x, center.y + topLeft.y),
			Vector2(center.x + topRight.x, center.y + topRight.y),
			Vector2(center.x + bottomRight.x, center.y + bottomRight.y),
			Vector2(center.x + bottomLeft.x, center.y + bottomLeft.y),
		};

		SetCorners(newCorners);
	}

	// Corners without rotation
	void BoxCollider::SimpleBoxUpdateCorners()
	{
		float step = FlatGui::sceneViewGridStep.x;
		Vector2 centerPoint = FlatGui::sceneViewCenter;
		std::shared_ptr<FlatEngine::Transform> transform = GetParent()->GetTransformComponent();
		Vector2 scale = transform->GetScale();

		// For visual representation
		Vector2 topLeft = { activeLeft, activeTop };
		Vector2 topRight = { activeRight, activeTop };
		Vector2 bottomRight = { activeRight, activeBottom };
		Vector2 bottomLeft = { activeLeft, activeBottom };

		Vector2 newCorners[4] =
		{
			topLeft,
			topRight,
			bottomRight,
			bottomLeft
		};
		SetCorners(newCorners);

		// Calculate activeRadius with pythag
		float rise = std::abs(topLeft.y - centerCoord.y);
		float run = std::abs(topLeft.x - centerCoord.x);
		activeRadiusScreen = std::sqrt((rise * rise) + (run * run));
		activeRadiusGrid = activeRadiusScreen / step;

		// For collision detection
		Vector2 nextTopLeft = { nextActiveLeft, nextActiveTop };
		Vector2 nextTopRight = { nextActiveRight, nextActiveTop };
		Vector2 nextBottomRight = { nextActiveRight, nextActiveBottom };
		Vector2 nextBottomLeft = { nextActiveLeft, nextActiveBottom };

		Vector2 newNextCorners[4] =
		{
			nextTopLeft,
			nextTopRight,
			nextBottomRight,
			nextBottomLeft
		};
		SetNextCorners(newNextCorners);
	}

	bool BoxCollider::OnActiveCollisionSet()
	{
		return _onActiveCollidingSet;
	}

	bool BoxCollider::OnCollisionEnterSet()
	{
		return _onCollisionEnterSet;
	}

	bool BoxCollider::OnCollisionLeaveSet()
	{
		return _onCollisionLeaveSet;
	}

	void BoxCollider::ResetCollisions()
	{
		if (GetParent() != nullptr && GetParent()->HasComponent("RigidBody"))
			GetParent()->GetRigidBody()->SetIsGrounded(false);

		_isCollidingRight = false;
		_isCollidingLeft = false;
		_isCollidingBottom = false;
		_isCollidingTop = false;

		rightCollision = 0;
		leftCollision = 0;
		bottomCollision = 0;
		topCollision = 0;
	}

	void BoxCollider::UpdateCenter()
	{
		float step = FlatGui::sceneViewGridStep.x;
		Vector2 centerPoint = FlatGui::sceneViewCenter;
		std::shared_ptr<FlatEngine::Transform> transform = GetParent()->GetTransformComponent();
		Vector2 scale = transform->GetScale();

		activeLeft = centerPoint.x + (center.x - (activeWidth * scale.x / 2) + activeOffset.x) * step;
		activeTop = centerPoint.y + (-center.y - (activeHeight * scale.y / 2) + activeOffset.y) * step;
		activeRight = centerPoint.x + (center.x + (activeWidth * scale.x / 2) + activeOffset.x) * step;
		activeBottom = centerPoint.y + (-center.y + (activeHeight * scale.y / 2) + activeOffset.y) * step;

		center = Vector2(activeLeft + (activeRight - activeLeft) / 2, activeTop + (activeBottom - activeTop) / 2);
	}

	void BoxCollider::SetCorners(Vector2 newCorners[4])
	{
		corners[0] = newCorners[0];
		corners[1] = newCorners[1];
		corners[2] = newCorners[2];
		corners[3] = newCorners[3];
	}

	void BoxCollider::SetNextCorners(Vector2 newCorners[4])
	{
		nextCorners[0] = newCorners[0];
		nextCorners[1] = newCorners[1];
		nextCorners[2] = newCorners[2];
		nextCorners[3] = newCorners[3];
	}

	Vector2* BoxCollider::GetCorners()
	{
		return corners;
	}

	void BoxCollider::SetNormals(Vector2 newNormals[4])
	{
		normals[0] = newNormals[0];
		normals[1] = newNormals[1];
		normals[2] = newNormals[2];
		normals[3] = newNormals[3];
	}

	Vector2 BoxCollider::GetCenter()
	{
		return centerCoord;
	}

	Vector2* BoxCollider::GetNormals()
	{
		return normals;
	}

	void BoxCollider::SetIsContinuous(bool _continuous)
	{
		_isContinuous = _continuous;
	}

	bool BoxCollider::IsContinuous()
	{
		return _isContinuous;
	}

	void BoxCollider::SetIsStatic(bool _newStatic)
	{
		_isStatic = _newStatic;
	}

	bool BoxCollider::IsStatic()
	{
		return _isStatic;
	}

	std::string BoxCollider::GetData()
	{
		json jsonData = {
			{ "type", "BoxCollider" },
			{ "id", GetID() },
			{ "_isCollapsed", IsCollapsed() },
			{ "_isActive", IsActive() },
			{ "activeWidth", activeWidth },
			{ "activeHeight", activeHeight },
			{ "activeOffsetX", activeOffset.x },
			{ "activeOffsetY", activeOffset.y },
			{ "_isContinuous", _isContinuous },
			{ "_isStatic", _isStatic },
			{ "activeLayer", activeLayer },
			{ "_showActiveRadius", _showActiveRadius },
		};

		std::string data = jsonData.dump();
		// Return dumped json object with required data for saving
		return data;
	}

	void BoxCollider::SetRotation(float newRotation)
	{
		rotation = newRotation;
	}

	void BoxCollider::UpdateRotation()
	{
		rotation = GetParent()->GetTransformComponent()->GetRotation();
	}

	void BoxCollider::RecalculateBounds()
	{
		UpdateRotation();
		UpdateActiveEdges();
		UpdateCenter();
		UpdateCorners();
		UpdateNormals();
	}
}
