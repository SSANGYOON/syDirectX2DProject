#include "pch.h"
#include "Entity.h"

Entity::Entity()
{
	static UINT nextID = 1;
	_ID = nextID++;

}

Entity::~Entity()
{
}
