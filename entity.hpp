#ifndef ENTITY_H_
#define ENTITY_H_

namespace Pform
{
	class Level;

	class StaticEntity
	{
		bool solid;
	public:
		StaticEntity(bool s = true);

		bool is_solid() const { return solid; }
	};

	class DynamicEntity
	{
		enum Type {
			X, Y, Corner
		};

		const Level* level;

		double position[2];
		double delta[2]; // amount left to move
		double terminal[2]; // terminal velocities
		double acceleration[2]; // from movement and gravity
		// coords of overlapped blocks
		unsigned int lower_limit[2];
		unsigned int upper_limit[2];

		double breaking; // deceleration when idle

		void update_relevant_region(); // recompute overlapped level blocks
		void resolve_movement(); // apply delta to position
	public:
		double velocity[2];
		int impulse[2]; // if actively moving
		bool standing; // if the entity is on the ground

		DynamicEntity(const Level& l, int x, int y, double tvx, double tvy, double accx, double accy, double brk);

		const double* get_position() const { return position; }
		unsigned int get_limit(int dir, unsigned int coord) const;

		void step(float seconds); // create delta

	};

	class EntityException : public std::runtime_error
	{
	public:
		EntityException()
			: std::runtime_error("Illegally overlapping entities")
		{}
	};
}

#endif
