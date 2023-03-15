#pragma once
#include "Component.h"
class RigidBody : public Component
{
public:
	RigidBody();
	virtual ~RigidBody();

	virtual void Update() override;

	void ApplyForce(Vector3 force);
	void SetSpeed(Vector3 speed);
	bool UseGravity() { return _useGravity; };

private:
	Vector2 _speed;
	bool _useGravity;
};

