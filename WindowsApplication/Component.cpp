#include "Component.h"


Component::Component()
{

}

Component::~Component()
{

}


void Component::SetType(ComponentTypes type)
{
	this->type = type;
}

Component::ComponentTypes Component::GetType()
{
	return this->type;
}