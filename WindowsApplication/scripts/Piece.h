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
	void SetName(std::string name);
	std::string GetName();
	std::shared_ptr<FlatEngine::GameObject> GetPieceObject();
	void PlaceOnSquare(std::shared_ptr<FlatEngine::GameObject> boardLocation);


private:
	std::string name;
	std::string spritePath;
	std::shared_ptr<FlatEngine::GameObject> pieceObject;
	std::shared_ptr<FlatEngine::Transform> pieceTransform;
	float spriteOffsetY;
};

