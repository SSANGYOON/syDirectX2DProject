#pragma once
#include "Script.h"
#include "SpriteRenderer.h"
#include "Trail.h"

enum class WEAPON_TYPE
{
	ONEHAND,
	//ONEHAND,
	//DAGGER,
	END
};
struct WeaponTransform
{
	Vector2 position;
	float angle;
};

struct WeaponStruct
{
	UINT frames;
	float duration;
	vector<WeaponTransform> transforms;
};
class Weapon : public Script
{
public:
	Weapon();
	~Weapon();

	virtual void Start();
	virtual void Update();

	const wstring GetWeaponAnimKey();
	void SetWeaponType(WEAPON_TYPE type);
	
	float GetElapsed() { return _elapsed; }
	void SetUse();
	WeaponStruct* weaponDesc;

private:
	WEAPON_TYPE _type;
	bool _using;
	float _elapsed;
	
	Trail* trail;
	SpriteRenderer* sr;
	Transform* tr;
};

