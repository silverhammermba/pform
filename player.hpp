#ifndef PLAYER_H_
#define PLAYER_H_

class Player : public Pform::DynamicEntity, public InputReader
{
	sf::Sprite sprite;
	std::map<std::string, Animation> animation;
	unsigned int joystick; // joystick number
	double jump_speed;
	bool keyboard;
	float axis[2]; // joystick position
	bool keys[2];
public:
	Player(unsigned int joy, bool kbd, const sf::Texture& texture, const std::string& animation_filename, double j, World& l, double tvx, double tvy, double accx, double accy, double brk);
	virtual bool process_event(const sf::Event& event);
	sf::Vector2f get_sprite_position() const { return sprite.getPosition(); }
	sf::FloatRect get_sprite_bounds() const { return sprite.getGlobalBounds(); }
	unsigned int get_joystick() const { return joystick; }
	void jump();
	void step(float seconds);
	void damage();
	void draw_on(sf::RenderWindow& window) { window.draw(sprite); }
};

#endif
