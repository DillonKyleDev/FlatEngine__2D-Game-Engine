#pragma once
#include "../GameScript.h"
#include "Piece.h"
#include "../Vector2.h"
#include "../GameObject.h"


class BoardSquare : public FlatEngine::GameScript
{
public:
	BoardSquare();
	~BoardSquare();

	void Start();
	void Update(float deltaTime);
	void SetBoardLocation(std::shared_ptr<FlatEngine::GameObject> boardLocation);
	std::shared_ptr<FlatEngine::GameObject> GetBoardLocation();
	void SetPiece(std::shared_ptr<FlatEngine::GameObject> piece);
	std::shared_ptr<FlatEngine::GameObject> GetPiece();

private:
	std::string squareName;
	std::shared_ptr<FlatEngine::GameObject> boardLocation;
	std::shared_ptr<FlatEngine::GameObject> piece;
};

