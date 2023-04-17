#pragma once
#include "Script.h"

class ParticleSystem;
class Laser;
class DarkLadyEye : public Script
{
public:
	DarkLadyEye(GameObject* eye);
	virtual ~DarkLadyEye();

	virtual void Start() override;
	virtual void Update() override;
private:
	Transform* _pupilTr;
	GameObject* _target;

	Laser* laser0;
	Laser* laser1;
	Laser* laser2;
	Laser* laser3;

	float LaserElapsed;
	ParticleSystem* AttackSphere;
};

