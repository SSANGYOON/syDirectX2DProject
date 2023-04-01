#pragma once
#include "BaseRenderer.h"
#include "ConstantBuffer.h"

class SpriteRenderer : public BaseRenderer
{
public:
	SpriteRenderer(class GameObject* owner);
	virtual ~SpriteRenderer();

	virtual void Render();

	void SetSourceOffset(Vector2 sourceOffset) { spCB.sourceOffset = sourceOffset; }
	void SetSourceSize(Vector2 originalSize) { spCB.sourceSize = originalSize; }
	void SetTargetOffset(Vector2 targetOffset) {spCB.targetOffset = targetOffset; }
	void SetTargetSizeRatio(Vector2 ratio) { spCB.targetSizeRatio = ratio; }
	void SetSheetSize(Vector2 sheetSize) { spCB.sourceSheetSize = sheetSize; }
	

private:
	void BindSpriteBuffer();
	SpriteCB spCB;
};

