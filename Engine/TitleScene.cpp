#include "pch.h"
#include "CollisionManager.h"
#include "TitleScene.h"
#include "GameObject.h"
#include "RectCollider.h"
#include "Camera.h"
#include "Grid.h"
#include "Player.h"
#include "DarkLady.h"
#include "CameraController.h"
#include "SpriteRenderer.h"
#include "Resources.h"
#include "Texture.h"
#include "UI.h"
#include "UIImage.h"
#include "Shader.h"
#include "Material.h"

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
	cameraObj->AddComponent<CameraController>();
	cameraObj->GetTransform()->SetPosition(Vector3(0.f, 0.f, -100.f));

	cameracomp->DisableLayerMasks();
	cameracomp->TurnLayerMask(UINT(LAYER_TYPE::BACKGROUND), true);
	cameracomp->TurnLayerMask(UINT(LAYER_TYPE::MONSTER), true);
	cameracomp->TurnLayerMask(UINT(LAYER_TYPE::PLAYER), true);
	cameracomp->TurnLayerMask(UINT(LAYER_TYPE::PLAYER_WEAPON), true);
	cameracomp->TurnLayerMask(UINT(LAYER_TYPE::FIXEDOBJECT), true);

	AddGameObject(cameraObj, LAYER_TYPE::CAMERA);
	shared_ptr<GameObject> uicameraObj = make_shared<GameObject>();
	Camera* uicamera = uicameraObj->AddComponent<Camera>();
	cameraObj->GetTransform()->SetPosition(Vector3(0.f, 0.f, -100.f));

	uicamera->DisableLayerMasks();
	uicamera->TurnLayerMask(UINT(LAYER_TYPE::UI), true);

	AddGameObject(uicameraObj, LAYER_TYPE::CAMERA);
	
	SetSceneSize(Vector2(960.f, 270.f));

	shared_ptr<GameObject> sky = make_shared<GameObject>();
	auto skySr = sky->AddComponent<SpriteRenderer>();
	skySr->SetSpriteSheet(GET_SINGLE(Resources)->Load<Texture>(L"SkyBox", L"FS017_Day_Sunless.png"));

	AddGameObject(sky, LAYER_TYPE::BACKGROUND);
	sky->GetTransform()->SetPosition(Vector3(0, 0, 50.f));

	GET_SINGLE(CollisionManager)->CollisionLayerCheck(LAYER_TYPE::PLAYER, LAYER_TYPE::MONSTER, true);
	GET_SINGLE(CollisionManager)->CollisionLayerCheck(LAYER_TYPE::PLAYER, LAYER_TYPE::FIXEDOBJECT, true);
	GET_SINGLE(CollisionManager)->CollisionLayerCheck(LAYER_TYPE::MONSTER, LAYER_TYPE::FIXEDOBJECT, true);
