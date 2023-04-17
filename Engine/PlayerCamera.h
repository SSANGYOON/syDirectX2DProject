#pragma once
#include "Script.h"
class PlayerCamera : public Script
{
public:
	PlayerCamera(GameObject* camera);
	virtual ~PlayerCamera();

	virtual void Start() override;
	virtual void FinalUpdate() override;
private:
	GameObject* player;
};

