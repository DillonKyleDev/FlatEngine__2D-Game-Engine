#include "Collider.h"
#include "BoxCollider.h"
#include "CircleCollider.h"
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
		activeWidth = 2;
		activeHeight = 2;
		activeOffset = Vector2(0, 0);
		activeEdges = Vector4(0, 0, 0, 0);
		_activeEdgesSet = false;

		activeLeft = 0;
		activeRight = 0;
		activeBottom = 0;
		activeTop = 0;
		nextActiveLeft = 0;
		nextActiveRight = 0;
		nextActiveBottom = 0;
		nextActiveTop = 0;
		nextCorners;

		_isCollidingRight = false;
		_isCollidingLeft = false;
		_isCollidingBottom = false;
		_isCollidingTop = false;

		_rightCollisionStatic = false;
		_leftCollisionStatic = false;
		_bottomCollisionStatic = false;
		_topCollisionStatic = false;

		_rightCollisionSolid = false;
		_leftCollisionSolid = false;
		_bottomCollisionSolid = false;
		_topCollisionSolid = false;

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
		SetActiveLayer(toCopy->GetActiveLayer());
		SetIsContinuous(toCopy->IsContinuous());
		SetIsStatic(toCopy->IsStatic());
		SetIsSolid(toCopy->IsSolid());
		SetColliding(toCopy->IsColliding());
		SetPreviousPosition(toCopy->GetPreviousPosition());
		SetActiveRadiusScreen(toCopy->GetActiveRadiusScreen());
		SetActiveRadiusGrid(toCopy->GetActiveRadiusGrid());	
		SetShowActiveRadius(toCopy->GetShowActiveRadius());
		SetOnActiveCollisionSet(toCopy->OnActiveCollisionSet());
		SetOnCollisionEnterSet(toCopy->OnCollisionEnterSet());
		SetOnCollisionLeaveSet(toCopy->OnCollisionLeaveSet());
		SetOnActiveCollision(toCopy->OnActiveCollision);
		SetOnCollisionEnter(toCopy->OnCollisionEnter);
		SetOnCollisionLeave(toCopy->OnCollisionLeave);

		activeWidth = toCopy->activeWidth;
		activeHeight = toCopy->activeHeight;
		activeOffset = toCopy->activeOffset;
		_activeEdgesSet = toCopy->_activeEdgesSet;
		activeLeft = 0;
		activeRight = 0;
		activeBottom = 0;
		activeTop = 0;
		nextActiveLeft = 0;
		nextActiveRight = 0;
		nextActiveBottom = 0;
		nextActiveTop = 0;

		_isCollidingRight = false;
		_isCollidingLeft = false;
		_isCollidingBottom = false;
		_isCollidingTop = false;

		_rightCollisionStatic = false;
		_leftCollisionStatic = false;
		_bottomCollisionStatic = false;
		_topCollisionStatic = false;

		_rightCollisionSolid = false;
		_leftCollisionSolid = false;
		_bottomCollisionSolid = false;
		_topCollisionSolid = false;

		rightCollision = 0;
		leftCollision = 0;
		bottomCollision = 0;
		topCollision = 0;
	}

	BoxCollider::BoxCollider(BoxCollider* exactCopy)
	{
		SetType(ComponentTypes::BoxCollider);
		SetID(exactCopy->GetID());
		SetParentID(exactCopy->GetParentID());
		SetActive(exactCopy->IsActive());
		SetActiveLayer(exactCopy->GetActiveLayer());
		SetIsContinuous(exactCopy->IsContinuous());
		SetIsStatic(exactCopy->IsStatic());
		SetIsSolid(exactCopy->IsSolid());
		SetColliding(exactCopy->IsColliding());
		SetPreviousPosition(exactCopy->GetPreviousPosition());
		SetActiveRadiusScreen(exactCopy->GetActiveRadiusScreen());
		SetActiveRadiusGrid(exactCopy->GetActiveRadiusGrid());
		SetShowActiveRadius(exactCopy->GetShowActiveRadius());
		SetOnActiveCollisionSet(exactCopy->OnActiveCollisionSet());
		SetOnCollisionEnterSet(exactCopy->OnCollisionEnterSet());
		SetOnCollisionLeaveSet(exactCopy->OnCollisionLeaveSet());
		SetOnActiveCollision(exactCopy->OnActiveCollision);
		SetOnCollisionEnter(exactCopy->OnCollisionEnter);
		SetOnCollisionLeave(exactCopy->OnCollisionLeave);
		SetCenterGrid(exactCopy->GetCenterGrid());
		SetCenterCoord(exactCopy->GetCenterCoord());
		SetNextCenterGrid(exactCopy->GetNextCenterGrid());
		SetNextCenterCoord(exactCopy->GetNextCenterCoord());

		_activeEdgesSet = exactCopy->_activeEdgesSet;
		activeWidth = exactCopy->activeWidth;
		activeHeight = exactCopy->activeHeight;
		activeOffset = exactCopy->activeOffset;

		activeLeft = exactCopy->activeLeft;
		activeRight = exactCopy->activeRight;
		activeBottom = exactCopy->activeBottom;
		activeTop = exactCopy->activeTop;
		nextActiveLeft = exactCopy->nextActiveLeft;
		nextActiveRight = exactCopy->nextActiveRight;
		nextActiveBottom = exactCopy->nextActiveBottom;
		nextActiveTop = exactCopy->nextActiveTop;

		_isCollidingRight = exactCopy->_isCollidingRight;
		_isCollidingLeft = exactCopy->_isCollidingLeft;
		_isCollidingBottom = exactCopy->_isCollidingBottom;
		_isCollidingTop = exactCopy->_isCollidingTop;

		_rightCollisionStatic = exactCopy->_rightCollisionStatic;
		_leftCollisionStatic = exactCopy->_leftCollisionStatic;
		_bottomCollisionStatic = exactCopy->_bottomCollisionStatic;
		_topCollisionStatic = exactCopy->_topCollisionStatic;

		_rightCollisionSolid = exactCopy->_rightCollisionSolid;
		_leftCollisionSolid = exactCopy->_leftCollisionSolid;
		_bottomCollisionSolid = exactCopy->_bottomCollisionSolid;
		_topCollisionSolid = exactCopy->_topCollisionSolid;

		rightCollision = exactCopy->rightCollision;
		leftCollision = exactCopy->leftCollision;
		bottomCollision = exactCopy->bottomCollision;
		topCollision = exactCopy->topCollision;
	}

	BoxCollider::~BoxCollider()
	{
	}

	bool BoxCollider::CheckForCollision(std::shared_ptr<FlatEngine::Collider> other)
	{
		if (loadedProject->GetCollisionDetection() == "Shared Axis")
		{
			return SharedAxisCheckForCollision(other);
		}
		else if (loadedProject->GetCollisionDetection() == "Separating Axis")
		{
			// TODO
			return false;
		}
		return false;
	}

	bool BoxCollider::SharedAxisCheckForCollision(std::shared_ptr<Collider> other)
	{		
		bool _colliding = false;
		Vector2 circleCenterGrid = other->GetCenterGrid();
		Vector2 boxCenterGrid = GetCenterGrid();

		// Calculate activeRadius with pythag
		float rise = std::abs(boxCenterGrid.y - circleCenterGrid.y);
		float run = std::abs(boxCenterGrid.x - circleCenterGrid.x);
		float centerDistance = std::sqrt((rise * rise) + (run * run));

		if (other->GetTypeString() == "BoxCollider")
		{
			std::shared_ptr<FlatEngine::BoxCollider> boxCol = std::static_pointer_cast<FlatEngine::BoxCollider>(other);

			if (centerDistance < boxCol->GetActiveRadiusGrid() + GetActiveRadiusGrid())
			{
				float A_TopEdge = nextActiveTop;
				float A_RightEdge = nextActiveRight;
				float A_BottomEdge = nextActiveBottom;
				float A_LeftEdge = nextActiveLeft;

				float B_TopEdge = boxCol->nextActiveTop;
				float B_RightEdge = boxCol->nextActiveRight;
				float B_BottomEdge = boxCol->nextActiveBottom;
				float B_LeftEdge = boxCol->nextActiveLeft;

				_colliding = ((A_LeftEdge < B_RightEdge) && (A_RightEdge > B_LeftEdge) && (A_BottomEdge < B_TopEdge) && (A_TopEdge > B_BottomEdge));

				if (_colliding)
				{
					AddCollidingObject(boxCol->GetParent());

					// Fire OnActiveCollision while there is a collision happening
					if (OnActiveCollisionSet())
						OnActiveCollision(GetParent(), boxCol->GetParent());

					// Check which direction the collision is happening from //
					// if self is to the right
					if (GetCenterGrid().x > boxCol->GetCenterGrid().x)
					{
						// if self is below other
						if (GetCenterGrid().y < boxCol->GetCenterGrid().y)
						{
							float leftRightOverlap = B_RightEdge - A_LeftEdge;
							float topBottomOverlap = A_TopEdge - B_BottomEdge;
							// Left/Right
							if (leftRightOverlap < topBottomOverlap)
							{
								_isCollidingLeft = true;
								_leftCollisionStatic = boxCol->IsStatic();
								_leftCollisionSolid = boxCol->IsSolid();
								boxCol->_isCollidingRight = true;
								boxCol->_rightCollisionStatic = IsStatic();
								boxCol->_rightCollisionSolid = boxCol->IsSolid();

								leftCollision = B_RightEdge;
								boxCol->rightCollision = A_LeftEdge;
							}
							// Top/Bottom
							else {
								_isCollidingTop = true;
								_topCollisionStatic = boxCol->IsStatic();
								_topCollisionSolid = boxCol->IsSolid();
								boxCol->_isCollidingBottom = true;
								boxCol->_bottomCollisionStatic = IsStatic();
								boxCol->_bottomCollisionSolid = IsSolid();
								topCollision = B_BottomEdge;
								boxCol->bottomCollision = A_TopEdge;
							}
						}
						// if self is above boxCol
						else if (GetCenterGrid().y > boxCol->GetCenterGrid().y)
						{
							float leftRightOverlap = B_RightEdge - A_LeftEdge;
							float topBottomOverlap = B_TopEdge - A_BottomEdge;
							// Left/Right
							if (leftRightOverlap < topBottomOverlap)
							{
								_isCollidingLeft = true;
								_leftCollisionStatic = boxCol->IsStatic();
								_leftCollisionSolid = boxCol->IsSolid();
								boxCol->_isCollidingRight = true;
								boxCol->_rightCollisionStatic = IsStatic();
								boxCol->_rightCollisionSolid = IsSolid();

								leftCollision = B_RightEdge;
								boxCol->rightCollision = A_LeftEdge;
							}
							// Top/Bottom
							else {
								_isCollidingBottom = true;
								_bottomCollisionStatic = boxCol->IsStatic();
								_bottomCollisionSolid = boxCol->IsSolid();
								boxCol->_isCollidingTop = true;
								boxCol->_topCollisionStatic = IsStatic();
								boxCol->_topCollisionSolid = IsSolid();
								bottomCollision = B_TopEdge;
								boxCol->topCollision = A_BottomEdge;
							}
						}
					}
					// if self is to the left
					else if (GetCenterGrid().x < boxCol->GetCenterGrid().x)
					{
						// if self is below boxCol
						if (GetCenterGrid().y < boxCol->GetCenterGrid().y)
						{
							float leftRightOverlap = A_RightEdge - B_LeftEdge;
							float topBottomOverlap = A_TopEdge - B_BottomEdge;
							// Left/Right
							if (leftRightOverlap < topBottomOverlap)
							{
								std::string name = GetParent()->GetName();
								_isCollidingRight = true;
								_rightCollisionStatic = boxCol->IsStatic();
								_rightCollisionSolid = boxCol->IsSolid();
								boxCol->_isCollidingLeft = true;
								boxCol->_leftCollisionStatic = IsStatic();
								boxCol->_leftCollisionSolid = IsSolid();

								rightCollision = B_LeftEdge;
								boxCol->leftCollision = A_RightEdge;
							}
							// Top/Bottom
							else {
								_isCollidingTop = true;
								_topCollisionStatic = boxCol->IsStatic();
								_topCollisionSolid = boxCol->IsSolid();
								boxCol->_isCollidingBottom = true;
								boxCol->_bottomCollisionStatic = IsStatic();
								boxCol->_bottomCollisionSolid = IsSolid();
								topCollision = B_BottomEdge;
								boxCol->bottomCollision = A_TopEdge;
							}
						}
						else if (GetCenterGrid().y > boxCol->GetCenterGrid().y)
						{
							float leftRightOverlap = A_RightEdge - B_LeftEdge;
							float topBottomOverlap = B_TopEdge - A_BottomEdge;
							// Left/Right
							if (leftRightOverlap < topBottomOverlap)
							{
								_isCollidingRight = true;
								_rightCollisionStatic = boxCol->IsStatic();
								_rightCollisionSolid = boxCol->IsSolid();
								boxCol->_isCollidingLeft = true;
								boxCol->_leftCollisionStatic = IsStatic();
								boxCol->_leftCollisionSolid = IsSolid();

								rightCollision = B_LeftEdge;
								boxCol->leftCollision = A_RightEdge;
							}
							// Top/Bottom
							else {
								_isCollidingBottom = true;
								_bottomCollisionStatic = boxCol->IsStatic();
								_bottomCollisionSolid = boxCol->IsSolid();
								boxCol->_isCollidingTop = true;
								boxCol->_topCollisionStatic = IsStatic();
								boxCol->_topCollisionSolid = IsSolid();
								bottomCollision = B_TopEdge;
								boxCol->topCollision = A_BottomEdge;
							}
						}
					}
				}

				if (_colliding)
				{
					SetColliding(_colliding);
					boxCol->SetColliding(_colliding);
				}
			}
		}
		else if (other->GetTypeString() == "CircleCollider")
		{
			std::shared_ptr<FlatEngine::CircleCollider> circleCol = std::static_pointer_cast<FlatEngine::CircleCollider>(other);
			
			_colliding = Collider::CheckForCollisionBoxCircle(std::make_shared<BoxCollider>(this), circleCol);

			// Implement this for events!!!
			//AddCollidingObject(boxCol->GetParent());

			//// Fire OnActiveCollision while there is a collision happening
			//if (OnActiveCollisionSet())
			//	OnActiveCollision(GetParent(), boxCol->GetParent());

			if (_colliding)
			{
				SetColliding(_colliding);
				circleCol->SetColliding(_colliding);
			}
		}

		return _colliding;
	}

	void BoxCollider::AddCollidingObject(std::shared_ptr<FlatEngine::GameObject> collidedWith)
	{
		for (std::shared_ptr<FlatEngine::GameObject> object : GetCollidingObjects())
		{
			// Leave function if the object is already known to be in active collision
			if (object->GetID() == collidedWith->GetID())
				return;
		}
		Collider::AddCollidingObject(collidedWith);

		// If OnCollisionEnter is set, fire it now. (upon initially adding the object to collidingObjects for the first time)
		if (OnCollisionEnterSet())
			OnCollisionEnter(GetParent(), collidedWith);
		if (collidedWith->GetBoxCollider()->OnCollisionEnterSet())
			collidedWith->GetBoxCollider()->OnCollisionEnter(collidedWith, GetParent());
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

	float BoxCollider::GetActiveWidth()
	{
		return activeWidth;
	}

	float BoxCollider::GetActiveHeight()
	{
		return activeHeight;
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
	void BoxCollider::UpdateActiveEdges()
	{
		if (loadedProject->GetCollisionDetection() == "Shared Axis")
		{
			SharedAxisUpdateEdges();
		}
		else if(loadedProject->GetCollisionDetection() == "Separating Axis")
		{

		}		
	}

	void BoxCollider::SharedAxisUpdateEdges()
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
			SetCenterGrid(transform->GetTruePosition());

			activeLeft = centerPoint.x + (GetCenterGrid().x - (activeWidth * scale.x / 2) + activeOffset.x) * step;
			activeTop = centerPoint.y + (-GetCenterGrid().y - (activeHeight * scale.y / 2) + activeOffset.y) * step;
			activeRight = centerPoint.x + (GetCenterGrid().x + (activeWidth * scale.x / 2) + activeOffset.x) * step;
			activeBottom = centerPoint.y + (-GetCenterGrid().y + (activeHeight * scale.y / 2) + activeOffset.y) * step;

			SetCenterCoord(Vector2(activeLeft + (activeRight - activeLeft) / 2, activeTop + (activeBottom - activeTop) / 2));

			// For collision detection ( grid space values )
			if (rigidBody != nullptr)
				SetNextCenterGrid(rigidBody->GetNextPosition());
			else
				SetNextCenterGrid(transform->GetTruePosition());

			nextActiveLeft = GetNextCenterGrid().x - (activeWidth * scale.x / 2) + activeOffset.x;
			nextActiveTop = GetNextCenterGrid().y + (activeHeight * scale.y / 2) + activeOffset.y;
			nextActiveRight = GetNextCenterGrid().x + (activeWidth * scale.x / 2) + activeOffset.x;
			nextActiveBottom = GetNextCenterGrid().y - (activeHeight * scale.y / 2) + activeOffset.y;
		
			SetNextCenterCoord(Vector2(nextActiveLeft + (nextActiveRight - nextActiveLeft) / 2, nextActiveTop + (nextActiveBottom - nextActiveTop) / 2));

			SharedAxisUpdateCorners();
		}
	}

	void BoxCollider::UpdateNormals()
	{
		float cos_a = cosf(GetRotation() * 2.0f * (float)M_PI / 360.0f); // Convert degrees into radians
		float sin_a = sinf(GetRotation() * 2.0f * (float)M_PI / 360.0f);
		float step = FlatGui::sceneViewGridStep.x;
		Vector2 centerPoint = FlatGui::sceneViewCenter;	
		Vector2 scale = GetParent()->GetTransformComponent()->GetScale();
		Vector2 center = GetCenterCoord();

		// Normal vectors without rotation
		Vector2 topNormal = Vector2(0, -activeHeight * step * scale.y);
		Vector2 rightNormal = Vector2(+activeWidth * step * scale.x, 0);
		Vector2 bottomNormal = Vector2(0, +activeHeight * step * scale.y);
		Vector2 leftNormal = Vector2(-activeWidth * step * scale.x, 0);

		if (GetRotation() != 0)
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
		float cos_a = cosf(GetRotation() * 2.0f * (float)M_PI / 360.0f); // Convert degrees into radians
		float sin_a = sinf(GetRotation() * 2.0f * (float)M_PI / 360.0f);
		float step = FlatGui::sceneViewGridStep.x;
		Vector2 centerPoint = FlatGui::sceneViewCenter;
		std::shared_ptr<FlatEngine::Transform> transform = GetParent()->GetTransformComponent();
		Vector2 scale = transform->GetScale();
		Vector2 center = GetCenterCoord();

		// Corners without rotation
		Vector2 topLeft = { activeLeft, activeTop };
		Vector2 bottomRight = { activeRight, activeBottom };
		Vector2 topRight = { activeRight, activeTop };
		Vector2 bottomLeft = { activeLeft, activeBottom };		

		if (GetRotation() != 0)
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
	void BoxCollider::SharedAxisUpdateCorners()
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
		float rise = std::abs(topLeft.y - GetCenterCoord().y);
		float run = std::abs(topLeft.x - GetCenterCoord().x);
		SetActiveRadiusScreen(std::sqrt((rise * rise) + (run * run)));
		SetActiveRadiusGrid(std::sqrt((rise * rise) + (run * run)) / step);

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

	void BoxCollider::ResetCollisions()
	{
		std::string name = GetParent()->GetName();

		ClearCollidingObjects();
		SetColliding(false);

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
		Vector2 position = transform->GetTruePosition();

		activeLeft = centerPoint.x + (position.x - (activeWidth * scale.x / 2) + activeOffset.x) * step;
		activeTop = centerPoint.y + (-position.y - (activeHeight * scale.y / 2) + activeOffset.y) * step;
		activeRight = centerPoint.x + (position.x + (activeWidth * scale.x / 2) + activeOffset.x) * step;
		activeBottom = centerPoint.y + (-position.y + (activeHeight * scale.y / 2) + activeOffset.y) * step;

		SetCenterCoord(Vector2(activeLeft + (activeRight - activeLeft) / 2, activeTop + (activeBottom - activeTop) / 2));
		SetCenterGrid(Vector2(position.x + activeOffset.x, position.y + activeOffset.y));
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

	Vector2* BoxCollider::GetNormals()
	{
		return normals;
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
			{ "_isContinuous", IsContinuous()},
			{ "_isSolid", IsSolid() },
			{ "_isStatic", IsStatic() },
			{ "activeLayer", GetActiveLayer() },
			{ "_showActiveRadius", GetShowActiveRadius()},
		};

		std::string data = jsonData.dump();
		// Return dumped json object with required data for saving
		return data;
	}

	void BoxCollider::RecalculateBounds()
	{
		//UpdateRotation();
		//UpdateActiveEdges();
		//UpdateCenter();
		//UpdateCorners();
		//UpdateNormals();

		UpdateActiveEdges();
	}
}
