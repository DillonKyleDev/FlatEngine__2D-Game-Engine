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

		void SetOnColliding(std::function<void(std::shared_ptr<GameObject>, std::shared_ptr<GameObject>)> callback);
		void SetOnCollisionEnter(std::function<void(std::shared_ptr<GameObject>, std::shared_ptr<GameObject>)> callback);
		void SetOnCollisionLeave(std::function<void(std::shared_ptr<GameObject>, std::shared_ptr<GameObject>)> callback);

		bool CheckForCollision(std::shared_ptr<BoxCollider> other);
		bool IsColliding();
		void SetColliding(bool _isColliding);
		void UpdatePreviousPosition();
		bool HasMoved();
		void RemoveCollidingObject(std::shared_ptr<GameObject> object);
		void AddCollidingObject(std::shared_ptr<GameObject> object);
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
		void UpdateNormals();
		void UpdateCorners();
		void SimpleBoxUpdateCorners();
		void UpdateCenter();
		void SetCorners(Vector2 corners[4]);
		Vector2* GetCorners();
		void SetNormals(Vector2 normals[4]);
		Vector2 GetCenter();
		Vector2* GetNormals();
		void SetIsContinuous(bool _continuous);
		bool IsContinuous();
		std::string GetData();
		void SetRotation(float rotation);
		void UpdateRotation();
		void RecalculateBounds();

		// Simple Box
		void SimpleBoxUpdateEdges();

		std::function<void(std::shared_ptr<GameObject>, std::shared_ptr<GameObject>)> OnActiveCollision;
		std::function<void(std::shared_ptr<GameObject>, std::shared_ptr<GameObject>)> OnCollisionEnter;
		std::function<void(std::shared_ptr<GameObject>, std::shared_ptr<GameObject>)> OnCollisionLeave;

		bool _onActiveCollidingSet;
		bool _onCollisionEnterSet;
		bool _onCollisionLeaveSet;

		Vector2 previousPosition;
	private:
		std::vector<std::shared_ptr<GameObject>> collidingObjects;
		bool _isColliding;
		float activeWidth;
		float activeHeight;
		float activeLeft;
		float activeRight;
		float activeBottom;
		float activeTop;
		Vector4 activeEdges;
		Vector2 activeOffset;
		bool _isContinious;
		int activeLayer;
		bool _activeEdgesSet;
		Vector2 corners[4];
		Vector2 normals[4];
		Vector2 center;
		float rotation;
	};
}