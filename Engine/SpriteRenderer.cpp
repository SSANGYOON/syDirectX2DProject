#include "pch.h"
#include "SpriteRenderer.h"
#include "GameObject.h"
#include "Transform.h"
#include "Shader.h"
#include "Resources.h"
#include "Mesh.h"
#include "Texture.h"
#include "Engine.h"

SpriteRenderer::SpriteRenderer(GameObject* owner)
	:BaseRenderer(owner), spCB{}
{
	spCB.targetSizeRatio = Vector2(0.1f, 0.1f);
	_mesh = GET_SINGLE(Resources)->Find<Mesh>(L"RectMesh");
}

SpriteRenderer::~SpriteRenderer()
{
}

void SpriteRenderer::Render()
{
	if (!_render)
		return;

	_owner->GetTransform()->SetTransformBuffer();

	auto shader = GET_SINGLE(Resources)->Find<Shader>(L"SpriteShader");
	shader->BindShader();
	

	if (_spriteSheet) {
		_spriteSheet->BindSRV(ShaderStage::VS, 0);
		_spriteSheet->BindSRV(ShaderStage::PS, 0);
	}
	else {
		int a = 0;
	}
	BindSpriteBuffer();
	
	_mesh->BindBuffer();
	_mesh->Render();

	if (_spriteSheet) {
		_spriteSheet->ClearSRV(ShaderStage::VS, 0);
		_spriteSheet->ClearSRV(ShaderStage::PS, 0);
	}
}

void SpriteRenderer::SetSpriteSheet(shared_ptr<Texture> texture)
{
	_spriteSheet = texture;
	spCB.sourceSize = _spriteSheet->GetSize();
	spCB.sourceSheetSize = _spriteSheet->GetSize();
}

void SpriteRenderer::BindSpriteBuffer()
{
	shared_ptr<ConstantBuffer> cb = GEngine->GetConstantBuffer(Constantbuffer_Type::SPRITE);
	cb->SetData(&spCB);
	cb->SetPipline(ShaderStage::VS);
	cb->SetPipline(ShaderStage::PS);
}
