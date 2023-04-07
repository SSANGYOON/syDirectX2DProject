#include "pch.h"
#include "Component.h"
#include "GameObject.h"

#include <iostream>
Component::Component(Component_Type type, GameObject* owner)
	:_type(type), _owner(owner)
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

void Component::OntriggerEnter(const Collision& collision)
{
}

void Component::OntriggerStay(const Collision& collision)
{
}

void Component::OntriggerExit(const Collision& collision)
{
}

void Component::OnCollisionEnter(const Collision& collision)
{
}

void Component::OnCollisionStay(const Collision& collision)
{
}

void Component::OnCollisionExit(const Collision& collision)
{
}
