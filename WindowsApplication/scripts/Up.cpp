#include "Up.h"
#include "../FlatEngine.h"


Up::Up()
{

}

Up::~Up()
{

}

void Up::Start()
{
	for (int i = 0; i < this->GetEntities().size(); i++)
	{
		FlatEngine::LogString("Button Script instantiated on: " + this->GetEntities()[i]->GetName());

		std::shared_ptr<FlatEngine::GameObject> thisObject = this->GetEntities()[i];
		std::shared_ptr<FlatEngine::Button> button = std::static_pointer_cast<FlatEngine::Button>(thisObject->GetComponent(FlatEngine::ComponentTypes::Button));

		//button->OnMouseOver(OnMouseOver);

	}
}

void Up::Update(float deltaTime)
{
	// For all entities attatched to this script:
	for (int i = 0; i < this->GetEntities().size(); i++)
	{
		std::shared_ptr<FlatEngine::GameObject> thisObject = this->GetEntities()[i];
		std::shared_ptr<FlatEngine::Transform> transform = std::static_pointer_cast<FlatEngine::Transform>(thisObject->GetComponent(FlatEngine::Component::ComponentTypes::Transform));
		Vector2 position = transform->GetPosition();
		float xPos = position.x;
		float yPos = position.y;
		//FlatEngine::LogFloat(deltaTime, "deltaTime: ");
		//transform->SetPosition(Vector2(xPos, yPos + .2f * deltaTime));


		std::shared_ptr<FlatEngine::Button> button = std::static_pointer_cast<FlatEngine::Button>(thisObject->GetComponent(FlatEngine::ComponentTypes::Button));
		float buttonWidth = button->GetActiveWidth();
		float buttonHeight = button->GetActiveHeight();
		Vector2 offset = button->GetActiveOffset();

		float top = yPos + offset.y + buttonHeight / 2;
		float bottom = yPos + offset.y - buttonHeight / 2;
		float left = xPos + offset.x - buttonWidth / 2;
		float right = xPos + offset.x + buttonWidth / 2;

		ImVec2 mousePos = ImGui::GetIO().MousePos;

		ImVec2 m_posWorld = FlatEngine::FlatGui::ViewportToWorld(mousePos);

		if (FlatEngine::AreColliding(ImVec4(top, right, bottom, left), ImVec4(m_posWorld.y, m_posWorld.x, m_posWorld.y, m_posWorld.x)))
			FlatEngine::LogString("Mouse OVER!!");
	}
}