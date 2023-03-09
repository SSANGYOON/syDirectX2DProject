#pragma once
#include "GameObject.h"
class DebugObject : public GameObject
{
public:
	DebugObject();
	virtual ~DebugObject();

	virtual void FinalUpdate() override;

private:
};

