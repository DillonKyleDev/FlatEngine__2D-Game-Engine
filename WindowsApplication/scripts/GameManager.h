#pragma once
#include "../GameScript.h"
#include "GameBoard.h"
#include "Piece.h"
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
	void DeselectPiece();
	void ActivateAvailableMoves();
	void DeactivateAvailableMoves();

	void PawnMoves();

private:
	std::string playerTurn;
	std::shared_ptr<BoardSquare> selectedSquare;
	std::shared_ptr<GameBoard> gameBoard;
	std::vector<std::shared_ptr<BoardSquare>> availableMoves;
	std::shared_ptr<FlatEngine::Audio> movePieceAudio;
	bool _whitePiecesActive;
	bool _blackPiecesActive;

};

