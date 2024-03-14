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
}

GameManager::~GameManager()
{
}

void GameManager::Start()
{
	FlatEngine::LogString("GameManager instantiated on: " + this->GetOwner()->GetName());
	this->gameBoard->SetupBoard();
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
	std::shared_ptr<FlatEngine::Sprite> whiteKingSprite = std::static_pointer_cast<FlatEngine::Sprite>(whiteKing->GetOwner()->GetComponent(FlatEngine::ComponentTypes::Sprite));
	std::shared_ptr<FlatEngine::Sprite> blackKingSprite = std::static_pointer_cast<FlatEngine::Sprite>(blackKing->GetOwner()->GetComponent(FlatEngine::ComponentTypes::Sprite));

	if (_whiteChecked)
		whiteKingSprite->SetTexture("assets/images/pieces/Checked.png");
	else
		whiteKingSprite->SetTexture("assets/images/pieces/Clear.png");
	if (_blackChecked)
		blackKingSprite->SetTexture("assets/images/pieces/Checked.png");
	else
		blackKingSprite->SetTexture("assets/images/pieces/Clear.png");

	// Set correct piece buttons to active
	if (this->playerTurn == "white" && !this->_whitePiecesActive)
	{
		SetWhiteActive(true);
		SetBlackActive(false);
	}
	else if (this->playerTurn == "black" && !this->_blackPiecesActive)
	{
		SetWhiteActive(false);
		SetBlackActive(true);
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
		// Take the piece
		gameBoard->TakePiece(square, selectedSquare);
		DeactivateAvailableMoves();
		movePieceAudio->Play();
		square->_hasMoved = true;

		if (square->pieceName == "King_W")
			whiteKing = square;
		else if (square->pieceName == "King_B")
			blackKing = square;

		// Change turns
		this->selectedSquare = nullptr;
		if (this->playerTurn == "white")
		{
			CheckForCheck(blackKing, _blackChecked);
			this->playerTurn = "black";
		}
		else
		{
			CheckForCheck(whiteKing, _whiteChecked);
			this->playerTurn = "white";
		}
	}

	if (selectedSquare != nullptr)
	{
		ActivateAvailableMoves();
	}
}

void GameManager::ActivateAvailableMoves()
{
	if (selectedSquare->pieceType == "Pawn_")
		PawnMoves();
	else if (selectedSquare->pieceType == "Rook_")
		RookMoves();
	else if (selectedSquare->pieceType == "Knight_")
		KnightMoves();
	else if (selectedSquare->pieceType == "Bishop_")
		BishopMoves();
	else if (selectedSquare->pieceType == "Queen_")
		QueenMoves();
	else if (selectedSquare->pieceType == "King_")
		KingMoves();

	for (std::shared_ptr<BoardSquare> square : availableMoves)
	{
		// Change the sprite
		std::shared_ptr<FlatEngine::Sprite> sprite = std::static_pointer_cast<FlatEngine::Sprite>(square->GetOwner()->GetComponent(FlatEngine::ComponentTypes::Sprite));
		sprite->SetTexture("assets/images/pieces/CanMoveTo.png");
		// Activate the button
		std::shared_ptr<FlatEngine::Button> button = std::static_pointer_cast<FlatEngine::Button>(square->GetOwner()->GetComponent(FlatEngine::ComponentTypes::Button));
		button->SetActive(true);
	}
}

void GameManager::DeactivateAvailableMoves()
{
	if (this->availableMoves.size() > 0)
	{
		for (std::shared_ptr<BoardSquare> square : this->availableMoves)
		{
			// Change the sprite
			std::shared_ptr<FlatEngine::Sprite> sprite = std::static_pointer_cast<FlatEngine::Sprite>(square->GetOwner()->GetComponent(FlatEngine::ComponentTypes::Sprite));
			sprite->SetTexture("assets/images/pieces/clear.png");

			if (square->pieceColor != playerTurn)
			{
				// Deactivate the button
				std::shared_ptr<FlatEngine::Button> button = std::static_pointer_cast<FlatEngine::Button>(square->GetOwner()->GetComponent(FlatEngine::ComponentTypes::Button));
				button->SetActive(false);
			}
		}
	}
}

