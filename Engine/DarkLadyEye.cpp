#include "pch.h"
#include "GameObject.h"
#include "DarkLadyEye.h"
#include "SpriteRenderer.h"
#include "Material.h"
#include "SpriteRenderer.h"
#include "SceneManager.h"
#include "Scene.h"
#include "ParticleSystem.h"

DarkLadyEye::DarkLadyEye(GameObject* eye)
	:Script(eye)
{
	eye->AddComponent<ParticleSystem>();
	SpriteRenderer* thirdEyeSr = eye->AddComponent<SpriteRenderer>();
	shared_ptr<Material> thirdEyeMaterial = make_shared<Material>();
	thirdEyeMaterial->Load(L"The_Dark_Lady_Eye.json");
	thirdEyeSr->SetMaterial(thirdEyeMaterial);

	GameObject* eyePupil = GET_SINGLE(SceneManager)->Instantiate(LAYER_TYPE::MONSTER);
	SpriteRenderer* eyePupilSr = eyePupil->AddComponent<SpriteRenderer>();

	shared_ptr<Material> DarkBodymaterial = make_shared<Material>();
	DarkBodymaterial->Load(L"The_Dark_Lady_Material.json");

	eyePupilSr->SetMaterial(DarkBodymaterial);
	eyePupilSr->SetSourceOffset(Vector2(19, 2381));
	eyePupilSr->SetSourceSize(Vector2(5, 9));
	_pupilTr = eyePupil->GetTransform();
	eye->GetTransform()->SetChild(_pupilTr, L"Pupil");
}

DarkLadyEye::~DarkLadyEye()
{
}

void DarkLadyEye::Update()
{
	if (!_target) {
		_target = GET_SINGLE(SceneManager)->GetActiveScene()->GetLayer(LAYER_TYPE::PLAYER).GetGameObjects()[0].get();
	}

	Vector3 diff = _target->GetTransform()->GetWorldPosition() - _pupilTr->GetWorldPosition();
	diff.Normalize();
	_pupilTr->SetPosition(diff);
}
