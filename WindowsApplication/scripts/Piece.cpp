#include "Piece.h"
#include "../Sprite.h"
#include "../FlatEngine.h"

Piece::Piece()
{
	this->name = "";
	this->level = 1;
	this->spritePath = "";
	this->pieceObject = nullptr;
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
}

void Piece::SetSprite(std::string path)
{
	std::shared_ptr<FlatEngine::Sprite> spriteComponent = std::static_pointer_cast<FlatEngine::Sprite>(this->pieceObject->GetComponent(FlatEngine::ComponentTypes::Sprite));
	spriteComponent->SetTexture(path);
}

std::string Piece::GetSprite()
{
	return this->spritePath;
}

void Piece::SetName(std::string name)
{
	this->pieceObject->SetName(name);
}

std::string Piece::GetName()
{
	return this->pieceObject->GetName();
}

std::shared_ptr<FlatEngine::GameObject> Piece::GetPieceObject()
{
	return this->pieceObject;
}
