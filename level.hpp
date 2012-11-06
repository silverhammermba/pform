#ifndef LEVEL_H_
#define LEVEL_H_

#include <vector>
#include "block.hpp"

class Level
{
	unsigned int height;
	unsigned int width;
	std::vector<Block> grid;
public:
	Level(int y, int x);
};

#endif
