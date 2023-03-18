#include "pch.h"
#include "Transform.h"
#include "RigidBody.h"
#include "Timer.h"
#include "GameObject.h"

RigidBody::RigidBody()
	:Component(Component_Type::RigidBody)
	,_useGravity(false)
{
}

RigidBody::~RigidBody()
{
}

void RigidBody::Start()
{
	_transform = GetOwner()->GetComponent<Transform>();
}

void RigidBody::Update()  
{
	float dt = GET_SINGLE(Timer)->DeltaTime();
	if (_useGravity)
		_velocity += Vector3(0.f, -300.f, 0.f) * dt;

	_transform->Translate(_velocity * dt);
}

void RigidBody::ApplyForce(Vector3 force, ForceMode mode)
{
	switch (mode)
	{
	case ForceMode::CONTINOUS:
		_velocity += force * GET_SINGLE(Timer)->DeltaTime();
		break;
	case ForceMode::IMPULSE:
		_velocity += force;
		break;
	default:
		break;
	}
}

Vector3 RigidBody::SmoothDamp(Vector3 current, Vector3 target, Vector3& currentVelocity, float smoothTime)
{
	smoothTime = max(0.0001F, smoothTime);
	float omega = 2.f / smoothTime;

	float dt = GET_SINGLE(Timer)->DeltaTime();
	float x = omega * dt;
	float exp = 1.f / (1.f + x + 0.48F * x * x + 0.235F * x * x * x);
	
	Vector3 change = current - target;
	Vector3 temp = (currentVelocity + omega * change) * dt;
	Vector3 output = target + (change + temp) * exp;
	currentVelocity = (currentVelocity - omega * temp) * exp;
	if ( Vector3::DistanceSquared(output,current) > Vector3::DistanceSquared(target,current))
	{
		output = target;
		currentVelocity = (output - current) / dt;
	}
	return output;
}