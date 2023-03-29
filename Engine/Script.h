#pragma once
#include "Component.h"
class Collider;
class GameObject;
class Script : public Component
{
public:
	Script(GameObject* owner);
	virtual ~Script();

	virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;
	virtual void FinalUpdate() override;
};

