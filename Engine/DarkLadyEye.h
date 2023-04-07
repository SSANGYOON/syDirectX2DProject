#pragma once
#include "Script.h"
class DarkLadyEye : public Script
{
public:
	DarkLadyEye(GameObject* eye);
	virtual ~DarkLadyEye();

	virtual void Update() override;
private:
	Transform* _pupilTr;
	GameObject* _target;
};

