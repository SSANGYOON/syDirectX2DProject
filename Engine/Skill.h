#pragma once
#include "Script.h"
class Skill : public Script
{
public:
	Skill(class GameObject* owner);
	~Skill();

	virtual void Start() override;
	virtual void Update() override;

	void Summon();
	bool isSummoned() { return summoned;}

private:
	bool summoned;
	const UINT phases;
	UINT currentPhase;
};

