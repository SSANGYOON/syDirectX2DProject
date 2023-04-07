#pragma once
#include "Entity.h"

enum class Component_Type
{
	None,
	Transform,
	Camera,
	Light,
	Collider,
	RigidBody,
	Animator,
	Renderer,
	Particle,
	UI,
	GRID,
	Script,
	End,
};

class GameObject;
struct Collision;
class Component : public Entity
{
public:
	Component(Component_Type type ,GameObject* owner);
	virtual ~Component();

	virtual void Start() ;
	virtual void Update();
	virtual void LateUpdate();
	virtual void FinalUpdate();
	virtual void Render();

	virtual void OntriggerEnter(const Collision& collision);
	virtual void OntriggerStay(const Collision& collision);
	virtual void OntriggerExit(const Collision& collision);

	virtual void OnCollisionEnter(const Collision& collision);
	virtual void OnCollisionStay(const Collision& collision);
	virtual void OnCollisionExit(const Collision& collision);

	GameObject* GetOwner() { return _owner; }
	Component_Type GetType() { return _type; }
	void SetOwner(GameObject* owner) { _owner = owner; }

protected:
	
	GameObject* _owner;

private:
	Component_Type _type;
	
};

