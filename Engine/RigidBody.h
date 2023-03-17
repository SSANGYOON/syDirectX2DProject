#pragma once
#include "Component.h"
class Transform;
class RigidBody : public Component
{
public:
	RigidBody();
	virtual ~RigidBody();
	
	virtual void Start() override;
	virtual void Update() override;

	void ApplyForce(Vector3 force);
	void SetVelocity(Vector3 velocity) { _velocity = velocity; }
	
	bool UseGravity() { return _useGravity; };
	void SetGravity(bool gravity) { _useGravity = gravity; }
	Vector3 GetVelocity() { return _velocity; }

private:
	Vector3 _velocity;
	shared_ptr<Transform> _transform;
	bool _stationary;
	bool _useGravity;
};

