#pragma once
#include "Component.h"
#include "ConstantBuffer.h"

enum class LIGHT_TYPE
{
	POINT,
	SPOT,
	DIRECTIONAL
};

class Light : public Component
{
public:
	Light(class GameObject* owner);
	virtual ~Light();

	void SetAngle(float angle);
	void SetRange(float range);
	void SetDirection(Vector3 direction);
	void SetColor(Vector4 color);
	void SetType(LIGHT_TYPE type) { _info.type = (UINT)type; }

	const LightInfo& GetLightInfo() { return _info; }

	virtual void Start();
	virtual void FinalUpdate();
	virtual void Render();

private:
	LightInfo _info;
	//class Halo* _halo;
};

