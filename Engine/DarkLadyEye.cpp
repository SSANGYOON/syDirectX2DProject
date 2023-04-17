#include "pch.h"
#include "GameObject.h"
#include "DarkLadyEye.h"
#include "SpriteRenderer.h"
#include "SpriteRenderer.h"
#include "SceneManager.h"
#include "Scene.h"
#include "ParticleSystem.h"
#include "Laser.h"
#include "Resources.h"
#include "Texture.h"

DarkLadyEye::DarkLadyEye(GameObject* eye)
	:Script(eye), LaserElapsed(0.f)
{
	AttackSphere = eye->AddComponent<ParticleSystem>();
	AttackSphere->setColor(Vector4(128.f / 255.f, 0.f, 1.f, 1.f));
	AttackSphere->setZoffset(-0.1f);
	AttackSphere->SetPositionVarianceFrom(Vector2(0.f, 0.f));
	AttackSphere->SetPositionVarienceType(POSVAR_TYPE::NORMALIZED);

	SpriteRenderer* thirdEyeSr = eye->AddComponent<SpriteRenderer>();
	thirdEyeSr->SetSpriteSheet(GET_SINGLE(Resources)->Load<Texture>(L"bossEye", L"BossLastEye.png"));

	GameObject* eyePupil = GET_SINGLE(SceneManager)->Instantiate(LAYER_TYPE::MONSTER);
	SpriteRenderer* eyePupilSr = eyePupil->AddComponent<SpriteRenderer>();


	eyePupilSr->SetSpriteSheet(GET_SINGLE(Resources)->Load<Texture>(L"DarkBody", L"DarkLady.png"));
	eyePupilSr->SetSourceOffset(Vector2(19, 2381));
	eyePupilSr->SetSourceSize(Vector2(5, 9));
	_pupilTr = eyePupil->GetTransform();
	eye->GetTransform()->SetChild(_pupilTr, L"Pupil");

	laser0 = eye->AddComponent<Laser>();
	laser1 = eye->AddComponent<Laser>();
	laser2 = eye->AddComponent<Laser>();
	laser3 = eye->AddComponent<Laser>();
}

DarkLadyEye::~DarkLadyEye()
{
}

void DarkLadyEye::Start()
{
	Vector3 dir = Vector3(-cos(60.f / 180.f * XM_PI), -sin(60.f / 180.f * XM_PI), 0.f);
	laser0->Shoot();
	laser0->SetDir(dir);

	dir = Vector3(-cos(45.f / 180.f * XM_PI), -sin(45.f / 180.f * XM_PI), 0.f);
	laser1->Shoot();
	laser1->SetDir(dir);

	dir = Vector3(cos(45.f / 180.f * XM_PI), -sin(45.f / 180.f * XM_PI), 0.f);
	laser2->Shoot();
	laser2->SetDir(dir);
	
	dir = Vector3(cos(60.f / 180.f * XM_PI), -sin(60.f / 180.f * XM_PI), 0.f);
	laser3->Shoot();
	laser3->SetDir(dir);
}

void DarkLadyEye::Update()
{
	if (!_target)
		_target = GET_SINGLE(SceneManager)->GetActiveScene()->FindGameObject("Player");

	LaserElapsed += TIME->DeltaTime();
	Vector3 diff = _target->GetTransform()->GetWorldPosition() - _pupilTr->GetWorldPosition();
	diff.Normalize();
	_pupilTr->SetPosition(diff);
}
