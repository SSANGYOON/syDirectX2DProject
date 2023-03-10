#include "pch.h"
#include "GameObject.h"
#include "Transform.h"
#include "Script.h"
#include "Collider.h"

GameObject::GameObject()
	:_state(PAUSED)
{
	
	shared_ptr<Transform> tr = make_shared<Transform>();
	_components[(UINT)Component_Type::Transform] = tr;
}

GameObject::~GameObject()
{
}

void GameObject::Start()
{
	_components[(UINT)Component_Type::Transform]->SetOwner(shared_from_this());
	for (shared_ptr<Component>& comp : _components)
	{
		_state = ACTIVE;
		if (comp == nullptr)
			continue;

		comp->Start();
	}

	for (shared_ptr<Script>& script : _scripts)
	{
		if (script == nullptr)
			continue;

		script->Start();
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

	for (shared_ptr<Script>& script : _scripts)
	{
		if (script == nullptr)
			continue;

		script->Update();

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

	for (shared_ptr<Script>& script : _scripts)
	{
		if (script == nullptr)
			continue;

		script->LateUpdate();
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

	for (shared_ptr<Script>& script : _scripts)
	{
		if (script == nullptr)
			continue;

		script->FinalUpdate();
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
	for (auto& script : _scripts)
		script->OntriggerEnter(collider);
}

void GameObject::OnTriggerStay(Collider* collider)
{
	for (auto& script : _scripts)
		script->OntriggerStay(collider);
}

void GameObject::OnTriggerExit(Collider* collider)
{
	for (auto& script : _scripts)
		script->OntriggerExit(collider);
}

void GameObject::OnCollisionEnter(Collider* collider)
{
	for (auto& script : _scripts)
		script->OnCollisionEnter(collider);
}

void GameObject::OnCollisionStay(Collider* collider)
{
	for (auto& script : _scripts)
		script->OnCollisionStay(collider);
}

void GameObject::OnCollisionExit(Collider* collider)
{
	for (auto& script : _scripts)
		script->OnCollisionExit(collider);
}

