#pragma once
#include <imgui.h>

class Vector4 : public ImVec4
{
public:
	Vector4(float x = 0, float y = 0, float z = 0, float w = 0);
	Vector4(const ImVec4& toCopy);
	~Vector4();

	Vector4 operator=(ImVec4& toCopy);
	Vector4 operator*(ImVec4& right);
	Vector4 operator*(Vector4& right);
	bool operator==(const Vector4& right);
	bool operator!=(const Vector4& right);

private:

};

