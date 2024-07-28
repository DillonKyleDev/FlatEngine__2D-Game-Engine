#include "Vector4.h"

Vector4::Vector4(float xValue, float yValue, float zValue, float wValue)
{
	x = xValue;
	y = yValue;
	z = zValue;
	w = wValue;
}

Vector4::Vector4(const ImVec4& toCopy)
{
	x = toCopy.x;
	y = toCopy.y;
	z = toCopy.z;
	w = toCopy.w;
}

Vector4::~Vector4()
{
}

Vector4 Vector4::operator=(ImVec4& toCopy)
{
	x *= toCopy.x;
	y *= toCopy.y;
	z *= toCopy.z;
	w *= toCopy.w;

	return *this;
}

Vector4 Vector4::operator*(ImVec4& right)
{
	x *= right.x;
	y *= right.y;
	z *= right.z;
	w *= right.w;

	return *this;
}

Vector4 Vector4::operator*(Vector4& right)
{
	x *= right.x;
	y *= right.y;
	z *= right.z;
	w *= right.w;

	return *this;
}

bool Vector4::operator==(const Vector4& right)
{
	return (x == right.x && y == right.y && z == right.z && w == right.w);
}

bool Vector4::operator!=(const Vector4& right)
{
	return !(x == right.x && y == right.y && z == right.z && w == right.w);
}
