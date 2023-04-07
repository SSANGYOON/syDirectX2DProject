#pragma once
#include "Collider.h"
class CircleCollider : public Collider
{
public:
	CircleCollider(GameObject* obj);
	~CircleCollider();

	virtual bool Intersects(Collider* other, OUT Vector3& dis) override;
	virtual bool RayCast(const Vector3& origin, const Vector3& dir, OUT float& dist) override;
};

