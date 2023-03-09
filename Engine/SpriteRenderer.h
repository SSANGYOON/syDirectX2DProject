#pragma once
#include "BaseRenderer.h"

class SpriteRenderer : public BaseRenderer
{
public:
	SpriteRenderer();
	virtual ~SpriteRenderer();

	virtual void Render();
};

