#pragma once
#include "Collider.h"
class RectCollider : public Collider
{
public:
	RectCollider(GameObject* obj);
	~RectCollider();

	virtual bool Intersects(Collider* other, OUT Vector3& dis) override;
};

