#include "BoardSquare.h"
#include "../GameScript.h"
#include "../Button.h"
#include "../FlatEngine.h"

BoardSquare::BoardSquare()
{
	this->SetName("BoardSquare");
}

BoardSquare::~BoardSquare()
{
}

void BoardSquareOnMouseOver(std::shared_ptr<FlatEngine::GameObject> thisObject)
{
	FlatEngine::LogString("Mouse Over... " + thisObject->GetName());
	std::shared_ptr<FlatEngine::Transform> transform = std::static_pointer_cast<FlatEngine::Transform>(thisObject->GetComponent(FlatEngine::ComponentTypes::Transform));
}

void BoardSquareOnMouseEnter(std::shared_ptr<FlatEngine::GameObject> thisObject)
{
	FlatEngine::LogString("Mouse Enter... " + thisObject->GetName());
	std::shared_ptr<FlatEngine::Transform> transform = std::static_pointer_cast<FlatEngine::Transform>(thisObject->GetComponent(FlatEngine::ComponentTypes::Transform));
}

void BoardSquareOnMouseLeave(std::shared_ptr<FlatEngine::GameObject> thisObject)
{
	FlatEngine::LogString("Mouse Leave... " + thisObject->GetName());
	std::shared_ptr<FlatEngine::Transform> transform = std::static_pointer_cast<FlatEngine::Transform>(thisObject->GetComponent(FlatEngine::ComponentTypes::Transform));
}

void BoardSquareOnLeftClick(std::shared_ptr<FlatEngine::GameObject> thisObject)
{
	// Get board square from the object and get the piece inside the square

	std::shared_ptr<FlatEngine::ScriptComponent> scriptComponent = std::static_pointer_cast<FlatEngine::ScriptComponent>(thisObject->GetComponent(FlatEngine::ComponentTypes::Script));
	std::shared_ptr<BoardSquare> boardSquare = std::static_pointer_cast<BoardSquare>(scriptComponent->GetScriptInstance());
	if (boardSquare->GetPiece() != nullptr)
		FlatEngine::gameManager->SetSelectedPiece(boardSquare->GetPiece(), thisObject);
}

void BoardSquareOnRightClick(std::shared_ptr<FlatEngine::GameObject> thisObject)
{
	FlatEngine::LogString("Right Click... " + thisObject->GetName());
	std::shared_ptr<FlatEngine::Transform> transform = std::static_pointer_cast<FlatEngine::Transform>(thisObject->GetComponent(FlatEngine::ComponentTypes::Transform));
}


void BoardSquare::Start()
{
	std::shared_ptr<FlatEngine::Button> button = std::static_pointer_cast<FlatEngine::Button>(this->GetOwner()->GetComponent(FlatEngine::ComponentTypes::Button));

	// Register Mouse Events functions to the Button
	button->SetOnMouseOver(BoardSquareOnMouseOver);
	button->SetOnMouseEnter(BoardSquareOnMouseEnter);
	button->SetOnMouseLeave(BoardSquareOnMouseLeave);
	button->SetOnMouseLeftClick(BoardSquareOnLeftClick);
	button->SetOnMouseRightClick(BoardSquareOnRightClick);
}

void BoardSquare::Update(float deltaTime)
{
}

void BoardSquare::SetBoardLocation(std::shared_ptr<FlatEngine::GameObject> boardLocation)
{
	this->boardLocation = boardLocation;
}

std::shared_ptr<FlatEngine::GameObject> BoardSquare::GetBoardLocation()
{
	return this->boardLocation;
}

void BoardSquare::SetPiece(std::shared_ptr<Piece> piece)
{
	this->piece = piece;
}

std::shared_ptr<Piece> BoardSquare::GetPiece()
{
	return this->piece;
}