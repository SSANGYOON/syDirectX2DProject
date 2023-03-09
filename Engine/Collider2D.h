#pragma once
#include "Collider.h"
#include "Material.h"
#include "Mesh.h"
#include "Resources.h"

class Transform;
class Collider2D : public Collider
{
public:
	Collider2D();
	virtual ~Collider2D();

	virtual void Start() override;
	virtual void FinalUpdate() override;
	virtual void Render() override;

	void SetType(Collider_TYPE type) { _type = type; }
	void SetSize(const Vector3& size) { _size = size; }
	void SetCenter(const Vector3 center) { _center = center; }
	bool IsTriiger() { return _trigger; }

	const Matrix& GetMatrix() { return _matrix; }
	const float GetRadius() { return _radius; }

private:
	Matrix _matrix;
	Vector3 _size;
	Vector3 _center;

	float _radius;
	bool _trigger;
	shared_ptr<Transform> _transform;
};

