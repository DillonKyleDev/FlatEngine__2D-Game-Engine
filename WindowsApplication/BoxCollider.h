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
	//class GameObject;

	class BoxCollider : public Component
	{
	public:
		BoxCollider(long myID = -1, long parentID = -1);
		BoxCollider(std::shared_ptr<BoxCollider> toCopy, long newParentID);
		~BoxCollider();

		void SetOnColliding(std::function<void(std::shared_ptr<GameObject> self, std::shared_ptr<GameObject> collidedWith)> callback);
		void SetOnCollisionEnter(std::function<void(std::shared_ptr<GameObject> self, std::shared_ptr<GameObject> collidedWith)> callback);
		void SetOnCollisionLeave(std::function<void(std::shared_ptr<GameObject> self, std::shared_ptr<GameObject> collidedWith)> callback);

		//bool CheckForCollision(std::shared_ptr<BoxCollider> other);
		bool IsColliding();
		void SetColliding(bool _isColliding);
		void UpdatePreviousPosition();
		bool HasMoved();
		void RemoveCollidingObject(std::shared_ptr<GameObject> object);
		void AddCollidingObject(std::shared_ptr<GameObject> collidedWith);
		std::vector<std::shared_ptr<GameObject>> GetCollidingObjects();
		void SetActiveDimensions(float width, float height);
		void SetActiveOffset(Vector2 offset);
		void SetActiveLayer(int layer);
		int GetActiveLayer();
		float GetActiveWidth();
		float GetActiveHeight();
		Vector2 GetActiveOffset();
		//Vector4(activeTop, activeRight, activeBottom, activeLeft)
		void SetActiveEdges(Vector4 edges);
		Vector4 GetActiveEdges();
		Vector4 UpdateActiveEdges();
		float GetActiveRadiusScreen();
		float GetActiveRadiusGrid();
		void SetShowActiveRadius(bool _show);
		bool GetShowActiveRadius();
		void UpdateNormals();
		void UpdateCorners();
		void UpdateCenter();

		void SetCorners(Vector2 corners[4]);
		void SetNextCorners(Vector2 corners[4]);

		Vector2* GetCorners();
		void SetNormals(Vector2 normals[4]);
		Vector2 GetCenter();
		Vector2* GetNormals();
		void SetIsContinuous(bool _continuous);
		bool IsContinuous();
		void SetIsStatic(bool _newStatic);
		bool IsStatic();
		std::string GetData();
		void SetRotation(float rotation);
		void UpdateRotation();
		void RecalculateBounds();

		bool CheckForCollision(std::shared_ptr<BoxCollider> other);
		
		// Simple Box
		bool SimpleBoxCheckForCollision(std::shared_ptr<BoxCollider> other);
		void SimpleBoxUpdateEdges();
		void SimpleBoxUpdateCorners();

		std::function<void(std::shared_ptr<GameObject> self, std::shared_ptr<GameObject> collidedWith)> OnActiveCollision;
		std::function<void(std::shared_ptr<GameObject> self, std::shared_ptr<GameObject> collidedWith)> OnCollisionEnter;
		std::function<void(std::shared_ptr<GameObject> self, std::shared_ptr<GameObject> collidedWith)> OnCollisionLeave;

		bool OnActiveCollisionSet();
		bool OnCollisionEnterSet();
		bool OnCollisionLeaveSet();

		void ResetCollisions();

		Vector2 previousPosition;

		float rightCollision;
		float leftCollision;
		float bottomCollision;
		float topCollision;

		bool _isCollidingRight;
		bool _isCollidingLeft;
		bool _isCollidingBottom;
		bool _isCollidingTop;

		bool _rightCollisionStatic;
		bool _leftCollisionStatic;
		bool _bottomCollisionStatic;
		bool _topCollisionStatic;

	private:
		bool _onActiveCollidingSet;
		bool _onCollisionEnterSet;
		bool _onCollisionLeaveSet;

		std::vector<std::shared_ptr<GameObject>> collidingObjects;
		bool _isColliding;
		float activeWidth;
		float activeHeight;

		// Current
		float activeLeft;
		float activeRight;
		float activeBottom;
		float activeTop;

		// Next
		float nextActiveLeft;
		float nextActiveRight;
		float nextActiveBottom;
		float nextActiveTop;

		Vector4 activeEdges;
		Vector2 activeOffset;
		bool _isContinuous;
		bool _isStatic;
		int activeLayer;
		bool _activeEdgesSet;
		Vector2 corners[4];
		Vector2 nextCorners[4];
		Vector2 normals[4];
		Vector2 center;
		Vector2 centerGrid;
		Vector2 nextCenterGrid;
		Vector2 centerCoord;
		Vector2 nextCenterCoord;
		float rotation;
		float activeRadiusScreen;
		float activeRadiusGrid;
		bool _showActiveRadius;
	};
}