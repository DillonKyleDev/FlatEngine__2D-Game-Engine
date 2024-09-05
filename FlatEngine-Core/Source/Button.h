#pragma once
#include "Component.h"
#include "Window.h"
#include "Texture.h"
#include "Vector2.h"
#include <functional>
#include <imgui.h>
#include <SDL.h>
#include <string>

namespace FlatEngine
{
	class GameObject;

	class Button : public Component
	{
	public:
		Button(long myID = -1, long parentID = -1);
		~Button();
		
		void SetOnMouseOver(std::function<void(GameObject*)> callback);
		void SetOnMouseEnter(std::function<void(GameObject*)> callback);
		void SetOnMouseLeave(std::function<void(GameObject*)> callback);
		void SetOnMouseLeftClick(std::function<void(GameObject*)> callback);
		void SetOnMouseRightClick(std::function<void(GameObject*)> callback);

		void SetActiveDimensions(float width, float height);
		void SetActiveOffset(Vector2 offset);
		void SetActiveLayer(int layer);
		int GetActiveLayer();
		float GetActiveWidth();
		float GetActiveHeight();
		Vector2 GetActiveOffset();
		void SetMouseIsOver(bool _isOver);
		void SetIsOverFired(bool _fired);
		bool MouseIsOver();
		//ImVec4(activeTop, activeRight, activeBottom, activeLeft)
		void SetActiveEdges(ImVec4 edges);
		ImVec4 GetActiveEdges();
		std::string GetData();
		void SetConnectedScript(std::string scriptName);
		std::string GetConnectedScript();

		std::function<void(GameObject*)> OnMouseOverFunction;
		std::function<void(GameObject*)> OnMouseEnterFunction;
		std::function<void(GameObject*)> OnMouseLeaveFunction;
		std::function<void(GameObject*)> OnLeftClickFunction;
		std::function<void(GameObject*)> OnRightClickFunction;

		bool MouseOverSet();
		bool MouseEnterSet();
		bool MouseLeaveSet();
		bool LeftClickSet();
		bool RightClickSet();

	private:
		bool _mouseOverSet;
		bool _mouseEnterSet;
		bool _mouseLeaveSet;
		bool _leftClickSet;
		bool _rightClickSet;

		bool _mouseIsOver;
		bool _hasMouseOverFired;
		float activeWidth;
		float activeHeight;
		ImVec4 activeEdges;
		Vector2 activeOffset;
		int activeLayer;
		std::string connectedScript;
	};
}