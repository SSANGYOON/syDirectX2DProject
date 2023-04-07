#pragma once
#include "Collider.h"
class RectCollider : public Collider
{
public:
	RectCollider(GameObject* obj);
	~RectCollider();

	virtual bool Intersects(Collider* other, OUT Vector3& dis) override;
	virtual bool RayCast(const Vector3& origin, const Vector3& dir, OUT float& dist) override;
};

