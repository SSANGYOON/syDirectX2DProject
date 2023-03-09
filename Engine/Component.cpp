#include "pch.h"
#include "Component.h"
#include "GameObject.h"

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
