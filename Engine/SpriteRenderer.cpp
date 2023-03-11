#include "pch.h"
#include "SpriteRenderer.h"
#include "GameObject.h"
#include "Transform.h"
#include "Material.h"
#include "Texture.h"
#include "Mesh.h"

SpriteRenderer::SpriteRenderer()
	:BaseRenderer()
{
}

SpriteRenderer::~SpriteRenderer()
{
}

void SpriteRenderer::Render()
{
	if (_material == nullptr || _material->GetShader() == nullptr)
		return;
	GetOwner()->GetTransform()->SetTransformBuffer();
	_material->SetVec2(2, _material->GetTexture(0)->GetSize());
	_material->Bind();
	_mesh->BindBuffer();
	_mesh->Render();
}
