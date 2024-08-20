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
		using RigidBody = FlatEngine::RigidBody;
		using GameObject = FlatEngine::GameObject;

		friend class Collider;
		friend class CircleCollider;
		class GameObject;		

	public:
		BoxCollider(long myID = -1, long parentID = -1);
		BoxCollider(BoxCollider* toCopy, long newParentID, long myID = -1);
		~BoxCollider();
					
		void SetActiveDimensions(float width, float height);
		float GetActiveWidth();
		float GetActiveHeight();
		//Vector4(activeTop, activeRight, activeBottom, activeLeft)
		void SetActiveEdges(Vector4 edges);
		Vector4 GetActiveEdges();
		void UpdateActiveEdges(float gridstep, Vector2 viewportCenter);
		void UpdateNormals(float gridstep, Vector2 viewportCenter);
		void UpdateCorners(float gridstep, Vector2 viewportCenter);
		void UpdateCenter(float gridstep, Vector2 viewportCenter);
		void SetCorners(Vector2 corners[4]);
		void SetNextCorners(Vector2 corners[4]);
		Vector2* GetCorners();
		void SetNormals(Vector2 normals[4]);
		Vector2* GetNormals();
		std::string GetData();
		void RecalculateBounds(float gridstep, Vector2 viewportCenter);

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
		bool _activeEdgesSet;
		Vector2 corners[4];
		Vector2 nextCorners[4];
		Vector2 normals[4];
	};
}