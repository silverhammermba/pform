#include "game.hpp"

Block::Block(const sf::Texture& t, bool s)
	: Pform::StaticEntity(s), sprite(t)
{}

void Block::draw_on(sf::RenderWindow& window, unsigned int x, unsigned int y)
{
	sprite.setPosition(x, y);
	window.draw(sprite);
}
