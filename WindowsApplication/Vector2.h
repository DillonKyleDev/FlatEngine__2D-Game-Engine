#pragma once
#include <imgui.h>

class Vector2 : public ImVec2
{
public:
	Vector2(float x = 0, float y = 0);
	Vector2(const ImVec2& toCopy);
	~Vector2();

	Vector2 operator=(ImVec2& toCopy);
	Vector2 operator*(ImVec2& right);
	Vector2 operator*(Vector2& right);
	bool operator==(const Vector2& right);
	bool operator!=(const Vector2& right);

private:

};

