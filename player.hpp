#ifndef PLAYER_H_
#define PLAYER_H_

class Player : public Pform::DynamicEntity
{
	sf::Sprite sprite;

	double jump_speed;
public:
	Player(const sf::Texture& texture, double j, const Pform::Level& l, int x, int y, double tvx, double tvy, double accx, double accy, double brk);
	void jump();
	void set_movement(int direction);
	void step(float seconds);
	void draw_on(sf::RenderWindow& window) { window.draw(sprite); }
};

#endif
