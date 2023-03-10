#include "pch.h"
#include "Collider.h"
#include "GameObject.h"

Collider::Collider(Collider_TYPE type)
	: Component(Component_Type::Collider)
	, _type(type)
	, _trigger(false)
{
}

Collider::~Collider()
{
}

void Collider::Start()
{
}

void Collider::FinalUpdate()
{

}

void Collider::OnCollisionEnter(Collider* collider)
{
	GetOwner()->OnCollisionEnter(collider);
}

void Collider::OnCollisionStay(Collider* collider)
{
	GetOwner()->OnCollisionStay(collider);
}

void Collider::OnCollisionExit(Collider* collider)
{
	GetOwner()->OnCollisionExit(collider);
}

void Collider::OnTriggerEnter(Collider* collider)
{
	GetOwner()->OnTriggerEnter(collider);
}

void Collider::OnTriggerStay(Collider* collider)
{
	GetOwner()->OnTriggerStay(collider);
}

void Collider::OnTriggerExit(Collider* collider)
{
	GetOwner()->OnTriggerExit(collider);
}
