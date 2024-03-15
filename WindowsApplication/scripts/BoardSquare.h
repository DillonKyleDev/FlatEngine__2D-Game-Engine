#pragma once
#include "../GameScript.h"
#include "../Vector2.h"


class BoardSquare : public FlatEngine::GameScript
{
public:
	BoardSquare();
	BoardSquare(std::shared_ptr<BoardSquare> toCopy);
	~BoardSquare();

	void Start();
	void Update(float deltaTime);
	void AssignPiece(std::string name, std::string spritePath, std::string color, float yOffset);
	void RemovePiece();
	std::vector<std::shared_ptr<BoardSquare>> GetAvailableMoves(std::vector<std::vector<std::shared_ptr<BoardSquare>>> boardSquares);

	std::string pieceName;
	std::string pieceType;
	std::string pieceColor;
	std::string spritePath;
	bool _hasMoved;
	float spriteYOffset;
	int column;
	int row;

private:
	std::string squareName;

};

