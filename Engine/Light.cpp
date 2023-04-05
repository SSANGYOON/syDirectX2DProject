#include "pch.h"
#include "Light.h"
#include "GameObject.h"
#include "SceneManager.h"
#include "Scene.h"

//#include "Halo.h"
Light::Light(GameObject* owner)
	:Component(Component_Type::Light, owner), /*_halo(nullptr),*/ _info{}
{
}

Light::~Light()
{
}

void Light::SetAngle(float angle)
{
	_info.angle = angle;
}

void Light::SetRange(float range)
{
	_info.range = range;
}

void Light::SetDirection(Vector3 direction)
{
	_info.dir = direction;
}

void Light::SetColor(Vector4 color)
{
	_info.color = color;
}

void Light::Start()
{
	Scene* curScene = GET_SINGLE(SceneManager)->GetActiveScene();
	curScene->AddLight(this);
}

void Light::FinalUpdate()
{
	_info.position = _owner->GetTransform()->GetWorldPosition();
}

void Light::Render()
{
	// 헤일로 렌더링
}
