#pragma once
#include "Collider.h"
class CircleCollider : public Collider
{
public:
	CircleCollider(GameObject* obj);
	~CircleCollider();

	virtual bool Intersects(Collider* other, OUT Vector3& dis) override;
};

