#pragma once
#include "Collider.h"

union ColliderID
{
	struct
	{
		UINT32 left;
		UINT32 right;
	};
	UINT64 id;
};

#define EPSILON 1e-4
class CollisionManager
{
DECLARE_SINGLE(CollisionManager);

public:
	void Initialize();
	void Update();

	void CollisionLayerCheck(LAYER_TYPE left, LAYER_TYPE right, bool enable = true);
	void LayerCollision(class Scene* scene, LAYER_TYPE left, LAYER_TYPE right);
	void ColliderCollision(Collider* left, Collider* right);
	void AddCollider(Collider* col, LAYER_TYPE type) { _colliders[UINT(type)].push_back(col); }
	bool RayCast(const Vector3& origin, const Vector3& dir, bitset<(UINT)LAYER_TYPE::END> layer_mask, OUT Collision& collision);

private:
	bitset<(UINT)LAYER_TYPE::END> _LayerCollisionMatrix[(UINT)LAYER_TYPE::END] = {};

	vector<vector<Collider*>> _colliders;
	map<UINT64, bool> _collisionMap;

};

