#include "pch.h"
#include "Collider.h"

Collider::Collider(Collider_TYPE type)
	: Component(Component_Type::Collider)
	, _type(type)
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

}

void Collider::OnCollisionStay(Collider* collider)
{

}

void Collider::OnCollisionExit(Collider* collider)
{

}

void Collider::OnTriggerEnter(Collider* collider)
{

}

void Collider::OnTriggerStay(Collider* collider)
{

}

void Collider::OnTriggerExit(Collider* collider)
{

}
