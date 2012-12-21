#ifndef BLOCK_H_
#define BLOCK_H_

class Block : public Pform::StaticEntity
{
	sf::Sprite sprite;
public:
	Block(sf::Texture t, bool s = true);

	const sf::Sprite& get_sprite() const { return sprite; }
	void draw_on(sf::RenderWindow& window) const { window.draw(sprite); }
};

#endif
