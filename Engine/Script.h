#pragma once
#include "Component.h"
class Collider;
class Script : public Component
{
public:
	Script();
	virtual ~Script();

	virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;
	virtual void FinalUpdate() override;

	virtual void OntriggerEnter(Collider* collider);
	virtual void OntriggerStay(Collider* collider);
	virtual void OntriggerExit(Collider* collider);

	virtual void OnCollisionEnter(Collider* collider);
	virtual void OnCollisionStay(Collider* collider);
	virtual void OnCollisionExit(Collider* collider);
};

