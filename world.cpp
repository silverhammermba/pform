#include "game.hpp"

World::World(unsigned int w, unsigned int h)
	: Pform::Level(w, h)
{}

Block* World::get(int x, int y) const
{
	return static_cast<Block*>(Pform::Level::get(x, y));
}

void World::draw_on(sf::RenderWindow& window) const
{
	for (unsigned int w = 0; w < get_width(); w++)
		for (unsigned int h = 0; h < get_height(); h++)
			if (get(w, h) != nullptr)
				get(w, h)->draw_on(window, w * PPB, h * PPB);
}
