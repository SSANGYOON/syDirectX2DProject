#pragma once
#include "Script.h"
#include "Weapon.h"

class Mesh;
class MeshRenderer;
class Material;


enum class TRAIL_TYPE
{
	HORIZON,
	VERTICAL,
	VERTICALFLIPED,
};
class Trail : public Script
{
public:
	Trail(class GameObject* owner);
	~Trail();

public:
	virtual void Start() override;
	virtual void FinalUpdate() override;

	void SetTrailType(WEAPON_TYPE type);

	void StartRecord(); 
	void StopRecord();
private:
	int curFrame;
	shared_ptr<Mesh> trailMesh;
	vector<Vertex> vertexes;
	vector<UINT> indexes;
	Vector3 _previousTipPosiiton;
	Vector3 _currentTiPosition;

	Transform* tr;
	MeshRenderer* mr;
	shared_ptr<Material> m;
	const UINT trailLength;
	float WeaponLength;

	bool _record = false;
	float elapsed = 0.f;
	TRAIL_TYPE _trailType;
	Vector4 previousTipPos;
	Vector4 previousBasePos;
};

