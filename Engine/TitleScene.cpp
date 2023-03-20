#include "pch.h"
#include "TitleScene.h"
#include "GameObject.h"
#include "SpriteRenderer.h"
#include "Collider2D.h"
#include "RigidBody.h"
#include "Resources.h"
#include "Shader.h"
#include "Texture.h"
#include "Camera.h"
#include "Animator.h"
#include "Grid.h"
#include "PlayerScript.h"
#include "CollisionManager.h"


TitleScene::TitleScene()
{
}

TitleScene::~TitleScene()
{
}

void TitleScene::Start()
{
	shared_ptr<GameObject> cameraObj = make_shared<GameObject>();
	shared_ptr<Camera> cameracomp = cameraObj->AddComponent<Camera>();
	cameracomp->DisableLayerMasks();
	cameracomp->TurnLayerMask(1, true);
	AddGameObject(cameraObj, LAYER_TYPE::CAMERA);

	shared_ptr<GameObject> gridOb = make_shared<GameObject>();
	shared_ptr<Grid> grid = gridOb->AddComponent<Grid>();
	AddGameObject(gridOb, LAYER_TYPE::PLAYER);

	GET_SINGLE(CollisionManager)->CollisionLayerCheck(LAYER_TYPE::PLAYER, LAYER_TYPE::MONSTER, true);
	GET_SINGLE(CollisionManager)->CollisionLayerCheck(LAYER_TYPE::PLAYER, LAYER_TYPE::FIXEDOBJECT, true);
	GET_SINGLE(CollisionManager)->CollisionLayerCheck(LAYER_TYPE::MONSTER, LAYER_TYPE::FIXEDOBJECT, true);
	{
		shared_ptr<GameObject> Player = make_shared<GameObject>();
		shared_ptr<SpriteRenderer> rd = Player->AddComponent<SpriteRenderer>();
		shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->Find<Mesh>(L"RectMesh");
		shared_ptr<Material> material = make_shared<Material>();
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Find<Shader>(L"SpriteShader");
		shared_ptr<Texture> TitleTexture = GET_SINGLE(Resources)->Load<Texture>(L"maid", L"Heroine Maid.png");
		material->SetShader(shader);
		material->SetTexture(0, TitleTexture);
		rd->SetMesh(mesh);
		rd->SetMaterial(material);
		shared_ptr<Transform> tr = Player->GetTransform();
		Player->AddComponent<PlayerScript>();
		auto rb = Player->AddComponent<RigidBody>();
		rb->SetGravity(true);
		shared_ptr<Animator> anim = Player->AddComponent<Animator>();

		shared_ptr<Collider2D> col = Player->AddComponent<Collider2D>();
		col->SetType(Collider_TYPE::RECTANGLE);
		col->SetSize(Vector3(1.f, 3.5f, 1.f));
		col->SetLocalCenter(Vector3(0.f, -0.5f, 0.f));

		shared_ptr<Collider2D> col2 = Player->AddComponent<Collider2D>();
		col2->SetType(Collider_TYPE::CIRCLE);
		col2->SetSize(Vector3(0.2f, 0.2f, 1.0));
		col2->SetLocalCenter(Vector3(0.f, -2.5f, 0.f));
		col2->SetTrigger(true);

		anim->AddAnimClip(L"IDLE", make_shared<AnimationClip2D>(Vector2(2.f, 15.f), Vector2(31, 50), Vector2(31, 53), 21, 56, 1.5, true));

		auto idle2run = make_shared<AnimationClip2D>(Vector2(2.f, 189.f), Vector2(30, 50), Vector2(36, 53), 21, 2, 0.1f, false);
		idle2run->SetNext(L"RUN");
		anim->AddAnimClip(L"IDLE2RUN",idle2run );

		anim->AddAnimClip(L"RUN", make_shared<AnimationClip2D>(Vector2(2.f, 256.f), Vector2(30, 50), Vector2(33, 53), 21, 8, 0.75f, true));

		auto run2idle = make_shared<AnimationClip2D>(Vector2(2.f, 322.f), Vector2(26, 50), Vector2(30, 53), 21, 4, 0.2f, false);
		run2idle->SetNext(L"IDLE");
		anim->AddAnimClip(L"RUN2IDLE", run2idle);

		anim->AddAnimClip(L"JUMP", make_shared<AnimationClip2D>(Vector2(2.f, 634.f), Vector2(36, 50), Vector2(40, 53), 21, 5, 0.5f, false));
		anim->AddAnimClip(L"FALLING", make_shared<AnimationClip2D>(Vector2(2.f, 702.f), Vector2(30, 52), Vector2(30, 53), 21, 3, 0.25f, true));

		auto landing = make_shared<AnimationClip2D>(Vector2(5.f, 771.f), Vector2(32, 45), Vector2(37, 53), 21, 3, 0.2f, false);
		landing->SetNext(L"IDLE");
		anim->AddAnimClip(L"LANDING", landing);

		auto crouch = make_shared<AnimationClip2D>(Vector2(3.f, 390.f), Vector2(32, 46), Vector2(34, 53), 21, 4, 0.2f, false);
		crouch->SetNext(L"CROUCHING");
		anim->AddAnimClip(L"CROUCH", crouch);

		anim->AddAnimClip(L"CROUCHING", make_shared<AnimationClip2D>(Vector2(3.f, 445.f), Vector2(28, 45), Vector2(30, 53), 21, 1, 0.1f, false));

		auto crouch2idle = make_shared<AnimationClip2D>(Vector2(3.f, 507.f), Vector2(28.f, 47.f), Vector2(34, 53), 21, 1, 0.2f, false);
		crouch2idle->SetNext(L"IDLE");
		anim->AddAnimClip(L"CROUCH2IDLE", crouch2idle);

		auto rolling = make_shared<AnimationClip2D>(Vector2(4.f, 573.f), Vector2(41.f, 43.f), Vector2(45, 53), 21, 9, 1.f, false);
		rolling->SetNext(L"IDLE");
		anim->AddAnimClip(L"ROLLING", rolling);

		anim->AddAnimClip(L"ATTACKED", make_shared<AnimationClip2D>(Vector2(2.f, 2607.f), Vector2(30, 45), Vector2(30, 53), 21, 1, 0.2f, false));



		anim->Play(L"IDLE");
		AddGameObject(Player, LAYER_TYPE::PLAYER);
	}
	{
		shared_ptr<GameObject> ground = make_shared<GameObject>();
		shared_ptr<Transform> tr = ground->GetTransform();
		tr->SetScale(Vector3(50.f, 5.f, 1.f));
		tr->SetPosition(Vector3(0.f, -10.f, 0.f));
		tr->SetFixed(true);

		shared_ptr<Collider2D> col = ground->AddComponent<Collider2D>();
		col->SetType(Collider_TYPE::RECTANGLE);
		AddGameObject(ground, LAYER_TYPE::FIXEDOBJECT);
	}

	{
		shared_ptr<GameObject> Enemy = make_shared<GameObject>();

		shared_ptr<Transform> tr = Enemy->GetTransform();
		tr->SetPosition(Vector3(5.f, -5.f, 0.f));
		tr->SetFixed(true);
		shared_ptr<Collider2D> col = Enemy->AddComponent<Collider2D>();
		col->SetTrigger(true);
		col->SetType(Collider_TYPE::RECTANGLE);
		col->SetSize(Vector3(1.5, 1.5f, 1.f));
		AddGameObject(Enemy, LAYER_TYPE::MONSTER);
	}
	Scene::Start();
}
