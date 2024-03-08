#pragma once
#include "../GameScript.h"



class Piece : public FlatEngine::GameScript
{
public:
	Piece();
	~Piece();

	void Start();
	void Update(float deltaTime);

	void CreatePieceObject(long parentID);
	void SetSprite(std::string path);
	std::string GetSprite();
	void SetSpriteOffsetY(float offsetY);
	void SetPieceName(std::string name);
	std::string GetName();
	void SetColor(std::string color);
	std::string GetColor();
	std::shared_ptr<FlatEngine::GameObject> GetPieceObject();
	void PlaceOnSquare(std::shared_ptr<FlatEngine::GameObject> boardLocation);


private:
	std::string pieceName;
	std::string spritePath;
	std::shared_ptr<FlatEngine::GameObject> pieceObject;
	std::shared_ptr<FlatEngine::Transform> pieceTransform;
	float spriteOffsetY;
	std::string color;
};

