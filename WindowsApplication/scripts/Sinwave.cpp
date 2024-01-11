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
	std::vector<std::shared_ptr<FlatEngine::GameObject>> attatchedEntities = this->GetEntities();
	for (int i = 0; i < attatchedEntities.size(); i++)
	{
		std::shared_ptr<FlatEngine::Transform> transform = std::static_pointer_cast<FlatEngine::Transform>(attatchedEntities[i]->GetComponent(FlatEngine::ComponentTypes::Transform));
		Vector2 position = transform->GetPosition();

		float pi = 3.14f;
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