#pragma endregion


	shared_ptr<GameObject> PlayerObj = make_shared<GameObject>();
	PlayerObj->AddComponent<Player>();
	AddGameObject(PlayerObj, LAYER_TYPE::PLAYER, "Player");

	{
		shared_ptr<GameObject> ground = make_shared<GameObject>();
		Transform* tr = ground->GetTransform();
		tr->SetScale(Vector3(960.f, 50.f, 1.f));
		tr->SetPosition(Vector3(0.f, -85.f, 0.f));
		tr->SetFixed(true);

		RectCollider* col = ground->AddComponent<RectCollider>();
		AddGameObject(ground, LAYER_TYPE::FIXEDOBJECT);
	}
	{
		shared_ptr<GameObject> ground1 = make_shared<GameObject>();
		Transform* tr1 = ground1->GetTransform();
		tr1->SetPosition(Vector3(-464.f, 90.f, 0.f));
		tr1->SetFixed(true);

		auto sr1 = ground1->AddComponent<SpriteRenderer>();
		sr1->SetSpriteSheet(GET_SINGLE(Resources)->Load<Texture>(L"LastTop1", L"LastTop/LastTop1.png"));
		AddGameObject(ground1, LAYER_TYPE::FIXEDOBJECT);
		RectCollider* col = ground1->AddComponent<RectCollider>();
		col->SetLocalCenter(Vector3(144, -180.f, 0));
		col->SetSize(Vector3(320.f, 360.f, 1.f));

	}
	{
		shared_ptr<GameObject> ground = make_shared<GameObject>();
		Transform* tr = ground->GetTransform();
		tr->SetPosition(Vector3(-432.f, 90.f, 0.f));
		tr->SetFixed(true);

		auto sr = ground->AddComponent<SpriteRenderer>();
		sr->SetSpriteSheet(GET_SINGLE(Resources)->Load<Texture>(L"LastTop5", L"LastTop/LastTop5.png"));
		AddGameObject(ground, LAYER_TYPE::FIXEDOBJECT);
	}
	{
		shared_ptr<GameObject> ground = make_shared<GameObject>();
		Transform* tr = ground->GetTransform();
		tr->SetPosition(Vector3(-400.f, 90.f, 0.f));
		tr->SetFixed(true);

		auto sr = ground->AddComponent<SpriteRenderer>();
		sr->SetSpriteSheet(GET_SINGLE(Resources)->Load<Texture>(L"LastTop3", L"LastTop/LastTop3.png"));
		AddGameObject(ground, LAYER_TYPE::FIXEDOBJECT);
	}
	{
		shared_ptr<GameObject> ground = make_shared<GameObject>();
		Transform* tr = ground->GetTransform();
		tr->SetPosition(Vector3(-368.f, 90.f, 0.f));
		tr->SetFixed(true);

		auto sr = ground->AddComponent<SpriteRenderer>();
		sr->SetSpriteSheet(GET_SINGLE(Resources)->Load<Texture>(L"LastTop3", L"LastTop/LastTop3.png"));
		AddGameObject(ground, LAYER_TYPE::FIXEDOBJECT);
	}
	{
		shared_ptr<GameObject> ground = make_shared<GameObject>();
		Transform* tr = ground->GetTransform();
		tr->SetPosition(Vector3(-336.f, 90.f, 0.f));
		tr->SetFixed(true);

		auto sr = ground->AddComponent<SpriteRenderer>();
		sr->SetSpriteSheet(GET_SINGLE(Resources)->Load<Texture>(L"LastTop5", L"LastTop/LastTop5.png"));
		AddGameObject(ground, LAYER_TYPE::FIXEDOBJECT);
	}
	{
		shared_ptr<GameObject> ground = make_shared<GameObject>();
		Transform* tr = ground->GetTransform();
		tr->SetPosition(Vector3(-304.f, 90.f, 0.f));
		tr->SetFixed(true);

		auto sr = ground->AddComponent<SpriteRenderer>();
		sr->SetSpriteSheet(GET_SINGLE(Resources)->Load<Texture>(L"LastTop2", L"LastTop/LastTop2.png"));
		AddGameObject(ground, LAYER_TYPE::FIXEDOBJECT);
	}
	{
		shared_ptr<GameObject> ground = make_shared<GameObject>();
		Transform* tr = ground->GetTransform();
		tr->SetPosition(Vector3(-272.f, 90.f, 0.f));
		tr->SetFixed(true);

		auto sr = ground->AddComponent<SpriteRenderer>();
		sr->SetSpriteSheet(GET_SINGLE(Resources)->Load<Texture>(L"LastTop1", L"LastTop/LastTop1.png"));
		AddGameObject(ground, LAYER_TYPE::FIXEDOBJECT);
	}
	{
		shared_ptr<GameObject> ground = make_shared<GameObject>();
		Transform* tr = ground->GetTransform();
		tr->SetPosition(Vector3(-240.f, 90.f, 0.f));
		tr->SetFixed(true);

		auto sr = ground->AddComponent<SpriteRenderer>();
		sr->SetSpriteSheet(GET_SINGLE(Resources)->Load<Texture>(L"LastTop3", L"LastTop/LastTop3.png"));
		AddGameObject(ground, LAYER_TYPE::FIXEDOBJECT);
	}
	{
		shared_ptr<GameObject> ground = make_shared<GameObject>();
		Transform* tr = ground->GetTransform();
		tr->SetPosition(Vector3(-208.f, 90.f, 0.f));
		tr->SetFixed(true);

		auto sr = ground->AddComponent<SpriteRenderer>();
		sr->SetSpriteSheet(GET_SINGLE(Resources)->Load<Texture>(L"LastTop3", L"LastTop/LastTop3.png"));
		AddGameObject(ground, LAYER_TYPE::FIXEDOBJECT);
	}
	{
		shared_ptr<GameObject> ground = make_shared<GameObject>();
		Transform* tr = ground->GetTransform();
		tr->SetPosition(Vector3(-176.f, 74.f, 0.f));
		tr->SetScale(Vector3(-1, 1.f, 1.f));
		tr->SetFixed(true);

		auto sr = ground->AddComponent<SpriteRenderer>();
		sr->SetSpriteSheet(GET_SINGLE(Resources)->Load<Texture>(L"LastTopCorner0", L"LastTop/LastTopCorner0.png"));
		AddGameObject(ground, LAYER_TYPE::FIXEDOBJECT);
	}
	{
		shared_ptr<GameObject> blackWall = make_shared<GameObject>();
		Transform* tr = blackWall->GetTransform();
		tr->SetPosition(Vector3(-320.f, -90.f, 1.f));
		tr->SetScale(Vector3(320.f, 360.f, 1.f));
		tr->SetFixed(true);

		auto sr = blackWall->AddComponent<SpriteRenderer>();
		AddGameObject(blackWall, LAYER_TYPE::FIXEDOBJECT);
	}
	{
		shared_ptr<GameObject> ground = make_shared<GameObject>();
		Transform* tr = ground->GetTransform();
		tr->SetPosition(Vector3(-176.f, 74.f, 0.f));
		tr->SetScale(Vector3(-1, 1.f, 1.f));
		tr->SetFixed(true);

		auto sr = ground->AddComponent<SpriteRenderer>();
		sr->SetSpriteSheet(GET_SINGLE(Resources)->Load<Texture>(L"LastTopCorner0", L"LastTop/LastTopCorner0.png"));
		AddGameObject(ground, LAYER_TYPE::FIXEDOBJECT);
	}
	{
		shared_ptr<GameObject> bridge = make_shared<GameObject>();
		Transform* tr = bridge->GetTransform();
		tr->SetPosition(Vector3(-144.f, -60.f, 0.f));
		tr->SetScale(Vector3(1, 1.f, 1.f));
		tr->SetFixed(true);

		auto sr = bridge->AddComponent<SpriteRenderer>();
		sr->SetSpriteSheet(GET_SINGLE(Resources)->Load<Texture>(L"brigeCorner", L"LastTop/SewerBossOneWayCorner.png"));
		AddGameObject(bridge, LAYER_TYPE::FIXEDOBJECT);
	}
	{
		shared_ptr<GameObject> bridge = make_shared<GameObject>();
		Transform* tr = bridge->GetTransform();
		tr->SetPosition(Vector3(-112.f, -72.f, 0.f));
		tr->SetScale(Vector3(1, 1.f, 1.f));
		tr->SetFixed(true);

		auto sr = bridge->AddComponent<SpriteRenderer>();
		sr->SetSpriteSheet(GET_SINGLE(Resources)->Load<Texture>(L"brige", L"LastTop/SewerBossOneWayTop.png"));
		AddGameObject(bridge, LAYER_TYPE::FIXEDOBJECT);
	}
	{
		shared_ptr<GameObject> bridge = make_shared<GameObject>();
		Transform* tr = bridge->GetTransform();
		tr->SetPosition(Vector3(-112.f, -72.f, 0.f));
		tr->SetScale(Vector3(1, 1.f, 1.f));
		tr->SetFixed(true);

		auto sr = bridge->AddComponent<SpriteRenderer>();
		sr->SetSpriteSheet(GET_SINGLE(Resources)->Load<Texture>(L"brige", L"LastTop/SewerBossOneWayTop.png"));
		AddGameObject(bridge, LAYER_TYPE::FIXEDOBJECT);
	}
	{
		shared_ptr<GameObject> bridge = make_shared<GameObject>();
		Transform* tr = bridge->GetTransform();
		tr->SetPosition(Vector3(-80.f, -72.f, 0.f));
		tr->SetScale(Vector3(1, 1.f, 1.f));
		tr->SetFixed(true);

		auto sr = bridge->AddComponent<SpriteRenderer>();
		sr->SetSpriteSheet(GET_SINGLE(Resources)->Load<Texture>(L"brige", L"LastTop/SewerBossOneWayTop.png"));
		AddGameObject(bridge, LAYER_TYPE::FIXEDOBJECT);
	}
	{
		shared_ptr<GameObject> bridge = make_shared<GameObject>();
		Transform* tr = bridge->GetTransform();
		tr->SetPosition(Vector3(-48.f, -72.f, 0.f));
		tr->SetScale(Vector3(1, 1.f, 1.f));
		tr->SetFixed(true);

		auto sr = bridge->AddComponent<SpriteRenderer>();
		sr->SetSpriteSheet(GET_SINGLE(Resources)->Load<Texture>(L"brige", L"LastTop/SewerBossOneWayTop.png"));
		AddGameObject(bridge, LAYER_TYPE::FIXEDOBJECT);
	}
	{
		shared_ptr<GameObject> bridge = make_shared<GameObject>();
		Transform* tr = bridge->GetTransform();
		tr->SetPosition(Vector3(-16.f, -72.f, 0.f));
		tr->SetScale(Vector3(1, 1.f, 1.f));
		tr->SetFixed(true);

		auto sr = bridge->AddComponent<SpriteRenderer>();
		sr->SetSpriteSheet(GET_SINGLE(Resources)->Load<Texture>(L"brige", L"LastTop/SewerBossOneWayTop.png"));
		AddGameObject(bridge, LAYER_TYPE::FIXEDOBJECT);
	}
	{
		shared_ptr<GameObject> bridge = make_shared<GameObject>();
		Transform* tr = bridge->GetTransform();
		tr->SetPosition(Vector3(16.f, -72.f, 0.f));
		tr->SetScale(Vector3(1, 1.f, 1.f));
		tr->SetFixed(true);

		auto sr = bridge->AddComponent<SpriteRenderer>();
		sr->SetSpriteSheet(GET_SINGLE(Resources)->Load<Texture>(L"brige", L"LastTop/SewerBossOneWayTop.png"));
		AddGameObject(bridge, LAYER_TYPE::FIXEDOBJECT);
	}
	{
		shared_ptr<GameObject> bridge = make_shared<GameObject>();
		Transform* tr = bridge->GetTransform();
		tr->SetPosition(Vector3(48.f, -72.f, 0.f));
		tr->SetScale(Vector3(1, 1.f, 1.f));
		tr->SetFixed(true);

		auto sr = bridge->AddComponent<SpriteRenderer>();
		sr->SetSpriteSheet(GET_SINGLE(Resources)->Load<Texture>(L"brige", L"LastTop/SewerBossOneWayTop.png"));
		AddGameObject(bridge, LAYER_TYPE::FIXEDOBJECT);
	}
	{
		shared_ptr<GameObject> bridge = make_shared<GameObject>();
		Transform* tr = bridge->GetTransform();
		tr->SetPosition(Vector3(80.f, -72.f, 0.f));
		tr->SetScale(Vector3(1, 1.f, 1.f));
		tr->SetFixed(true);

		auto sr = bridge->AddComponent<SpriteRenderer>();
		sr->SetSpriteSheet(GET_SINGLE(Resources)->Load<Texture>(L"brige", L"LastTop/SewerBossOneWayTop.png"));
		AddGameObject(bridge, LAYER_TYPE::FIXEDOBJECT);
	}
	{
		shared_ptr<GameObject> bridge = make_shared<GameObject>();
		Transform* tr = bridge->GetTransform();
		tr->SetPosition(Vector3(112.f, -72.f, 0.f));
		tr->SetScale(Vector3(1, 1.f, 1.f));
		tr->SetFixed(true);

		auto sr = bridge->AddComponent<SpriteRenderer>();
		sr->SetSpriteSheet(GET_SINGLE(Resources)->Load<Texture>(L"brige", L"LastTop/SewerBossOneWayTop.png"));
		AddGameObject(bridge, LAYER_TYPE::FIXEDOBJECT);
	}
	{
		shared_ptr<GameObject> bridge = make_shared<GameObject>();
		Transform* tr = bridge->GetTransform();
		tr->SetPosition(Vector3(144.f, -72.f, 0.f));
		tr->SetScale(Vector3(1, 1.f, 1.f));
		tr->SetFixed(true);

		auto sr = bridge->AddComponent<SpriteRenderer>();
		sr->SetSpriteSheet(GET_SINGLE(Resources)->Load<Texture>(L"brige", L"LastTop/SewerBossOneWayTop.png"));
		AddGameObject(bridge, LAYER_TYPE::FIXEDOBJECT);
	}
	{
		shared_ptr<GameObject> bridge = make_shared<GameObject>();
		Transform* tr = bridge->GetTransform();
		tr->SetPosition(Vector3(176.f, -72.f, 0.f));
		tr->SetScale(Vector3(1, 1.f, 1.f));
		tr->SetFixed(true);

		auto sr = bridge->AddComponent<SpriteRenderer>();
		sr->SetSpriteSheet(GET_SINGLE(Resources)->Load<Texture>(L"brige", L"LastTop/SewerBossOneWayTop.png"));
		AddGameObject(bridge, LAYER_TYPE::FIXEDOBJECT);
	}
	{
		shared_ptr<GameObject> bridge = make_shared<GameObject>();
		Transform* tr = bridge->GetTransform();
		tr->SetPosition(Vector3(208.f, -72.f, 0.f));
		tr->SetScale(Vector3(1, 1.f, 1.f));
		tr->SetFixed(true);

		auto sr = bridge->AddComponent<SpriteRenderer>();
		sr->SetSpriteSheet(GET_SINGLE(Resources)->Load<Texture>(L"brige", L"LastTop/SewerBossOneWayTop.png"));
		AddGameObject(bridge, LAYER_TYPE::FIXEDOBJECT);
	}
	{
		shared_ptr<GameObject> bridge = make_shared<GameObject>();
		Transform* tr = bridge->GetTransform();
		tr->SetPosition(Vector3(240.f, -72.f, 0.f));
		tr->SetScale(Vector3(1, 1.f, 1.f));
		tr->SetFixed(true);

		auto sr = bridge->AddComponent<SpriteRenderer>();
		sr->SetSpriteSheet(GET_SINGLE(Resources)->Load<Texture>(L"brige", L"LastTop/SewerBossOneWayTop.png"));
		AddGameObject(bridge, LAYER_TYPE::FIXEDOBJECT);
	}
	{
		shared_ptr<GameObject> bridge = make_shared<GameObject>();
		Transform* tr = bridge->GetTransform();
		tr->SetPosition(Vector3(272.f, -72.f, 0.f));
		tr->SetScale(Vector3(1, 1.f, 1.f));
		tr->SetFixed(true);

		auto sr = bridge->AddComponent<SpriteRenderer>();
		sr->SetSpriteSheet(GET_SINGLE(Resources)->Load<Texture>(L"brige", L"LastTop/SewerBossOneWayTop.png"));
		AddGameObject(bridge, LAYER_TYPE::FIXEDOBJECT);
	}
	{
		shared_ptr<GameObject> bridge = make_shared<GameObject>();
		Transform* tr = bridge->GetTransform();
		tr->SetPosition(Vector3(304.f, -72.f, 0.f));
		tr->SetScale(Vector3(1, 1.f, 1.f));
		tr->SetFixed(true);

		auto sr = bridge->AddComponent<SpriteRenderer>();
		sr->SetSpriteSheet(GET_SINGLE(Resources)->Load<Texture>(L"brige", L"LastTop/SewerBossOneWayTop.png"));
		AddGameObject(bridge, LAYER_TYPE::FIXEDOBJECT);
	}
	{
		shared_ptr<GameObject> bridge = make_shared<GameObject>();
		Transform* tr = bridge->GetTransform();
		tr->SetPosition(Vector3(336.f, -60.f, 0.f));
		tr->SetScale(Vector3(-1, 1.f, 1.f));
		tr->SetFixed(true);

		auto sr = bridge->AddComponent<SpriteRenderer>();
		sr->SetSpriteSheet(GET_SINGLE(Resources)->Find<Texture>(L"brigeCorner"));
		AddGameObject(bridge, LAYER_TYPE::FIXEDOBJECT);
	}
	{
		shared_ptr<GameObject> ground = make_shared<GameObject>();
		Transform* tr = ground->GetTransform();
		tr->SetPosition(Vector3(368.f, -76.f, 0.f));
		tr->SetScale(Vector3(1, 1.f, 1.f));
		tr->SetFixed(true);

		auto sr = ground->AddComponent<SpriteRenderer>();
		sr->SetSpriteSheet(GET_SINGLE(Resources)->Load<Texture>(L"LastTopCorner1", L"LastTop/LastTopCorner0.png"));
		AddGameObject(ground, LAYER_TYPE::FIXEDOBJECT);
	}
	{
		shared_ptr<GameObject> ground = make_shared<GameObject>();
		Transform* tr = ground->GetTransform();
		tr->SetPosition(Vector3(400.f, -60.f, 0.f));
		tr->SetFixed(true);

		auto sr = ground->AddComponent<SpriteRenderer>();
		sr->SetSpriteSheet(GET_SINGLE(Resources)->Find<Texture>(L"LastTop3"));
		AddGameObject(ground, LAYER_TYPE::FIXEDOBJECT);
	}
	{
		shared_ptr<GameObject> ground = make_shared<GameObject>();
		Transform* tr = ground->GetTransform();
		tr->SetPosition(Vector3(432.f, -60.f, 0.f));
		tr->SetFixed(true);

		auto sr = ground->AddComponent<SpriteRenderer>();
		sr->SetSpriteSheet(GET_SINGLE(Resources)->Find<Texture>(L"LastTop2"));
		AddGameObject(ground, LAYER_TYPE::FIXEDOBJECT);
	}
	{
		shared_ptr<GameObject> ground = make_shared<GameObject>();
		Transform* tr = ground->GetTransform();
		tr->SetPosition(Vector3(464.f, -60.f, 0.f));
		tr->SetFixed(true);

		auto sr = ground->AddComponent<SpriteRenderer>();
		sr->SetSpriteSheet(GET_SINGLE(Resources)->Find<Texture>(L"LastTop2"));
		AddGameObject(ground, LAYER_TYPE::FIXEDOBJECT);
	}
	{
		shared_ptr<GameObject> blackWall = make_shared<GameObject>();
		Transform* tr = blackWall->GetTransform();
		tr->SetPosition(Vector3(-320.f, -90.f, 1.f));
		tr->SetScale(Vector3(320.f, 360.f, 1.f));
		tr->SetFixed(true);

		auto sr = blackWall->AddComponent<SpriteRenderer>();
		AddGameObject(blackWall, LAYER_TYPE::FIXEDOBJECT);
	}
	{
		shared_ptr<GameObject> darkLady = make_shared<GameObject>();

		darkLady->AddComponent<DarkLady>();
		AddGameObject(darkLady, LAYER_TYPE::MONSTER);
	}

