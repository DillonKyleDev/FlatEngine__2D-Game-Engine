#pragma once
#include "Component.h"
#include "Vector2.h"

class Transform : public Component
{
public:
	Transform(float posX = 0, float posY = 0);
	~Transform();
	void SetPosition(Vector2 position);
	Vector2 GetPosition();

private:
	Vector2 position;
	//Rotation
};