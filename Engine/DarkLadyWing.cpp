#include "pch.h"
#include "DarkLadyWing.h"
#include "GameObject.h"
#include "SceneManager.h"
#include "Resources.h"
#include "MeshRenderer.h"
#include "SpriteRenderer.h"
#include "Material.h"
#include "Mesh.h"

DarkLadyWing::DarkLadyWing(GameObject* wing)
	: Script(wing), time(0.f)
{

	shared_ptr<Material> DarkBodymaterial = make_shared<Material>();
	DarkBodymaterial->Load(L"The_Dark_Lady_Material.json");

	//LeftWing
	GameObject* LeftWing = GET_SINGLE(SceneManager)->Instantiate(LAYER_TYPE::MONSTER);
	_leftWingTr = LeftWing->GetTransform();
	_leftWingTr->SetPosition(Vector3(-68, 0, 0));
	wing->GetTransform()->SetChild(_leftWingTr, L"LeftWing");
	GameObject* LeftDarkWing = GET_SINGLE(SceneManager)->Instantiate(LAYER_TYPE::MONSTER);
	SpriteRenderer* LeftDarkWingSr = LeftDarkWing->AddComponent<SpriteRenderer>();
	LeftDarkWingSr->SetMaterial(DarkBodymaterial);
	_leftWingTr->SetChild(LeftDarkWing->GetTransform(), L"DarkWing");
	LeftDarkWing->GetTransform()->SetPosition(Vector3(-47, 54, 0));
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
	BrightMaterial0->SetVec2(0, Vector2(160, 80));
	BrightMaterial0->SetVec4(0, Vector4(1, 0, 1, 1));

	_leftWingTr->SetChild(LeftBrightWing0->GetTransform(), L"BrightWing0");
	LeftBrightWing0->GetTransform()->SetPosition(Vector3(-70.7f, 42.4f, 0.f));
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
	LeftBrightWing1->GetTransform()->SetPosition(Vector3(-78, 62, 0));
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
	LeftBrightWing2->GetTransform()->SetPosition(Vector3(-77, 82, 0));
	LeftBrightWing2->GetTransform()->SetRotation(Quaternion::CreateFromAxisAngle(Vector3::Backward, -7 * DirectX::XM_PI / 180.f));
	LeftBrightWingMr2->SetMesh(GET_SINGLE(Resources)->Find<Mesh>(L"RectMesh"));

	//RightWing
	GameObject* RightWing = GET_SINGLE(SceneManager)->Instantiate(LAYER_TYPE::MONSTER);
	_rightWingTr = RightWing->GetTransform();
	wing->GetTransform()->SetChild(_rightWingTr, L"RightWing");
	_rightWingTr->SetPosition(Vector3(68, 0, 0));
	_rightWingTr->SetScale(Vector3(-1, 1, 1));
	GameObject* RightDarkWing = GET_SINGLE(SceneManager)->Instantiate(LAYER_TYPE::MONSTER);

	SpriteRenderer* RightDarkWingSr = RightDarkWing->AddComponent<SpriteRenderer>();
	RightDarkWingSr->SetMaterial(DarkBodymaterial);
	_rightWingTr->SetChild(RightDarkWing->GetTransform(), L"DarkWing");
	RightDarkWing->GetTransform()->SetPosition(Vector3(-47, 54, 0));
	RightDarkWingSr->SetSourceOffset(Vector2(3, 2197));
	RightDarkWingSr->SetSourceSize(Vector2(97, 131));

	GameObject* RightBrightWing0 = GET_SINGLE(SceneManager)->Instantiate(LAYER_TYPE::MONSTER);
	MeshRenderer* RightBrightWingMr0 = RightBrightWing0->AddComponent<MeshRenderer>();
	RightBrightWingMr0->SetMaterial(BrightMaterial0);

	_rightWingTr->SetChild(RightBrightWing0->GetTransform(), L"BrightWing0");
	RightBrightWing0->GetTransform()->SetPosition(Vector3(-70.7f, 42.4f, 0.f));
	RightBrightWing0->GetTransform()->SetRotation(Quaternion::CreateFromAxisAngle(Vector3::Backward, 26 * DirectX::XM_PI / 180.f));
	RightBrightWingMr0->SetMesh(GET_SINGLE(Resources)->Find<Mesh>(L"RectMesh"));

	GameObject* RightBrightWing1 = GET_SINGLE(SceneManager)->Instantiate(LAYER_TYPE::MONSTER);
	MeshRenderer* RightBrightWingMr1 = RightBrightWing1->AddComponent<MeshRenderer>();
	RightBrightWingMr1->SetMaterial(BrightMaterial1);


	_rightWingTr->SetChild(RightBrightWing1->GetTransform(), L"BrightWing1");
	RightBrightWing1->GetTransform()->SetPosition(Vector3(-78, 62, 0));
	RightBrightWing1->GetTransform()->SetRotation(Quaternion::CreateFromAxisAngle(Vector3::Backward, 10 * DirectX::XM_PI / 180.f));
	RightBrightWingMr1->SetMesh(GET_SINGLE(Resources)->Find<Mesh>(L"RectMesh"));

	GameObject* RightBrightWing2 = GET_SINGLE(SceneManager)->Instantiate(LAYER_TYPE::MONSTER);
	MeshRenderer* RightBrightWingMr2 = RightBrightWing2->AddComponent<MeshRenderer>();
	RightBrightWingMr2->SetMaterial(BrightMaterial2);

	_rightWingTr->SetChild(RightBrightWing2->GetTransform(), L"BrightWing2");
	RightBrightWing2->GetTransform()->SetPosition(Vector3(-77, 82, 0));
	RightBrightWing2->GetTransform()->SetRotation(Quaternion::CreateFromAxisAngle(Vector3::Backward, -7 * DirectX::XM_PI / 180.f));
	RightBrightWingMr2->SetMesh(GET_SINGLE(Resources)->Find<Mesh>(L"RectMesh"));
}

DarkLadyWing::~DarkLadyWing()
{
}

void DarkLadyWing::Update()
{
	time += TIME->DeltaTime();

	_leftWingTr->SetRotation(Quaternion::CreateFromAxisAngle(Vector3::Backward, (10.f + 5.f * sinf(time * buyoncyFrequency * DirectX::XM_2PI)) * DirectX::XM_PI / 180.f));
	_rightWingTr->SetRotation(Quaternion::CreateFromAxisAngle(Vector3::Backward, (-10.f - 5.f * sinf(time * buyoncyFrequency * DirectX::XM_2PI)) * DirectX::XM_PI / 180.f));
}
