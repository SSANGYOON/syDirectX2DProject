#include "pch.h"
#include "Trail.h"
#include "Mesh.h"
#include "Material.h"
#include "Texture.h"
#include "MeshRenderer.h"
#include "GameObject.h"
#include "Resources.h"
#include "Shader.h"
#include "Timer.h"
#include "Weapon.h"

Trail::Trail(GameObject* owner)
	:Script(owner), trailLength(10), WeaponLength(36.f), curFrame(-1), tr(nullptr), mr(nullptr)
{
	trailMesh = make_shared<Mesh>();

	mr = _owner->AddComponent<MeshRenderer>();
	mr->SetRender(false);
	m = make_shared<Material>();

	tr = GetOwner()->GetTransform();
	

	mr->SetMaterial(m);
	mr->SetMesh(trailMesh);
}
Trail::~Trail()
{
}

void Trail::Start()
{
	auto weponType = tr->GetParent()->GetOwner()->GetComponent<Weapon>()->GetWeaponType();
	vertexes.resize(trailLength * 6);
	indexes.resize(trailLength * 6);
	for (UINT i = 0; i < trailLength; i++)
	{
		for (UINT j = 0; j < 6; j++)
		{
			vertexes[6 * i + j] = {};
			indexes[6 * i + j] = 6 * i + j;
		}
	}
	trailMesh->CreateVertexBuffer(vertexes.data(), vertexes.size(),D3D11_USAGE_DYNAMIC);
	trailMesh->CreateIndexBuffer(indexes.data(), indexes.size());
}

void Trail::FinalUpdate()
{
	if (!_record)
		return;
	elapsed += TIME->DeltaTime();

	if (elapsed > 1.f / 60.f)
	{
		if (curFrame >= 0)
		{
			mr->SetRender(true);
			vertexes[curFrame * 6].pos = previousBasePos;
			vertexes[curFrame * 6 + 1].pos = previousTipPos;
			vertexes[curFrame * 6 + 2].pos = Vector4(tr->GetWorldPosition(), 1.f);
			vertexes[curFrame * 6 + 3].pos = previousTipPos;
			vertexes[curFrame * 6 + 4].pos = Vector4(tr->GetWorldPosition(), 1.f);
			vertexes[curFrame * 6 + 5].pos = Vector4(tr->GetLocalToWorld(Vector3::Up * WeaponLength), 1.f);


			if (_trailType == TRAIL_TYPE::HORIZON)
			{
				vertexes[curFrame * 6].uv.x = 0;
				vertexes[curFrame * 6].uv.y = curFrame;
				vertexes[curFrame * 6 + 1].uv.x = 1;
				vertexes[curFrame * 6 + 1].uv.y = curFrame;
				vertexes[curFrame * 6 + 2].uv.x = 0;
				vertexes[curFrame * 6 + 2].uv.y = curFrame + 1;
				vertexes[curFrame * 6 + 3].uv.x = 1;
				vertexes[curFrame * 6 + 3].uv.y = curFrame;
				vertexes[curFrame * 6 + 4].uv.x = 0;
				vertexes[curFrame * 6 + 4].uv.y = curFrame + 1;
				vertexes[curFrame * 6 + 5].uv.x = 1;
				vertexes[curFrame * 6 + 5].uv.y = curFrame + 1;
			}
			else if (_trailType == TRAIL_TYPE::VERTICAL)
			{
				vertexes[curFrame * 6].uv.x = curFrame;
				vertexes[curFrame * 6].uv.y = 1;
				vertexes[curFrame * 6 + 1].uv.x = curFrame;
				vertexes[curFrame * 6 + 1].uv.y = 0;
				vertexes[curFrame * 6 + 2].uv.x = curFrame + 1;
				vertexes[curFrame * 6 + 2].uv.y = 1;
				vertexes[curFrame * 6 + 3].uv.x = curFrame;
				vertexes[curFrame * 6 + 3].uv.y = 0;
				vertexes[curFrame * 6 + 4].uv.x = curFrame + 1;
				vertexes[curFrame * 6 + 4].uv.y = 1;
				vertexes[curFrame * 6 + 5].uv.x = curFrame + 1;
				vertexes[curFrame * 6 + 5].uv.y = 0;
			}
			else if (_trailType == TRAIL_TYPE::VERTICALFLIPED)
			{
				vertexes[curFrame * 6].uv.x = curFrame;
				vertexes[curFrame * 6].uv.y = 0;
				vertexes[curFrame * 6 + 1].uv.x = curFrame;
				vertexes[curFrame * 6 + 1].uv.y = 1;
				vertexes[curFrame * 6 + 2].uv.x = curFrame + 1;
				vertexes[curFrame * 6 + 2].uv.y = 0;
				vertexes[curFrame * 6 + 3].uv.x = curFrame;
				vertexes[curFrame * 6 + 3].uv.y = 1;
				vertexes[curFrame * 6 + 4].uv.x = curFrame + 1;
				vertexes[curFrame * 6 + 4].uv.y = 0;
				vertexes[curFrame * 6 + 5].uv.x = curFrame + 1;
				vertexes[curFrame * 6 + 5].uv.y = 1;
			}
			trailMesh->SetVertexData(vertexes.data(), vertexes.size());
		}

		previousBasePos = Vector4(tr->GetWorldPosition(), 1.f);
		previousTipPos = Vector4(tr->GetLocalToWorld(Vector3::Up * WeaponLength), 1.f);
		curFrame = (curFrame + 1) % trailLength;
		elapsed -= 1.f / 60.f;

		m->SetUInt(0, curFrame);
		m->SetUInt(1, trailLength);
		m->SetUInt(2, UINT(_trailType == TRAIL_TYPE::HORIZON));
	}	
}

void Trail::SetTrailType(WEAPON_TYPE type)
{
	if (type == WEAPON_TYPE::ONEHAND) {
		m->Load(L"OneHandTrail.json");
		WeaponLength = 36.f;
		_trailType = TRAIL_TYPE::VERTICAL;
	}
	else if (type == WEAPON_TYPE::DAGGER) {
		m->Load(L"DaggerTrail.json");
		WeaponLength = 20.f;
		_trailType = TRAIL_TYPE::HORIZON;
	}
	else if (type == WEAPON_TYPE::SUMMON) {
		m->Load(L"OneHandTrail.json");
		WeaponLength = 36.f;
		_trailType = TRAIL_TYPE::VERTICALFLIPED;
	}
}

void Trail::StartRecord()
{
	
	Vertex v = {};
	std::fill(vertexes.begin(), vertexes.end(), v);
	curFrame = -1; _record = true;
	elapsed = 0.f;
}

void Trail::StopRecord()
{
	_record = false; mr->SetRender(false);
}
