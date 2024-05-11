#pragma once
#include <imgui.h>

class Vector2 : public ImVec2
{
public:
	Vector2(float x=0, float y=0);
	~Vector2();

	bool operator==(const Vector2 &right);
	bool operator!=(const Vector2& right);

private:

};

