#pragma once
#include "Script.h"
class DarkLady : public Script
{
public:
	DarkLady(class GameObject* obj);

	virtual void Start() override;
	virtual void Update() override;

private:
	class Transform* _transform;

	float buyoncyRange = 0.5f;
	float buyoncyFrequency = 0.5f;

	float time = 0.f;

	GameObject* _target;
	Transform* _pupilTr;

	Transform* _leftWingTr;
	Transform* _rightWingTr;
};

