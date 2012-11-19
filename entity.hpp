#ifndef ENTITY_H_
#define ENTITY_H_

class Entity
{
	bool solid;
	unsigned int y;
	unsigned int x;
public:
	Entity(bool s = true);

	inline bool is_solid() const { return solid; };
}

class StaticEntity : public Entity
{
	bool solid;
public:
	StaticEntity(bool s = true);
};

class DynamicEntity : public Entity
{
	// pixel position, for displaying
	unsigned int py;
	unsigned int px;
	// floating point position, for physics
	long double fy;
	long double fx;
public:
	DynamicEntity(bool s = true);
};

#endif
