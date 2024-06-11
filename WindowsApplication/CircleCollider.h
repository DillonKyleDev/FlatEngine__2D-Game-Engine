#pragma once
#include "Collider.h"
#include "FlatEngine.h"

namespace FlatEngine 
{
	class CircleCollider : public Collider
	{
		friend class Collider;
		friend class BoxCollider;
		class GameObject;

	public:
		CircleCollider(long myID = -1, long parentID = -1);
		CircleCollider(std::shared_ptr<CircleCollider> toCopy, long newParentID);
		CircleCollider(CircleCollider* exactCopy);
		~CircleCollider();

		void AddCollidingObject(std::shared_ptr<FlatEngine::GameObject> collidedWith);
		void UpdateCenter();
		void UpdateActiveEdges();
		void SharedAxisUpdateActiveEdges();
		std::string GetData();
		void RecalculateBounds();
		bool CheckForCollision(std::shared_ptr<FlatEngine::Collider> other);		
		void ResetCollisions();

		// Shared Axis
		bool SharedAxisCheckForCollision(std::shared_ptr<FlatEngine::Collider> other);		

		// Separating Axis
		// Todo

		Vector2 collisionOffset;

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
		Vector2 activeOffset;
		bool _activeEdgesSet;

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
	};
}