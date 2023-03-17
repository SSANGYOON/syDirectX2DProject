#include "pch.h"
#include "Collider.h"
#include "GameObject.h"

Collider::Collider(Collider_TYPE type)
	: Component(Component_Type::Collider)
	, _type(type)
	, _trigger(false)
	, _size(Vector3(1.f))
	, _center{ Vector3::Zero }
{
}

Collider::~Collider()
{
}

void Collider::Start()
{
	_transform = GetOwner()->GetTransform();
}

void Collider::FinalUpdate()
{

}

void Collider::Render()
{
}

Vector3 Collider::GetFarthestPoint(const Vector3& dir)
{
	return Vector3();
}
