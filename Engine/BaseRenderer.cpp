#include "pch.h"
#include "BaseRenderer.h"
#include "GameObject.h"
#include "Material.h"
#include "Mesh.h"

BaseRenderer::BaseRenderer(GameObject* owner)
	:Component(Component_Type::Renderer, owner), _render(true)
{
}

BaseRenderer::~BaseRenderer()
{
}

void BaseRenderer::Render()
{
	Transform* tr = GetOwner()->GetTransform();
	tr->SetTransformBuffer();
	_material->Render(_mesh);
}
