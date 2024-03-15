#include "GameManager.h"
#include "../FlatEngine.h"
#include "../ScriptComponent.h"


GameManager::GameManager()
{
	this->SetName("GameManager");
	this->playerTurn = "white";
	this->selectedSquare = nullptr;
	// Get the GameBoard script in the scene
	std::shared_ptr<FlatEngine::GameObject> boardObject = FlatEngine::GetObjectByName("GameBoard");
	std::shared_ptr<FlatEngine::ScriptComponent> scriptComponent = std::static_pointer_cast<FlatEngine::ScriptComponent>(boardObject->GetComponent(FlatEngine::ComponentTypes::Script));
	this->gameBoard = std::static_pointer_cast<GameBoard>(scriptComponent->GetScriptInstance());
	movePieceAudio = nullptr;
	_whitePiecesActive = false;
	_blackPiecesActive = false;
	_whiteChecked = false;
	_blackChecked = false;
	whiteKing = nullptr;
	blackKing = nullptr;
	fromSquare = nullptr;
	toSquare = nullptr;
	peiceDestroyed = nullptr;
	_checkMate = false;
}

GameManager::~GameManager()
{
}

void GameManager::Start()
{
	FlatEngine::LogString("GameManager instantiated on: " + this->GetOwner()->GetName());
	gameBoard->SetupBoard();
	SetWhiteActive(true);
	SetBlackActive(false);
	movePieceAudio = std::static_pointer_cast<FlatEngine::Audio>(GetOwner()->GetComponent(FlatEngine::ComponentTypes::Audio));
	movePieceAudio->SetIsMusic(false);
	movePieceAudio->LoadEffect("assets/audio/MovePiece.wav");
}

void GameManager::Update(float deltaTime)
{
	// Check for win conditions
	// if no win conditions
	if (!_checkMate)
	{
		std::shared_ptr<FlatEngine::Sprite> whiteKingSprite = std::static_pointer_cast<FlatEngine::Sprite>(whiteKing->GetOwner()->GetComponent(FlatEngine::ComponentTypes::Sprite));
		std::shared_ptr<FlatEngine::Sprite> blackKingSprite = std::static_pointer_cast<FlatEngine::Sprite>(blackKing->GetOwner()->GetComponent(FlatEngine::ComponentTypes::Sprite));

		// Set correct piece buttons to active
		UpdateCheckmateCheck();

		if (_whiteChecked)
			whiteKingSprite->SetTexture("assets/images/pieces/Checked.png");
		else if (whiteKing != nullptr && selectedSquare != nullptr && whiteKing->pieceName == selectedSquare->pieceName)
			whiteKingSprite->SetTexture("assets/images/pieces/Selected.png");
		else
			whiteKingSprite->SetTexture("assets/images/pieces/Clear.png");

		if (_blackChecked)
			blackKingSprite->SetTexture("assets/images/pieces/Checked.png");
		else if (blackKing != nullptr && selectedSquare != nullptr && blackKing->pieceName == selectedSquare->pieceName)
			blackKingSprite->SetTexture("assets/images/pieces/Selected.png");
		else
			blackKingSprite->SetTexture("assets/images/pieces/Clear.png");
	}
}

void GameManager::SetWhiteActive(bool _isActive)
{
	for (std::shared_ptr<BoardSquare> whiteSquare : this->gameBoard->GetWhiteSquares())
	{
		if (whiteSquare->pieceName == "King_W")
			whiteKing = whiteSquare;

		std::shared_ptr<FlatEngine::Button> whiteButton = std::static_pointer_cast<FlatEngine::Button>(whiteSquare->GetOwner()->GetComponent(FlatEngine::ComponentTypes::Button));
		whiteButton->SetActive(_isActive);
	}
	this->_whitePiecesActive = _isActive;
}

void GameManager::SetBlackActive(bool _isActive)
{
	for (std::shared_ptr<BoardSquare> blackSquare : this->gameBoard->GetBlackSquares())
	{
		if (blackSquare->pieceName == "King_B")
			blackKing = blackSquare;

		std::shared_ptr<FlatEngine::Button> blackButton = std::static_pointer_cast<FlatEngine::Button>(blackSquare->GetOwner()->GetComponent(FlatEngine::ComponentTypes::Button));
		blackButton->SetActive(_isActive);
	}
	this->_blackPiecesActive = _isActive;
}

