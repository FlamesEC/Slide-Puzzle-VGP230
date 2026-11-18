#include "SlidingManager.h"

void SlidingManager::Start()
{
	if (gameState == GAME)
		Reset();
}

void SlidingManager::Update()
{
	std::string lengthText = "";

	switch (gameState)
	{
		case MENU:

			if (IsKeyPressed(KEY_LEFT)) tileSideCount--;
			if (IsKeyPressed(KEY_RIGHT)) tileSideCount++;

			tileSideCount = Clamp(tileSideCount, MINIMUM_SIDE_SIZE, MAXIMUM_SIDE_SIZE);

			DrawText("Block Puzzle", WINDOW_SIZE_X / 2 - MeasureText("Block Puzzle", 80) / 2, MENU_BLEED_Y, 80, RAYWHITE);
			DrawText("LENGTH: ", MENU_BLEED_X, WINDOW_SIZE_Y - MENU_BLEED_Y - 40, 40, RAYWHITE);

			if (tileSideCount > MINIMUM_SIDE_SIZE) lengthText += "<-  "; else lengthText += "    ";
			lengthText += std::to_string(tileSideCount);
			if (tileSideCount < MAXIMUM_SIDE_SIZE) lengthText += "  ->"; else lengthText += "    ";

			DrawText(lengthText.c_str(), WINDOW_SIZE_X - MENU_BLEED_X - MeasureText(lengthText.c_str(), 40), WINDOW_SIZE_Y - MENU_BLEED_Y - 40, 40, RAYWHITE);

			if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
			{
				Reset();
				gameState = GAME;
			}

		break;

		case GAME:

			for (int y = 0; y < tileSideCount; y++)
			{
				for (int x = 0; x < tileSideCount; x++)
				{
					SlidingTile* tile = grid[x][y]->GetSlidingTile();
					if (tile == nullptr)
						continue;

					tile->Update();
					tile->Draw();
				}
			}

			DrawTimer();

			if (!running)
			{
				if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
					gameState = MENU;

				return;
			}

			secondsInRound += GetFrameTime();

			if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
			{
				for (int x = 0; x < tileSideCount; x++)
				{
					for (int y = 0; y < tileSideCount; y++)
					{
						if (grid[x][y]->GetSlidingTile() == nullptr)
							continue;

						if (!CheckCollisionPointRec(GetMousePosition(), grid[x][y]->GetSlidingTile()->GetTileRect()))
							continue;

						SlidingTileHolder* holder = GetEmptyNeighboringTile(x, y);
						if (holder != nullptr)
							grid[x][y]->ExchangeTileContents(holder);

						break;
					}
				}
			}

			if (GetSolved())
				running = false;

		break;
	}
}

void SlidingManager::Main()
{
	InitWindow(WINDOW_SIZE_X, WINDOW_SIZE_Y, "Sliding Puzzle");
	SetTargetFPS(60);

	Start();

	while (!WindowShouldClose())
	{
		BeginDrawing();
		ClearBackground(BLACK);

		Update();

		EndDrawing();
	}
}

SlidingTileHolder* SlidingManager::GetEmptyNeighboringTile(int x, int y)
{
	//std::cout << "A = " << std::max(x - 1, 0) << " -> " << std::min(tileSideCount - 1, x + 1) << "\n";
	//std::cout << "B = " << std::max(y - 1, 0) << " -> " << std::min(tileSideCount - 1, y + 1) << "\n";

	for (int a = std::max(x - 1, 0); a <= std::min(tileSideCount - 1, x + 1); a++)
	{
		for (int b = std::max(y - 1, 0); b <= std::min(tileSideCount - 1, y + 1); b++)
		{
			if (a == x && b == y) continue;
			if (a != x && b != y) continue;

			if (grid[a][b]->GetSlidingTile() != nullptr)
				continue;

			return grid[a][b];
		}
	}

	return nullptr;
}

