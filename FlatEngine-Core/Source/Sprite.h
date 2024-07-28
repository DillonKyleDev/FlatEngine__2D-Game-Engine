#pragma once
#include "Component.h"
#include "Texture.h"
#include <SDL.h>
#include "json.hpp"
#include "Vector4.h"
using json = nlohmann::json;
using namespace nlohmann::literals;


namespace FlatEngine
{
	class Sprite : public Component
	{
	public:
		enum PivotPoint {
			Center,
			Left,
			Right,
			Top,
			Bottom,
			TopLeft,
			TopRight,
			BottomRight,
			BottomLeft
		};

		Sprite(long myID = -1, long parentID = -1);
		Sprite(Sprite* toCopy, long newParentID, long myID = -1);
		~Sprite();
		void SetTexture(std::string path);
		SDL_Texture* GetTexture();
		void SetScale(Vector2 newScale);
		Vector2 GetScale();
		float GetTextureWidth();
		float GetTextureHeight();
		std::string GetPath();
		void SetRenderOrder(int order);
		int GetRenderOrder();
		void SetOffset(Vector2 offset);
		Vector2 GetOffset();
		void RemoveTexture();
		void SetPivotPoint(PivotPoint newPivot);
		PivotPoint GetPivotPoint();
		std::string GetPivotPointString();
		void SetPivotOffset(Vector2 newPivotOffset);
		void SetPivotPoint(std::string newPivot);
		Vector2 GetPivotOffset();
		void SetTintColor(Vector4 newTintColor);
		Vector4 GetTintColor();
		std::string GetData();

	private:
		Texture texture;
		int renderOrder;
		float textureWidth;
		float textureHeight;
		Vector2 scale;
		PivotPoint pivotPoint;
		Vector2 pivotOffset;
		Vector2 offset;
		std::string path;
		Vector4 tintColor;
	};
}
