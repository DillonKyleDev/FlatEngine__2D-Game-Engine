#include "Transform.h"

Transform::Transform(float posX, float posY)
{
	this->position.x = posX;
	this->position.y = posY;
	this->SetType(Component::ComponentTypes::Transform);
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