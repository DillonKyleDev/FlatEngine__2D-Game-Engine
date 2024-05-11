#include "Vector2.h"

Vector2::Vector2(float xValue, float yValue)
{
	x = xValue;
	y = yValue;
}

Vector2::Vector2(const ImVec2& toCopy)
{
	x = toCopy.x;
	y = toCopy.y;
}

Vector2::~Vector2()
{
}

Vector2 Vector2::operator=(ImVec2& right)
{
	x = right.x;
	y = right.y;

	return *this;
}

bool Vector2::operator==(const Vector2 &right)
{
	return (x == right.x && y == right.y);
}

bool Vector2::operator!=(const Vector2& right)
{
	return !(x == right.x && y == right.y);
}
