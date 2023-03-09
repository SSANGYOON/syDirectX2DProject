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
#include "RectScript.h"
#include "Collider2D.h"
#include "CollisionManager.h"

void SceneManager::Update()
{
	if (_activeScene == nullptr)
		return;

	_activeScene->Update();
	_activeScene->LateUpdate();
	_activeScene->FinalUpdate();
}

void SceneManager::Render()
{
	if (_activeScene != nullptr)
		_activeScene->Render();
}

void SceneManager::LoadScene(const wstring& SceneName)
{
	_activeScene = make_shared<Scene>();
	shared_ptr<GameObject> obj = make_shared<GameObject>();
	shared_ptr<SpriteRenderer> rd = obj->AddComponent<SpriteRenderer>();
	obj->AddComponent<RectScript>();
	obj->AddComponent<Collider2D>();
	auto col1 = obj->AddComponent<Collider2D>();
	shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->Find<Mesh>(L"RectMesh");
	shared_ptr<Material> material = GET_SINGLE(Resources)->Find<Material>(L"DefaultMaterial");

	rd->SetMesh(mesh);
	rd->SetMaterial(material);
	_activeScene->AddGameObject(obj, LAYER_TYPE::PLAYER);

	shared_ptr<GameObject> obj2 = make_shared<GameObject>();
	shared_ptr<SpriteRenderer> rd2 = obj2->AddComponent<SpriteRenderer>();
	auto col2 = obj2->AddComponent<Collider2D>();
	rd2->SetMesh(mesh);
	rd2->SetMaterial(material);
	shared_ptr<Transform> tr2 = obj2->GetTransform();
	tr2->SetPosition(Vector3(0.9f, 0.9f, 0.f));
	tr2->SetRotation(Quaternion::CreateFromAxisAngle(Vector3::Forward, 0.2f));
	_activeScene->AddGameObject(obj2, LAYER_TYPE::PLAYER);

	shared_ptr<GameObject> cameraObj = make_shared<GameObject>();
	shared_ptr<Camera> cameracomp = cameraObj->AddComponent<Camera>();
	cameracomp->DisableLayerMasks();
	cameracomp->TurnLayerMask(1, true);
	_activeScene->AddGameObject(cameraObj, LAYER_TYPE::CAMERA);
	GET_SINGLE(CollisionManager)->CollisionLayerCheck(LAYER_TYPE::PLAYER, LAYER_TYPE::PLAYER);
}

shared_ptr<GameObject> SceneManager::Instantiate(LAYER_TYPE type)
{
	shared_ptr<GameObject> obj = make_shared<GameObject>();
	obj->SetGameObjectState(GameObject::ACTIVE);
	_activeScene->AddGameObject(obj,type);
	return obj;
}
