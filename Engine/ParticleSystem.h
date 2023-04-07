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
	void setCreateInterval(float interval) { _createInterval = interval; }
	void setMinLifeTime(float lifeTime) {_minLifeTime = lifeTime;}
	void setMaxLifeTime(float lifeTime) {_maxLifeTime = lifeTime;}
	void setMinSpeed(float speed){_minSpeed = speed;}
	void setMaxSpeed(float speed){_maxSpeed = speed;}
	void setMinRange(Vector2 LB){GenPosLB = LB;}
	void setMaxRange(Vector2 RT){GenPosRT = RT;}
	void setTargetOffset(Vector4 targetOffset) { _targetOffset = targetOffset; }
	void setColor(Vector4 color) { _color = color; }

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
	float				_minSpeed = 50;
	float				_maxSpeed = 100;

	Vector2 GenPosLB = Vector2(40, 40);
	Vector2 GenPosRT = Vector2(80, 80);
	Vector4 _targetOffset = Vector4::Zero;
	Vector4 _color = Vector4::Zero;
};

