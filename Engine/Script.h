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
};

