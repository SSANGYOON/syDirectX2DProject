#pragma once
#include "Script.h"

class CameraController : public Script
{
public:
	CameraController(GameObject* camObj);
	virtual ~CameraController();

	virtual void Start();
	virtual void FinalUpdate();

private:
	class Transform* playerTr;
	class Camera* cameraComp;
};

