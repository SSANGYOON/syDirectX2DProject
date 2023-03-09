#include "pch.h"
#include "Scene.h"
#include "Layer.h"
#include "Camera.h"
#include "GameObject.h"

Scene::Scene()
{
	_layers.resize((UINT)LAYER_TYPE::END);
}

Scene::~Scene()
{
}

void Scene::Start()
{
	//for (auto layer : _layers)
		//layer.Start();
}

void Scene::Update()
{
	for (auto layer : _layers)
		layer.Update();
}

void Scene::LateUpdate()
{
	for (auto layer : _layers)
		layer.LateUpdate();
}

void Scene::FinalUpdate()
{
	for (auto layer : _layers)
		layer.FinalUpdate();
}

void Scene::Render()
{
	for (shared_ptr<Camera> cam : _cameras)
		cam->Render();
}

void Scene::AddGameObject(shared_ptr<GameObject> gameObject, LAYER_TYPE type)
{
	_layers[(UINT)type].AddGameObject(gameObject);
	if (type == LAYER_TYPE::CAMERA)
		_cameras.push_back(static_pointer_cast<Camera>(gameObject->GetComponent(Component_Type::Camera)));
	gameObject->Start();
}

void Scene::RemoveGameObject(shared_ptr<GameObject> gameObject, LAYER_TYPE type)
{
	_layers[(UINT)type].RemoveGameObject(gameObject);
}
