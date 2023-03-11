#include "pch.h"
#include "TitleScene.h"
#include "GameObject.h"
#include "SpriteRenderer.h"
#include "Collider2D.h"
#include "Resources.h"
#include "Shader.h"
#include "Texture.h"
#include "Camera.h"
#include "Animator.h"

TitleScene::TitleScene()
{
}

TitleScene::~TitleScene()
{
}

void TitleScene::Start()
{
	/*{
		shared_ptr<GameObject> BackGround = make_shared<GameObject>();
		shared_ptr<SpriteRenderer> rd = BackGround->AddComponent<SpriteRenderer>();
		shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->Find<Mesh>(L"RectMesh");
		shared_ptr<Material> material = make_shared<Material>();
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Find<Shader>(L"DefaultShader");
		shared_ptr<Texture> TitleTexture = GET_SINGLE(Resources)->Find<Texture>(L"TitleSky");
		material->SetShader(shader);
		material->SetTexture(0,TitleTexture);
		shared_ptr<Transform> tr = BackGround->GetTransform();
		tr->SetScale(Vector3(1280.f, 720.f, 1.f));
		rd->SetMaterial(material);
		rd->SetMesh(mesh);
		AddGameObject(BackGround, LAYER_TYPE::PLAYER);
	}

	{
		shared_ptr<GameObject> title = make_shared<GameObject>();
		shared_ptr<SpriteRenderer> rd = title->AddComponent<SpriteRenderer>();
		shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->Find<Mesh>(L"RectMesh");
		shared_ptr<Material> material = make_shared<Material>();
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Find<Shader>(L"DefaultShader");
		shared_ptr<Texture> TitleTexture = GET_SINGLE(Resources)->Find<Texture>(L"TitleText");
		material->SetShader(shader);
		material->SetTexture(0,TitleTexture);
		shared_ptr<Transform> tr = title->GetTransform();
		tr->SetPosition(Vector3(-250.f, 300.f, 0.f));
		tr->SetScale(Vector3(720.f, 90.f, 1.f));
		rd->SetMaterial(material);
		rd->SetMesh(mesh);
		AddGameObject(title, LAYER_TYPE::PLAYER);
	}*/
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

	shared_ptr<Animator> anim = Player->AddComponent<Animator>();
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

	shared_ptr<GameObject> cameraObj = make_shared<GameObject>();
	shared_ptr<Camera> cameracomp = cameraObj->AddComponent<Camera>();
	cameracomp->DisableLayerMasks();
	cameracomp->TurnLayerMask(1, true);
	AddGameObject(cameraObj, LAYER_TYPE::CAMERA);


	Scene::Start();
}
