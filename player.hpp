#ifndef PLAYER_H_
#define PLAYER_H_

class Player : public Pform::DynamicEntity, public InputReader
{
	sf::Sprite sprite;
	unsigned int joystick;
	double jump_speed;
public:
	Player(unsigned int joy, const sf::Texture& texture, double j, World& l, double tvx, double tvy, double accx, double accy, double brk);
	virtual bool process_event(const sf::Event& event);
	sf::Vector2f get_sprite_position() const { return sprite.getPosition(); };
	void jump();
	void step(float seconds);
	void draw_on(sf::RenderWindow& window) { window.draw(sprite); }
};

#endif
