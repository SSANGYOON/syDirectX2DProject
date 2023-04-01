#pragma once
#include "Script.h"

class SpriteRenderer;
class Animator;
class Trail;

enum class WEAPON_TYPE
{
	TWOHAND,
	ONEHAND,
	STAFF,
	DAGGER,
	SUMMON,
	BESTFRIEND
};

class Weapon : public Script
{
public:
	Weapon(GameObject* owner);
	~Weapon();

	virtual void Start();
	virtual void Update();

	const wstring GetWeaponAnimKey();
	void SetWeaponType(WEAPON_TYPE type);
	WEAPON_TYPE GetWeaponType() { return _type; }

	void SetUse();
	void Stop();
	bool IsUsing() { return _using;}
	
private:
	WEAPON_TYPE _type;
	bool _using;
	float _elapsed;
	float _duration;

	Trail* trail;
	SpriteRenderer* sr;
	Animator* animator;
};

