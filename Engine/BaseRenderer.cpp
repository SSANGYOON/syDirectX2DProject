#include "pch.h"
#include "BaseRenderer.h"
#include "GameObject.h"
#include "Material.h"
#include "Mesh.h"

BaseRenderer::BaseRenderer()
	:Component(Component_Type::Renderer)
{
}

BaseRenderer::~BaseRenderer()
{
}

void BaseRenderer::Render()
{
	shared_ptr<Transform> tr = GetOwner()->GetTransform();
	tr->SetTransformBuffer();
	_material->Bind();
	_mesh->BindBuffer();
	_mesh->Render();
}
