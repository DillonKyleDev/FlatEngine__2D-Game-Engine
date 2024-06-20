#include "Collider.h"
#include "CircleCollider.h"
#include "BoxCollider.h"
#include "RigidBody.h"
#include "FlatEngine.h"
#include "GameObject.h"
#include "Transform.h"
#include "imgui_internal.h"
#include <cmath>


namespace FlatEngine {

	CircleCollider::CircleCollider(long myID, long parentID) : Collider(myID, parentID)
	{
		SetType(ComponentTypes::CircleCollider);
		SetActiveRadiusGrid(1.5);

		_activeEdgesSet = false;

		activeLeft = 0;
		activeRight = 0;
		activeBottom = 0;
		activeTop = 0;
		nextActiveLeft = 0;
		nextActiveRight = 0;
		nextActiveBottom = 0;
		nextActiveTop = 0;
	}

	CircleCollider::CircleCollider(std::shared_ptr<CircleCollider> toCopy, long newParentID) : Collider(toCopy, newParentID)
	{		
		SetType(ComponentTypes::CircleCollider);
		SetActiveRadiusScreen(toCopy->GetActiveRadiusScreen());
		SetActiveRadiusGrid(toCopy->GetActiveRadiusGrid());
		SetShowActiveRadius(toCopy->GetShowActiveRadius());

		_activeEdgesSet = false;

		activeLeft = 0;
		activeRight = 0;
		activeBottom = 0;
		activeTop = 0;
		nextActiveLeft = 0;
		nextActiveRight = 0;
		nextActiveBottom = 0;
		nextActiveTop = 0;
	}

	CircleCollider::~CircleCollider()
	{
	}

	//void CircleCollider::ResetCollisions()
	//{

	//}

	void CircleCollider::UpdateCenter()
	{
		float step = FlatGui::sceneViewGridStep.x;
		Vector2 centerPoint = FlatGui::sceneViewCenter;
		std::shared_ptr<FlatEngine::Transform> transform = GetParent()->GetTransformComponent();
		Vector2 position = transform->GetTruePosition();
		float activeRadius = GetActiveRadiusGrid();
		Vector2 activeOffset = GetActiveOffset();

		float xCenter = centerPoint.x + (position.x + activeOffset.x) * step;
		float yCenter = centerPoint.y - (position.y + activeOffset.y) * step;
		
		SetCenterGrid(Vector2(position.x + activeOffset.x, position.y + activeOffset.y));
		SetCenterCoord(Vector2(xCenter, yCenter));
	}

	// Just based on actual pixel locations (0,0) being the top left of the window
	// You can use it for either game view or scene view, you just need the correct center location of whichever you choose
	void CircleCollider::UpdateActiveEdges()
	{
		if (loadedProject->GetCollisionDetection() == "Shared Axis")
		{
			SharedAxisUpdateActiveEdges();
		}
		else if (loadedProject->GetCollisionDetection() == "Separating Axis")
		{
			// TODO
		}
	}

	void CircleCollider::SharedAxisUpdateActiveEdges()
	{
		// Only if the activeEdges has not been set or if the velocity is not 0 do we update the active edges
		bool _shouldUpdate = false;

		std::shared_ptr<FlatEngine::GameObject> parent = GetParent();
		std::shared_ptr<FlatEngine::Transform> transform = nullptr;
		Vector2 scale = Vector2(1, 1);
		float activeRadius = GetActiveRadiusGrid();

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
			Vector2 activeOffset = GetActiveOffset();

			// For visual representation ( screen space values )
			SetCenterGrid(Vector2(transform->GetTruePosition().x + activeOffset.x, transform->GetTruePosition().y + activeOffset.y));

			activeLeft = centerPoint.x + (GetCenterGrid().x - (activeRadius * scale.x)) * step;
			activeTop = centerPoint.y + (-GetCenterGrid().y - (activeRadius * scale.y)) * step;
			activeRight = centerPoint.x + (GetCenterGrid().x + (activeRadius * scale.x)) * step;
			activeBottom = centerPoint.y + (-GetCenterGrid().y + (activeRadius * scale.y)) * step;

			SetCenterCoord(Vector2(activeLeft + (activeRight - activeLeft) / 2, activeTop + (activeBottom - activeTop) / 2));

			// For collision detection ( grid space values )
			if (rigidBody != nullptr)
				SetNextCenterGrid(Vector2(rigidBody->GetNextPosition().x + activeOffset.x, rigidBody->GetNextPosition().y + activeOffset.y));
			else
				SetNextCenterGrid(GetCenterGrid());

			nextActiveLeft = GetNextCenterGrid().x - (activeRadius * scale.x);
			nextActiveTop = GetNextCenterGrid().y + (activeRadius * scale.y);
			nextActiveRight = GetNextCenterGrid().x + (activeRadius * scale.x);
			nextActiveBottom = GetNextCenterGrid().y - (activeRadius * scale.y);

			SetNextCenterCoord(Vector2(nextActiveLeft + (nextActiveRight - nextActiveLeft) / 2, nextActiveTop + (nextActiveBottom - nextActiveTop) / 2));
			_activeEdgesSet = true;
		}
	}

	std::string CircleCollider::GetData()
	{
		json jsonData = {
			{ "type", "CircleCollider" },
			{ "id", GetID() },
			{ "_isCollapsed", IsCollapsed() },
			{ "_isActive", IsActive() },
			{ "_isContinuous", IsContinuous()},
			{ "_isSolid", IsSolid() },
			{ "_isStatic", IsStatic() },
			{ "activeLayer", GetActiveLayer() },
			{ "activeRadius", GetActiveRadiusGrid() },
			{ "_showActiveRadius", GetShowActiveRadius()},
			{ "activeOffsetX", GetActiveOffset().x },
			{ "activeOffsetY", GetActiveOffset().y },
			{ "_isComposite", IsComposite() },
		};

		std::string data = jsonData.dump();
		// Return dumped json object with required data for saving
		return data;
	}

	void CircleCollider::RecalculateBounds()
	{
		UpdateCenter();
		UpdateActiveEdges();
	}
}
