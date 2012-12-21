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
		throw Pform::LevelException();

	return grid[x + width * y];
}

void Pform::Level::set(int x, int y, Pform::StaticEntity* entity)
{
	if (x < 0)
		x += width;
	if (y < 0)
		y += height;

	if (x < 0 || y < 0 || x >= (int)width || y >= (int)height)
		throw Pform::LevelException();

	grid[x + width * y] = entity;
}

Pform::LevelException::LevelException() : std::runtime_error("Invalid grid index") {}
