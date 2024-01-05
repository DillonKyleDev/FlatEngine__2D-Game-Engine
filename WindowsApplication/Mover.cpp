#include "Mover.h"
#include "FlatEngine.h"

namespace FlatEngine
{
	Mover::Mover()
	{

	}

	Mover::~Mover()
	{

	}

	void Mover::Awake()
	{

	}

	void Mover::Start()
	{
		for (int i = 0; i < this->GetEntities().size(); i++)
		{
			FlatEngine::LogString("Mover instantiated on: " + this->GetEntities()[i]->GetName());
		}
	}

	void Mover::Update(float deltaTime)
	{
		std::vector<GameObject*> attatchedEntities = this->GetEntities();
		for (int i = 0; i < attatchedEntities.size(); i++)
		{
			FlatEngine::Transform* transform = static_cast<FlatEngine::Transform*>(attatchedEntities[i]->GetComponent(Component::ComponentTypes::Transform));
			Vector2 position = transform->GetPosition();
			float xPos = position.x;
			float yPos = position.y;
			transform->SetPosition(Vector2(xPos+1, yPos+1));
		}
	}
}
