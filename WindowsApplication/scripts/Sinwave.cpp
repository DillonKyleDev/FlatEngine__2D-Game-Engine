#include "Sinwave.h"
#include "../FlatEngine.h"
#include <math.h>


Sinwave::Sinwave()
{

}

Sinwave::~Sinwave()
{

}

void Sinwave::Start()
{
	for (int i = 0; i < this->GetEntities().size(); i++)
	{
		FlatEngine::LogString("Sinwave instantiated on: " + this->GetEntities()[i]->GetName());
	}
}

void Sinwave::Update(float deltaTime)
{
	std::vector<FlatEngine::GameObject*> attatchedEntities = this->GetEntities();
	for (int i = 0; i < attatchedEntities.size(); i++)
	{
		FlatEngine::Transform* transform = static_cast<FlatEngine::Transform*>(attatchedEntities[i]->GetComponent(FlatEngine::Component::ComponentTypes::Transform));
		Vector2 position = transform->GetPosition();

		float pi = 3.14;
		float xPos = 0;
		float yPos = 0;

		if (this->amplitudeY < 200) {
			yPos = (float)0 - sin(this->amplitudeY * pi / 100) * 25;
			transform->SetPosition(Vector2(position.x, yPos));
			this->amplitudeY++;
		}
		else
		{
			this->amplitudeY = 0;
		}

		if (this->amplitudeX < 200) {
			xPos = (float)0 - sin(this->amplitudeX * pi / 50) * 25;
			transform->SetPosition(Vector2(xPos, transform->GetPosition().y));
			this->amplitudeX++;
		}
		else
		{
			this->amplitudeX = 0;
		}
	}
}