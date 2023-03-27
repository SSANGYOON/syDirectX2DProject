#pragma once
#include "Component.h"

enum Collider_TYPE
{
	LINEAR,
	CIRCLE,
	RECTANGLE,
	SPHERE,
	CUBOID,
};

class Transform;
class Collider : public Component
{
public:
	Collider(Collider_TYPE type);
	virtual ~Collider();

	virtual void Start() override;
	virtual void FinalUpdate() override;
	virtual void Render() override;
	virtual Vector3 GetFarthestPoint(const Vector3& dir);
	Vector3 GetCenter() { return _matrix.Translation(); }

	void SetType(Collider_TYPE type) { _type = type; }
	void SetSize(const Vector3& size) { _size = size; }
	void SetLocalCenter(const Vector3& center) { _center = center; }
	bool IsTriiger() { return _trigger; }

	const Matrix& GetMatrix() { return _matrix; }
	const Vector3& GetSize() { return _size; }
	const Vector3& GetLocalCenter() { return _center; }

	Collider_TYPE GetColiderType() { return _type; }
	void SetTrigger(bool trigger) { _trigger = trigger; }
	bool IsTrigger() { return _trigger; }

protected:
	Matrix _matrix;
	Matrix _invMatrix;
	Collider_TYPE _type;
	Vector3 _size;
	Vector3 _center;
	bool _trigger;
	Transform* _transform;
};