void SlidingManager::Reset()
{
	float tileWidth = (WINDOW_SIZE_X - (GRID_BLEED * 2)) / ((float)tileSideCount);
	float tileHeight = (WINDOW_SIZE_X - (GRID_BLEED * 2)) / ((float)tileSideCount);

	if (grid.size() > 0)
	{
		for (int i = grid.size() - 1; i >= 0; i--)
		{
			for (int j = 0; j < grid[i].size(); j++)
				delete grid[i][j];

			grid.erase(grid.begin() + i);
		}

		grid.clear();
	}

	for (int i = 0; i < tileSideCount; i++)
	{
		grid.push_back(std::vector<SlidingTileHolder*>());

		for (int j = 0; j < tileSideCount; j++)
			grid[i].push_back(nullptr);
	}

	for (int y = 0; y < tileSideCount; y++)
	{
		for (int x = 0; x < tileSideCount; x++)
		{
			int index = x + (y * tileSideCount);

			if (index < (tileSideCount * tileSideCount) - 1)
				grid[x][y] = new SlidingTileHolder
				(
					{ GRID_BLEED + tileWidth * x, GRID_BLEED + tileWidth * y + TIMER_BLEED },
					{ tileWidth, tileHeight },
					std::to_string(index + 1),
					ORANGE,
					index,
					TILE_DEFAULT_FONT_SIZE + ((4 - tileSideCount) * TILE_INCREMENT_FONT_SIZE)
				);
			else
				grid[x][y] = new SlidingTileHolder({ GRID_BLEED + tileWidth * x, GRID_BLEED + tileWidth * y + TIMER_BLEED }, -1);
		}
	}

	Shuffle();
}

void SlidingManager::Shuffle()
{
	int a = tileSideCount * tileSideCount - 1;

	while (a > 0)
	{
		int b = floor(GetRandomValue(0, a));

		SlidingTileHolder* holderA = grid[a % tileSideCount][floor(a / (float)tileSideCount)];
		SlidingTileHolder* holderB = grid[b % tileSideCount][floor(b / (float)tileSideCount)];

		holderA->ExchangeTileContents(holderB, true);

		a--;
	}

	if (!IsSolvable())
	{
		Vector2 emptyPos = GetEmptyPos();

		if (emptyPos.y == 0 && emptyPos.x <= 1)
			grid[tileSideCount - 2][tileSideCount - 1]->ExchangeTileContents(grid[tileSideCount - 1][tileSideCount - 1], true);
		else
			grid[0][0]->ExchangeTileContents(grid[1][0], true);
	}

	secondsInRound = 0;
	running = true;
}

int SlidingManager::CountInversions(int x, int y)
{
	int inversions = 0;
	int tileIndex = y * tileSideCount + x;
	
	int lastTile = tileSideCount * tileSideCount;
	int tileValue = lastTile - 1;
	if (grid[x][y]->GetSlidingTile() != nullptr)
		tileValue = grid[x][y]->GetSlidingTile()->GetIndex();

	for (int q = tileIndex + 1; q < lastTile; q++)
	{
		int a = q % tileSideCount;
		int b = floor(q / (float)tileSideCount);

		int compVal = lastTile - 1;
		if (grid[a][b]->GetSlidingTile() != nullptr)
			compVal = grid[a][b]->GetSlidingTile()->GetIndex();

		if (tileValue > compVal && tileValue != (lastTile - 1))
			inversions++;
	}

	return inversions;
}

int SlidingManager::SumInversions()
{
	int sum = 0;

	for (int y = 0; y < tileSideCount; y++)
		for (int x = 0; x < tileSideCount; x++)
			sum += CountInversions(x, y);

	return sum;
}

bool SlidingManager::IsSolvable()
{
	if (tileSideCount % 2 == 1)
		return SumInversions() % 2 == 0;

	return (SumInversions() + tileSideCount - (int)round(GetEmptyPos().y + 1)) % 2 == 0;
}

Vector2 SlidingManager::GetEmptyPos()
{
	for (int y = 0; y < tileSideCount; y++)
		for (int x = 0; x < tileSideCount; x++)
			if (grid[x][y]->GetSlidingTile() == nullptr)
				return { (float)x, (float)y };

	return {0, 0};
}

bool SlidingManager::GetSolved()
{
	for (int y = 0; y < tileSideCount; y++)
		for (int x = 0; x < tileSideCount; x++)
			if (!grid[x][y]->GetCorrect())
				return false;

	return true;
}

void SlidingManager::DrawTimer()
{
	std::string minutes = std::to_string((int)floor(secondsInRound / 60.0f));
	if (minutes.length() == 1)
		minutes = '0' + minutes;

	std::string seconds = std::to_string((int)floor(secondsInRound) % 60);
	if (seconds.length() == 1)
		seconds = '0' + seconds;

	std::string millis = std::to_string((int)((secondsInRound - floor(secondsInRound)) * 100));
	if (millis.length() == 1)
		millis = '0' + millis;

	std::string timerText = minutes + ":" + seconds + ":" + millis;
	DrawText(timerText.c_str(), (WINDOW_SIZE_X / 2) - (MeasureText(timerText.c_str(), 50) / 2), (TIMER_BLEED * 2 + GRID_BLEED) / 2 - 25, 50, WHITE);
}
