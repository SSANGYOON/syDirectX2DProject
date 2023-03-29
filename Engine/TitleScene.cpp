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
#pragma region DefaultSetting
	shared_ptr<GameObject> cameraObj = make_shared<GameObject>();
	Camera* cameracomp = cameraObj->AddComponent<Camera>();
	cameracomp->DisableLayerMasks();
	cameracomp->TurnLayerMask(1, true);
	cameracomp->TurnLayerMask(UINT(LAYER_TYPE::MONSTER), true);
	AddGameObject(cameraObj, LAYER_TYPE::CAMERA);

	shared_ptr<GameObject> gridOb = make_shared<GameObject>();
	Grid* grid = gridOb->AddComponent<Grid>();
	AddGameObject(gridOb, LAYER_TYPE::PLAYER);

	GET_SINGLE(CollisionManager)->CollisionLayerCheck(LAYER_TYPE::PLAYER, LAYER_TYPE::MONSTER, true);
	GET_SINGLE(CollisionManager)->CollisionLayerCheck(LAYER_TYPE::PLAYER, LAYER_TYPE::FIXEDOBJECT, true);
	GET_SINGLE(CollisionManager)->CollisionLayerCheck(LAYER_TYPE::MONSTER, LAYER_TYPE::FIXEDOBJECT, true);
#pragma endregion


	shared_ptr<GameObject> PlayerObj = make_shared<GameObject>();
	PlayerObj->AddComponent<Player>();
	AddGameObject(PlayerObj, LAYER_TYPE::PLAYER);

#pragma region Todo
		/*shared_ptr<GameObject> WeaponObj = make_shared<GameObject>();
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
		*/
	 /*{
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
	}*/
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
		tr->SetPosition(Vector3(2.f, -5.f, -1.f));
		tr->SetFixed(true);
		Collider2D* col = Enemy->AddComponent<Collider2D>();
		SpriteRenderer* sr = Enemy->AddComponent<SpriteRenderer>();
		shared_ptr<Material> material = make_shared<Material>();
		material->SetVec4(0, Vector4(0.f, 1.f, 0.f, 1.f));
		material->Load(L"BloomRenderPath.json");
		sr->SetMaterial(material);
		sr->SetMesh(GET_SINGLE(Resources)->Find<Mesh>(L"RectMesh"));
		col->SetTrigger(true);
		col->SetType(Collider_TYPE::RECTANGLE);
		col->SetSize(Vector3(1.5, 1.5f, 1.f));
		AddGameObject(Enemy, LAYER_TYPE::MONSTER);
	}
#pragma endregion
	Scene::Start();
}
