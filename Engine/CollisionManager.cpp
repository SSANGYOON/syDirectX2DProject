#include "pch.h"
#include "CollisionManager.h"
#include "Collider2D.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Layer.h"
#include "GameObject.h"

void CollisionManager::Initialize()
{

}

void CollisionManager::Update()
{
	Scene* scene = GET_SINGLE(SceneManager)->GetActiveScene().get();
	for (UINT row = 0; row < (UINT)LAYER_TYPE::END; row++)
	{
		for (UINT column = 0; column < (UINT)(UINT)LAYER_TYPE::END; column++)
		{
			if (_LayerCollisionMatrix[row][column])
			{
				LayerCollision(scene, (LAYER_TYPE)row, (LAYER_TYPE)column);
			}
		}
	}
}

void CollisionManager::Render()
{
	
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

	for (shared_ptr<GameObject> left : lefts)
	{
		Component* lcol;
		Component* rcol;
		if (left->GetState() != GameObject::ACTIVE)
			continue;
		if ((lcol = left->GetComponent(Component_Type::Collider).get()) == nullptr)
			continue;


		for (shared_ptr<GameObject> right : rights)
		{
			if (right->GetState() != GameObject::ACTIVE)
				continue;
			if ((rcol = right->GetComponent(Component_Type::Collider).get()) == nullptr)
				continue;
			if (left == right)
				continue;

			ColliderCollision(static_cast<Collider*>(lcol), static_cast<Collider*>(rcol));
		}
	}
}

void CollisionManager::ColliderCollision(Collider* left, Collider* right)
{
	ColliderID colliderID;
	colliderID.left = (UINT)left->GetID();
	colliderID.right = (UINT)right->GetID();

	// 이전 충돌 정보를 검색한다.
	// 만약에 충돌정보가 없는 상태라면
	// 충돌정보를 생성해준다.
	std::map<UINT64, bool>::iterator iter = _collisionMap.find(colliderID.id);
	if (iter == _collisionMap.end())
	{
		_collisionMap.insert(std::make_pair(colliderID.id, false));
		iter = _collisionMap.find(colliderID.id);
	}

	// 충돌체크를 해준다.
	if (Intersect(left, right)) // 충돌을 한 상태
	{
		if (iter->second == false)
		{
			
			if (!left->IsTrigger() && !right->IsTrigger())
			{
				right->OnCollisionEnter(left);
				left->OnCollisionEnter(right);
			}
			else if (left->IsTrigger())
				right->OnTriggerEnter(left);
			else
				left->OnTriggerEnter(right);

			iter->second == true;
		}
		else
		{
			if (!left->IsTrigger() && !right->IsTrigger())
			{
				right->OnCollisionStay(left);
				left->OnCollisionStay(right);
			}
			else if (left->IsTrigger())
				right->OnTriggerStay(left);
			else
				left->OnTriggerStay(right);
		}
	}
	else
	{
		if (iter->second == true)
		{
			if (!left->IsTrigger() && !right->IsTrigger())
			{
				right->OnCollisionExit(left);
				left->OnCollisionExit(right);
			}
			else if (left->IsTrigger())
				right->OnTriggerExit(left);
			else
				left->OnTriggerExit(right);
			iter->second == true;;
		}
	}
}

bool CollisionManager::Intersect(Collider* left, Collider* right)
{

	static const Vector3 LocalPos[4] =
	{
		 Vector3{-0.5f, 0.5f, 0.0f}
		,Vector3{0.5f, 0.5f, 0.0f}
		,Vector3{0.5f, -0.5f, 0.0f}
		,Vector3{-0.5f, -0.5f, 0.0f}
	};

	if (left->GetColiderType() == Collider_TYPE::RECTANGLE && right->GetColiderType() == Collider_TYPE::RECTANGLE)
	{
		auto left2d = static_cast<Collider2D*>(left);
		auto right2d = static_cast<Collider2D*>(right);

		Matrix leftMat = left2d->GetMatrix();
		Matrix rightMat = right2d->GetMatrix();

		Vector3	leftPos = leftMat.Translation();
		Vector3	rightPos = rightMat.Translation();

		Vector3 Axis[4] = {};

		Axis[0] = Vector3::Transform(LocalPos[1], leftMat);
		Axis[1] = Vector3::Transform(LocalPos[3], leftMat);
		Axis[2] = Vector3::Transform(LocalPos[1], rightMat);
		Axis[3] = Vector3::Transform(LocalPos[3], rightMat);

		Axis[0] -= Vector3::Transform(LocalPos[0], leftMat);
		Axis[1] -= Vector3::Transform(LocalPos[0], leftMat);
		Axis[2] -= Vector3::Transform(LocalPos[0], rightMat);
		Axis[3] -= Vector3::Transform(LocalPos[0], rightMat);

		

		Vector3 centerdiff = rightPos - leftPos;

		for (size_t i = 0; i < 4; i++)
		{
			Vector3 vA = Axis[i];
			vA.Normalize();

			float projDist = 0.0f;
			for (size_t j = 0; j < 4; j++)
				projDist += fabsf(Axis[j].Dot(vA) / 2.0f);

			if (projDist < fabsf(centerdiff.Dot(vA)))
				return false;
		}
		return true;
	}

	else if (left->GetColiderType() == Collider_TYPE::CIRCLE && right->GetColiderType() == Collider_TYPE::RECTANGLE)
	{
		Collider2D* Circle2d;
		Collider2D* Rect2d;
		if (left->GetColiderType() == Collider_TYPE::RECTANGLE)
		{
			Rect2d = static_cast<Collider2D*>(left);
			Circle2d = static_cast<Collider2D*>(right);
		}
		else
		{
			Rect2d = static_cast<Collider2D*>(right);
			Circle2d = static_cast<Collider2D*>(left);
		}

		Matrix RectMat = Rect2d->GetMatrix();
		Matrix CircleMat = Circle2d->GetMatrix();

		Vector3 RectPos = RectMat.Translation();
		Vector3	CirclePos = CircleMat.Translation();
		
		for (auto& la : LocalPos)
		{
			if (Vector3::Distance(Vector3::Transform(la, RectMat) , CirclePos) < Circle2d->GetRadius())
				return true;
		}
		return false;
	}

	else if (left->GetColiderType() == Collider_TYPE::CIRCLE && right->GetColiderType() == Collider_TYPE::CIRCLE)
	{
		auto Circle1 = static_cast<Collider2D*>(right);
		auto Circle2 = static_cast<Collider2D*>(left);

		Matrix m1 = Circle1->GetMatrix();
		Matrix m2 = Circle2->GetMatrix();

		Vector3 pos1 = m1.Translation();
		Vector3	pos2 = m2.Translation();

		if (Vector3::Distance(pos2, pos1) < Circle1->GetRadius() + Circle2->GetRadius())
			return true;
		else
			return false;

	}
}
