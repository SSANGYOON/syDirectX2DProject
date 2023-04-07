#include "pch.h"
#include "Skill.h"
#include "Animator.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "SpriteRenderer.h"
#include "Weapon.h"
#include "Material.h"

Skill::Skill(GameObject* owner)
	:Script(owner), phases(2), summoned(false), currentPhase(0)
{
	GameObject* SworldHolder = GET_SINGLE(SceneManager)->Instantiate(LAYER_TYPE::PLAYER_WEAPON);
	auto holdtr = SworldHolder->GetTransform();
	holderAnim = SworldHolder->AddComponent<Animator>();
	holderAnim->LoadTransformFromJson("SummonSword.json");

	for (int i = 0; i < 5; i++)
	{
		GameObject* satlelight = GET_SINGLE(SceneManager)->Instantiate(LAYER_TYPE::PLAYER_WEAPON);
		Transform* satletr = satlelight->GetTransform();
		holdtr->SetChild(satletr, L"satlelight" + i);

		satleliteAnim[i] = satlelight->AddComponent<Animator>();
		satleliteAnim[i]->LoadTransformFromJson("SummonSword.json");
		
		GameObject* swordObj = GET_SINGLE(SceneManager)->Instantiate(LAYER_TYPE::PLAYER_WEAPON);
		weapons[i] = swordObj->AddComponent<Weapon>();
		satletr->SetChild(swordObj->GetTransform(), L"sword");
		swordAnim[i] = swordObj->AddComponent<Animator>();
		swordAnim[i]->LoadTransformFromJson("SummonSword.json");

		
	}
}

Skill::~Skill()
{
}

void Skill::Start()
{
	for (int i = 0; i < 5; i++)
	{
		weapons[i]->SetWeaponType(WEAPON_TYPE::SUMMON);
	}
}

void Skill::Update()
{
	_phaseTime += TIME->DeltaTime();
	if (currentPhase == 0 && INPUT->GetKeyState(KEY_TYPE::A) == KEY_STATE::DOWN)
	{
		
		holderAnim->Play(L"SwordHolderPhase1");
		for (int i = 0; i < 5; i++)
		{
			satleliteAnim[i]->GetOwner()->GetTransform()->SetRotation(Quaternion::CreateFromAxisAngle(Vector3::Backward, DirectX::XM_PI * 0.4f * i));
			weapons[i]->SetUse();
			weapons[i]->GetOwner()->GetTransform()->SetPosition(Vector3::Zero);
			weapons[i]->GetOwner()->GetTransform()->SetRotation(Quaternion::CreateFromAxisAngle(Vector3::Backward, -DirectX::XM_PI * 0.4f * i));
			swordAnim[i]->Play(L"SummonedSwordPhase1", i * 0.6f, 0.6);
		}
		currentPhase++;
		_phaseTime = 0.f;
	}
	else if (currentPhase == 1 && INPUT->GetKeyState(KEY_TYPE::A) == KEY_STATE::DOWN)
	{
		holderAnim->Play(L"SwordHolderPhase2");
		for (int i = 0; i < 5; i++)
		{
			satleliteAnim[i]->Play(L"SatlelitePhase2", i * 0.6f);
			swordAnim[i]->Play(L"SummonedSwordPhase2", 0, 0.3);
		}
		currentPhase++;
		_phaseTime = 0.f;
	}

	if (currentPhase == 2 && _phaseTime > 0.6f)
	{
		for (int i = 0; i < 5; i++)
		{
			satleliteAnim[i]->Play(L"SatlelitePhase3", i * 0.2f + 0.2f);
		}
		currentPhase++;
		_phaseTime = 0.f;
	}
	if (currentPhase == 3 && _phaseTime > 5.0f)
	{
		for (int i = 0; i < 5; i++)
		{
			satleliteAnim[i]->Stop();
			swordAnim[i]->Stop();
			weapons[i]->Stop();
		}
		holderAnim->Stop();
		currentPhase = 0;
		_phaseTime = 0.f;
	}
}

void Skill::Summon()
{
}