void GameManager::SetSelectedSquare(std::shared_ptr<BoardSquare> square)
{
	std::shared_ptr<FlatEngine::Sprite> newSprite = square->GetOwner()->GetSpriteComponent();
	std::shared_ptr<FlatEngine::Transform> newTransform = square->GetOwner()->GetFirstChild()->GetTransformComponent();
	std::shared_ptr<FlatEngine::Sprite> oldSprite = nullptr;
	std::shared_ptr<FlatEngine::Transform> oldTransform = nullptr;

	if (selectedSquare != nullptr)
	{
		DeactivateAvailableMoves();

		oldSprite = selectedSquare->GetOwner()->GetSpriteComponent();
		oldTransform = selectedSquare->GetOwner()->GetFirstChild()->GetTransformComponent();
		// Deselect piece that was already selected
		oldSprite->SetTexture("assets/images/pieces/Clear.png");
		oldTransform->SetPosition(Vector2(oldTransform->GetPosition().x, oldTransform->GetPosition().y - 0.1f));
	}

	// All squares being selected should have valid actions

	// If no square has been clicked yet
	if (selectedSquare == nullptr)
	{
		selectedSquare = square;
		newSprite->SetTexture("assets/images/pieces/Selected.png");
		newTransform->SetPosition(Vector2(newTransform->GetPosition().x, newTransform->GetPosition().y + 0.1f));
	}
	// If clicking on the same square as before
	else if (selectedSquare == square)
	{
		selectedSquare = nullptr;
		DeactivateAvailableMoves();
	}
	// If square is the same color as the already selected square
	else if (square->pieceColor == selectedSquare->pieceColor)
	{
		// Select the new piece in its place
		selectedSquare = square;
		newSprite->SetTexture("assets/images/pieces/Selected.png");
		newTransform->SetPosition(Vector2(newTransform->GetPosition().x, newTransform->GetPosition().y + 0.1f));
	}
	// If the square is a piece that has a different color (even an empty square)
	else if (square->pieceColor != selectedSquare->pieceColor)
	{
		// Save square data for undo function
		fromSquare = selectedSquare;
		toSquare = square;
		if (square->pieceColor != "")
			peiceDestroyed = std::make_shared<BoardSquare>(square);
		else
			peiceDestroyed = nullptr;
		// Take the piece
		gameBoard->TakePiece(square, selectedSquare);
		DeactivateAvailableMoves();
		movePieceAudio->Play();
		square->_hasMoved = true;

		if (square->pieceName == "King_W")
			whiteKing = square;
		else if (square->pieceName == "King_B")
			blackKing = square;

		ChangeTurns();
		UpdateCheckmateCheck();
	}

	if (selectedSquare != nullptr)
	{
		ActivateAvailableMoves();
	}
}

void GameManager::ActivateAvailableMoves()
{
	std::vector<std::shared_ptr<BoardSquare>> unfilteredMoves;
	std::vector<std::shared_ptr<BoardSquare>> filteredMoves;

	if (selectedSquare->pieceType == "Pawn_")
		unfilteredMoves = PawnMoves(selectedSquare);
	else if (selectedSquare->pieceType == "Rook_")
		unfilteredMoves = RookMoves(selectedSquare);
	else if (selectedSquare->pieceType == "Knight_")
		unfilteredMoves = KnightMoves(selectedSquare);
	else if (selectedSquare->pieceType == "Bishop_")
		unfilteredMoves = BishopMoves(selectedSquare);
	else if (selectedSquare->pieceType == "Queen_")
		unfilteredMoves = QueenMoves(selectedSquare);
	else if (selectedSquare->pieceType == "King_")
		unfilteredMoves = KingMoves(selectedSquare);

	// Make sure each move doesn't cause a check on the player moving
	for (std::shared_ptr<BoardSquare> moveTo : unfilteredMoves)
	{
		if (playerTurn == "white" && !CheckMoveForStillchecked(selectedSquare, moveTo, whiteKing) || playerTurn == "black" && !CheckMoveForStillchecked(selectedSquare, moveTo, blackKing))
		{
			filteredMoves.push_back(moveTo);
		}
	}

	for (std::shared_ptr<BoardSquare> square : filteredMoves)
	{
		// Change the sprite
		std::shared_ptr<FlatEngine::Sprite> sprite = std::static_pointer_cast<FlatEngine::Sprite>(square->GetOwner()->GetComponent(FlatEngine::ComponentTypes::Sprite));
		sprite->SetTexture("assets/images/pieces/CanMoveTo.png");
		// Activate the button
		std::shared_ptr<FlatEngine::Button> button = std::static_pointer_cast<FlatEngine::Button>(square->GetOwner()->GetComponent(FlatEngine::ComponentTypes::Button));
		button->SetActive(true);
	}

	availableMoves = filteredMoves;
}

