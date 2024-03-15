#pragma once
#include "../GameScript.h"
#include "GameBoard.h"
#include "../Audio.h"


// After copying the contents of these files to the newly created .h and .cpp files of your new script
// #include the script file in GameLoop.h and add a new private pointer to the class in .h
// New the variable in GameLoop()::Start() function and delete it in GameLoop()::Stop();
// Add a check to the GameLoop::Start(); function for the new script type and ->AddEntity()'s to the pointer to the class there
// Then at the end of GameLoop::Start(); function don't forget to call the GameScript::Start(); method on the new class object pointer.
// In GameLoop::Update(); call the GameScript::Update(); method on the new class object pointer.


class GameManager : public FlatEngine::GameScript
{
public:
	GameManager();
	~GameManager();

	void Start();
	void Update(float deltaTime);

	void SetWhiteActive(bool _isActive);
	void SetBlackActive(bool _isActive);
	void SetSelectedSquare(std::shared_ptr<BoardSquare> square);
	void ActivateAvailableMoves();
	void DeactivateAvailableMoves();

	void ChangeTurns();
	void UpdateCheckmateCheck();
	bool CheckForCheck(std::shared_ptr<BoardSquare> king);
	bool CheckForCheckmate(std::shared_ptr<BoardSquare> king);
	bool CheckMoveForStillchecked(std::shared_ptr<BoardSquare> moveFrom, std::shared_ptr<BoardSquare> moveTo, std::shared_ptr<BoardSquare> kingToCheck);
	void UndoMove();
	void UndoMoveChecking();

	std::vector<std::shared_ptr<BoardSquare>> PawnMoves(std::shared_ptr<BoardSquare> square);
	std::vector<std::shared_ptr<BoardSquare>> RookMoves(std::shared_ptr<BoardSquare> square);
	std::vector<std::shared_ptr<BoardSquare>> KnightMoves(std::shared_ptr<BoardSquare> square);
	std::vector<std::shared_ptr<BoardSquare>> BishopMoves(std::shared_ptr<BoardSquare> square);
	std::vector<std::shared_ptr<BoardSquare>> QueenMoves(std::shared_ptr<BoardSquare> square);
	std::vector<std::shared_ptr<BoardSquare>> KingMoves(std::shared_ptr<BoardSquare> square);
	std::vector< std::shared_ptr<BoardSquare>> CheckRow(std::vector< std::shared_ptr<BoardSquare>> squares, std::vector< std::shared_ptr<BoardSquare>>& availableSquares, int row, int col);
	std::vector< std::shared_ptr<BoardSquare>> CheckCol(std::vector< std::shared_ptr<BoardSquare>> squares, std::vector< std::shared_ptr<BoardSquare>>& availableSquares, int row, int col);
	std::vector< std::shared_ptr<BoardSquare>> CheckPosDiag(std::vector< std::shared_ptr<BoardSquare>> squares, std::vector< std::shared_ptr<BoardSquare>>& availableSquares, int row, int col);
	std::vector< std::shared_ptr<BoardSquare>> CheckNegDiag(std::vector< std::shared_ptr<BoardSquare>> squares, std::vector< std::shared_ptr<BoardSquare>>& availableSquares, int row, int col);

private:
	std::string playerTurn;
	std::shared_ptr<BoardSquare> selectedSquare;
	std::shared_ptr<GameBoard> gameBoard;
	std::vector<std::shared_ptr<BoardSquare>> availableMoves;
	std::shared_ptr<FlatEngine::Audio> movePieceAudio;
	bool _whitePiecesActive;
	bool _blackPiecesActive;
	std::shared_ptr<BoardSquare> whiteKing;
	std::shared_ptr<BoardSquare> blackKing;
	bool _whiteChecked;
	bool _blackChecked;

	std::shared_ptr<BoardSquare> fromSquare;
	std::shared_ptr<BoardSquare> toSquare;
	std::shared_ptr<BoardSquare> peiceDestroyed;

	std::shared_ptr<BoardSquare> fromSquareChecking;
	std::shared_ptr<BoardSquare> toSquareChecking;
	std::shared_ptr<BoardSquare> peiceDestroyedChecking;

	bool _checkMate;
};

