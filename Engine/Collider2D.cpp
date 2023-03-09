#include "pch.h"
#include "Collider2D.h"
#include "Transform.h"
#include "GameObject.h"
#include "Resources.h"

Collider2D::Collider2D()
	:Collider(Collider_TYPE::CIRCLE)
	,_size(Vector3(1.f))
	,_center{0.f,0.f,0.001f}
	,_trigger(false)
	,_transform(nullptr)
	,_radius(0.5f)
{

}

Collider2D::~Collider2D()
{
}

void Collider2D::Start()
{
	_transform = GetOwner()->GetTransform();
}

void Collider2D::FinalUpdate()
{
	Vector3 scale = _transform->GetScale();
	scale *= _size;

	Quaternion rotation = _transform->GetRotation();

	Vector3 position = _transform->GetPosition();
	Vector3 colliderPos = position + Vector3::Transform(_center,Matrix::CreateFromQuaternion(rotation));

	if (_type == Collider_TYPE::CIRCLE)
	{
		_radius = max(scale.x, scale.y)/2.f;
		scale = Vector3(_radius * 2.f);
	}
	_matrix = Matrix::CreateScale(scale) * Matrix::CreateFromQuaternion(rotation) * Matrix::CreateTranslation(colliderPos);

#ifdef _DEBUG
	DebugAttribute att;
	if (_type == Collider_TYPE::CIRCLE)
		att.dtype = DebugMeshType::CIRCLE;
	else
		att.dtype = DebugMeshType::RECT;
	att.position = colliderPos;
	att.rotation = rotation;
	att.scale = scale;
	GET_SINGLE(Resources)->InsertDebug(att);
#endif

}

void Collider2D::Render()
{

}
