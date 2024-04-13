#include "Vector2.h"

Vector2::Vector2(float x, float y)
{
	this->x = x;
	this->y = y;
}


Vector2::~Vector2()
{

}

bool Vector2::operator==(const Vector2 &right)
{
	return (x == right.x && y == right.y);
}

bool Vector2::operator!=(const Vector2& right)
{
	return !(x == right.x && y == right.y);
}
