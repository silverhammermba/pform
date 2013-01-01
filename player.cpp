#include "game.hpp"

Player::Player(const sf::Texture& texture, double j, World& l, double tvx, double tvy, double accx, double accy, double brk)
 : DynamicEntity(l, tvx, tvy, accx, accy, brk), sprite(texture)
{
	jump_speed = j;
	auto pos = l.get_next_start();
	set_position(pos[0], pos[1]);
	update_relevant_region();
}

void Player::jump()
{
	if (standing)
	{
		// TODO set jump power somewhere else
		velocity[1] = -jump_speed;
		standing = false;
	}
}

void Player::set_movement(int direction)
{
	impulse[0] = direction;
}

void Player::step(float seconds)
{
	DynamicEntity::step(seconds);

	// TODO would like to somehow round position to pixel coordinates here, if possible

	const double* pos = get_position();

	sprite.setPosition(std::round(pos[0] * PPB), std::round(pos[1] * PPB));
}
