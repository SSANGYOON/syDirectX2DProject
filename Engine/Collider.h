#pragma once
#include "Component.h"

enum Collider_TYPE
{
	CIRCLE,
	RECTANGLE,
	SPHERE,
	CUBOID,
};

class Collider : public Component
{
public:
	Collider(Collider_TYPE type);
	virtual ~Collider();

	virtual void Start() override;
	virtual void FinalUpdate() override;

	void OnCollisionEnter(Collider* collider);
	void OnCollisionStay(Collider* collider);
	void OnCollisionExit(Collider* collider);

	void OnTriggerEnter(Collider* collider);
	void OnTriggerStay(Collider* collider);
	void OnTriggerExit(Collider* collider);

	Collider_TYPE GetColiderType() { return _type; }
	void SetTrigger(bool trigger) { _trigger = trigger; }
	bool IsTrigger() { return _trigger; }

protected:
	Collider_TYPE _type;
	bool _trigger;
};

