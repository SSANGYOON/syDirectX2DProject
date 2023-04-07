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

	bool RayCast(const Vector3& origin, const Vector3& dir, LAYER_TYPE type, OUT Collision& collision);

private:
	bitset<(UINT)LAYER_TYPE::END> _LayerCollisionMatrix[(UINT)LAYER_TYPE::END] = {};
	map<UINT64, bool> _collisionMap;

};

