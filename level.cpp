#include "pform.hpp"

Level::Level(int y, int x) : grid(y * x)
{
	height = y;
	width = x;

	for (unsigned int h = 0; h < height; h++)
	{
		for (unsigned int w = 0; w < width; w++)
		{
			if (h == 0 || h == height - 1 || w == 0 || w == width - 1)
				set(h, w, new Block());
			else
				set(h, w, nullptr);
		}
	}
}
