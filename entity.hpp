#ifndef ENTITY_H_
#define ENTITY_H_

namespace Pform
{
	class StaticEntity
	{
		bool solid;
	public:
		StaticEntity(bool s = true);
	};

	class DynamicEntity
	{
		Level* level;

		double position[2];
		double delta[2]; // amount left to move
		double velocity[2];
		double terminal[2]; // terminal velocities
		double acceleration[2]; // from movement and gravity
		int impulse[2]; // if actively moving
		// coords of overlapped blocks
		unsigned int lower_limit[2];
		unsigned int upper_limit[2];

		double breaking; // deceleration when idle
		bool standing; // if the entity is on the ground
	public:
		DynamicEntity(const Level& l, int x, int y, double tvx, double tvy, double accx, double accy, double brk);

		double [2] get_position() const { return position; }
		unsigned int get_limit(int dir, unsigned int coord) const;

		void update_relevant_region(); // recompute overlapped level blocks
		void jump();
		void step(float seconds);
		void stand();
		void move_to(unsigned int x, unsigned int y);
		void resolve_movement();
	};
}

#endif
