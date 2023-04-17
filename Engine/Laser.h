#pragma once
#include "Script.h"

class Laser : public Script
{
public:
	Laser(GameObject* obj);
	~Laser();

	virtual void Start();
	virtual void FinalUpdate();
	virtual void Render();

	void SetDir(Vector3 dir) { _dir = dir; }

	void Shoot();
	void End();

private:
	bool _using;
	const Vector3 origin;
	Vector3 _dir;

	float fadeTime = 1.f;
	float elapsed = 0.f;

	float laserWidth = 10.f;
	float zoffset = -0.1f;
	Vector4 color = Vector4(1, 0, 1, 1);

	vector<Vertex> _vertexes;
	vector<UINT> _indexes;

	shared_ptr<class Mesh> _mesh;
	shared_ptr<class Material> _material;
};

