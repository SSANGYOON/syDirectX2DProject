#pragma once
#include "Script.h"
#include "Camera.h"
class GridScript : public Script
{
public:
	GridScript();
	virtual ~GridScript();

	virtual void FinalUpdate() override;
	void SetCamera(shared_ptr<Camera> camera) { _camera = camera; }

private:
	shared_ptr<Camera> _camera;
};

