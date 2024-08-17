#pragma once
#include <imgui.h>

class Vector4 : public ImVec4
{
public:
	Vector4(float x = 0, float y = 0, float z = 0, float w = 0);
	Vector4(const ImVec4& toCopy);
	~Vector4();

	void _xyzw(float newX, float newY, float newZ, float newW);
	float GetX();
	float GetY();
	float GetZ();
	float GetW();
	void SetX(float newX);
	void SetY(float newY);
	void SetZ(float newZ);
	void SetW(float newW);
	Vector4 operator=(ImVec4& toCopy);
	Vector4 operator*(ImVec4& right);
	Vector4 operator*(Vector4& right);
	bool operator==(const Vector4& right);
	bool operator!=(const Vector4& right);

private:

};

