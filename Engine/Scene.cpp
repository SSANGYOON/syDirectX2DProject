#include "pch.h"
#include "Scene.h"
#include "Layer.h"
#include "Camera.h"
#include "GameObject.h"
#include "Timer.h"
Scene::Scene()
{
	_layers.resize((UINT)LAYER_TYPE::END);
}

Scene::~Scene()
{
}

void Scene::Start()
{
	GET_SINGLE(Timer)->Init();
	for (auto layer : _layers)
		layer.Start();
}

void Scene::Update()
{
	for (auto layer : _layers)
		layer.Update();
}

void Scene::FinalUpdate()
{
	for (auto layer : _layers)
		layer.FinalUpdate();
}

void Scene::Render()
{
	for (Camera* cam : _cameras)
		cam->Render();
}

Camera* Scene::GetMainCamera()
{
	if (_cameras.size())
		return _cameras[0];
	else
		return nullptr;
}
void Scene::AddGameObject(shared_ptr<GameObject> gameObject, LAYER_TYPE type)
{
	_layers[(UINT)type].AddGameObject(gameObject);
	if (type == LAYER_TYPE::CAMERA)
		_cameras.push_back(gameObject->GetComponent<Camera>());
	gameObject->SetType(type);
}

void Scene::RemoveGameObject(shared_ptr<GameObject> gameObject, LAYER_TYPE type)
{
	_layers[(UINT)type].RemoveGameObject(gameObject);
}
