#include "game.hpp"

Block::Block(sf::Texture t, bool s)
	: Pform::StaticEntity(s), sprite(t)
{}
