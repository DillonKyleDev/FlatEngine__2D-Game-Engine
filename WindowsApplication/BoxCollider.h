#pragma once
#include "Component.h"
#include <SDL.h>
#include <string>
#include "Window.h"
#include "Texture.h"
#include "Vector2.h"
#include <functional>
#include "GameObject.h"
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

		bool IsColliding();
		void SetColliding(bool _isColliding);
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
		//ImVec4(activeTop, activeRight, activeBottom, activeLeft)
		void SetActiveEdges(ImVec4 edges);
		ImVec4 GetActiveEdges();
		std::string GetData();
		void SetConnectedScript(std::string scriptName);
		std::string GetConnectedScript();

		std::function<void(std::shared_ptr<GameObject>, std::shared_ptr<GameObject>)> OnActiveCollision;
		std::function<void(std::shared_ptr<GameObject>, std::shared_ptr<GameObject>)> OnCollisionEnter;
		std::function<void(std::shared_ptr<GameObject>, std::shared_ptr<GameObject>)> OnCollisionLeave;

		bool _onActiveCollidingSet;
		bool _onCollisionEnterSet;
		bool _onCollisionLeaveSet;

	private:
		bool _isColliding;
		std::vector<std::shared_ptr<GameObject>> collidingObjects;
		float activeWidth;
		float activeHeight;
		ImVec4 activeEdges;
		Vector2 activeOffset;
		int activeLayer;
		std::string connectedScript;
	};
}