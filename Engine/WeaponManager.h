#pragma once
#include "Weapon.h"


class WeaponManager
{
DECLARE_SINGLE(WeaponManager);

public:
	void Init();
	WeaponStruct* GetWeaponStruct(WEAPON_TYPE type);

private:
	array<WeaponStruct, (UINT)WEAPON_TYPE::END> weaponArr;
};

