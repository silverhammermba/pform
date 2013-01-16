#include "pform.hpp"
#include <iostream>

Pform::Level::Level() : grid()
{
}

Pform::Level::Level(unsigned int w, unsigned int h) : grid(w * h, nullptr)
{
	size[0] = w;
	size[1] = h;
}

void Pform::Level::set_size(unsigned int w, unsigned int h)
{
	size[0] = w;
	size[1] = h;
}

Pform::StaticEntity* Pform::Level::get(int x, int y) const
{
	if (x < 0)
		x += size[0];
	if (y < 0)
		y += size[1];

	if (x < 0 || y < 0 || x >= (int)size[0] || y >= (int)size[1])
		throw LevelException();

	return grid[x + size[0] * y];
}

void Pform::Level::set(int x, int y, Pform::StaticEntity* entity)
{
	if (x < 0)
		x += size[0];
	if (y < 0)
		y += size[1];

	if (x < 0 || y < 0 || x >= (int)size[0] || y >= (int)size[1])
		throw LevelException();

	grid[x + size[0] * y] = entity;
}

bool Pform::Level::is_passable(int x, int y) const
{
	StaticEntity* ent = get(x, y);
	if (ent != nullptr && ent->is_solid())
		return false;
	return true;
}
