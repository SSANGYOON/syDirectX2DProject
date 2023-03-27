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
Trail::Trail()
	:Script(), trailLength(60), WeaponLength(3.6f), curFrame(0), tr(nullptr)
{
}
Trail::~Trail()
{
}

void Trail::Start()
{
	trailMesh = make_shared<Mesh>();
	tr = GetOwner()->GetTransform();
	vertexes.resize(trailLength * 2);
	indexes.resize(trailLength * 6);
	for (int i = 0; i < trailLength; i++)
	{
		vertexes[2 * i] = {};
		vertexes[2 * i + 1] = {};
		if (i > 1) {
			indexes[(i - 1) * 6]     = i * 2 - 2;
			indexes[(i - 1) * 6 + 1] = i * 2 - 1;
			indexes[(i - 1) * 6 + 2] = i * 2;

			indexes[(i - 1) * 6 + 3] = i * 2;
			indexes[(i - 1) * 6 + 4] = i * 2 + 1;
			indexes[(i - 1) * 6 + 5] = i * 2 - 1;
		}
	}
	mr = GetOwner()->GetComponent<MeshRenderer>();
	mr->SetMesh(trailMesh);
	m = make_shared<Material>();
	m->SetShader(GET_SINGLE(Resources)->Find<Shader>(L"TrailShader"));
	mr->SetMaterial(m);
	m->SetTexture(0,GET_SINGLE(Resources)->Load<Texture>(L"TrailTexture", L"SlashTrail.png"));
	mr->SetRender(false);
}

void Trail::FinalUpdate()
{
	if (!_record)
		return;
	elapsed += TIME->DeltaTime();

	if (elapsed > 1.f / 60.f)
	{
		vertexes[curFrame * 2].pos = Vector4(tr->GetWorldPosition(),1.f);
		vertexes[curFrame * 2].uv.x = curFrame;
		vertexes[curFrame * 2].uv.y = 1;

		vertexes[curFrame * 2 + 1].pos = Vector4(tr->GetLocalToWorld(Vector3::Up * WeaponLength),1.f);
		vertexes[curFrame * 2 + 1].uv.x = curFrame;
		vertexes[curFrame * 2 + 1].uv.y = 0;

		previousBasePos = vertexes[curFrame * 2].pos;
		previousTipPos = vertexes[curFrame * 2 + 1].pos;

		

		if (curFrame){
			mr->SetRender(true);
			trailMesh->CreateVertexBuffer(vertexes.data(), (curFrame + 1) * 2);
			trailMesh->CreateIndexBuffer(indexes.data(), curFrame * 6);
		}

		curFrame++;
		elapsed -= 1.f / 60.f;
		m->SetInt(0, curFrame);
		m->SetInt(1, 60);
	}	
}

void Trail::StartRecord()
{
	_record = true; curFrame = 0;
	elapsed = 0.f;
}

void Trail::StopRecord()
{
	_record = false; mr->SetRender(false);
}
