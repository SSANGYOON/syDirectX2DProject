#include "pch.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Layer.h"
#include "Engine.h"
#include "GameObject.h"
#include "SpriteRenderer.h"
#include "Resources.h"
#include "Mesh.h"
#include "Material.h"
#include "Camera.h"
#include "Collider2D.h"
#include "CollisionManager.h"
#include "TitleScene.h"
void SceneManager::Update()
{
	if (_activeScene == nullptr)
		return;

	_activeScene->Update();
	_activeScene->FinalUpdate();
}

void SceneManager::Render()
{
	if (_activeScene != nullptr)
		_activeScene->Render();
}

void SceneManager::LoadScene(const wstring& SceneName)
{
	_activeScene = make_shared<TitleScene>();
	_activeScene->Start();
}

GameObject* SceneManager::Instantiate(LAYER_TYPE type)
{
	shared_ptr<GameObject> obj = make_shared<GameObject>();
	obj->SetGameObjectState(GameObject::ACTIVE);
	_activeScene->AddGameObject(obj,type);
	return obj.get();
}
