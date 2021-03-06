#include "game.hpp"
#include <iostream>

#define CLAMP(l, x, u) ((x) < (l) ? (l) : ((x) > (u) ? (u) : (x)))
#define SIGN(x) ((x) > 0 ? 1 : ((x) < 0 ? -1 : 0))

Player::Player(unsigned int joy, bool kbd, const sf::Texture& texture, const std::string& animation_filename, double j, World& l, double w, double h, double tvx, double tvy, double accx, double accy, double brk)
 : DynamicEntity(l, w, h, tvx, tvy, accx, accy, brk), animation(), sprite(texture), shift(), action("idle"), axis {0, 0}, keys {false, false}
{
	YAML::Node anim_node = YAML::LoadFile(animation_filename);

	for (auto anim = anim_node.begin(); anim != anim_node.end(); anim++)
		animation[anim->first.as<std::string>()] = anim->second.as<Animation>();

	flip = 1;
	animation_timer = 0;
	auto frame = animation[action].get_frame(0);
	shift = frame.shift;
	sprite.setTextureRect(frame.position);

	keyboard = kbd;
	joystick = joy;
	jump_speed = j;
	auto pos = l.get_next_start();
	set_position(pos[0], pos[1]);
	update_relevant_region();
}

bool Player::process_event(const sf::Event& event)
{
	if (keyboard)
	{
		switch(event.type)
		{
			case sf::Event::KeyPressed:
				switch (event.key.code)
				{
					case sf::Keyboard::A:
						keys[0] = true;
						break;
					case sf::Keyboard::D:
						keys[1] = true;
						break;
					case sf::Keyboard::W:
						jump();
						break;
					default:
						break;
				}
				break;
			case sf::Event::KeyReleased:
				switch (event.key.code)
				{
					case sf::Keyboard::A:
						keys[0] = false;
						break;
					case sf::Keyboard::D:
						keys[1] = false;
						break;
					default:
						break;
				}
				break;
			default:
				break;
		}
	}
	else
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
	animation_timer += seconds;

	auto color = sprite.getColor();
	sprite.setColor(sf::Color(CLAMP(0, color.r + 200 * seconds, 255), CLAMP(0, color.g + 200 * seconds, 255), CLAMP(0, color.b + 200 * seconds, 255)));

	if (keyboard)
	{
		// TODO this is a kinda naive way to handle this
		action = "run";
		if (keys[0])
			impulse[0] = -1;
		else if (keys[1])
			impulse[0] = 1;
		else
		{
			impulse[0] = 0;
			action = "idle";
		}
	}
	else
	{
		if (std::fabs(axis[0]) > 20)
		{
			impulse[0] = CLAMP(-80, impulse[0], 80) - 20;
			impulse[0] = axis[0] / 60;
			action = "run";
		}
		else
		{
			impulse[0] = 0;
			action = "idle";
		}
	}

	DynamicEntity::step(seconds);

	// if standing and not moving and close to a block position
	if (standing && velocity[0] == 0 && velocity[1] == 0 && std::fabs(position[0] * PPB - std::round(position[0]) * PPB) < 1)
	{
		// round to nearest block coords
		position[0] = std::round(position[0]);
		update_relevant_region();
		// TODO hacky way to make player fall
		if (level->is_passable(get_limit(1, 0), get_limit(1, 1) + 1))
			standing = false;
	}

	if (!standing)
		action = "jump";

	if (velocity[0] > 0)
		flip = 1;
	else if (velocity[0] < 0)
		flip = -1;

	auto frame = animation[action].get_frame(animation_timer);
	shift = frame.shift;
	sprite.setTextureRect(frame.position);

	sprite.setScale(flip, 1);
	sprite.setPosition(std::round(position[0] * PPB) + shift.x + (flip < 0 ? PPB : 0), std::round(position[1] * PPB) + shift.y);
}

sf::Vector2f Player::get_sprite_position() const
{
	sf::Vector2f pos = sprite.getPosition();
	pos.x += flip < 0 ? -PPB : 0;

	return pos;
}

void Player::damage()
{
	sprite.setColor(sf::Color(255, 0, 0));
}
