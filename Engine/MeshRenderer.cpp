#include "pch.h"
#include "MeshRenderer.h"
#include "GameObject.h"
#include "Transform.h"
#include "Material.h"
#include "Mesh.h"

MeshRenderer::MeshRenderer()
{
}

MeshRenderer::~MeshRenderer()
{
}

void MeshRenderer::Render()
{
	if (!_render)
		return;
	Transform* tr = GetOwner()->GetTransform();
	tr->SetTransformBuffer();
	_material->Bind();
	_mesh->BindBuffer();
	_mesh->Render();
}
