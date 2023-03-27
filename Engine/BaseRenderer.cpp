#include "pch.h"
#include "BaseRenderer.h"
#include "GameObject.h"
#include "Material.h"
#include "Mesh.h"

BaseRenderer::BaseRenderer()
	:Component(Component_Type::Renderer), _render(true)
{
}

BaseRenderer::~BaseRenderer()
{
}

void BaseRenderer::Render()
{
	
	Transform* tr = GetOwner()->GetTransform();
	tr->SetTransformBuffer();
	_material->Bind();
	_mesh->BindBuffer();
	_mesh->Render();
}
