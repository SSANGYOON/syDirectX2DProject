#pragma once
#include "BaseRenderer.h"

class MeshRenderer : public BaseRenderer
{
public:
	MeshRenderer();
	virtual ~MeshRenderer();

	virtual void Render();
};

