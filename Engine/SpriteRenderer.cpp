#include "pch.h"
#include "SpriteRenderer.h"
#include "GameObject.h"
#include "Transform.h"
#include "Material.h"
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
	shared_ptr<Transform> tr = GetOwner()->GetTransform();
	tr->SetTransformBuffer();
	_material->Bind();
	_mesh->BindBuffer();
	_mesh->Render();
}
