#pragma once
#include "Script.h"

class Transform;
class DarkLadyWing : public Script
{
public:
	DarkLadyWing(GameObject* wing);
	virtual ~DarkLadyWing();

	virtual void Update();
private:
	float time;
	float buyoncyFrequency = 0.5f;
	Transform* _leftWingTr;
	Transform* _rightWingTr;
};

