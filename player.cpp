#include "game.hpp"

Player::Player(const sf::Texture& texture, double j, World& l, double tvx, double tvy, double accx, double accy, double brk)
 : DynamicEntity(l, tvx, tvy, accx, accy, brk), sprite(texture)
{
	jump_speed = j;
	auto pos = l.get_next_start();
	set_position(pos[0], pos[1]);
	update_relevant_region();
}

bool Player::process_event(const sf::Event& event)
{
	switch(event.type)
	{
		case sf::Event::KeyPressed:
			switch (event.key.code)
			{
				case sf::Keyboard::Left:
					impulse[0] = -1;
					break;
				case sf::Keyboard::Right:
					impulse[0] = 1;
					break;
				case sf::Keyboard::Up:
					jump();
					break;
				default:
					break;
			}
			break;
		case sf::Event::KeyReleased:
			switch (event.key.code)
			{
				case sf::Keyboard::Left:
				case sf::Keyboard::Right:
					impulse[0] = 0;
					break;
				default:
					break;
			}
			break;
		default:
			break;
	}

	return true;
}

void Player::jump()
{
	if (standing)
	{
		velocity[1] = -jump_speed;
		standing = false;
	}
}

void Player::step(float seconds)
{
	DynamicEntity::step(seconds);

	// TODO would like to somehow round position to pixel coordinates here, if possible

	const double* pos = get_position();

	sprite.setPosition(std::round(pos[0] * PPB), std::round(pos[1] * PPB));
}
