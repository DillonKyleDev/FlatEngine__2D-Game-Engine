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


private:
	std::string name;
	int level;
	std::string spritePath;
	std::shared_ptr<FlatEngine::GameObject> pieceObject;
};

