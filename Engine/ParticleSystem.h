#pragma once
#include "Component.h"

class Material;
class Mesh;
class StructuredBuffer;

struct ParticleInfo
{
	Vector3	Position;
	float	curTime;
	Vector3	worldDir;
	float	lifeTime;
	UINT	alive;
	UINT padding[3];
};

struct ComputeSharedInfo
{
	UINT addCount;
	UINT padding[3];
};


class ParticleSystem : public Component
{
public:
	ParticleSystem(GameObject* owner);
	virtual ~ParticleSystem();

public:
	virtual void Start();
	virtual void FinalUpdate();
	virtual void Render();

	void SetGraphicsMaterial(shared_ptr<Material> graphicsMaterial) {_graphicsMaterial = graphicsMaterial;};

private:
	shared_ptr<StructuredBuffer> _particleBuffer;
	shared_ptr<StructuredBuffer> _computeSharedBuffer;
	UINT _maxParticle = 256;

	shared_ptr<Material>		_graphicsMaterial;
	shared_ptr<Mesh>			_pointMesh;
	shared_ptr<Material> _computeMaterial;

	float				_createInterval = 0.005f;
	float				_accTime = 0.f;

	float				_minLifeTime = 0.5f;
	float				_maxLifeTime = 1.f;
	float				_minSpeed = 5;
	float				_maxSpeed = 10;
	float				_startScale = 1.f;
	float				_endScale = 1.f;

	Vector2 GenPosLB = Vector2(4, 4);
	Vector2 GenPosRT = Vector2(8, 8);
};