#pragma region
	shared_ptr<GameObject> uiObj = make_shared<GameObject>();
	uiObj->GetTransform()->SetPosition(Vector3(0, 0, 0.f));
	UI* uicomp = uiObj->AddComponent<UI>();

	shared_ptr<UIImage> uipanel = make_shared<UIImage>();

	shared_ptr<Texture> hpTex = GET_SINGLE(Resources)->Load<Texture>(L"HP", L"UI/hp.png");
	shared_ptr<Texture> mpHpBar = GET_SINGLE(Resources)->Load<Texture>(L"HPBar", L"UI/HpBar.png");
	shared_ptr<Shader> mpHpShader = make_shared<Shader>();
	ShaderEntry entry = {};
	entry.VS = true;
	entry.PS = true;

	ShaderInfo info = {};
	info.bst = BSType::AlphaBlend;
	info.dst = DSType::Less;
	info.rst = RSType::SolidNone;
	info.topology = D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	mpHpShader->CreateShader(info,entry,L"GaugeShader.hlsl");

	shared_ptr<Material> hpMaterial = make_shared<Material>();

	hpMaterial->SetTexture(0,hpTex);
	hpMaterial->SetTexture(1,mpHpBar);
	hpMaterial->SetShader(mpHpShader);
	hpMaterial->SetVec2(0, hpTex->GetSize());
	hpMaterial->SetFloat(0, 0.5f);
	hpMaterial->SetFloat(1, 1.f);
	uipanel->SetMaterial(hpMaterial);

	uicomp->AddChild(uipanel);
	AddGameObject(uiObj, LAYER_TYPE::UI);
#pragma endregion
	Scene::Start();
}
