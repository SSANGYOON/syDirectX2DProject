#include "pch.h"
#include "Laser.h"
#include "GameObject.h"
#include "Material.h"
#include "Mesh.h"
#include "CollisionManager.h"

Laser::Laser(GameObject* obj)
	:Script(obj), _dir(0.f, -1.f, 0), _using(false)
{ 
	_material = make_shared<Material>();
	_mesh = make_shared<Mesh>();
	_material->Load(L"Laser.json");
}

Laser::~Laser()
{
}

void Laser::Start()
{
	_vertexes.resize(4);
	_indexes.resize(6);

	_indexes = { 0,1,3,2,0,3 };
	_mesh->CreateVertexBuffer(_vertexes.data(), _vertexes.size(), D3D11_USAGE_DYNAMIC);
	_mesh->CreateIndexBuffer(_indexes.data(), _indexes.size());
}

void Laser::FinalUpdate()
{

}

void Laser::Render()
{
	if (_using)
	{
		elapsed += TIME->DeltaTime();
		elapsed = clamp<float>(elapsed, 0.f, 2.f);
		_material->SetFloat(0, elapsed / 2.f);
		_material->SetFloat(1, zoffset);
		_material->SetVec4(0, color);
		Vector3 origin = _owner->GetTransform()->GetWorldPosition();

		Collision col;
		bitset<(UINT)LAYER_TYPE::END> layer_mask;
		layer_mask.reset();
		layer_mask[(UINT)LAYER_TYPE::FIXEDOBJECT] = true;
		layer_mask[(UINT)LAYER_TYPE::PLAYER] = true;

		if (GET_SINGLE(CollisionManager)->RayCast(origin, _dir, layer_mask, col))
		{
			Vector3 widthVector = _dir.Cross(Vector3::Forward) * laserWidth * 0.5f;
			_vertexes[0].pos = Vector4(origin + widthVector, 1.f);
			_vertexes[1].pos = Vector4(origin - widthVector, 1.f);
			_vertexes[2].pos = Vector4(origin + col.repulse + widthVector, 1.f);
			_vertexes[3].pos = Vector4(origin + col.repulse - widthVector, 1.f);

			_mesh->SetVertexData(_vertexes.data(), 4);
			_material->Bind();
			_mesh->BindBuffer();
			_mesh->Render();
		}
		
	}
}

void Laser::Shoot()
{
	_using = true;
	elapsed = 0.f;
}

void Laser::End()
{
}
