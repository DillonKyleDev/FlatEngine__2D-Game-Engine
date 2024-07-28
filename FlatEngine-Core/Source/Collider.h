#pragma once
#include "Component.h"
#include <SDL.h>
#include <string>
#include "Window.h"
#include "Texture.h"
#include "Vector2.h"
#include <functional>
#include "GameObject.h"
#include "Vector4.h"
#include <imgui.h>


namespace FlatEngine
{
	class Collider : public Component
	{
		friend class RigidBody;
	public:
		Collider(long myID = -1, long parentID = -1);
		Collider(Collider* toCopy, long newParentID, long myID = -1);
		~Collider();

		static bool SharedAxisCheckForCollision(Collider* collider1, Collider* collider2);
		static bool CheckForCollisionBoxCircle(FlatEngine::BoxCollider* boxCol, FlatEngine::CircleCollider* circleCol);
		static bool CheckForCollisionBoxBox(FlatEngine::BoxCollider* boxCol1, FlatEngine::BoxCollider* boxCol2);

		virtual void RecalculateBounds();
		static bool CheckForCollision(std::string collisionDetectionType, Collider* collider1, Collider* collider2);
		virtual void ResetCollisions();
		virtual void UpdateCenter(); // May be useless??

		void AddCollidingObject(Collider* collidedWith);
		void RemoveCollidingObject(GameObject object);
		void SetOnActiveCollision(std::function<void(GameObject* self, GameObject* collidedWith)> callback);
		void SetOnCollisionEnter(std::function<void(GameObject* self, GameObject* collidedWith)> callback);
		void SetOnCollisionLeave(std::function<void(GameObject* self, GameObject* collidedWith)> callback);
		std::function<void(GameObject* self, GameObject* collidedWith)> OnActiveCollision;
		std::function<void(GameObject* self, GameObject* collidedWith)> OnCollisionEnter;
		std::function<void(GameObject* self, GameObject* collidedWith)> OnCollisionLeave;
		bool OnActiveCollisionSet();
		bool OnCollisionEnterSet();
		bool OnCollisionLeaveSet();
		void SetOnActiveCollisionSet(bool _set);
		void SetOnCollisionEnterSet(bool _set);
		void SetOnCollisionLeaveSet(bool _set);

		bool IsColliding();
		void SetColliding(bool _isColliding);
		void UpdatePreviousPosition();
		Vector2 GetPreviousPosition();
		void SetPreviousPosition(Vector2 prevPos);
		bool HasMoved();
		std::vector<GameObject> &GetCollidingObjects();
		void ClearCollidingObjects();
		void SetActiveOffset(Vector2 offset);
		Vector2 GetActiveOffset();
		void SetActiveLayer(int layer);
		int GetActiveLayer();
		void SetActiveRadiusScreen(float radius);
		float GetActiveRadiusScreen();
		void SetActiveRadiusGrid(float radius);
		float GetActiveRadiusGrid();
		void SetShowActiveRadius(bool _show);
		bool GetShowActiveRadius();
		
		void SetCenterGrid(Vector2 newCenter);
		Vector2 GetCenterGrid();
		void SetCenterCoord(Vector2 newCenter);
		Vector2 GetCenterCoord();
		void SetNextCenterGrid(Vector2 nextCenter);
		Vector2 GetNextCenterGrid();
		void SetNextCenterCoord(Vector2 nextCenter);
		Vector2 GetNextCenterCoord();
		void SetIsContinuous(bool _continuous);
		bool IsContinuous();
		void SetIsStatic(bool _newStatic);
		bool IsStatic();
		void SetIsSolid(bool _solid);
		bool IsSolid();
		void SetRotation(float rotation);
		void UpdateRotation();
		float GetRotation();
		void SetIsComposite(bool _newComposite);
		bool IsComposite();

		Vector2 collidedPosition;
		Vector2 leftCollidedPosition;
		Vector2 rightCollidedPosition;
		Vector2 bottomCollidedPosition;
		Vector2 topCollidedPosition;
		Vector2 topRightCollidedPosition;
		Vector2 bottomRightCollidedPosition;
		Vector2 topLeftCollidedPosition;
		Vector2 bottomLeftCollidedPosition;

		float rightCollision;
		float leftCollision;
		float bottomCollision;
		float topCollision;

		bool _isCollidingRight;
		bool _isCollidingLeft;
		bool _isCollidingBottom;
		bool _isCollidingTop;

		bool _isCollidingTopRight;
		bool _isCollidingTopLeft;
		bool _isCollidingBottomRight;
		bool _isCollidingBottomLeft;

		bool _rightCollisionStatic;
		bool _leftCollisionStatic;
		bool _bottomCollisionStatic;
		bool _topCollisionStatic;
		bool _bottomLeftCollisionStatic;
		bool _bottomRightCollisionStatic;
		//bool _topLeftCollisionStatic;
		//bool _topRightCollisionStatic;

		bool _rightCollisionSolid;
		bool _leftCollisionSolid;
		bool _bottomCollisionSolid;
		bool _topCollisionSolid;
		bool _bottomLeftCollisionSolid;
		bool _bottomRightCollisionSolid;
		bool _topLeftCollisionSolid;
		bool _topRightCollisionSolid;

	private:
		std::vector<GameObject> collidingObjects;
		std::vector<GameObject> collidingLastFrame;
		Vector2 activeOffset;
		Vector2 previousPosition;
		Vector2 centerGrid;
		Vector2 nextCenterGrid;
		Vector2 centerCoord;
		Vector2 nextCenterCoord;
		float rotation;
		float activeRadiusScreen;
		float activeRadiusGrid;
		int activeLayer;
		bool _isColliding;
		bool _isContinuous;
		bool _isStatic;
		bool _isSolid;
		bool _showActiveRadius;
		bool _onActiveCollidingSet;
		bool _onCollisionEnterSet;
		bool _onCollisionLeaveSet;
		bool _isComposite;
	};
}