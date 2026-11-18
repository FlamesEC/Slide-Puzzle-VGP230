#pragma once

#include <vector>
#include <algorithm>
#include <iostream>

#include "SlidingTile.h"

#define WINDOW_SIZE_X 800
#define WINDOW_SIZE_Y 800

#define GRID_BLEED 100
#define TIMER_BLEED 30

#define MENU_BLEED_X 200
#define MENU_BLEED_Y 280

#define MINIMUM_SIDE_SIZE 3
#define MAXIMUM_SIDE_SIZE 8

#define TILE_DEFAULT_FONT_SIZE 50
#define TILE_INCREMENT_FONT_SIZE 6

enum GAME_STATES
{
	MENU,
	GAME
};

class SlidingManager
{
	std::vector<std::vector<SlidingTileHolder*>> grid;
	float secondsInRound = 0;
	bool running = false;

	int tileSideCount = 4;
	GAME_STATES gameState = MENU;

	void Start();
	void Update();

public:

	void Main();

	SlidingTileHolder* GetEmptyNeighboringTile(int x, int y);
	void Reset();
	void Shuffle();

	int CountInversions(int x, int y);
	int SumInversions();
	
	bool IsSolvable();
	Vector2 GetEmptyPos();
	bool GetSolved();

	void DrawTimer();
};