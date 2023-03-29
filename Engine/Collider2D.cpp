#include "pch.h"
#include "Collider2D.h"
#include "Transform.h"
#include "GameObject.h"
#include "Resources.h"

Collider2D::Collider2D(GameObject* owner)
	:Collider(Collider_TYPE::CIRCLE, owner)
	,_radius(0.f)
	
{

}

Collider2D::~Collider2D()
{
}

void Collider2D::FinalUpdate()
{
	Vector3 trSize = _transform->GetScale();
	if (_type == Collider_TYPE::CIRCLE)
	{
		_radius = max(_size.x * trSize.x, _size.y * trSize.y) / 2.f;
	}
	_matrix = Matrix::CreateScale(_size) * Matrix::CreateTranslation(_center) * _transform->GetWorld();
	_invMatrix = _matrix.Invert();
#ifdef _DEBUG
	DebugAttribute att;
	if (_type == Collider_TYPE::CIRCLE)
		att.dtype = DebugMeshType::CIRCLE;
	else
		att.dtype = DebugMeshType::RECT;
	att.position = _center;
	att.scale = _size;
	att._target = _transform;
	GET_SINGLE(Resources)->InsertDebug(att);
#endif

}

void Collider2D::Render()
{

}

Vector3 Collider2D::GetFarthestPoint(const Vector3& direction)
{
	if (_type == Collider_TYPE::RECTANGLE)
	{
		static const Vector3 LocalPos[4] =
		{
			 Vector3{-0.5f, 0.5f, 0.0f}
			,Vector3{0.5f, 0.5f, 0.0f}
			,Vector3{0.5f, -0.5f, 0.0f}
			,Vector3{-0.5f, -0.5f, 0.0f}
		};
		
		Vector4 localDir = Vector4::Transform(Vector4(direction.x, direction.y, direction.z, 0.f), _invMatrix);
		Vector3 dir = { localDir.x, localDir.y, localDir.z };

		float maxProj = LocalPos[0].Dot(dir);
		size_t target = 0;
		
		for (size_t i = 1; i < 4; i++)
		{
			float proj = LocalPos[i].Dot(dir);
			if (proj > maxProj)
			{
				target = i;
				maxProj = proj;
			}
		}
		return Vector3::Transform(LocalPos[target], _matrix);
	}
	else
	{
		Vector3 r;
		direction.Normalize(r);
		Vector3 p = _matrix.Translation() + r * _radius;
		return p;
	}
}
