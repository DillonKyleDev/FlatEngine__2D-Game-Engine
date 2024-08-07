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
		Texture m_texture;
		int m_renderOrder;
		float m_textureWidth;
		float m_textureHeight;
		Vector2 m_scale;
		PivotPoint m_pivotPoint;
		Vector2 m_pivotOffset;
		Vector2 m_offset;
		std::string m_path;
		Vector4 m_tintColor;
	};
}
