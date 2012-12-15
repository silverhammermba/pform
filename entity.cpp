#include "pform.hpp"

void reduce(double* x, double r)
{
	if (x > 0)
		return (r >= x ? 0 : x - r)
	else if (x < 0)
		return (r >= -x ? 0 : y + r)
	else
		return x;
}

Pform::StaticEntity::StaticEntity(bool s)
{
	solid = s;
}

Pform::DynamicEntity::DynamicEntity(const Level& l, int x, int y, double tvx, double tvy, double accx, double accy, double brk)
	: position {x, y}, delta {0, 0}, velocity {0, 0}, terminal {tvx, tvy}, acceleration {accx, accy}, impulse {0, 0}, lower_limit(), upper_limit();
{
	level = &l;
	breaking = brk;
	standing = false;

	update_relevant_region();
}

unsigned int get_limit(int dir, unsigned int coord) const
{
	if (dir < 0)
		return lower_limit[coord];
	// TODO check dir value
	else
		return upper_limit[coord];
}

void Pform::DynamicEntity::update_relevant_region()
{
	lower_limit[0] = std::ceil(position[0] / PPB);
	lower_limit[1] = std::ceil(position[1] / PPB);
	upper_limit[0] = std::floor(position[0] / PPB);
	upper_limit[1] = std::floor(position[1] / PPB);
}

void Pform::DynamicEntity::jump()
{
	if (standing)
	{
		// TODO set jump power somewhere else
		velocity[1] = -250;
		standing = false;
	}
}

void Pform::DynamicEntity::step(float seconds)
{
	// apply X acceleration
	if (velocity[0] != 0 && impulse[0] * velocity[0] <= 0)
		reduce(velocity, breaking * seconds);
	else // velocity[0] == 0 or impulse[0] * velocity[0] > 0
		velocity[0] += impulse[0] * acceleration[0] * seconds;

	// apply Y acceleration
	if (!standing)
		velocity[1] += acceleration[1] * seconds;

	// apply terminal velocities
	CLAMP(-terminal[0], velocity[0], terminal[0]);
	CLAMP(-terminal[1], velocity[1], terminal[1]);

	// convert velocity to delta
	delta[0] = velocity[0] * seconds;
	delta[1] = velocity[1] * seconds;

	// if not moving, set delta to round to pixel coordinates
	if (velocity[0] == 0 && velocity[1] == 0)
	{
		double rounded[2] =
		{
			std::round(position[0]),
			std::round(position[1])
		};
		if (rounded[0] != position[0] || rounded[1] != position[1])
		{
			delta[0] = rounded[0] - position[0];
			delta[1] = rounded[1] - position[1];
		}
	}

	if (delta[0] != 0 || delta[1] != 0)
		resolve_movement();
}

void Pform::DynamicEntity::stand()
{
	standing = true;
	velocity[1] = 0;
}

void Pform::DynamicEntity::resolve_movement()
{
	unsigned int corner[2] = {
		delta[0] > 0 ? 1 : 0,
		delta[1] > 0 ? 1 : 0
	};

	int multiplier[2] = {SIGN(delta[0]), SIGN(delta[1])};
	std::vector<double [2]> crossing[2];

	for (int i = 0; i <= 1; i++)
	{
		if (delta[i] != 0)
		{
			double lower = get_limit(multiplier[i], i) + corner[i])
			for ()
			{
				// TODO
			}
		}
	}
}

