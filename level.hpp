#ifndef LEVEL_H_
#define LEVEL_H_

class Level
{
	unsigned int height;
	unsigned int width;
	std::vector<Block*> grid;
public:
	Level(int y, int x);

	const inline Block* at(int y, int x) const { return grid[y * width + x]; };
	inline void set(int y, int x, Block* block) { grid[y * width + x] = block; };
};

#endif
