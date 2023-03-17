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

void RigidBody::ApplyForce(Vector3 force)
{
	_velocity += force * GET_SINGLE(Timer)->DeltaTime();
}
