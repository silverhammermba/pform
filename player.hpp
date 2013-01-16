#ifndef PLAYER_H_
#define PLAYER_H_

class Player : public Pform::DynamicEntity, public InputReader
{
	std::map<std::string, Animation> animation;
	sf::Sprite sprite;
	sf::Vector2i shift;
	std::string action;
	int flip;
	float animation_timer;
	unsigned int joystick; // joystick number
	double jump_speed;
	bool keyboard;
	float axis[2]; // joystick position
	bool keys[2];
public:
	Player(unsigned int joy, bool kbd, const sf::Texture& texture, const std::string& animation_filename, double j, World& l, double w, double h, double tvx, double tvy, double accx, double accy, double brk);
	virtual bool process_event(const sf::Event& event);
	sf::Vector2f get_sprite_position() const;
	sf::FloatRect get_sprite_bounds() const { return sprite.getGlobalBounds(); }
	unsigned int get_joystick() const { return joystick; }
	void jump();
	void step(float seconds);
	void damage();
	void draw_on(sf::RenderWindow& window) { window.draw(sprite); }
};

#endif
