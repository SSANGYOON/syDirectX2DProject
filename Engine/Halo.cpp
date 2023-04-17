#include "pch.h"
#include "Halo.h"
#include "GameObject.h"
#include "Mesh.h"
#include "Material.h"
#include "Resources.h"

Halo::Halo(GameObject* obj) : Component(Component_Type::Script, obj)
{
	_circleMesh = GET_SINGLE(Resources)->Find<Mesh>(L"Circle2D");
	_haloMaterial = make_shared<Material>();
	_haloMaterial->Load(L"Halo.json");
}

Halo::~Halo()
{
}

void Halo::SetRadius(float radius)
{
	_haloMaterial->SetFloat(1, radius);
}

void Halo::SetCenterAlpha(float alpha)
{
	_haloMaterial->SetFloat(0, alpha);
}

void Halo::SetColor(Vector4 color)
{
	_haloMaterial->SetVec4(1, color);
}

void Halo::Render()
{
	_haloMaterial->SetVec4(0, Vector4(_owner->GetTransform()->GetWorldPosition(),1.f));
	_owner->GetTransform()->SetTransformBuffer();
	_haloMaterial->Bind();
	_circleMesh->BindBuffer();
	_circleMesh->Render();
}
