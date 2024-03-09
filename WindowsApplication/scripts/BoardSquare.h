#pragma once
#include "../GameScript.h"
#include "../Vector2.h"


class BoardSquare : public FlatEngine::GameScript
{
public:
	BoardSquare();
	~BoardSquare();

	void Start();
	void Update(float deltaTime);
	void AssignPiece(std::string name, std::string spritePath, std::string color, float yOffset);
	void RemovePiece();
	std::vector<std::shared_ptr<BoardSquare>> GetAvailableMoves(std::vector<std::vector<std::shared_ptr<BoardSquare>>> boardSquares);

	std::string pieceName;
	std::string pieceColor;
	std::string spritePath;
	float spriteYOffset;
	int column;
	int row;

private:
	std::string squareName;
};

