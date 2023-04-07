#include "pch.h"
#include "CollisionManager.h"
#include "Collider.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Layer.h"
#include "GameObject.h"
#include "RigidBody.h"

void CollisionManager::Initialize()
{

}

void CollisionManager::Update()
{
	Scene* scene = GET_SINGLE(SceneManager)->GetActiveScene();
	for (UINT row = 0; row < (UINT)LAYER_TYPE::END; row++)
	{
		for (UINT column = row; column < (UINT)(UINT)LAYER_TYPE::END; column++)
		{
			if (_LayerCollisionMatrix[row][column])
				LayerCollision(scene, (LAYER_TYPE)row, (LAYER_TYPE)column);
		}
	}
}

void CollisionManager::CollisionLayerCheck(LAYER_TYPE left, LAYER_TYPE right, bool enable)
{
	int row = 0;
	int column = 0;

	if ((UINT)left <= (UINT)right)
	{
		row = (UINT)left;
		column = (UINT)right;
	}
	else
	{
		row = (UINT)right;
		column = (UINT)left;
	}

	_LayerCollisionMatrix[row][column] = enable;
}

void CollisionManager::LayerCollision(Scene* scene, LAYER_TYPE left, LAYER_TYPE right)
{
	const std::vector<shared_ptr<GameObject>>& lefts = scene->GetLayer(left).GetGameObjects();
	const std::vector<shared_ptr<GameObject>>& rights = scene->GetLayer(right).GetGameObjects();

	if (left == right)
	{
		for (size_t i =0; i<lefts.size()-1; i++)
		{
			if (lefts[i]->GetState() != GameObject::ACTIVE)
				continue;
			auto lcols = lefts[i]->GetComponents<Collider>();
			for (const auto& lcol : lcols)
			{
				for (size_t j = i+1; j < rights.size(); j++)
				{
					if (lefts[j]->GetState() != GameObject::ACTIVE)
						continue;
					auto rcols = rights[j]->GetComponents<Collider>();
					for (const auto& rcol : rcols)
					{
						ColliderCollision(lcol, rcol);
					}
				}
			}
		}
	}
	else
	{
		for (const auto& left : lefts)
		{
			if (left->GetState() != GameObject::ACTIVE)
				continue;
			auto lcols = left->GetComponents<Collider>();
			for (const auto& lcol : lcols)
			{
				for (const auto& right : rights)
				{
					if (right->GetState() != GameObject::ACTIVE)
						continue;
					auto rcols = right->GetComponents<Collider>();
					for (const auto& rcol : rcols)
					{
						ColliderCollision(lcol, rcol);
					}
				}
			}
		}
	}
}

void CollisionManager::ColliderCollision(Collider* left, Collider* right)
{
	ColliderID colliderID;


	colliderID.left = (UINT)left->GetID();
	colliderID.right = (UINT)right->GetID();

	std::map<UINT64, bool>::iterator iter = _collisionMap.find(colliderID.id);
	if (iter == _collisionMap.end())
	{
		_collisionMap.insert(std::make_pair(colliderID.id, false));
		iter = _collisionMap.find(colliderID.id);
	}

	GameObject* leftob = left->GetOwner();
	GameObject* rightob = right->GetOwner();

	Vector3 dis;
	if (left->Intersects(right,dis))
	{
		if (!left->IsTrigger() && !right->IsTrigger())
		{

			auto leftTr = leftob->GetTransform();
			leftTr->Translate(-dis);

			auto rightTr = rightob->GetTransform();
			rightTr->Translate(dis);

			RigidBody* leftRb = leftob->GetComponent<RigidBody>();
			RigidBody* rightRb = rightob->GetComponent<RigidBody>();
			if (leftRb)
			{
				Vector3 velocity = leftRb->GetVelocity();
				if (velocity.Dot(dis) > 0.f) {
					velocity -= velocity.Dot(dis) * dis / dis.LengthSquared();
					leftRb->SetVelocity(velocity);
				}
			}
			if (rightRb)
			{
				Vector3 velocity = rightRb->GetVelocity();
				if (velocity.Dot(dis) < 0.f) {
					velocity -= velocity.Dot(dis) * dis / dis.LengthSquared();
					rightRb->SetVelocity(velocity);
				}
			}
				
		}
		
		if (!iter->second)
		{
			if (!left->IsTrigger() && !right->IsTrigger())
			{
				rightob->OnCollisionEnter({dis, leftob});
				leftob->OnCollisionEnter({-dis,rightob});
			}
			else 
			{
				rightob->OnTriggerEnter({ dis, leftob });
				leftob->OnTriggerEnter({ -dis,rightob });
			}

			iter->second = true;
		}
		else
		{
			if (!left->IsTrigger() && !right->IsTrigger())
			{
				rightob->OnCollisionStay({ dis, leftob });
				leftob->OnCollisionStay({ -dis,rightob });
			}
			else
			{
				rightob->OnTriggerStay({ dis, leftob });
				leftob->OnTriggerStay({ -dis,rightob });
			}
		}
	}
	else
	{
		if (iter->second)
		{
			if (!left->IsTrigger() && !right->IsTrigger())
			{
				rightob->OnCollisionExit({ dis, leftob });
				leftob->OnCollisionExit({ -dis,rightob });
			}
			else
			{
				rightob->OnTriggerExit({ dis, leftob });
				leftob->OnTriggerExit({ -dis,rightob });
			}
			iter->second = false;
		}
	}
}

bool CollisionManager::RayCast(const Vector3& origin, const Vector3& dir, LAYER_TYPE type, OUT Collision& collision)
{
	const std::vector<shared_ptr<GameObject>>& objs = GET_SINGLE(SceneManager)->GetActiveScene()->GetLayer(type).GetGameObjects();

	bool ret = false;
	float len = 1e9;
	GameObject* target = nullptr;

	for (auto& obj : objs)
	{
		auto cols = obj->GetComponents<Collider>();

		for (const auto col : cols)
		{
			if (!col->IsTrigger()) {
				float dist = 1e9;
				bool hit = col->RayCast(origin, dir, OUT dist);
				if (hit) {
					ret = true;
					if (len > dist) {
						target = obj.get();
						len = dist;
					}
				}
			}
		}
	}
	collision.other = target;
	collision.repulse = dir * len;

	return ret;
}
