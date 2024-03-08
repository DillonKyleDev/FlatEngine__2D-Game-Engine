#pragma once
#include "../GameScript.h"
#include "BoardSquare.h"
#include <map>
#include <string>


class GameBoard : public FlatEngine::GameScript
{
public:
	GameBoard();
	~GameBoard();

	void Awake();
	void Start();
	void Update(float deltaTime);

	void InitializeBoard();
	void SetupPieces();
	void CreatePiece(std::shared_ptr<FlatEngine::GameObject> gameObject, std::shared_ptr<BoardSquare> boardSquare, std::shared_ptr<Piece> piece, std::string name, std::string spritePath, std::string color, float yOffset);

	void SetupBoard();

private:
	std::vector<long> squareIDs;
	std::vector<std::vector<std::shared_ptr<BoardSquare>>> boardSquares;
};

