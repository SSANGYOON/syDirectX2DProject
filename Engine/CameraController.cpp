#include "pch.h"
#include "CameraController.h"
#include "GameObject.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Layer.h"
#include "Camera.h"

CameraController::CameraController(GameObject* camObj)
	:Script(camObj)
{
}

CameraController::~CameraController()
{
}

void CameraController::Start()
{
	playerTr = GET_SINGLE(SceneManager)->GetActiveScene()->FindGameObject("Player")->GetTransform();
	cameraComp = _owner->GetComponent<Camera>();
}

void CameraController::FinalUpdate()
{
	const Vector2& sceneSize = GET_SINGLE(SceneManager)->GetActiveScene()->GetSceneSize();

	Vector3 playerPos = playerTr->GetWorldPosition();
	Vector3 targetPos = Vector3(playerPos.x, playerPos.y, _owner->GetTransform()->GetWorldPosition().z);
	Vector2 cameraSize = cameraComp->GetScreenSize();
	Vector3 minPos = Vector3(cameraSize / 2.f - sceneSize / 2.f, targetPos.z);
	Vector3 maxPos = Vector3(sceneSize / 2.f - cameraSize /2.f, targetPos.z);
	targetPos.Clamp(minPos, maxPos);

	_owner->GetTransform()->SetPosition(targetPos);
}
