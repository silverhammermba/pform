#ifndef BLOCK_H_
#define BLOCK_H_

class Block : public Pform::StaticEntity
{
	sf::Sprite sprite;
public:
	Block(sf::Texture t, bool s = true);

	void draw_on(sf::RenderWindow& window) { window.draw(sprite); }
};

#endif