void GameManager::CheckForCheck(std::shared_ptr<BoardSquare> king, bool & _kingChecked)
{
	FlatEngine::LogString(whiteKing->pieceName);
	std::vector<std::shared_ptr<BoardSquare>> connectedSquares;
	std::vector<std::shared_ptr<BoardSquare>> availableSquares;
	std::vector<std::shared_ptr<BoardSquare>> boardSquares = gameBoard->GetBoardSquares();

	std::vector<std::shared_ptr<BoardSquare>> squaresInSameRow;
	std::vector<std::shared_ptr<BoardSquare>> squaresInSameCol;
	std::vector<std::shared_ptr<BoardSquare>> squaresInPositiveDiag;
	std::vector<std::shared_ptr<BoardSquare>> squaresInNegativeDiag;

	for (std::shared_ptr<BoardSquare> boardSquare : boardSquares)
	{
		if (boardSquare->row == king->row && boardSquare->column != king->column)
			squaresInSameRow.push_back(boardSquare);
		if (boardSquare->column == king->column)
			squaresInSameCol.push_back(boardSquare);
		if (boardSquare->row != king->row && boardSquare->column != king->column)
		{
			if (boardSquare->row - king->row == boardSquare->column - king->column)
				squaresInPositiveDiag.push_back(boardSquare);
			else if (boardSquare->row - king->row == king->column - boardSquare->column)
				squaresInNegativeDiag.push_back(boardSquare);
		}
	}

	bool _checked = false;

	for (std::shared_ptr<BoardSquare> enemySquare : CheckRow(squaresInSameRow, availableSquares, king->row, king->column))
	{
		if (enemySquare->pieceColor == king->pieceColor && (enemySquare->pieceType == "Rook_" || enemySquare->pieceType == "Queen_"))
		{
			_kingChecked = true;
			_checked = true;
		}
	}

	if (!_checked)
		_kingChecked = false;

	//CheckCol(squaresInSameCol, availableSquares, whiteKing->row, whiteKing->column);
	//CheckPosDiag(squaresInPositiveDiag, availableSquares, whiteKing->row, whiteKing->column);
	//CheckNegDiag(squaresInNegativeDiag, availableSquares, whiteKing->row, whiteKing->column);
}


void GameManager::PawnMoves()
{
	std::vector<std::shared_ptr<BoardSquare>> availableSquares;
	std::vector<std::shared_ptr<BoardSquare>> boardSquares = gameBoard->GetBoardSquares();

	for (std::shared_ptr<BoardSquare> boardSquare : boardSquares)
	{
		if (selectedSquare->pieceColor == "white")
		{
			// Moving
			if (boardSquare->column == selectedSquare->column &&
				(boardSquare->row == selectedSquare->row + 1 || (boardSquare->row == selectedSquare->row + 2 && selectedSquare->_hasMoved == false)) &&
				boardSquare->pieceName == "")
			{
				availableSquares.push_back(boardSquare);
			}
			// For taking enemy piece
			else if ((boardSquare->column == selectedSquare->column + 1 || boardSquare->column == selectedSquare->column - 1) && boardSquare->row == selectedSquare->row + 1 &&
				boardSquare->pieceColor == "black")
			{
				availableSquares.push_back(boardSquare);
			}
		}
		else // if (selectedSquare->pieceColor == "black")
		{
			// Moving
			if (boardSquare->column == selectedSquare->column &&
				(boardSquare->row == selectedSquare->row - 1 || (boardSquare->row == selectedSquare->row - 2 && selectedSquare->_hasMoved == false)) &&
				boardSquare->pieceName == "")
			{
				availableSquares.push_back(boardSquare);
			}
			// For taking enemy piece
			else if ((boardSquare->column == selectedSquare->column + 1 || boardSquare->column == selectedSquare->column - 1) && boardSquare->row == selectedSquare->row - 1 &&
				boardSquare->pieceColor == "white")
			{
				availableSquares.push_back(boardSquare);
			}
		}
	}

	availableMoves = availableSquares;
}

void GameManager::RookMoves()
{
	std::vector<std::shared_ptr<BoardSquare>> availableSquares;
	std::vector<std::shared_ptr<BoardSquare>> boardSquares = gameBoard->GetBoardSquares();

	std::vector<std::shared_ptr<BoardSquare>> squaresInSameRow;
	std::vector<std::shared_ptr<BoardSquare>> squaresInSameCol;


	for (std::shared_ptr<BoardSquare> boardSquare : boardSquares)
	{
		if (boardSquare->row == selectedSquare->row && boardSquare->column != selectedSquare->column)
			squaresInSameRow.push_back(boardSquare);
		else if (boardSquare->column == selectedSquare->column && boardSquare->row != selectedSquare->row)
			squaresInSameCol.push_back(boardSquare);
	}

	CheckRow(squaresInSameRow, availableSquares, selectedSquare->row, selectedSquare->column);
	CheckCol(squaresInSameCol, availableSquares, selectedSquare->row, selectedSquare->column);

	availableMoves = availableSquares;
}

