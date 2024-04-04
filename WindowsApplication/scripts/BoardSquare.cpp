#include "BoardSquare.h"
#include "../GameScript.h"
#include "../Button.h"
#include "../FlatEngine.h"
#include "../Sprite.h"

BoardSquare::BoardSquare()
{
	this->SetName("BoardSquare");
	pieceName = "";
	pieceType = "";
	pieceColor = "";
	spritePath = "";
	spriteYOffset = 0;
	column = -1;
	row = -1;
	_hasMoved = false;
}

BoardSquare::BoardSquare(std::shared_ptr<BoardSquare> toCopy)
{
	this->SetName("BoardSquare");
	pieceName = toCopy->pieceName;
	pieceType = toCopy->pieceType;
	pieceColor = toCopy->pieceColor;
	spritePath = toCopy->spritePath;
	spriteYOffset = toCopy->spriteYOffset;
	column = toCopy->column;
	row = toCopy->row;
	_hasMoved = toCopy->_hasMoved;
}

BoardSquare::~BoardSquare()
{
}

void BoardSquareOnMouseOver(std::shared_ptr<FlatEngine::GameObject> thisObject)
{
}

void BoardSquareOnMouseEnter(std::shared_ptr<FlatEngine::GameObject> thisObject)
{
}

void BoardSquareOnMouseLeave(std::shared_ptr<FlatEngine::GameObject> thisObject)
{
}

void BoardSquareOnLeftClick(std::shared_ptr<FlatEngine::GameObject> thisObject)
{
	// Get board square from the object and get the piece inside the square
	std::shared_ptr<FlatEngine::ScriptComponent> scriptComponent = std::static_pointer_cast<FlatEngine::ScriptComponent>(thisObject->GetComponent(FlatEngine::ComponentTypes::Script));
	std::shared_ptr<BoardSquare> boardSquare = std::static_pointer_cast<BoardSquare>(scriptComponent->GetScriptInstance());
	
	FlatEngine::gameManager->SetSelectedSquare(boardSquare);
}

void BoardSquareOnRightClick(std::shared_ptr<FlatEngine::GameObject> thisObject)
{
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

void BoardSquare::AssignPiece(std::string name, std::string path, std::string color, float yOffset)
{
	pieceName = name;
	pieceType = name;
	if (pieceName.length() > 0)
		pieceType.pop_back();
	pieceColor = color;
	spritePath = path;
	spriteYOffset = yOffset;
	GetOwner()->GetFirstChild()->GetSpriteComponent()->SetTexture(spritePath);
	Vector2 offset = GetOwner()->GetFirstChild()->GetSpriteComponent()->GetOffset();
	GetOwner()->GetFirstChild()->GetSpriteComponent()->SetOffset(Vector2(offset.x, yOffset));
}

void BoardSquare::RemovePiece()
{
	pieceName = "";
	pieceColor = "";
	pieceType = "";
	spritePath = "assets/images/pieces/Clear.png";
	spriteYOffset = 0;
	GetOwner()->GetFirstChild()->GetSpriteComponent()->SetTexture(spritePath);
}

std::vector<std::shared_ptr<BoardSquare>> BoardSquare::GetAvailableMoves(std::vector<std::vector<std::shared_ptr<BoardSquare>>> boardSquares)
{
	return std::vector<std::shared_ptr<BoardSquare>>();
}
