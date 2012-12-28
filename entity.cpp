#include "pform.hpp"

void reduce(double* x, double r)
{
	if (*x > 0)
		*x = (r >= *x ? 0 : *x - r);
	else if (*x < 0)
		*x = (r >= -*x ? 0 : *x + r);
}

Pform::StaticEntity::StaticEntity(bool s)
{
	solid = s;
}

// TODO make all of this PPB-independet?
Pform::DynamicEntity::DynamicEntity(const Level& l, int x, int y, double tvx, double tvy, double accx, double accy, double brk)
	: position {(double)(x * PPB), (double)(y * PPB)}, delta {0, 0}, terminal {tvx * PPB, tvy * PPB}, acceleration {accx * PPB, accy * PPB}, lower_limit(), upper_limit(), velocity {0, 0}, impulse {0, 0}
{
	level = &l;
	breaking = brk * PPB;
	standing = false;

	update_relevant_region();
}

unsigned int Pform::DynamicEntity::get_limit(int dir, unsigned int coord) const
{
	if (dir < 0)
		return lower_limit[coord];
	// TODO check dir value
	else
		return upper_limit[coord];
}

void Pform::DynamicEntity::update_relevant_region()
{
	lower_limit[0] = std::floor(position[0] / PPB);
	lower_limit[1] = std::floor(position[1] / PPB);
	upper_limit[0] = std::ceil(position[0] / PPB);
	upper_limit[1] = std::ceil(position[1] / PPB);

	if (!(level->is_passable(lower_limit[0], lower_limit[1]) && level->is_passable(lower_limit[0], upper_limit[1]) && level->is_passable(upper_limit[0], upper_limit[1]) && level->is_passable(upper_limit[0], lower_limit[1])))
		throw EntityException();
}

void Pform::DynamicEntity::step(float seconds)
{
	// TODO occassionally very slow to reverse direction
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

void Pform::DynamicEntity::resolve_movement()
{
	// TODO block clipping bug when walking left off a short ledge onto flat ground

	// shift amount to get leading corner
	unsigned int corner[2] = {
		(unsigned int)(delta[0] > 0 ? 1 : 0),
		(unsigned int)(delta[1] > 0 ? 1 : 0)
	};

	// multiplier to account for negative movement
	int multiplier[2] = {SIGN(delta[0]), SIGN(delta[1])};
	// array of crossing queues
	std::queue<std::array<double, 2>> crossing[2];

	// find crossing in each axis
	for (int i = 0; i <= 1; i++)
	{
		if (delta[i] != 0)
		{
			int lower = (get_limit(multiplier[i], i) + corner[i]) * PPB * multiplier[i];
			double upper = (position[i] + delta[i] + corner[i] * PPB) * multiplier[i];
			for (int c = lower; c < upper; c += PPB)
			{
				std::array<double, 2> cross;
				cross[i] = c * multiplier[i]; // remove multiplier
				cross[1 - i] =
					(delta[1 - i] * c * multiplier[i])               / delta[i] -
					(delta[1 - i] * (position[i] + corner[i] * PPB)) / delta[i] +
					(position[1 - i] + corner[1 - i] * PPB);
				crossing[i].push(cross);
			}
		}
	}

	// process grid crossings
	while (!crossing[0].empty() || !crossing[1].empty())
	{
		Type type; // the type of crossing
		std::array<double, 2> point; // the crossing point

		// determine what kind of crossing
		if (crossing[1].empty())
		{
			type = Type::X;
			point = crossing[0].front();
			crossing[0].pop();
		}
		else if (crossing[0].empty())
		{
			type = Type::Y;
			point = crossing[1].front();
			crossing[1].pop();
		}
		else if (crossing[0].front()[0] == crossing[1].front()[0] && crossing[0].front()[1] == crossing[1].front()[1])
		{
			type = Type::Corner;
			point = crossing[0].front();
			crossing[0].pop();
			crossing[1].pop();
		}
		else if (crossing[0].front()[0] * multiplier[0] < crossing[1].front()[0] * multiplier[0] || crossing[0].front()[1] * multiplier[1] < crossing[1].front()[1] * multiplier[1])
		{
			type = Type::X;
			point = crossing[0].front();
			crossing[0].pop();
		}
		else
		{
			type = Type::Y;
			point = crossing[1].front();
			crossing[1].pop();
		}

		// convert from leading corner to position
		point[0] = point[0] - corner[0] * PPB;
		point[1] = point[1] - corner[1] * PPB;

		// move to crossing, subtract from delta
		delta[0] -= point[0] - position[0];
		delta[1] -= point[1] - position[1];

		position[0] = point[0];
		position[1] = point[1];

		update_relevant_region();

		bool collision = false;
		unsigned int next[2];

		if (multiplier[0] != 0)
			next[0] = get_limit(multiplier[0], 0) + multiplier[0];
		if (multiplier[1] != 0)
			next[1] = get_limit(multiplier[1], 1) + multiplier[1];

		// crossing X
		if (type == Type::X or type == Type::Corner)
		{
			// if standing and nothing underneath, fall
			if (standing && level->is_passable(get_limit(1, 0), get_limit(1, 1) + 1))
			{
				standing = false;
				position[1] += 1;
				update_relevant_region();
				collision = true;
			}
			// check for walls
			if (!(level->is_passable(next[0], get_limit(-1, 1))) || !(level->is_passable(next[0], get_limit(1, 1))))
			{
				delta[0] = 0;
				collision = true;
			}
		}
		// crossing Y
		if (type == Type::Y || type == Type::Corner)
		{
			// if landing or hitting ceiling
			if (!(level->is_passable(get_limit(-1, 0), next[1])) || !(level->is_passable(get_limit(1, 0), next[1])))
			{
				delta[1] = 0;
				velocity[1] = 0;
				if (multiplier[1] > 0)
					standing = true;

				collision = true;
			}
			// ledge climbing
			if (impulse[0] != 0 && std::fmod(position[0], (double)PPB) == 0 && level->is_passable(get_limit(1, 0) + impulse[0], get_limit(1, 1)))
			{
				position[0] += impulse[0]; // TODO kinda hacky
				collision = true;
			}
		}
		// corner crossing
		if (type == Type::Corner && delta[0] != 0 && delta[1] != 0)
		{
			if (!(level->is_passable(next[0], next[1])))
			{
				delta[1] = 0; // TODO arbitrary resolution
				collision = true;
			}
		}

		if (collision)
			return resolve_movement();
	}

	position[0] += delta[0];
	position[1] += delta[1];

	update_relevant_region();

	// the final movement can align, causing a fall
	if (standing && level->is_passable(get_limit(-1, 0), get_limit(1, 1) + 1) && level->is_passable(get_limit(1, 0), get_limit(1, 1) + 1))
		standing = false;
}

