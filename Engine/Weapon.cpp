#include "pch.h"
#include "Weapon.h"

#include "SceneManager.h"
#include "GameObject.h"
#include "Transform.h"
#include "SpriteRenderer.h"
#include "Animator.h"
#include "Trail.h"

#include "Material.h"

#include "Timer.h"

Weapon::Weapon(GameObject* owner)
	: Script(owner), _using(false), _type(WEAPON_TYPE::ONEHAND)
{
	sr = owner->AddComponent<SpriteRenderer>();	
	sr->SetRender(false);

	GameObject* trailObj = GET_SINGLE(SceneManager)->Instantiate(LAYER_TYPE::PLAYER_WEAPON);
	auto tr = _owner->GetTransform();
	tr->SetChild(trailObj->GetTransform(), L"trail");
	trail = trailObj->AddComponent<Trail>();
	int a = 0;
}

Weapon::~Weapon()
{
}

const wstring Weapon::GetWeaponAnimKey()
{
	switch (_type)
	{
	case WEAPON_TYPE::TWOHAND:
		return L"TWOHAND";
	case WEAPON_TYPE::ONEHAND:
		return L"ONEHAND";
	case WEAPON_TYPE::STAFF:
		return L"STAFF";
	case WEAPON_TYPE::DAGGER:
		return L"DAGGER";
	case WEAPON_TYPE::BESTFRIEND:
		return L"BESTFRIEND";
	default:
		return L"";
	}
}

void Weapon::SetWeaponType(WEAPON_TYPE type)
{
	shared_ptr<Material> material = make_shared<Material>();
	if (type == WEAPON_TYPE::ONEHAND) {
		material->Load(L"OneHandShader.json");
		trail->SetTrailType(WEAPON_TYPE::ONEHAND);
		_duration = 1.0f;
	}
	else if (type == WEAPON_TYPE::DAGGER) {
		material->Load(L"DaggerShader.json");
		trail->SetTrailType(WEAPON_TYPE::DAGGER);
		_duration = 0.8f;
	}
	else if (type == WEAPON_TYPE::SUMMON)
	{
		material->Load(L"SummonSwordShader.json");
		trail->SetTrailType(WEAPON_TYPE::SUMMON);
		_duration = 20.f;
		auto trailtr = trail->GetOwner()->GetTransform();
		trailtr->SetPosition(Vector3(0, -30, 0));
	}
	_type = type;
	sr->SetMaterial(material);
}

void Weapon::Start()
{
	if (_type != WEAPON_TYPE::SUMMON) {
		animator = _owner->AddComponent<Animator>();
		animator->LoadTransformFromJson("PlayerWeapon.json");
	}
	
}

void Weapon::Update()
{
	if (_using)
	{
		_elapsed += TIME->DeltaTime();
		if (_elapsed > _duration)
			_using = false;
	}
}

void Weapon::SetUse()
{
	_using = true;

	sr->SetRender(true);
	if (_type != WEAPON_TYPE::SUMMON) {
		auto clip = animator->GetClip(GetWeaponAnimKey());
		clip->SetEndEvent(std::bind(&Weapon::Stop, this));
		animator->Play(GetWeaponAnimKey());
	}
	trail->StartRecord();
	_elapsed = 0.f;


}

void Weapon::Stop()
{
	//_using = false;
	sr->SetRender(false);
	trail->StopRecord();
}
