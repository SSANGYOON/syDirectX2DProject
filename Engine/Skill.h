#pragma once
#include "Script.h"
class Animator;
class Weapon;
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
	bool summoned = false;
	const UINT phases;
	UINT currentPhase;

	float _phaseTime = 0.f;
	Animator* holderAnim;
	array<Animator*, 5> satleliteAnim;
	array<Animator*, 5> swordAnim;
	array<Weapon*, 5> weapons;
};

