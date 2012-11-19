#include "pform.hpp"

Entity::Entity(bool s)
{
	solid = s;
}

StaticEntity::StaticEntity(bool s) : Entity(s)
{
}

DynamicEntity::DynamicEntity(bool s) : Entity(s)
{
}
