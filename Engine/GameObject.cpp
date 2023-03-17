#include "pch.h"
#include "GameObject.h"
#include "BaseRenderer.h"
#include "Script.h"
#include "Collider.h"

GameObject::GameObject()
	:_state(PAUSED)
{
	_components.push_back(make_shared<Transform>());
}

GameObject::~GameObject()
{
}

shared_ptr<Transform> GameObject::GetTransform()
{
	return GetComponent<Transform>();
}
shared_ptr<BaseRenderer> GameObject::GetRenderer()
{
	return GetComponent<BaseRenderer>();
}
void GameObject::Start()
{
	_components[0]->SetOwner(shared_from_this());
	_state = ACTIVE;
	for (shared_ptr<Component>& comp : _components)
	{
		if (comp == nullptr)
			continue;

		comp->Start();
	}
}

void GameObject::Update()
{
	for (shared_ptr<Component>& comp : _components)
	{
		if (comp == nullptr)
			continue;

		comp->Update();
	}
}

void GameObject::LateUpdate()
{
	for (shared_ptr<Component>& comp : _components)
	{
		if (comp == nullptr)
			continue;

		comp->LateUpdate();
	}
}

void GameObject::FinalUpdate()
{
	for (shared_ptr<Component>& comp : _components)
	{
		if (comp == nullptr)
			continue;

		comp->FinalUpdate();
	}
}

void GameObject::Render()
{

	for (shared_ptr<Component>& comp : _components)
	{
		if (comp == nullptr)
			continue;

		comp->Render();
	}
}

void GameObject::OnTriggerEnter(Collider* collider)
{
	for (auto& comp : _components)
		comp->OntriggerEnter(collider);
}

void GameObject::OnTriggerStay(Collider* collider)
{
	for (auto& comp : _components)
		comp->OntriggerStay(collider);
}

void GameObject::OnTriggerExit(Collider* collider)
{
	for (auto& comp : _components)
		comp->OntriggerExit(collider);
}

void GameObject::OnCollisionEnter(Collider* collider)
{
	for (auto& comp : _components)
		comp->OnCollisionEnter(collider);
}

void GameObject::OnCollisionStay(Collider* collider)
{
	for (auto& comp : _components)
		comp->OnCollisionStay(collider);
}

void GameObject::OnCollisionExit(Collider* collider)
{
	for (auto& comp : _components)
		comp->OnCollisionExit(collider);
}

