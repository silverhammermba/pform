#ifndef BLOCK_H_
#define BLOCK_H_

#include "pform.hpp"

class Block : public StaticEntity
{
	sf::Sprite sprite;
public:
	Block(const sf::Texture & texture);
};

#endif