void GameManager::DeactivateAvailableMoves()
{
	if (this->availableMoves.size() > 0)
	{
		for (std::shared_ptr<BoardSquare> square : this->availableMoves)
		{
			// Change the sprite
			std::shared_ptr<FlatEngine::Sprite> sprite = std::static_pointer_cast<FlatEngine::Sprite>(square->GetOwner()->GetComponent(FlatEngine::ComponentTypes::Sprite));
			sprite->SetTexture("assets/images/pieces/Clear.png");

			if (square->pieceColor != playerTurn)
			{
				// Deactivate the button
				std::shared_ptr<FlatEngine::Button> button = std::static_pointer_cast<FlatEngine::Button>(square->GetOwner()->GetComponent(FlatEngine::ComponentTypes::Button));
				button->SetActive(false);
			}
		}
	}
}

void GameManager::ChangeTurns()
{
	// Change turns
	this->selectedSquare = nullptr;
	if (this->playerTurn == "white")
	{
		this->playerTurn = "black";
	}
	else
	{
		this->playerTurn = "white";
	}
}

void GameManager::UpdateCheckmateCheck()
{
	if (this->playerTurn == "white" && !this->_whitePiecesActive)
	{
		_whiteChecked = CheckForCheck(whiteKing);
		_blackChecked = CheckForCheck(blackKing);

		if (_whiteChecked)
		{
			// Check for at least one move that will uncheck the white king
			if (CheckForCheckmate(whiteKing))
			{
				// Game Over
				FlatEngine::LogString("GameOVER!!!");
				_checkMate = true;
				SetWhiteActive(false);
				SetBlackActive(false);
			}
		}

		if (!_checkMate)
		{
			SetWhiteActive(true);
			SetBlackActive(false);
		}
	}
	else if (this->playerTurn == "black" && !this->_blackPiecesActive)
	{
		_whiteChecked = CheckForCheck(whiteKing);
		_blackChecked = CheckForCheck(blackKing);

		if (_blackChecked)
		{
			// Check for at least one move that will uncheck the black king
			if (CheckForCheckmate(blackKing))
			{
				// Game Over
				FlatEngine::LogString("GameOVER!!!");
				_checkMate = true;
				SetWhiteActive(false);
				SetBlackActive(false);
			}
		}

		if (!_checkMate)
		{
			SetWhiteActive(false);
			SetBlackActive(true);
		}
	}
}

