#include "pch.h"
#include "Weapon.h"
#include "GameObject.h"
#include "Timer.h"
#include "Transform.h"
#include "WeaponManager.h"

Weapon::Weapon(GameObject* owner)
	: Script(owner), _using(false)
{
	
}

Weapon::~Weapon()
{
}

const wstring Weapon::GetWeaponAnimKey()
{
	switch (_type)
	{
	case WEAPON_TYPE::ONEHAND:
		return L"ONEHAND";
	default:
		return L"";
	}
}

void Weapon::SetWeaponType(WEAPON_TYPE type)
{
	_type = type;
	weaponDesc = GET_SINGLE(WeaponManager)->GetWeaponStruct(_type);
}

void Weapon::SetUse()
{
	_using = true; _elapsed = 0.f; sr->SetRender(true);
}

void Weapon::Stop()
{
	_using = false; sr->SetRender(false);
	if(trail)
		trail->StopRecord();
}

void Weapon::Start()
{
	tr = GetOwner()->GetTransform();
	sr = GetOwner()->GetComponent<SpriteRenderer>();
	sr->SetRender(false);

	auto trailtr = GetOwner()->GetTransform()->GetChild(L"trail");
	
	if (trailtr)
	{
		auto trailObj = trailtr->GetOwner();
		trail = trailObj->GetComponent<Trail>();
	}
}

void Weapon::Update()
{
	if (_using)
	{
		if (_elapsed < 0.4f && _elapsed + TIME->DeltaTime() > 0.4f)
		{
			auto trailtr = GetOwner()->GetTransform()->GetChild(L"trail");

			if (trail)
				trail->StartRecord();
		}
		_elapsed += TIME->DeltaTime();
		float curFrameTime = _elapsed / weaponDesc->duration * weaponDesc->frames;
		if (curFrameTime < weaponDesc->transforms.size()-1)
		{
			UINT curFrame = (UINT)curFrameTime;
			Vector2 pos = weaponDesc->transforms[curFrame].position;
			float angle = weaponDesc->transforms[curFrame].angle;

			Vector2 nextpos = weaponDesc->transforms[curFrame+1].position;
			float nextangle = weaponDesc->transforms[curFrame+1].angle;

			tr->SetPosition(10.f * Vector3::Lerp({ pos.x,pos.y,0.f }, { nextpos.x,nextpos.y,0.f }, curFrameTime - curFrame));
			tr->SetRotation(Quaternion::CreateFromAxisAngle(Vector3::Backward, 
				(angle + (nextangle-angle) * (curFrameTime - curFrame)) / 360.f * DirectX::XM_2PI));
		}
		else if (curFrameTime < weaponDesc->frames)
		{
			sr->SetRender(false);
			if (trail)
				trail->StopRecord();
		}
		else {
			_using = false;
			
		}
	}
}
