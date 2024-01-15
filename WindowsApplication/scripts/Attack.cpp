#include "Attack.h"
#include "../FlatEngine.h"
#include "../Animation.h"


Attack::Attack()
{

}

Attack::~Attack()
{
}

void AttackOnMouseOver(std::shared_ptr<FlatEngine::GameObject> thisObject)
{
}

void AttackOnMouseLeave(std::shared_ptr<FlatEngine::GameObject> thisObject)
{
}

void AttackOnLeftClick(std::shared_ptr<FlatEngine::GameObject> thisObject)
{
	std::shared_ptr<FlatEngine::Transform> transform = std::static_pointer_cast<FlatEngine::Transform>(thisObject->GetComponent(FlatEngine::ComponentTypes::Transform));
	Vector2 position = transform->GetPosition();

	// Get Animation Component
	std::shared_ptr<FlatEngine::Animation> animation = std::static_pointer_cast<FlatEngine::Animation>(thisObject->GetComponent(FlatEngine::ComponentTypes::Animation));

	if (animation != nullptr)
		animation->Play();
}

void AttackOnRightClick(std::shared_ptr<FlatEngine::GameObject> thisObject)
{
	std::shared_ptr<FlatEngine::Transform> transform = std::static_pointer_cast<FlatEngine::Transform>(thisObject->GetComponent(FlatEngine::ComponentTypes::Transform));
}


void Attack::Start()
{
	for (int i = 0; i < this->GetEntities().size(); i++)
	{
		std::shared_ptr<FlatEngine::GameObject> thisObject = this->GetEntities()[i];
		std::shared_ptr<FlatEngine::Button> button = std::static_pointer_cast<FlatEngine::Button>(thisObject->GetComponent(FlatEngine::ComponentTypes::Button));

		if (button != nullptr)
		{
			// Register Mouse Events functions to the Button
			button->OnMouseOver(AttackOnMouseOver);
			button->OnMouseLeave(AttackOnMouseLeave);
			button->OnMouseLeftClick(AttackOnLeftClick);
			button->OnMouseRightClick(AttackOnRightClick);
		}
	}
}


void Attack::Update(float deltaTime)
{
	// For all entities attatched to this script:
	for (int i = 0; i < this->GetEntities().size(); i++)
	{
		std::shared_ptr<FlatEngine::GameObject> thisObject = this->GetEntities()[i];
		std::shared_ptr<FlatEngine::Transform> transform = std::static_pointer_cast<FlatEngine::Transform>(thisObject->GetComponent(FlatEngine::Component::ComponentTypes::Transform));
		Vector2 position = transform->GetPosition();
		float xPos = position.x;
		float yPos = position.y;
	}
}

