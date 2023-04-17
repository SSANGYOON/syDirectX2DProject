#include "pch.h"
#include "MeshRenderer.h"
#include "GameObject.h"
#include "Transform.h"
#include "Material.h"
#include "Mesh.h"

MeshRenderer::MeshRenderer(GameObject* owner)
	:BaseRenderer(owner) ,_accTime(0.f)
{
}

MeshRenderer::~MeshRenderer()
{
}

void MeshRenderer::Render()
{
	_accTime += TIME->DeltaTime();
	if (!_render)
		return;
	Transform* tr = GetOwner()->GetTransform();
	tr->SetTransformBuffer();
	_material->Bind();
	_material->SetFloat(2, _accTime);
	_mesh->BindBuffer();
	_mesh->Render();
}
