#pragma once
#include "BaseRenderer.h"

class MeshRenderer : public BaseRenderer
{
public:
	MeshRenderer(class GameObject* owner);
	virtual ~MeshRenderer();

	virtual void Render();
};

