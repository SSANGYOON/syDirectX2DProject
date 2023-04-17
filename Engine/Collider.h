#pragma once
#include "Component.h"
class Transform;

struct Collision
{
	Vector3 repulse;
	GameObject* other;
};

enum class ColliderType
{
	RECT,
	CIRCLE,
	BOX,
	SPHERE
};
class Collider : public Component
{
public:
	Collider(class GameObject* owner);
	virtual ~Collider();

	bool IsTriiger() { return _trigger; }

	void SetLocalCenter(Vector3 localCenter) { _localCenter = localCenter;}

	void SetSize(Vector3 size) { _size = size; }
	const Vector3 GetSize() { return _size; }

	void SetTrigger(bool trigger) { _trigger = trigger; }
	bool IsTrigger() { return _trigger; }

	virtual void Start() override;
	virtual void FinalUpdate() override;
	virtual void Render() override;

	virtual bool Intersects(Collider* other, OUT Vector3& dis) abstract;
	virtual bool RayCast(const Vector3& origin, const Vector3& dir, OUT float& dist) abstract;
	const Matrix& GetColliderTransform() { return toWorld; }

	UINT getDim() { return _dim; }
	ColliderType GetType() { return _type; }

	


protected:
	UINT _dim;
	Vector3 _localCenter;
	Vector3 _size;
	Matrix toWorld;
	ColliderType _type;
	bool _trigger;
};

