#include "pch.h"
#include "CollisionManager.h"
#include "TitleScene.h"
#include "GameObject.h"
#include "SpriteRenderer.h"
#include "Collider2D.h"
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
TitleScene::TitleScene()
{
}

TitleScene::~TitleScene()
{
}

void TitleScene::Start()
{
	shared_ptr<GameObject> cameraObj = make_shared<GameObject>();
	Camera* cameracomp = cameraObj->AddComponent<Camera>();
	cameracomp->DisableLayerMasks();
	cameracomp->TurnLayerMask(1, true);
	AddGameObject(cameraObj, LAYER_TYPE::CAMERA);

	shared_ptr<GameObject> gridOb = make_shared<GameObject>();
	Grid* grid = gridOb->AddComponent<Grid>();
	AddGameObject(gridOb, LAYER_TYPE::PLAYER);

	GET_SINGLE(CollisionManager)->CollisionLayerCheck(LAYER_TYPE::PLAYER, LAYER_TYPE::MONSTER, true);
	GET_SINGLE(CollisionManager)->CollisionLayerCheck(LAYER_TYPE::PLAYER, LAYER_TYPE::FIXEDOBJECT, true);
	GET_SINGLE(CollisionManager)->CollisionLayerCheck(LAYER_TYPE::MONSTER, LAYER_TYPE::FIXEDOBJECT, true);

	//paint shader
	shared_ptr<ComputeShader> paintShader = GET_SINGLE(Resources)->Find<ComputeShader>(L"TestCompute");
	//L"SmileTexture"
	shared_ptr<Texture> paintTex = GET_SINGLE(Resources)->Find<Texture>(L"PaintTexture");
	paintTex->BindUAV(0);
	paintShader->Dispatch();
	

	{
		shared_ptr<GameObject> ComputObj = make_shared<GameObject>();
		SpriteRenderer* rd = ComputObj->AddComponent<SpriteRenderer>();
		shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->Find<Mesh>(L"RectMesh");
		shared_ptr<Material> material = make_shared<Material>();
		rd->SetOriginSize(Vector2(256, 256));
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Find<Shader>(L"SpriteShader");
		material->SetShader(shader);
		material->SetTexture(0, paintTex);
		rd->SetMesh(mesh);
		rd->SetMaterial(material);
		AddGameObject(ComputObj, LAYER_TYPE::PLAYER);
	}
	 {
		shared_ptr<GameObject> PlayerObj = make_shared<GameObject>();
		SpriteRenderer* rd = PlayerObj->AddComponent<SpriteRenderer>();
		shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->Find<Mesh>(L"RectMesh");
		shared_ptr<Material> material = make_shared<Material>();
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Find<Shader>(L"SpriteShader");
		shared_ptr<Texture> TitleTexture = GET_SINGLE(Resources)->Load<Texture>(L"maid", L"Heroine Maid.png");
		material->SetShader(shader);
		material->SetTexture(0, TitleTexture);
		rd->SetMesh(mesh);
		rd->SetMaterial(material);

		auto rb = PlayerObj->AddComponent<RigidBody>();
		rb->SetGravity(true);
		Animator* anim = PlayerObj->AddComponent<Animator>();

		Collider2D* col = PlayerObj->AddComponent<Collider2D>();
		col->SetType(Collider_TYPE::RECTANGLE);
		col->SetSize(Vector3(1.f, 4.2f, 1.f));
		col->SetLocalCenter(Vector3(0.f, -0.5f, 0.f));

		Collider2D* col2 = PlayerObj->AddComponent<Collider2D>();
		col2->SetType(Collider_TYPE::CIRCLE);
		col2->SetSize(Vector3(0.2f, 0.2f, 1.0));
		col2->SetLocalCenter(Vector3(0.f, -2.5f, 0.f));
		col2->SetTrigger(true);

		Player* player = PlayerObj->AddComponent<Player>();

		AddGameObject(PlayerObj, LAYER_TYPE::PLAYER);

		shared_ptr<GameObject> WeaponObj = make_shared<GameObject>();
		SpriteRenderer* weaponsr = WeaponObj->AddComponent<SpriteRenderer>();
		weaponsr->SetMesh(mesh);
		auto WeaponMaterial = make_shared<Material>();
		weaponsr->SetMaterial(WeaponMaterial);
		shared_ptr<Texture> weapontext = make_shared<Texture>();
		weapontext->Load(L"HolySword.png");
		WeaponMaterial->SetTexture(0, weapontext);
		WeaponMaterial->SetShader(shader);
		Weapon* weapon = WeaponObj->AddComponent<Weapon>();
		weaponsr->SetOriginSize(weapontext->GetSize());
		weaponsr->SetOriginPos(Vector2::Zero);

		PlayerObj->GetTransform()->SetChild(WeaponObj->GetTransform(), L"weapon1");
		AddGameObject(WeaponObj, LAYER_TYPE::PLAYER);

		shared_ptr<GameObject> TrailObj = make_shared<GameObject>();
		TrailObj->AddComponent<MeshRenderer>();
		TrailObj->AddComponent<Trail>();
		WeaponObj->GetTransform()->SetChild(TrailObj->GetTransform(), L"trail");
		AddGameObject(TrailObj, LAYER_TYPE::PLAYER);
#pragma region weapon2
		{
		shared_ptr<GameObject> WeaponObj2 = make_shared<GameObject>();
		SpriteRenderer* weaponsr2 = WeaponObj2->AddComponent<SpriteRenderer>();
		weaponsr2->SetMesh(mesh);
		auto WeaponMaterial2 = make_shared<Material>();
		weaponsr2->SetMaterial(WeaponMaterial2);
		WeaponMaterial2->SetTexture(0, weapontext);
		WeaponMaterial2->SetShader(shader);
		Weapon* weapon2 = WeaponObj2->AddComponent<Weapon>();
		weaponsr2->SetOriginSize(weapontext->GetSize());
		weaponsr2->SetOriginPos(Vector2::Zero);

		PlayerObj->GetTransform()->SetChild(WeaponObj2->GetTransform(), L"weapon2");
		AddGameObject(WeaponObj2, LAYER_TYPE::PLAYER);
		}
#pragma endregion
	}
	{
		shared_ptr<GameObject> ground = make_shared<GameObject>();
		Transform* tr = ground->GetTransform();
		tr->SetScale(Vector3(50.f, 5.f, 1.f));
		tr->SetPosition(Vector3(0.f, -10.f, 0.f));
		tr->SetFixed(true);

		Collider2D* col = ground->AddComponent<Collider2D>();
		col->SetType(Collider_TYPE::RECTANGLE);
		AddGameObject(ground, LAYER_TYPE::FIXEDOBJECT);
	}

	{
		shared_ptr<GameObject> Enemy = make_shared<GameObject>();

		Transform* tr = Enemy->GetTransform();
		tr->SetPosition(Vector3(5.f, -5.f, 0.f));
		tr->SetFixed(true);
		Collider2D* col = Enemy->AddComponent<Collider2D>();
		col->SetTrigger(true);
		col->SetType(Collider_TYPE::RECTANGLE);
		col->SetSize(Vector3(1.5, 1.5f, 1.f));
		AddGameObject(Enemy, LAYER_TYPE::MONSTER);
	}

	Scene::Start();
}