bool GameManager::CheckForCheck(std::shared_ptr<BoardSquare> king)
{
	std::vector<std::shared_ptr<BoardSquare>> squaresInLineOfSight;
	std::vector<std::shared_ptr<BoardSquare>> boardSquares = gameBoard->GetBoardSquares();

	for (std::shared_ptr<BoardSquare> boardSquare : boardSquares)
	{
		if (boardSquare->row == king->row && boardSquare->column != king->column)
			squaresInLineOfSight.push_back(boardSquare);
		if (boardSquare->column == king->column)
			squaresInLineOfSight.push_back(boardSquare);
		if (boardSquare->row != king->row && boardSquare->column != king->column)
		{
			if (boardSquare->row - king->row == boardSquare->column - king->column)
				squaresInLineOfSight.push_back(boardSquare);
			else if (boardSquare->row - king->row == king->column - boardSquare->column)
				squaresInLineOfSight.push_back(boardSquare);
		}
	}

	for (std::shared_ptr<BoardSquare> square : squaresInLineOfSight)
	{
		if (square->pieceColor != king->pieceColor && square->pieceName != "")
		{
			if (square->pieceType == "Pawn_")
			{
				for (std::shared_ptr<BoardSquare> target : PawnMoves(square))
				{
					if (target->pieceName == king->pieceName)
					{
						return true;
					}
				}
			}
			else if (square->pieceType == "Rook_")
			{
				for (std::shared_ptr<BoardSquare> target : RookMoves(square))
				{
					if (target->pieceName == king->pieceName)
					{
						return true;
					}
				}
			}
			else if (square->pieceType == "Knight_")
			{
				for (std::shared_ptr<BoardSquare> target : KnightMoves(square))
				{
					if (target->pieceName == king->pieceName)
					{
						return true;
					}
				}
			}
			else if (square->pieceType == "Bishop_")
			{
				for (std::shared_ptr<BoardSquare> target : BishopMoves(square))
				{
					if (target->pieceName == king->pieceName)
					{
						return true;
					}
				}
			}
			else if (square->pieceType == "Queen_")
			{
				for (std::shared_ptr<BoardSquare> target : QueenMoves(square))
				{
					if (target->pieceName == king->pieceName)
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}

bool GameManager::CheckForCheckmate(std::shared_ptr<BoardSquare> king)
{
	if (playerTurn == "white")
	{
		for (std::shared_ptr<BoardSquare> square : gameBoard->GetWhiteSquares())
		{
			std::vector<std::shared_ptr<BoardSquare>> potentialMoves;

			if (square->pieceType == "Pawn_")
				potentialMoves = PawnMoves(square);
			else if (square->pieceType == "Rook_")
				potentialMoves = RookMoves(square);
			else if (square->pieceType == "Knight_")
				potentialMoves = KnightMoves(square);
			else if (square->pieceType == "Bishop_")
				potentialMoves = BishopMoves(square);
			else if (square->pieceType == "Queen_")
				potentialMoves = QueenMoves(square);
			else if (square->pieceType == "King_")
				potentialMoves = KingMoves(square);

			for (std::shared_ptr<BoardSquare> moveTo : potentialMoves)
			{
				if (!CheckMoveForStillchecked(square, moveTo, king))
					return false;
			}
		}
	}

	return true;
}

bool GameManager::CheckMoveForStillchecked(std::shared_ptr<BoardSquare> moveFrom, std::shared_ptr<BoardSquare> moveTo, std::shared_ptr<BoardSquare> kingToCheck)
{
	bool _stillChecked;
	std::shared_ptr<BoardSquare> tempKingSquare;

	// Save square data for undo function
	fromSquareChecking = moveFrom;
	toSquareChecking = moveTo;
	if (moveTo->pieceColor != "")
		peiceDestroyedChecking = std::make_shared<BoardSquare>(moveTo);
	else
		peiceDestroyedChecking = nullptr;

	gameBoard->TakePiece(moveTo, moveFrom);

	if (moveTo->pieceName == "King_W")
		tempKingSquare = moveTo;
	else if (moveTo->pieceName == "King_B")
		tempKingSquare = moveTo;
	else
		tempKingSquare = kingToCheck;

	_stillChecked = CheckForCheck(tempKingSquare);

	UndoMoveChecking();

	return _stillChecked;
}

void GameManager::UndoMove()
{
	gameBoard->TakePiece(fromSquare, toSquare);
	std::shared_ptr<FlatEngine::Button> button = std::static_pointer_cast<FlatEngine::Button>(fromSquare->GetOwner()->GetComponent(FlatEngine::ComponentTypes::Button));
	button->SetActive(true);

	if (peiceDestroyed != nullptr)
		gameBoard->SpawnPiece(toSquare, peiceDestroyed);
}

void GameManager::UndoMoveChecking()
{
	gameBoard->TakePiece(fromSquareChecking, toSquareChecking);
	std::shared_ptr<FlatEngine::Button> button = std::static_pointer_cast<FlatEngine::Button>(fromSquareChecking->GetOwner()->GetComponent(FlatEngine::ComponentTypes::Button));
	button->SetActive(true);

	if (peiceDestroyedChecking != nullptr)
		gameBoard->SpawnPiece(toSquareChecking, peiceDestroyedChecking);
}

std::vector<std::shared_ptr<BoardSquare>> GameManager::PawnMoves(std::shared_ptr<BoardSquare> square)
{
	std::vector<std::shared_ptr<BoardSquare>> availableSquares;
	std::vector<std::shared_ptr<BoardSquare>> boardSquares = gameBoard->GetBoardSquares();

	for (std::shared_ptr<BoardSquare> boardSquare : boardSquares)
	{
		if (square->pieceColor == "white")
		{
			// Moving
			if (boardSquare->column == square->column &&
				(boardSquare->row == square->row + 1 || (boardSquare->row == square->row + 2 && square->_hasMoved == false)) &&
				boardSquare->pieceName == "")
			{
				availableSquares.push_back(boardSquare);
			}
			// For taking enemy piece
			else if ((boardSquare->column == square->column + 1 || boardSquare->column == square->column - 1) && boardSquare->row == square->row + 1 &&
				boardSquare->pieceColor == "black")
			{
				availableSquares.push_back(boardSquare);
			}
		}
		else // if (square->pieceColor == "black")
		{
			// Moving
			if (boardSquare->column == square->column &&
				(boardSquare->row == square->row - 1 || (boardSquare->row == square->row - 2 && square->_hasMoved == false)) &&
				boardSquare->pieceName == "")
			{
				availableSquares.push_back(boardSquare);
			}
			// For taking enemy piece
			else if ((boardSquare->column == square->column + 1 || boardSquare->column == square->column - 1) && boardSquare->row == square->row - 1 &&
				boardSquare->pieceColor == "white")
			{
				availableSquares.push_back(boardSquare);
			}
		}
	}

	return availableSquares;
}

std::vector<std::shared_ptr<BoardSquare>> GameManager::RookMoves(std::shared_ptr<BoardSquare> square)
{
	std::vector<std::shared_ptr<BoardSquare>> availableSquares;
	std::vector<std::shared_ptr<BoardSquare>> boardSquares = gameBoard->GetBoardSquares();

	std::vector<std::shared_ptr<BoardSquare>> squaresInSameRow;
	std::vector<std::shared_ptr<BoardSquare>> squaresInSameCol;


	for (std::shared_ptr<BoardSquare> boardSquare : boardSquares)
	{
		if (boardSquare->row == square->row && boardSquare->column != square->column)
			squaresInSameRow.push_back(boardSquare);
		else if (boardSquare->column == square->column && boardSquare->row != square->row)
			squaresInSameCol.push_back(boardSquare);
	}

	CheckRow(squaresInSameRow, availableSquares, square->row, square->column);
	CheckCol(squaresInSameCol, availableSquares, square->row, square->column);

	return availableSquares;
}

std::vector<std::shared_ptr<BoardSquare>> GameManager::KnightMoves(std::shared_ptr<BoardSquare> square)
{
	std::vector<std::shared_ptr<BoardSquare>> availableSquares;
	std::vector<std::shared_ptr<BoardSquare>> boardSquares = gameBoard->GetBoardSquares();

	for (std::shared_ptr<BoardSquare> boardSquare : boardSquares)
	{
		if (((boardSquare->row == square->row + 1 && (boardSquare->column == square->column - 2 || boardSquare->column == square->column + 2)) ||
			(boardSquare->row == square->row - 1 && (boardSquare->column == square->column - 2 || boardSquare->column == square->column + 2)) ||
			(boardSquare->row == square->row + 2 && (boardSquare->column == square->column - 1 || boardSquare->column == square->column + 1)) ||
			(boardSquare->row == square->row - 2 && (boardSquare->column == square->column - 1 || boardSquare->column == square->column + 1))) &&
			boardSquare->pieceColor != playerTurn)
			availableSquares.push_back(boardSquare);
	}

	return availableSquares;
}

std::vector<std::shared_ptr<BoardSquare>> GameManager::BishopMoves(std::shared_ptr<BoardSquare> square)
{
	std::vector<std::shared_ptr<BoardSquare>> availableSquares;
	std::vector<std::shared_ptr<BoardSquare>> boardSquares = gameBoard->GetBoardSquares();

	std::vector<std::shared_ptr<BoardSquare>> squaresInPositiveDiag;
	std::vector<std::shared_ptr<BoardSquare>> squaresInNegativeDiag;


	for (std::shared_ptr<BoardSquare> boardSquare : boardSquares)
	{
		if (boardSquare->row != square->row && boardSquare->column != square->column)
		{
			if (boardSquare->row - square->row == boardSquare->column - square->column)
				squaresInPositiveDiag.push_back(boardSquare);
			else if (boardSquare->row - square->row == square->column - boardSquare->column)
				squaresInNegativeDiag.push_back(boardSquare);
		}
	}

	CheckPosDiag(squaresInPositiveDiag, availableSquares, square->row, square->column);
	CheckNegDiag(squaresInNegativeDiag, availableSquares, square->row, square->column);

	return availableSquares;
}

std::vector<std::shared_ptr<BoardSquare>> GameManager::QueenMoves(std::shared_ptr<BoardSquare> square)
{
	std::vector<std::shared_ptr<BoardSquare>> availableSquares;
	std::vector<std::shared_ptr<BoardSquare>> boardSquares = gameBoard->GetBoardSquares();

	std::vector<std::shared_ptr<BoardSquare>> squaresInSameRow;
	std::vector<std::shared_ptr<BoardSquare>> squaresInSameCol;
	std::vector<std::shared_ptr<BoardSquare>> squaresInPositiveDiag;
	std::vector<std::shared_ptr<BoardSquare>> squaresInNegativeDiag;

	for (std::shared_ptr<BoardSquare> boardSquare : boardSquares)
	{
		if (boardSquare->row == square->row && boardSquare->column != square->column)
			squaresInSameRow.push_back(boardSquare);
		if (boardSquare->column == square->column)
			squaresInSameCol.push_back(boardSquare);
		if (boardSquare->row != square->row && boardSquare->column != square->column)
		{
			if (boardSquare->row - square->row == boardSquare->column - square->column)
				squaresInPositiveDiag.push_back(boardSquare);
			else if (boardSquare->row - square->row == square->column - boardSquare->column)
				squaresInNegativeDiag.push_back(boardSquare);
		}
	}

	CheckRow(squaresInSameRow, availableSquares, square->row, square->column);
	CheckCol(squaresInSameCol, availableSquares, square->row, square->column);
	CheckPosDiag(squaresInPositiveDiag, availableSquares, square->row, square->column);
	CheckNegDiag(squaresInNegativeDiag, availableSquares, square->row, square->column);

	return availableSquares;
}

std::vector<std::shared_ptr<BoardSquare>> GameManager::KingMoves(std::shared_ptr<BoardSquare> square)
{
	std::vector<std::shared_ptr<BoardSquare>> availableSquares;
	std::vector<std::shared_ptr<BoardSquare>> boardSquares = gameBoard->GetBoardSquares();

	std::vector<std::shared_ptr<BoardSquare>> squaresInSameRow;
	std::vector<std::shared_ptr<BoardSquare>> squaresInSameCol;
	std::vector<std::shared_ptr<BoardSquare>> squaresInPositiveDiag;
	std::vector<std::shared_ptr<BoardSquare>> squaresInNegativeDiag;


	for (std::shared_ptr<BoardSquare> boardSquare : boardSquares)
	{
		if (boardSquare->row == square->row && (boardSquare->column == square->column - 1 || boardSquare->column == square->column + 1) && boardSquare->pieceColor != playerTurn)
			squaresInSameRow.push_back(boardSquare);
		else if (boardSquare->column == square->column && (boardSquare->row == square->row - 1 || boardSquare->row == square->row + 1) && boardSquare->pieceColor != playerTurn)
			squaresInSameCol.push_back(boardSquare);
		else if (boardSquare->row == square->row + 1 && boardSquare->column == square->column + 1)
			squaresInPositiveDiag.push_back(boardSquare);
		else if (boardSquare->row == square->row + 1 && boardSquare->column == square->column - 1)
			squaresInNegativeDiag.push_back(boardSquare);
		else if (boardSquare->row == square->row - 1 && boardSquare->column == square->column + 1)
			squaresInNegativeDiag.push_back(boardSquare);
		else if (boardSquare->row == square->row - 1 && boardSquare->column == square->column - 1)
			squaresInPositiveDiag.push_back(boardSquare);
	}

	CheckRow(squaresInSameRow, availableSquares, square->row, square->column);
	CheckCol(squaresInSameCol, availableSquares, square->row, square->column);
	CheckPosDiag(squaresInPositiveDiag, availableSquares, square->row, square->column);
	CheckNegDiag(squaresInNegativeDiag, availableSquares, square->row, square->column);

	return availableSquares;
}

std::vector< std::shared_ptr<BoardSquare>> GameManager::CheckRow(std::vector< std::shared_ptr<BoardSquare>> squares, std::vector< std::shared_ptr<BoardSquare>>& availableSquares, int row, int col)
{
	bool _leftEdgeFound = false;
	bool _rightEdgeFound = false;
	std::vector< std::shared_ptr<BoardSquare>> enemySquares;

	for (int i = 1; i < 8; i++)
	{
		for (std::shared_ptr<BoardSquare> boardSquare : squares)
		{
			if (!_leftEdgeFound && col - i > 0 && boardSquare->column == col - i)
			{
				_leftEdgeFound = boardSquare->pieceName != "";
				if (boardSquare->pieceColor != playerTurn)
					availableSquares.push_back(boardSquare);
				if (boardSquare->pieceColor != "")
					enemySquares.push_back(boardSquare);
			}

			if (!_rightEdgeFound && col + i < 9 && boardSquare->column == col + i)
			{
				_rightEdgeFound = boardSquare->pieceName != "";
				if (boardSquare->pieceColor != playerTurn)
					availableSquares.push_back(boardSquare);
				if (boardSquare->pieceColor != "")
					enemySquares.push_back(boardSquare);
			}
		}
	}

	return enemySquares;
}

std::vector< std::shared_ptr<BoardSquare>> GameManager::CheckCol(std::vector< std::shared_ptr<BoardSquare>> squares, std::vector< std::shared_ptr<BoardSquare>>& availableSquares, int row, int col)
{
	bool _topEdgeFound = false;
	bool _bottomEdgeFound = false;
	std::vector< std::shared_ptr<BoardSquare>> enemySquares;

	for (int i = 1; i < 8; i++)
	{
		for (std::shared_ptr<BoardSquare> boardSquare : squares)
		{
			if (!_topEdgeFound && row + i < 9 && boardSquare->row == row + i)
			{
				_topEdgeFound = boardSquare->pieceName != "";
				if (boardSquare->pieceColor != playerTurn)
				{
					availableSquares.push_back(boardSquare);
					if (boardSquare->pieceColor != "")
						enemySquares.push_back(boardSquare);
				}
			}

			if (!_bottomEdgeFound && row - i > 0 && boardSquare->row == row - i)
			{
				_bottomEdgeFound = boardSquare->pieceName != "";
				if (boardSquare->pieceColor != playerTurn)
				{
					availableSquares.push_back(boardSquare);
					if (boardSquare->pieceColor != "")
						enemySquares.push_back(boardSquare);
				}
			}
		}
	}

	return enemySquares;
}

std::vector< std::shared_ptr<BoardSquare>> GameManager::CheckPosDiag(std::vector<std::shared_ptr<BoardSquare>> squares, std::vector<std::shared_ptr<BoardSquare>>& availableSquares, int row, int col)
{
	bool _topEdgeFound = false;
	bool _bottomEdgeFound = false;
	std::vector< std::shared_ptr<BoardSquare>> enemySquares;

	for (int i = 1; i < 8; i++)
	{
		for (std::shared_ptr<BoardSquare> boardSquare : squares)
		{
			if (!_topEdgeFound && row + i < 9 && (boardSquare->row == row + i && boardSquare->column == col + i))
			{
				_topEdgeFound = boardSquare->pieceName != "";
				if (boardSquare->pieceColor != playerTurn)
				{
					availableSquares.push_back(boardSquare);
					if (boardSquare->pieceColor != "")
						enemySquares.push_back(boardSquare);
				}
			}

			if (!_bottomEdgeFound && row - i > 0 && (boardSquare->row == row - i && boardSquare->column == col - i))
			{
				_bottomEdgeFound = boardSquare->pieceName != "";
				if (boardSquare->pieceColor != playerTurn)
				{
					availableSquares.push_back(boardSquare);
					if (boardSquare->pieceColor != "")
						enemySquares.push_back(boardSquare);
				}
			}
		}
	}

	return enemySquares;
}

std::vector< std::shared_ptr<BoardSquare>> GameManager::CheckNegDiag(std::vector<std::shared_ptr<BoardSquare>> squares, std::vector<std::shared_ptr<BoardSquare>>& availableSquares, int row, int col)
{
	bool _topEdgeFound = false;
	bool _bottomEdgeFound = false;
	std::vector< std::shared_ptr<BoardSquare>> enemySquares;

	for (int i = 1; i < 8; i++)
	{
		for (std::shared_ptr<BoardSquare> boardSquare : squares)
		{
			if (!_topEdgeFound && row + i < 9 && (boardSquare->row == row + i && boardSquare->column == col - i))
			{
				_topEdgeFound = boardSquare->pieceName != "";
				if (boardSquare->pieceColor != playerTurn)
					availableSquares.push_back(boardSquare);
			}

			if (!_bottomEdgeFound && row - i > 0 && (boardSquare->row == row - i && boardSquare->column == col + i))
			{
				_bottomEdgeFound = boardSquare->pieceName != "";
				if (boardSquare->pieceColor != playerTurn)
					availableSquares.push_back(boardSquare);
			}
		}
	}

	return enemySquares;
}