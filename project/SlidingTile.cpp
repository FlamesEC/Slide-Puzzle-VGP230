#include "SlidingTile.h"

SlidingTile::SlidingTile(Vector2 startPos, Vector2 size, std::string name, Color color, int index)
{
	this->currentPos = startPos;
	this->targetPos = startPos;
	this->size = size;
	this->name = name;
	this->color = color;
	this->index = index;
}

Rectangle SlidingTile::GetTileRect()
{
	return Rectangle{targetPos.x, targetPos.y, size.x, size.y};
}

void SlidingTile::Update()
{
	if (currentPos == targetPos)
		return;

	currentPos = Vector2Lerp(currentPos, targetPos, 0.5f);
	
	if (Vector2Distance(currentPos, targetPos) < 0.01f)
		currentPos = targetPos;
}

void SlidingTile::Draw()
{
	DrawRectangleRounded(Rectangle{ currentPos.x + TILE_BLEED, currentPos.y + TILE_BLEED, size.x - TILE_BLEED * 2, size.y - TILE_BLEED * 2 }, 0.2f, 6, color);
	DrawText(name.c_str(), currentPos.x + (size.x / 2.0f) - (MeasureText(name.c_str(), 50) / 2), currentPos.y + (size.y / 2.0f) - 25, 50, BLACK);
}

void SlidingTile::SetPosition(Vector2 pos)
{
	targetPos = pos;
}

int SlidingTile::GetIndex()
{
	return index;
}

SlidingTileHolder::SlidingTileHolder()
{
	currentTile = nullptr;
	position = { 0,0 };
	index = -1;
}

SlidingTileHolder::SlidingTileHolder(Vector2 pos, int index)
{
	currentTile = nullptr;
	position = pos;
	this->index = index;
}

SlidingTileHolder::SlidingTileHolder(Vector2 pos, Vector2 size, std::string name, Color color, int index)
{
	currentTile = new SlidingTile(pos, size, name, color, index);
	position = pos;
	this->index = index;
}

SlidingTileHolder::~SlidingTileHolder()
{
	if (currentTile != nullptr)
		delete currentTile;
}

bool SlidingTileHolder::GetCorrect()
{
	if (index == -1)
		return currentTile == nullptr;

	return currentTile != nullptr && currentTile->GetIndex() == index;
}

void SlidingTileHolder::ExchangeTileContents(SlidingTileHolder* other)
{
	if (currentTile != nullptr)
		currentTile->SetPosition(other->position);
	if (other->currentTile != nullptr)
		other->currentTile->SetPosition(position);

	SlidingTile* self = currentTile;
	currentTile = other->currentTile;
	other->currentTile = self;
}

SlidingTile* SlidingTileHolder::GetSlidingTile()
{
	return currentTile;
}
