#include "level.hpp"

Level::Level(int y, int x) : grid((y + 2) * (x + 2))
{
	height = y;
	width = x;
}
