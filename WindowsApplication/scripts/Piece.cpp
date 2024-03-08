#include "Piece.h"
#include "../Sprite.h"
#include "../FlatEngine.h"

Piece::Piece()
{
	this->pieceName = "";
	this->spritePath = "";
	this->pieceObject = nullptr;
	this->pieceTransform = nullptr;
	this->spriteOffsetY = 0;
}

Piece::~Piece()
{
}

void Piece::Start()
{
}

void Piece::Update(float deltaTime)
{
}

void Piece::CreatePieceObject(long parentID)
{
	this->pieceObject = FlatEngine::CreateGameObject(parentID);
	this->pieceObject->AddComponent(FlatEngine::ComponentTypes::Transform);
	this->pieceObject->AddComponent(FlatEngine::ComponentTypes::Sprite);
	this->pieceTransform = std::static_pointer_cast<FlatEngine::Transform>(this->GetPieceObject()->GetComponent(FlatEngine::ComponentTypes::Transform));
}

void Piece::SetSprite(std::string path)
{
	std::shared_ptr<FlatEngine::Sprite> spriteComponent = std::static_pointer_cast<FlatEngine::Sprite>(this->pieceObject->GetComponent(FlatEngine::ComponentTypes::Sprite));
	spriteComponent->SetTexture(path);
	spriteComponent->SetRenderOrder(5);
}

std::string Piece::GetSprite()
{
	return this->spritePath;
}

void Piece::SetSpriteOffsetY(float offsetY)
{
	this->spriteOffsetY = offsetY;
	std::shared_ptr<FlatEngine::Sprite> spriteComponent = std::static_pointer_cast<FlatEngine::Sprite>(this->pieceObject->GetComponent(FlatEngine::ComponentTypes::Sprite));
	float currentXOffset = spriteComponent->GetOffset().x;
	spriteComponent->SetOffset(Vector2(currentXOffset, offsetY));
}

void Piece::SetPieceName(std::string name)
{
	this->pieceObject->SetName(name);
}

std::string Piece::GetName()
{
	return this->pieceObject->GetName();
}

void Piece::SetColor(std::string color)
{
	this->color = color;
}

std::string Piece::GetColor()
{
	return this->color;
}

std::shared_ptr<FlatEngine::GameObject> Piece::GetPieceObject()
{
	return this->pieceObject;
}

void Piece::PlaceOnSquare(std::shared_ptr<FlatEngine::GameObject> boardLocation)
{
	std::shared_ptr<FlatEngine::Transform> squareTransform = std::static_pointer_cast<FlatEngine::Transform>(boardLocation->GetComponent(FlatEngine::ComponentTypes::Transform));
	Vector2 squarePosition = squareTransform->GetPosition();

	this->pieceTransform->SetPosition(squarePosition);
}