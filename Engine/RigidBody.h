#pragma once
#include "Component.h"
class Transform;
class RigidBody : public Component
{
public:
	RigidBody(class GameObject* owner);
	virtual ~RigidBody();
	
	enum ForceMode
	{
		IMPULSE,
		CONTINOUS
	};
	virtual void Start() override;
	virtual void Update() override;

	void ApplyForce(Vector3 force ,ForceMode mode);
	void SetVelocity(Vector3 velocity) { _velocity = velocity; }

	bool UseGravity() { return _useGravity; };
	void SetGravity(bool gravity) { _useGravity = gravity; }
	Vector3 GetVelocity() { return _velocity; }

	static Vector3 SmoothDamp(Vector3 current, Vector3 target, Vector3& currentVelocity, float smoothing);
	

private:
	friend class Script;
	Vector3 _velocity;
	Transform* _transform;
	bool _stationary;
	bool _useGravity;
};

