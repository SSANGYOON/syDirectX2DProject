#include "pch.h"
#include "SpriteRenderer.h"
#include "GameObject.h"
#include "Transform.h"
#include "Material.h"
#include "Texture.h"
#include "Mesh.h"

SpriteRenderer::SpriteRenderer()
	:BaseRenderer(), _offset{}, _originPos{}, _originSize{}
{
}

SpriteRenderer::~SpriteRenderer()
{
}

void SpriteRenderer::Render()
{
	if (_material == nullptr || _material->GetShader() == nullptr)
		return;
	if (!_render)
		return;
	GetOwner()->GetTransform()->SetTransformBuffer();

	_material->SetVec2(0, _offset);
	_material->SetVec2(1, _originPos);
	_material->SetVec2(2, _originSize);

	_material->Bind();
	_mesh->BindBuffer();
	_mesh->Render();
}
