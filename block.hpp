#ifndef BLOCK_H_
#define BLOCK_H_

class Block : public Pform::StaticEntity
{
	sf::Sprite sprite;
public:
	Block(const sf::Texture& texture, const sf::IntRect & rect, bool s = true);

	const sf::Sprite& get_sprite() const { return sprite; }
	void draw_on(sf::RenderWindow& window, unsigned int x, unsigned int y);
};

#endif
