#include "pch.h"
#include "ParticleSystem.h"
#include "GameObject.h"
#include "StructuredBuffer.h"
#include "Resources.h"
#include "Material.h"
#include "Mesh.h"
#include "Shader.h"
#include "ComputeShader.h"

ParticleSystem::ParticleSystem(GameObject* owner)
	:Component(Component_Type::Particle, owner)
{
}

ParticleSystem::~ParticleSystem()
{

}

void ParticleSystem::Start()
{
	_particleBuffer = make_shared<StructuredBuffer>();

	ParticleInfo particles[256] = {};
	_particleBuffer->Create(sizeof(ParticleInfo), _maxParticle, particles, true, true);

	_computeSharedBuffer = make_shared<StructuredBuffer>();


	_computeSharedBuffer->Create(sizeof(ComputeSharedInfo), 1, nullptr, true, true);

	_pointMesh = GET_SINGLE(Resources)->Find<Mesh>(L"PointMesh");

	_computeMaterial = make_shared<Material>();
	_computeMaterial->SetComputeShader(GET_SINGLE(Resources)->Find<ComputeShader>(L"ParticleShader"));

	_graphicsMaterial = make_shared<Material>();
	_graphicsMaterial->Load(L"The_Dark_Lady_particle.json");
}

void ParticleSystem::FinalUpdate()
{
	_accTime += TIME->DeltaTime();

	UINT add = 0;
	if (_createInterval < _accTime)
	{
		_accTime = _accTime - _createInterval;
		add = 1;
	}

	_particleBuffer->BindUAV(0);
	_computeSharedBuffer->BindUAV(1);

	_computeMaterial->SetUInt(0, _maxParticle);
	_computeMaterial->SetUInt(1, add);

	_computeMaterial->SetFloat(0, _zOffset);

	_computeMaterial->SetVec2(0, Vector2(TIME->DeltaTime(), _accTime));
	_computeMaterial->SetVec2(1, _graphicsMaterial->GetTexture(0)->GetSize());
	_computeMaterial->SetVec2(2, _localPosition);
	_computeMaterial->SetVec2(3, _force);

	_computeMaterial->SetVec4(0, Vector4(_posVarFrom, _posVarTo));
	_computeMaterial->SetVec4(1, Vector4(_dir, _dirVar));
	_computeMaterial->SetVec4(2, Vector4(_aliveZone, Vector2(_aliveZoneType, _posvarType)));
	_computeMaterial->SetVec4(3, Vector4(_minLifeTime, _maxLifeTime, _minSpeed, _maxSpeed));

	_computeMaterial->Dispatch();

	_particleBuffer->Clear();
	_computeSharedBuffer->Clear();
}

void ParticleSystem::Render()
{
	_owner->GetTransform()->SetTransformBuffer();

	_particleBuffer->BindSRV(4);
	_graphicsMaterial->SetVec4(1, _color);
	_graphicsMaterial->Bind();
	_pointMesh->BindBuffer();
	_pointMesh->RenderIndexed(_maxParticle);
	_particleBuffer->Clear();
}


