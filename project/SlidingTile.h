#pragma once
#include <raylib.h>
#include <raymath.h>
#include <string>

#define TILE_BLEED 10

class SlidingTile
{
	Vector2 currentPos;
	Vector2 targetPos;
	Vector2 size;
	std::string name;
	Color color;

	int index;

public:

	SlidingTile(Vector2 startPos, Vector2 size, std::string name, Color color, int index);
	Rectangle GetTileRect();

	void Update();
	void Draw();

	void SetPosition(Vector2 pos);
	int GetIndex();
};

class SlidingTileHolder
{
	SlidingTile* currentTile = nullptr;
	Vector2 position;
	int index;

public: 
	
	SlidingTileHolder();
	SlidingTileHolder(Vector2 pos, int index);
	SlidingTileHolder(Vector2 pos, Vector2 size, std::string name, Color color, int index);

	~SlidingTileHolder();

	bool GetCorrect();

	void ExchangeTileContents(SlidingTileHolder* other);
	SlidingTile* GetSlidingTile();
};