void GameManager::KnightMoves()
{
	std::vector<std::shared_ptr<BoardSquare>> availableSquares;
	std::vector<std::shared_ptr<BoardSquare>> boardSquares = gameBoard->GetBoardSquares();

	std::vector<std::shared_ptr<BoardSquare>> validSquares;

	for (std::shared_ptr<BoardSquare> boardSquare : boardSquares)
	{
		if (((boardSquare->row == selectedSquare->row + 1 && (boardSquare->column == selectedSquare->column - 2 || boardSquare->column == selectedSquare->column + 2)) ||
			(boardSquare->row == selectedSquare->row - 1 && (boardSquare->column == selectedSquare->column - 2 || boardSquare->column == selectedSquare->column + 2)) ||
			(boardSquare->row == selectedSquare->row + 2 && (boardSquare->column == selectedSquare->column - 1 || boardSquare->column == selectedSquare->column + 1)) ||
			(boardSquare->row == selectedSquare->row - 2 && (boardSquare->column == selectedSquare->column - 1 || boardSquare->column == selectedSquare->column + 1))) &&
			boardSquare->pieceColor != playerTurn)
			validSquares.push_back(boardSquare);
	}

	availableMoves = validSquares;
}

void GameManager::BishopMoves()
{
	std::vector<std::shared_ptr<BoardSquare>> availableSquares;
	std::vector<std::shared_ptr<BoardSquare>> boardSquares = gameBoard->GetBoardSquares();

	std::vector<std::shared_ptr<BoardSquare>> squaresInPositiveDiag;
	std::vector<std::shared_ptr<BoardSquare>> squaresInNegativeDiag;


	for (std::shared_ptr<BoardSquare> boardSquare : boardSquares)
	{
		if (boardSquare->row != selectedSquare->row && boardSquare->column != selectedSquare->column)
		{
			if (boardSquare->row - selectedSquare->row == boardSquare->column - selectedSquare->column)
				squaresInPositiveDiag.push_back(boardSquare);
			else if (boardSquare->row - selectedSquare->row == selectedSquare->column - boardSquare->column)
				squaresInNegativeDiag.push_back(boardSquare);
		}
	}

	CheckPosDiag(squaresInPositiveDiag, availableSquares, selectedSquare->row, selectedSquare->column);
	CheckNegDiag(squaresInNegativeDiag, availableSquares, selectedSquare->row, selectedSquare->column);

	availableMoves = availableSquares;
}

void GameManager::QueenMoves()
{
	std::vector<std::shared_ptr<BoardSquare>> availableSquares;
	std::vector<std::shared_ptr<BoardSquare>> boardSquares = gameBoard->GetBoardSquares();

	std::vector<std::shared_ptr<BoardSquare>> squaresInSameRow;
	std::vector<std::shared_ptr<BoardSquare>> squaresInSameCol;
	std::vector<std::shared_ptr<BoardSquare>> squaresInPositiveDiag;
	std::vector<std::shared_ptr<BoardSquare>> squaresInNegativeDiag;

	for (std::shared_ptr<BoardSquare> boardSquare : boardSquares)
	{
		if (boardSquare->row == selectedSquare->row && boardSquare->column != selectedSquare->column)
			squaresInSameRow.push_back(boardSquare);
		if (boardSquare->column == selectedSquare->column)
			squaresInSameCol.push_back(boardSquare);
		if (boardSquare->row != selectedSquare->row && boardSquare->column != selectedSquare->column)
		{
			if (boardSquare->row - selectedSquare->row == boardSquare->column - selectedSquare->column)
				squaresInPositiveDiag.push_back(boardSquare);
			else if (boardSquare->row - selectedSquare->row == selectedSquare->column - boardSquare->column)
				squaresInNegativeDiag.push_back(boardSquare);
		}
	}

	CheckRow(squaresInSameRow, availableSquares, selectedSquare->row, selectedSquare->column);
	CheckCol(squaresInSameCol, availableSquares, selectedSquare->row, selectedSquare->column);
	CheckPosDiag(squaresInPositiveDiag, availableSquares, selectedSquare->row, selectedSquare->column);
	CheckNegDiag(squaresInNegativeDiag, availableSquares, selectedSquare->row, selectedSquare->column);

	availableMoves = availableSquares;
}

void GameManager::KingMoves()
{
	std::vector<std::shared_ptr<BoardSquare>> availableSquares;
	std::vector<std::shared_ptr<BoardSquare>> boardSquares = gameBoard->GetBoardSquares();

	std::vector<std::shared_ptr<BoardSquare>> squaresInSameRow;
	std::vector<std::shared_ptr<BoardSquare>> squaresInSameCol;


	for (std::shared_ptr<BoardSquare> boardSquare : boardSquares)
	{
		if (boardSquare->row == selectedSquare->row && (boardSquare->column == selectedSquare->column - 1 || boardSquare->column == selectedSquare->column + 1) && boardSquare->pieceColor != playerTurn)
			squaresInSameRow.push_back(boardSquare);
		if (boardSquare->column == selectedSquare->column && (boardSquare->row == selectedSquare->row - 1 || boardSquare->row == selectedSquare->row + 1) && boardSquare->pieceColor != playerTurn)
			squaresInSameCol.push_back(boardSquare);
	}

	CheckRow(squaresInSameRow, availableSquares, selectedSquare->row, selectedSquare->column);
	CheckCol(squaresInSameCol, availableSquares, selectedSquare->row, selectedSquare->column);

	availableMoves = availableSquares;
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