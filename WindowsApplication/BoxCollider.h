#pragma once
#include "Component.h"
#include "CircleCollider.h"
#include "FlatEngine.h"
#include <SDL.h>
#include <string>
#include "Window.h"
#include "Collider.h"
#include "Texture.h"
#include "Vector2.h"
#include <functional>
#include "Vector4.h"
#include <imgui.h>


namespace FlatEngine
{
	class BoxCollider : public Collider
	{
		friend class Collider;
		friend class CircleCollider;
		class GameObject;		

	public:
		BoxCollider(long myID = -1, long parentID = -1);
		BoxCollider(std::shared_ptr<BoxCollider> toCopy, long newParentID);
		BoxCollider(BoxCollider* exactCopy);
		~BoxCollider();
		
		void AddCollidingObject(std::shared_ptr<FlatEngine::GameObject> collidedWith);		
		void SetActiveDimensions(float width, float height);
		float GetActiveWidth();
		float GetActiveHeight();
		//Vector4(activeTop, activeRight, activeBottom, activeLeft)
		void SetActiveEdges(Vector4 edges);
		Vector4 GetActiveEdges();
		void UpdateActiveEdges();
		void UpdateNormals();
		void UpdateCorners();
		void UpdateCenter();
		void SetCorners(Vector2 corners[4]);
		void SetNextCorners(Vector2 corners[4]);
		Vector2* GetCorners();
		void SetNormals(Vector2 normals[4]);
		Vector2* GetNormals();
		std::string GetData();
		void RecalculateBounds();
		bool CheckForCollision(std::shared_ptr<FlatEngine::Collider> other);		
		void ResetCollisions();

		// Shared Axis
		bool SharedAxisCheckForCollision(std::shared_ptr<FlatEngine::Collider> other);
		void SharedAxisUpdateEdges();
		void SharedAxisUpdateCorners();

		// Separating Axis
		// Todo

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

		bool _rightCollisionSolid;
		bool _leftCollisionSolid;
		bool _bottomCollisionSolid;
		bool _topCollisionSolid;

	private:		
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
		bool _activeEdgesSet;
		Vector2 corners[4];
		Vector2 nextCorners[4];
		Vector2 normals[4];
	};
}