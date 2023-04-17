#include "pch.h"
#include "Collider.h"
#include "GameObject.h"
#include "Resources.h"
#include "CollisionManager.h"

Collider::Collider(GameObject* owner)
	: Component(Component_Type::Collider, owner)
	, _trigger(false)
	, _size(Vector3(1.f, 1.f, 1.f))
{
}

Collider::~Collider()
{
}

void Collider::Start()
{
	GET_SINGLE(CollisionManager)->AddCollider(this, _owner->GetLayer());
}

void Collider::FinalUpdate()
{
	toWorld = Matrix::CreateScale(_size) * Matrix::CreateTranslation(_localCenter) * Matrix(_owner->GetTransform()->GetWorld());

#ifdef _DEBUG
	DebugAttribute att;
	if (_type == ColliderType::CIRCLE)
		att.dtype = DebugMeshType::CIRCLE;
	else
		att.dtype = DebugMeshType::RECT;
	att.transform = toWorld;
	GET_SINGLE(Resources)->InsertDebug(att);
#endif
}

void Collider::Render()
{

}
