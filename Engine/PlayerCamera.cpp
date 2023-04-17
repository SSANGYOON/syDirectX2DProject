#include "pch.h"
#include "PlayerCamera.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Layer.h"
#include "GameObject.h"
PlayerCamera::PlayerCamera(GameObject* camObj)
	: Script(camObj)
{
}

PlayerCamera::~PlayerCamera()
{
}

void PlayerCamera::Start()
{
	Scene* curScene = GET_SINGLE(SceneManager)->GetActiveScene();
	if (!player) 
		player = GET_SINGLE(SceneManager)->GetActiveScene()->GetLayer(LAYER_TYPE::PLAYER).GetGameObjects()[0].get();
}

void PlayerCamera::FinalUpdate()
{
	if(player->GetT)
}
