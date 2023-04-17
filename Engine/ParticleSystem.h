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

enum ALIVE_ZONE_TYPE {
	NO_DEAD_ZONE,
	SPHERE,
	RECT_ZONE,
};

enum POSVAR_TYPE {
	NORMALIZED,
	EMISSION,
	BOX
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

	void SetInitialLocalPosition(Vector2 position) { _localPosition = position; }
	void SetPositionVarianceFrom(Vector2 posvar) { _posVarFrom = posvar; }
	void SetPositionVarianceTo(Vector2 posvar) { _posVarTo = posvar; }

	void SetInitialDirection(Vector2 dir) { _dir = dir; }
	void SetDirVariance(Vector2 dirVar) { _dirVar = dirVar; }

	void SetForce(Vector2 force) { _force = force; }

	void setColor(Vector4 color) { _color = color; }
	void setZoffset(float offset) { _zOffset = offset; }

	void SetAliveZone(Vector2 aliveZone) { _aliveZone = aliveZone; }
	void SetAliveZoneType(ALIVE_ZONE_TYPE type) { _aliveZoneType = type; }

	void SetPositionVarienceType(POSVAR_TYPE type) { _posvarType = type; }
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
	float				_zOffset = 0.f;

	Vector2 _localPosition = Vector2::Zero;
	Vector2 _posVarFrom = Vector2(25.f, 25.f);
	Vector2 _posVarTo = Vector2(50.f, 50.f);
	Vector2 _dir = Vector2::Zero;
	Vector2 _dirVar = Vector2::One;
	Vector2 _force = Vector2(50,50);
	Vector2 _aliveZone = Vector2::Zero;

	ALIVE_ZONE_TYPE _aliveZoneType = ALIVE_ZONE_TYPE::NO_DEAD_ZONE;
	POSVAR_TYPE _posvarType = POSVAR_TYPE::EMISSION;


	Vector2 _targetOffset = Vector2::Zero;
	Vector4 _color = Vector4::Zero;
};

