#pragma once
#include "Script.h"
class Mesh;
class MeshRenderer;
class Material;
class Trail : public Script
{
public:
	Trail(class GameObject* owner);
	~Trail();

public:
	virtual void Start() override;
	virtual void FinalUpdate() override;

	void StartRecord(); 
	void StopRecord();
private:
	UINT curFrame;
	shared_ptr<Mesh> trailMesh;
	vector<Vertex> vertexes;
	vector<UINT> indexes;
	Vector3 _previousTipPosiiton;
	Vector3 _currentTiPosition;

	Transform* tr;
	MeshRenderer* mr;
	shared_ptr<Material> m;
	const UINT trailLength;
	const float WeaponLength;

	bool _record = false;
	float elapsed = 0.f;
	Vector4 previousTipPos;
	Vector4 previousBasePos;
};

