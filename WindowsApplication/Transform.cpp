#include "Transform.h"

Transform::Transform(float posX, float posY)
{
	this->position.x = posX;
	this->position.y = posY;
}


Transform::~Transform()
{

}


void Transform::SetPosition(Vector2 position)
{
	this->position = position;
}


Vector2 Transform::GetPosition()
{
	return this->position;
}