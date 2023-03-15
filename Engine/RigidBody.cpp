#include "pch.h"
#include "RigidBody.h"

RigidBody::RigidBody()
	:Component(Component_Type::RigidBody)
{
}

RigidBody::~RigidBody()
{
}

void RigidBody::Update()
{
}

void RigidBody::ApplyForce(Vector3 force)
{
}

void RigidBody::SetSpeed(Vector3 speed)
{
}
