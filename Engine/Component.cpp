#include "pch.h"
#include "Component.h"
#include "GameObject.h"

#include <iostream>
Component::Component(Component_Type type)
	:_type(type)
{
}

Component::~Component()
{
}

void Component::Start()
{
}

void Component::Update()
{
}

void Component::LateUpdate()
{
}

void Component::FinalUpdate()
{
}

void Component::Render()
{
}

void Component::OntriggerEnter(Collider* collider)
{
}

void Component::OntriggerStay(Collider* collider)
{
}

void Component::OntriggerExit(Collider* collider)
{
}

void Component::OnCollisionEnter(Collider* collider)
{
}

void Component::OnCollisionStay(Collider* collider)
{
}

void Component::OnCollisionExit(Collider* collider)
{
}
