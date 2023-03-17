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
	Script,
	End,
};

class GameObject;
class Collider;
class Component : public Entity
{
public:
	Component(Component_Type type);
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

	shared_ptr<GameObject> GetOwner() { return _owner.lock(); }
	Component_Type GetType() { return _type; }
protected:
	friend class GameObject;
	void SetOwner(shared_ptr<GameObject> owner) { _owner = owner; }
	
protected:
	Component_Type _type;
	weak_ptr<GameObject> _owner;
};

