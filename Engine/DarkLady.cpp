#include "pch.h"
#include "DarkLady.h"
#include "DarkLadyWing.h"
#include "DarkLadyEye.h"

#include "GameObject.h"
#include "Transform.h"
#include "SpriteRenderer.h"
#include "Animator.h"
#include "ParticleSystem.h"
#include "Light.h"
#include "Halo.h"

#include "Mesh.h"
#include "Resources.h"
#include "Texture.h"

#include "SceneManager.h"
#include "Scene.h"
#include "Layer.h"

DarkLady::DarkLady(GameObject* obj)
	:Script(obj), _transform(obj->GetTransform()), _pupilTr(nullptr), _target(nullptr)
{
	//DarkBody
	GameObject* DarkBody = GET_SINGLE(SceneManager)->Instantiate(LAYER_TYPE::MONSTER);
	SpriteRenderer* DarkBodySr = DarkBody->AddComponent<SpriteRenderer>();

	DarkBodySr->SetSpriteSheet(GET_SINGLE(Resources)->Load<Texture>(L"DarkBody", L"DarkLady.png"));
	Animator* DarkBodyAnim = DarkBody->AddComponent<Animator>();
	DarkBodyAnim->LoadAnimation2dFromJson("The_Dark_Lady_clips.json");
	DarkBodyAnim->Play(L"IDLE");
	//BossHair
	GameObject* hair = GET_SINGLE(SceneManager)->Instantiate(LAYER_TYPE::MONSTER);
	Transform* tr = hair->GetTransform();
	tr->SetPosition(Vector3(0, 140, 0.1f));
	SpriteRenderer* HairSr = hair->AddComponent<SpriteRenderer>();
	Animator* HairAnim = hair->AddComponent<Animator>();
	HairSr->SetSpriteSheet(GET_SINGLE(Resources)->Load<Texture>(L"DarkBody", L"DarkLady.png"));
	HairAnim->LoadAnimation2dFromJson("The_Dark_Lady_clips.json");
	HairAnim->Play(L"HAIR");

	obj->GetTransform()->SetChild(DarkBody->GetTransform(), L"DarkBody");
	DarkBody->GetTransform()->SetChild(hair->GetTransform(), L"BossHair");

	//Circlet
	GameObject* circlet = GET_SINGLE(SceneManager)->Instantiate(LAYER_TYPE::MONSTER);

	SpriteRenderer* circletSr = circlet->AddComponent<SpriteRenderer>();
	circletSr->SetSpriteSheet(GET_SINGLE(Resources)->Load<Texture>(L"DarkBody", L"DarkLady.png"));
	circletSr->SetSourceOffset(Vector2(1,2050));
	circletSr->SetSourceSize(Vector2(140, 97));

	//ParticleSystem* p = circlet->AddComponent<ParticleSystem>();
	//p->setColor(Vector4(1.f, 1.f, 128.f / 255.f, 1.f));

	Transform* circletr = circlet->GetTransform();
	circletr->SetPosition(Vector3(0, 315.f, 0.05f));
	DarkBody->GetTransform()->SetChild(circlet->GetTransform(), L"Circlet");

	Light* light = circlet->AddComponent<Light>();

	light->SetColor(Vector4(1, 1, 0, 1));
	light->SetRange(100.f);
	light->SetType(LIGHT_TYPE::POINT);
	Halo* halo = circlet->AddComponent<Halo>();
	halo->SetCenterAlpha(0.5);
	halo->SetColor(Vector4(1.f, 1.f, 0.f, 1.f));
	halo->SetRadius(1000.f);

	//Wings
	GameObject* wings = GET_SINGLE(SceneManager)->Instantiate(LAYER_TYPE::MONSTER);
	wings->AddComponent<DarkLadyWing>();
	DarkBody->GetTransform()->SetChild(wings->GetTransform(), L"wings");
	wings->GetTransform()->SetPosition(Vector3(0, 200, 0));


	//eye
	GameObject* thirdEye = GET_SINGLE(SceneManager)->Instantiate(LAYER_TYPE::MONSTER);
	thirdEye->GetTransform()->SetPosition(Vector3(0, 270, 0.05f));
	DarkBody->GetTransform()->SetChild(thirdEye->GetTransform(), L"BossEye");
	thirdEye->AddComponent<DarkLadyEye>();

}

void DarkLady::Start()
{


}

void DarkLady::Update()
{
	time += TIME->DeltaTime();

	_transform->SetPosition(Vector3(128, -180 + buyoncyRange * sinf(time * buyoncyFrequency * DirectX::XM_2PI), 0.1f));
}
