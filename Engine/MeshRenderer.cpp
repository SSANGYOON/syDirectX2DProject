#include "pch.h"
#include "MeshRenderer.h"
#include "GameObject.h"
#include "Transform.h"
#include "Material.h"
#include "Mesh.h"

MeshRenderer::MeshRenderer(GameObject* owner)
	:BaseRenderer(owner)
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
	_material->Render(_mesh);
}
