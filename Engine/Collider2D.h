#pragma once
#include "Collider.h"
#include "Material.h"
#include "Mesh.h"
#include "Resources.h"

class Transform;
class Collider2D : public Collider
{
public:
	Collider2D(class GameObject* owner);
	virtual ~Collider2D();

	virtual void FinalUpdate() override;
	virtual void Render() override;

	virtual Vector3 GetFarthestPoint(const Vector3& direction) override;

	const float GetRadius() { return _radius; }
	const Matrix& GetMatrix() { return _matrix; }
	const Vector3& GetSize() { return _size; }
	const Vector3& GetCenter() { return _center; }

private:
	float _radius;
};

