#include "pch.h"
#include "CollisionManager.h"
#include "Collider2D.h"
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
	Scene* scene = GET_SINGLE(SceneManager)->GetActiveScene().get();
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
						ColliderCollision(lcol.get(), rcol.get());
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
						ColliderCollision(lcol.get(), rcol.get());
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

	vector<Vector3> simplex;
	shared_ptr<GameObject> leftob = left->GetOwner();
	shared_ptr<GameObject> rightob = right->GetOwner();
	if (detect(left, right, simplex))
	{
		if (!left->IsTrigger() && !right->IsTrigger())
		{
			

			Vector3 normal;
			float depth;
			FindPenetration(simplex, left, right, normal, depth);

			auto leftTr = leftob->GetTransform();
			leftTr->Translate(- normal * (depth));

			auto rightTr = rightob->GetTransform();
			rightTr->Translate(normal * (depth));

			shared_ptr<RigidBody> leftRb = leftob->GetComponent<RigidBody>();
			shared_ptr<RigidBody> rightRb = rightob->GetComponent<RigidBody>();
			if (leftRb)
			{
				Vector3 velocity = leftRb->GetVelocity();
				velocity -= velocity.Dot(normal) * normal;
				leftRb->SetVelocity(velocity);
			}
			if (rightRb)
			{
				Vector3 velocity = rightRb->GetVelocity();
				velocity -= velocity.Dot(normal) * normal;
				rightRb->SetVelocity(velocity);
			}
				
		}
		
		if (!iter->second)
		{
			if (!left->IsTrigger() && !right->IsTrigger())
			{
				rightob->OnCollisionEnter(left);
				leftob->OnCollisionEnter(right);
			}
			else if (left->IsTrigger())
				leftob->OnTriggerEnter(right);
			else
				rightob->OnTriggerEnter(left);

			iter->second = true;
		}
		else
		{
			if (!left->IsTrigger() && !right->IsTrigger())
			{
				rightob->OnCollisionStay(left);
				leftob->OnCollisionStay(right);
			}
			else if (left->IsTrigger())
				leftob->OnTriggerStay(right);
			else
				rightob->OnTriggerStay(left);
		}
	}
	else
	{
		if (iter->second)
		{
			if (!left->IsTrigger() && !right->IsTrigger())
			{
				rightob->OnCollisionExit(left);
				leftob->OnCollisionExit(right);
			}
			else if (left->IsTrigger())
				leftob->OnTriggerExit(right);
			else
				rightob->OnTriggerExit(left);
			iter->second = false;
		}
	}
}

bool CollisionManager::detect(Collider* left, Collider* right, Simplex& simplex)
{
	Vector3 dir = left->GetCenter() - right->GetCenter();
	if (dir.LengthSquared() < EPSILON)
		dir = Vector3(1.0f, 0.f, 0.f);

	simplex.push_back(left->GetFarthestPoint(dir) - right->GetFarthestPoint(-dir));

	if (simplex[0].Dot(dir) <= 0.0f)
		return false;

	dir *= -1.0;

	while (true) {
		simplex.push_back(left->GetFarthestPoint(dir) - right->GetFarthestPoint(-dir));

		Vector3 lastPoint = simplex.back();
		if (lastPoint.Dot(dir) <= 0.f)
			return false;
		else 
		{
			if (checkSimplex(simplex, dir))
				return true;
		}
	}
}

bool CollisionManager::checkSimplex(Simplex& simplex, Vector3& dir)
{
	Vector3 a = simplex.back();
	Vector3 ao = -a;

	if (simplex.size() == 3)
	{
		Vector3 b = simplex[1];
		Vector3 c = simplex[0];

		Vector3 ab = b - a;
		Vector3 ac = c - a;

		Vector3 abPerp = ac.Cross(ab).Cross(ab);
		Vector3 acPerp = ab.Cross(ac).Cross(ac);

		float acLocation = acPerp.Dot(ao);

		if (acLocation >= 0.0f) {
			simplex.erase(simplex.begin() + 1);
			dir = acPerp;
		}
		else {
			float abLocation = abPerp.Dot(ao);
			if (abLocation < 0.0f) {
				return true;
			}
			else {
				simplex.erase(simplex.begin());
				dir = abPerp;
			}
		}
	}
	else {

		Vector3 b = simplex[0];
		Vector3 ab = b - a;
		dir = ab.Cross(ao).Cross(ab);
	}

	dir.Normalize();
	return false;
}

void CollisionManager::FindPenetration(Simplex& simplex, Collider* left, Collider* right, Vector3& normal, float& depth)
{
	ExpandingSimplex expandingSimplex(simplex);

	for (int i = 0; i < 10; i++)
	{
		shared_ptr < SimplexEdge > edge = expandingSimplex.findClosestEdge();
		normal = edge->getNormal();
		Vector3 point = left->GetFarthestPoint(normal) -right->GetFarthestPoint(-normal);
		depth = abs(point.Dot(normal));
		if (depth - edge->getDistance() < EPSILON)
			return;
		expandingSimplex.expand(point);
	}
	return;
}
