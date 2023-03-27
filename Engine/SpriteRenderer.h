#pragma once
#include "BaseRenderer.h"

class SpriteRenderer : public BaseRenderer
{
public:
	SpriteRenderer();
	virtual ~SpriteRenderer();

	virtual void Render();

	void SetOffSet(Vector2 offset) { _offset = offset; }
	void SetOriginPos(Vector2 originPos) { _originPos = originPos; }
	void SetOriginSize(Vector2 originSize) { _originSize = originSize; }
	

private:
	Vector2 _offset;
	Vector2 _originPos;
	Vector2 _originSize;

	
};

