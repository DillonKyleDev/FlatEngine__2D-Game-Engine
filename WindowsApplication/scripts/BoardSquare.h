#pragma once
#include "../GameScript.h"
#include "Piece.h"
#include "../Vector2.h"


class BoardSquare : public FlatEngine::GameScript
{
public:
	BoardSquare();
	~BoardSquare();

	void Start();
	void Update(float deltaTime);

private:
	std::string squareName;
	std::shared_ptr<Piece> piece;
};

