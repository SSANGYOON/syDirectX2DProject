#pragma once
#include "Entity.h"

enum class Component_Type
{
	None,
	Transform,
	Camera,
	Mesh,
	Collider,
	RigidBody,
	Animator,
	Renderer,
	UI,
	GRID,
	Script,
	End,
};

class GameObject;
class Collider;
class Component : public Entity
{
public:
	Component(Component_Type type ,GameObject* owner);
	virtual ~Component();

	virtual void Start();
	virtual void Update();
	virtual void LateUpdate();
	virtual void FinalUpdate();
	virtual void Render();

	virtual void OntriggerEnter(Collider* collider);
	virtual void OntriggerStay(Collider* collider);
	virtual void OntriggerExit(Collider* collider);

	virtual void OnCollisionEnter(Collider* collider);
	virtual void OnCollisionStay(Collider* collider);
	virtual void OnCollisionExit(Collider* collider);

	GameObject* GetOwner() { return _owner; }
	Component_Type GetType() { return _type; }
	void SetOwner(GameObject* owner) { _owner = owner; }

protected:
	
	GameObject* _owner;

private:
	Component_Type _type;
	
};

