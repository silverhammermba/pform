#include "pform.hpp"

Pform::Level::Level(unsigned int w, unsigned int h) : grid(w * h, nullptr)
{
	width = w;
	height = h;
}

Pform::StaticEntity* Pform::Level::get(int x, int y) const
{
	if (x < 0)
		x += width;
	if (y < 0)
		y += height;

	if (x < 0 || y < 0 || x >= (int)width || y >= (int)height)
		throw LevelException();

	return grid[x + width * y];
}

void Pform::Level::set(int x, int y, Pform::StaticEntity* entity)
{
	if (x < 0)
		x += width;
	if (y < 0)
		y += height;

	if (x < 0 || y < 0 || x >= (int)width || y >= (int)height)
		throw LevelException();

	grid[x + width * y] = entity;
}

bool Pform::Level::is_passable(int x, int y) const
{
	StaticEntity* ent = get(x, y);
	if (ent != nullptr && ent->is_solid())
		return false;
	return true;
}
