#pragma once
#include "Component.h"
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

