#include "pch.h"
#include "DarkLady.h"
#include "GameObject.h"
#include "Transform.h"
#include "SpriteRenderer.h"
#include "MeshRenderer.h"
#include "Animator.h"
#include "ParticleSystem.h"
#include "Light.h"
#include "Halo.h"
#include "Material.h"
#include "Resources.h"
#include "Mesh.h"

#include "SceneManager.h"
#include "Scene.h"
#include "Layer.h"

DarkLady::DarkLady(GameObject* obj)
	:Script(obj), _transform(obj->GetTransform()), _pupilTr(nullptr)
{
	//DarkBody
	GameObject* DarkBody = GET_SINGLE(SceneManager)->Instantiate(LAYER_TYPE::MONSTER);
	SpriteRenderer* DarkBodySr = DarkBody->AddComponent<SpriteRenderer>();


	shared_ptr<Material> DarkBodymaterial = make_shared<Material>();
	DarkBodymaterial->Load(L"The_Dark_Lady_Material.json");
	DarkBodySr->SetMaterial(DarkBodymaterial);
	Animator* DarkBodyAnim = DarkBody->AddComponent<Animator>();
	DarkBodyAnim->LoadAnimation2dFromJson("The_Dark_Lady_clips.json");
	DarkBodyAnim->Play(L"IDLE");
	//BossHair
	GameObject* hair = GET_SINGLE(SceneManager)->Instantiate(LAYER_TYPE::MONSTER);
	Transform* tr = hair->GetTransform();
	tr->SetPosition(Vector3(0, 14.0, 0.1f));
	SpriteRenderer* HairSr = hair->AddComponent<SpriteRenderer>();
	Animator* HairAnim = hair->AddComponent<Animator>();
	HairSr->SetMaterial(DarkBodymaterial);
	HairAnim->LoadAnimation2dFromJson("The_Dark_Lady_clips.json");
	HairAnim->Play(L"HAIR");

	obj->GetTransform()->SetChild(DarkBody->GetTransform(), L"DarkBody");
	DarkBody->GetTransform()->SetChild(hair->GetTransform(), L"BossHair");

	//Circlet
	GameObject* circlet = GET_SINGLE(SceneManager)->Instantiate(LAYER_TYPE::MONSTER);
	circlet->AddComponent<ParticleSystem>();
	SpriteRenderer* circletSr = circlet->AddComponent<SpriteRenderer>();
	circletSr->SetMaterial(DarkBodymaterial);
	circletSr->SetSourceOffset(Vector2(1,2050));
	circletSr->SetSourceSize(Vector2(140, 97));
	Transform* circletr = circlet->GetTransform();
	circletr->SetPosition(Vector3(0, 31.5f, 0.05f));
	DarkBody->GetTransform()->SetChild(circlet->GetTransform(), L"Circlet");

	Light* light = circlet->AddComponent<Light>();

	light->SetColor(Vector4(1, 1, 0, 1));
	light->SetRange(10.f);
	light->SetType(LIGHT_TYPE::POINT);
	Halo* halo = circlet->AddComponent<Halo>();
	halo->SetCenterAlpha(0.5);
	halo->SetColor(Vector4(1.f, 1.f, 0.f, 1.f));
	halo->SetRadius(100.f);

	//Wings
	GameObject* wings = GET_SINGLE(SceneManager)->Instantiate(LAYER_TYPE::MONSTER);
	DarkBody->GetTransform()->SetChild(wings->GetTransform(), L"wings");
	wings->GetTransform()->SetPosition(Vector3(0, 20, 0));
	//LeftWing
	GameObject* LeftWing = GET_SINGLE(SceneManager)->Instantiate(LAYER_TYPE::MONSTER);
	_leftWingTr = LeftWing->GetTransform();
	_leftWingTr->SetPosition(Vector3(-6.8, 0, 0));
	wings->GetTransform()->SetChild(LeftWing->GetTransform(), L"LeftWing");
	GameObject* LeftDarkWing = GET_SINGLE(SceneManager)->Instantiate(LAYER_TYPE::MONSTER);
	SpriteRenderer* LeftDarkWingSr = LeftDarkWing->AddComponent<SpriteRenderer>();
	LeftDarkWingSr->SetMaterial(DarkBodymaterial);
	_leftWingTr->SetChild(LeftDarkWing->GetTransform(), L"DarkWing");
	LeftDarkWing->GetTransform()->SetPosition(Vector3(-4.7, 5.4, 0));
	LeftDarkWingSr->SetSourceOffset(Vector2(3, 2197));
	LeftDarkWingSr->SetSourceSize(Vector2(97, 131));

	GameObject* LeftBrightWing0 = GET_SINGLE(SceneManager)->Instantiate(LAYER_TYPE::MONSTER);
	MeshRenderer* LeftBrightWingMr0 = LeftBrightWing0->AddComponent<MeshRenderer>();
	shared_ptr<Material> BrightMaterial0 = make_shared<Material>();
	LeftBrightWingMr0->SetMaterial(BrightMaterial0);
	BrightMaterial0->Load(L"BrightWing.json");

	BrightMaterial0->SetFloat(0, 60.f);
	BrightMaterial0->SetFloat(1, 2.f);
	BrightMaterial0->SetFloat(3, 10.f);
	BrightMaterial0->SetVec2(0, Vector2(160,80));
	BrightMaterial0->SetVec4(0, Vector4(1,0,1,1));

	_leftWingTr->SetChild(LeftBrightWing0->GetTransform(), L"BrightWing0");
	LeftBrightWing0->GetTransform()->SetPosition(Vector3(-7.07, 4.24, 0));
	LeftBrightWing0->GetTransform()->SetRotation(Quaternion::CreateFromAxisAngle(Vector3::Backward, 26 * DirectX::XM_PI / 180.f));
	LeftBrightWingMr0->SetMesh(GET_SINGLE(Resources)->Find<Mesh>(L"RectMesh"));

	GameObject* LeftBrightWing1 = GET_SINGLE(SceneManager)->Instantiate(LAYER_TYPE::MONSTER);
	MeshRenderer* LeftBrightWingMr1 = LeftBrightWing1->AddComponent<MeshRenderer>();
	shared_ptr<Material> BrightMaterial1 = make_shared<Material>();
	LeftBrightWingMr1->SetMaterial(BrightMaterial1);
	BrightMaterial1->Load(L"BrightWing.json");

	BrightMaterial1->SetFloat(0, 75.f);
	BrightMaterial1->SetFloat(1, 2.f);
	BrightMaterial1->SetFloat(3, 10.f);
	BrightMaterial1->SetVec2(0, Vector2(200, 80));
	BrightMaterial1->SetVec4(0, Vector4(1, 0, 1, 1));

	_leftWingTr->SetChild(LeftBrightWing1->GetTransform(), L"BrightWing1");
	LeftBrightWing1->GetTransform()->SetPosition(Vector3(-7.8, 6.2, 0));
	LeftBrightWing1->GetTransform()->SetRotation(Quaternion::CreateFromAxisAngle(Vector3::Backward, 10 * DirectX::XM_PI / 180.f));
	LeftBrightWingMr1->SetMesh(GET_SINGLE(Resources)->Find<Mesh>(L"RectMesh"));

	GameObject* LeftBrightWing2 = GET_SINGLE(SceneManager)->Instantiate(LAYER_TYPE::MONSTER);
	MeshRenderer* LeftBrightWingMr2 = LeftBrightWing2->AddComponent<MeshRenderer>();
	shared_ptr<Material> BrightMaterial2 = make_shared<Material>();
	LeftBrightWingMr2->SetMaterial(BrightMaterial2);
	BrightMaterial2->Load(L"BrightWing.json");

	BrightMaterial2->SetFloat(0, 90.f);
	BrightMaterial2->SetFloat(1, 2.f);
	BrightMaterial2->SetFloat(3, 10.f);
	BrightMaterial2->SetVec2(0, Vector2(240, 80));
	BrightMaterial2->SetVec4(0, Vector4(1, 0, 1, 1));

	_leftWingTr->SetChild(LeftBrightWing2->GetTransform(), L"BrightWing2");
	LeftBrightWing2->GetTransform()->SetPosition(Vector3(-7.7, 8.2, 0));
	LeftBrightWing2->GetTransform()->SetRotation(Quaternion::CreateFromAxisAngle(Vector3::Backward, -7 * DirectX::XM_PI / 180.f));
	LeftBrightWingMr2->SetMesh(GET_SINGLE(Resources)->Find<Mesh>(L"RectMesh"));

	//RightWing
	GameObject* RightWing = GET_SINGLE(SceneManager)->Instantiate(LAYER_TYPE::MONSTER);
	_rightWingTr = RightWing->GetTransform();
	wings->GetTransform()->SetChild(_rightWingTr, L"RightWing");
	_rightWingTr->SetPosition(Vector3(6.8, 0, 0));
	_rightWingTr->SetScale(Vector3(-1, 1, 1));
	GameObject* RightDarkWing = GET_SINGLE(SceneManager)->Instantiate(LAYER_TYPE::MONSTER);

	SpriteRenderer* RightDarkWingSr = RightDarkWing->AddComponent<SpriteRenderer>();
	RightDarkWingSr->SetMaterial(DarkBodymaterial);
	_rightWingTr->SetChild(RightDarkWing->GetTransform(), L"DarkWing");
	RightDarkWing->GetTransform()->SetPosition(Vector3(-4.7, 5.4, 0));
	RightDarkWingSr->SetSourceOffset(Vector2(3, 2197));
	RightDarkWingSr->SetSourceSize(Vector2(97, 131));

	GameObject* RightBrightWing0 = GET_SINGLE(SceneManager)->Instantiate(LAYER_TYPE::MONSTER);
	MeshRenderer* RightBrightWingMr0 = RightBrightWing0->AddComponent<MeshRenderer>();
	RightBrightWingMr0->SetMaterial(BrightMaterial0);

	_rightWingTr->SetChild(RightBrightWing0->GetTransform(), L"BrightWing0");
	RightBrightWing0->GetTransform()->SetPosition(Vector3(-7.07, 4.24, 0));
	RightBrightWing0->GetTransform()->SetRotation(Quaternion::CreateFromAxisAngle(Vector3::Backward, 26 * DirectX::XM_PI / 180.f));
	RightBrightWingMr0->SetMesh(GET_SINGLE(Resources)->Find<Mesh>(L"RectMesh"));

	GameObject* RightBrightWing1 = GET_SINGLE(SceneManager)->Instantiate(LAYER_TYPE::MONSTER);
	MeshRenderer* RightBrightWingMr1 = RightBrightWing1->AddComponent<MeshRenderer>();
	RightBrightWingMr1->SetMaterial(BrightMaterial1);


	_rightWingTr->SetChild(RightBrightWing1->GetTransform(), L"BrightWing1");
	RightBrightWing1->GetTransform()->SetPosition(Vector3(-7.8, 6.2, 0));
	RightBrightWing1->GetTransform()->SetRotation(Quaternion::CreateFromAxisAngle(Vector3::Backward, 10 * DirectX::XM_PI / 180.f));
	RightBrightWingMr1->SetMesh(GET_SINGLE(Resources)->Find<Mesh>(L"RectMesh"));

	GameObject* RightBrightWing2 = GET_SINGLE(SceneManager)->Instantiate(LAYER_TYPE::MONSTER);
	MeshRenderer* RightBrightWingMr2 = RightBrightWing2->AddComponent<MeshRenderer>();
	RightBrightWingMr2->SetMaterial(BrightMaterial2);

	_rightWingTr->SetChild(RightBrightWing2->GetTransform(), L"BrightWing2");
	RightBrightWing2->GetTransform()->SetPosition(Vector3(-7.7, 8.2, 0));
	RightBrightWing2->GetTransform()->SetRotation(Quaternion::CreateFromAxisAngle(Vector3::Backward, -7 * DirectX::XM_PI / 180.f));
	RightBrightWingMr2->SetMesh(GET_SINGLE(Resources)->Find<Mesh>(L"RectMesh"));

	//eye
	GameObject* thirdEye = GET_SINGLE(SceneManager)->Instantiate(LAYER_TYPE::MONSTER);
	thirdEye->GetTransform()->SetPosition(Vector3(0, 27, 0.05f));
	SpriteRenderer* thirdEyeSr = thirdEye->AddComponent<SpriteRenderer>();
	shared_ptr<Material> thirdEyeMaterial = make_shared<Material>();
	thirdEyeMaterial->Load(L"The_Dark_Lady_Eye.json");
	thirdEyeSr->SetMaterial(thirdEyeMaterial);
	DarkBody->GetTransform()->SetChild(thirdEye->GetTransform(), L"BossEye");

	//eyePupil
	GameObject* eyePupil = GET_SINGLE(SceneManager)->Instantiate(LAYER_TYPE::MONSTER);
	SpriteRenderer* eyePupilSr = eyePupil->AddComponent<SpriteRenderer>();
	eyePupilSr->SetMaterial(DarkBodymaterial);
	eyePupilSr->SetSourceOffset(Vector2(19,2381));
	eyePupilSr->SetSourceSize(Vector2(5, 9));
	_pupilTr = eyePupil->GetTransform();
	thirdEye->GetTransform()->SetChild(_pupilTr, L"Pupil");
}

void DarkLady::Start()
{


}

void DarkLady::Update()
{
	if (!_target) {
		_target = GET_SINGLE(SceneManager)->GetActiveScene()->GetLayer(LAYER_TYPE::PLAYER).GetGameObjects()[0].get();
	}

	Vector3 diff = _target->GetTransform()->GetWorldPosition() - _pupilTr->GetWorldPosition();
	diff.Normalize();
	_pupilTr->SetPosition(diff/10.f);


	time += TIME->DeltaTime();

	_transform->SetPosition(Vector3(0, -20 + buyoncyRange * sinf(time * buyoncyFrequency * DirectX::XM_2PI), 0.1f));
	_leftWingTr->SetRotation(Quaternion::CreateFromAxisAngle(Vector3::Backward, (10.f +5.f * sinf(time * buyoncyFrequency * DirectX::XM_2PI)) * DirectX::XM_PI / 180.f));
	_rightWingTr->SetRotation(Quaternion::CreateFromAxisAngle(Vector3::Backward, (-10.f - 5.f * sinf(time * buyoncyFrequency * DirectX::XM_2PI)) * DirectX::XM_PI / 180.f));
}
