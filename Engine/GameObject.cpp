#include "pch.h"
#include "GameObject.h"
#include "BaseRenderer.h"
#include "Script.h"
#include "Collider.h"

GameObject::GameObject()
	:_state(PAUSED)
{
	_transform = make_unique<Transform>(this);
	_transform->SetOwner(this);
}

GameObject::~GameObject()
{
}

Transform* GameObject::GetTransform()
{
	return _transform.get();
}
BaseRenderer* GameObject::GetRenderer()
{
	return GetComponent<BaseRenderer>();
}
void GameObject::Start()
{
	_state = ACTIVE;

	for (unique_ptr<Component>& comp : _components)
	{
		if (comp == nullptr)
			continue;

		comp->Start();
	}
}

void GameObject::Update()
{
	for (unique_ptr<Component>& comp : _components)
	{
		if (comp == nullptr)
			continue;

		comp->Update();
	}
}

void GameObject::FinalUpdate()
{
	_transform->FinalUpdate();
	for (unique_ptr<Component>& comp : _components)
	{
		if (comp == nullptr)
			continue;

		comp->FinalUpdate();
	}
}

void GameObject::Render()
{
	for (unique_ptr<Component>& comp : _components)
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

