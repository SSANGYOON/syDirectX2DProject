#include "pch.h"
#include "SpriteRenderer.h"
#include "GameObject.h"
#include "Transform.h"
#include "Material.h"
#include "Texture.h"
#include "Mesh.h"
#include "Animator.h"
#include "Engine.h"
SpriteRenderer::SpriteRenderer(GameObject* owner)
	:BaseRenderer(owner), spCB{}
{
	spCB.targetSizeRatio = Vector2(0.1f, 0.1f);
}

SpriteRenderer::~SpriteRenderer()
{
}

void SpriteRenderer::Render()
{
	if (_material == nullptr)
		return;
	if (!_render)
		return;
	_owner->GetTransform()->SetTransformBuffer();
	BindSpriteBuffer();
	_material->Render(_mesh);
}

void SpriteRenderer::BindSpriteBuffer()
{
	shared_ptr<ConstantBuffer> cb = GEngine->GetConstantBuffer(Constantbuffer_Type::SPRITE);
	spCB.sourceSheetSize = _material->GetTexture(0)->GetSize();
	cb->SetData(&spCB);
	cb->SetPipline(ShaderStage::VS);
}
