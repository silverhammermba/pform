#include "game.hpp"
#include <iostream>

#define CLAMP(l, x, u) ((x) < (l) ? (l) : ((x) > (u) ? (u) : (x)))
#define SIGN(x) ((x) > 0 ? 1 : ((x) < 0 ? -1 : 0))

Player::Player(unsigned int joy, const sf::Texture& texture, double j, World& l, double tvx, double tvy, double accx, double accy, double brk)
 : DynamicEntity(l, tvx, tvy, accx, accy, brk), sprite(texture), axis {0, 0}
{
	std::cerr << "New joystick: " << joy << "\n";
	joystick = joy;
	jump_speed = j;
	auto pos = l.get_next_start();
	set_position(pos[0], pos[1]);
	update_relevant_region();
}

bool Player::process_event(const sf::Event& event)
{
	if (event.type == sf::Event::JoystickButtonPressed && event.joystickButton.joystickId == joystick)
	{
		if (event.joystickButton.button == 0)
			jump();
	}
	else if (event.type == sf::Event::JoystickMoved && event.joystickMove.joystickId == joystick)
	{
		if (event.joystickMove.axis < 2)
			axis[event.joystickMove.axis] = event.joystickMove.position;
	}

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
	auto color = sprite.getColor();
	sprite.setColor(sf::Color(CLAMP(0, color.r + 200 * seconds, 255), CLAMP(0, color.g + 200 * seconds, 255), CLAMP(0, color.b + 200 * seconds, 255)));

	// TODO update impulse, acceleration from axis
	if (axis[0] > 20 || axis[0] < -20)
		impulse[0] = SIGN(axis[0]);
	else
		impulse[0] = 0;

	DynamicEntity::step(seconds);

	// TODO would like to somehow round position to pixel coordinates here, if possible

	const double* pos = get_position();

	sprite.setPosition(std::round(pos[0] * PPB), std::round(pos[1] * PPB));
}

void Player::damage()
{
	sprite.setColor(sf::Color(255, 0, 0));
}
