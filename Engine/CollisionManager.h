#pragma once
#include <unordered_set>
#include "box2d/b2_world.h"

namespace SY {
	struct Collision
	{
		UINT64 entityID;
		Vector2 normal;
		Vector2 ContactPoint;
		UINT CollisionLayer;
	};

	class CollisionListener : public b2ContactListener
	{
		virtual void BeginContact(b2Contact* contact) override;
		virtual void EndContact(b2Contact* contact) override;
	};
}
