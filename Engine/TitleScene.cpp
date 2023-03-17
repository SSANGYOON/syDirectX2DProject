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
#include "CollisionManager.h"
#include "PlayerScript.h"

TitleScene::TitleScene()
{
}

TitleScene::~TitleScene()
{
}

void TitleScene::Start()
{
	GET_SINGLE(CollisionManager)->CollisionLayerCheck(LAYER_TYPE::PLAYER, LAYER_TYPE::PLAYER, true);
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
		tr->SetScale(Vector3(200.f, 200.f, 1.f));
		Player->AddComponent<PlayerScript>();
		auto rb = Player->AddComponent<RigidBody>();
		rb->SetGravity(true);
		shared_ptr<Animator> anim = Player->AddComponent<Animator>();

		shared_ptr<Collider2D> col = Player->AddComponent<Collider2D>();
		col->SetType(Collider_TYPE::RECTANGLE);
		col->SetLocalCenter(Vector3(0.f, 0.1f, 0.f));
		col->SetSize(Vector3(0.6f, 0.6f, 1.0));

		shared_ptr<Collider2D> col2 = Player->AddComponent<Collider2D>();
		col2->SetType(Collider_TYPE::CIRCLE);
		col2->SetSize(Vector3(0.2f, 0.2f, 1.0));
		col2->SetLocalCenter(Vector3(0.f, -0.3f, 0.f));
		col2->SetTrigger(true);

		SpriteAnimClip clip = {};
		clip.offset = { 2,15 };
		clip.size = { 31,53 };
		clip.duration = 1.6f;
		clip.col = 21;
		clip.row = 3;
		clip.frame_count = 56;
		anim->AddSpriteAnimation(0, clip);
		anim->Play(0, true);
		anim->SetSpriteSheet(TitleTexture);
		AddGameObject(Player, LAYER_TYPE::PLAYER);
	}

	{
		shared_ptr<GameObject> Player = make_shared<GameObject>();
		shared_ptr<SpriteRenderer> rd = Player->AddComponent<SpriteRenderer>();
		shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->Find<Mesh>(L"RectMesh");
		shared_ptr<Material> material = make_shared<Material>();
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Find<Shader>(L"Rect");
		material->SetShader(shader);
		rd->SetMesh(mesh);
		rd->SetMaterial(material);
		shared_ptr<Transform> tr = Player->GetTransform();
		tr->SetScale(Vector3(1000.f, 100.f, 1.f));
		tr->SetPosition(Vector3(0.f, -400.f, 0.f));
		tr->SetFixed(true);

		shared_ptr<Collider2D> col = Player->AddComponent<Collider2D>();
		col->SetType(Collider_TYPE::RECTANGLE);

		AddGameObject(Player, LAYER_TYPE::PLAYER);
	}

	shared_ptr<GameObject> cameraObj = make_shared<GameObject>();
	shared_ptr<Camera> cameracomp = cameraObj->AddComponent<Camera>();
	cameracomp->DisableLayerMasks();
	cameracomp->TurnLayerMask(1, true);
	AddGameObject(cameraObj, LAYER_TYPE::CAMERA);

	Scene::Start();
}
