#include "pch.h"
#include "CollisionManager.h"
#include "TitleScene.h"
#include "GameObject.h"
#include "SpriteRenderer.h"
#include "RectCollider.h"
#include "CircleCollider.h"
#include "RigidBody.h"
#include "Resources.h"
#include "Shader.h"
#include "ComputeShader.h"
#include "Texture.h"
#include "Camera.h"
#include "Animator.h"
#include "Grid.h"
#include "Player.h"
#include "Texture.h"
#include "Weapon.h"
#include "MeshRenderer.h"
#include "Trail.h"
#include "DarkLady.h"

TitleScene::TitleScene()
{
}

TitleScene::~TitleScene()
{
}

void TitleScene::Start()
{
#pragma region DefaultSetting
	shared_ptr<GameObject> cameraObj = make_shared<GameObject>();
	Camera* cameracomp = cameraObj->AddComponent<Camera>();
	cameraObj->GetTransform()->SetPosition(Vector3(0.f, 0.f, -100.f));
	cameracomp->DisableLayerMasks();
	cameracomp->TurnLayerMask(UINT(LAYER_TYPE::MONSTER), true);
	cameracomp->TurnLayerMask(UINT(LAYER_TYPE::PLAYER), true);

	cameracomp->TurnLayerMask(UINT(LAYER_TYPE::PLAYER_WEAPON), true);
	AddGameObject(cameraObj, LAYER_TYPE::CAMERA);

	//shared_ptr<GameObject> gridOb = make_shared<GameObject>();
	//gridOb->AddComponent<Grid>();
	//AddGameObject(gridOb, LAYER_TYPE::UI);

	GET_SINGLE(CollisionManager)->CollisionLayerCheck(LAYER_TYPE::PLAYER, LAYER_TYPE::MONSTER, true);
	GET_SINGLE(CollisionManager)->CollisionLayerCheck(LAYER_TYPE::PLAYER, LAYER_TYPE::FIXEDOBJECT, true);
	GET_SINGLE(CollisionManager)->CollisionLayerCheck(LAYER_TYPE::MONSTER, LAYER_TYPE::FIXEDOBJECT, true);
#pragma endregion


	shared_ptr<GameObject> PlayerObj = make_shared<GameObject>();
	PlayerObj->AddComponent<Player>();
	AddGameObject(PlayerObj, LAYER_TYPE::PLAYER);

	{
		shared_ptr<GameObject> ground = make_shared<GameObject>();
		Transform* tr = ground->GetTransform();
		tr->SetScale(Vector3(500.f, 50.f, 1.f));
		tr->SetPosition(Vector3(0.f, -100.f, 0.f));
		tr->SetFixed(true);

		RectCollider* col = ground->AddComponent<RectCollider>();
		AddGameObject(ground, LAYER_TYPE::FIXEDOBJECT);
	}

	/*{
		shared_ptr<GameObject> Enemy = make_shared<GameObject>();
		Transform* tr = Enemy->GetTransform();
		tr->SetPosition(Vector3(2.f, -5.f, 0.f));
		tr->SetFixed(true);
		Collider2D* col = Enemy->AddComponent<Collider2D>();
		SpriteRenderer* sr = Enemy->AddComponent<SpriteRenderer>();
		shared_ptr<Material> material = make_shared<Material>();
		material->SetColor(0, Vector4(0.f, 1.f, 0.f, 1.f));
		material->Load(L"OneHandShader.json");
		sr->SetMaterial(material);
		sr->SetMesh(GET_SINGLE(Resources)->Find<Mesh>(L"RectMesh"));
		col->SetTrigger(true);
		col->SetType(Collider_TYPE::RECTANGLE);
		col->SetSize(Vector3(1.5, 1.5f, 1.f));
		AddGameObject(Enemy, LAYER_TYPE::MONSTER);
	}*/
	{
		shared_ptr<GameObject> darkLady = make_shared<GameObject>();
		darkLady->AddComponent<DarkLady>();
		AddGameObject(darkLady, LAYER_TYPE::MONSTER);
	}
#pragma endregion
	Scene::Start();
}
