#include "game.hpp"

Player::Player(const sf::Texture& texture, double j, const Pform::Level& l, int x, int y, double tvx, double tvy, double accx, double accy, double brk)
 : DynamicEntity(l, x, y, tvx, tvy, accx, accy, brk), sprite(texture)
{
	jump_speed = j * PPB;
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
	const double* pos = get_position();
	sprite.setPosition(std::round(pos[0]), std::round(pos[1]));
}
