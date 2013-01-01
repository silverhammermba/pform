#include "game.hpp"

Block::Block(const sf::Texture& texture, const sf::IntRect & rect, bool s)
	: Pform::StaticEntity(s), sprite(texture, rect)
{}

void Block::draw_on(sf::RenderWindow& window, unsigned int x, unsigned int y)
{
	sprite.setPosition(x, y);
	window.draw(sprite);
}
