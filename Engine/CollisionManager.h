#pragma once
#include "ExpandingSimplex.h"
#define MAX_ITERATIONS 10
#define EPSILON 0.1f
class Collider;
class Scene;

union ColliderID
{
	struct
	{
		UINT32 left;
		UINT32 right;
	};
	UINT64 id;
};

using Simplex = vector<Vector3>;
class CollisionManager
{
DECLARE_SINGLE(CollisionManager);

public:
	void Initialize();
	void Update();

	void CollisionLayerCheck(LAYER_TYPE left, LAYER_TYPE right, bool enable = true);
	void LayerCollision(class Scene* scene, LAYER_TYPE left, LAYER_TYPE right);
	void ColliderCollision(Collider* left, Collider* right);

	void FindPenetration(Simplex& simplex, Collider* left, Collider* right, Vector3& edge, float& depth);
private:
	bool detect(Collider* left, Collider* right, Simplex& simplex);
	bool checkSimplex(Simplex& simplex, Vector3& dir);

private:
	bitset<(UINT)LAYER_TYPE::END> _LayerCollisionMatrix[(UINT)LAYER_TYPE::END] = {};
	map<UINT64, bool> _collisionMap;

};

