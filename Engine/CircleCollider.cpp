#include "pch.h"
#include "CircleCollider.h"
#include "Resources.h"
#include "GameObject.h"

CircleCollider::CircleCollider(GameObject* obj)
	:Collider(obj)
{
	_type = ColliderType::CIRCLE;
	_dim = 2;
}

CircleCollider::~CircleCollider()
{
}

bool CircleCollider::Intersects(Collider* other, OUT Vector3& dis)
{
	if (other->GetType() == ColliderType::RECT) {
		bool ret =  other->Intersects(this, dis);
		dis *= -1.f;
		return ret;
	}

	Vector3 centerDiff =  other->GetColliderTransform().Translation() -  toWorld.Translation();

	if (centerDiff.Length() > other->GetSize().x + this->_size.x)
		return false;

	dis = centerDiff / centerDiff.Length() * (other->GetSize().x + this->_size.x - centerDiff.Length());
	return true;
}
