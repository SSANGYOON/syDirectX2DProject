#include "pch.h"
#include "Scene.h"
#include "Layer.h"
#include "Camera.h"
#include "GameObject.h"
#include "Light.h"
#include "ConstantBuffer.h"

#include "Timer.h"
#include "Engine.h"

Scene::Scene()
{
}

Scene::~Scene()
{
}

void Scene::Start()
{
	GET_SINGLE(Timer)->Init();
	for (auto& obj : _gameObjects)
		obj->Start();
}

void Scene::Update()
{
	hiearchy();
	for (auto& obj : _gameObjects)
		obj->Update();
}

void Scene::FinalUpdate()
{
	for (auto& obj : _gameObjects)
		obj->FinalUpdate();
}

void Scene::Render()
{
	PushLightData();
	for (Camera* cam : _cameras)
		cam->Render();
}

GameObject* Scene::FindGameObject(string tag)
{
	auto it = _tags.find(tag);
	if (it == _tags.end())
		return nullptr;
	else
		return it->second.get();
}

Camera* Scene::GetMainCamera()
{
	if (_cameras.size())
		return _cameras[0];
	else
		return nullptr;
}
void Scene::hiearchy()
{
}
void Scene::PushLightData()
{
	LightCB lightParams = {};

	for (auto& light : _lights)
	{
		const LightInfo& lightInfo = light->GetLightInfo();

		lightParams.lights[lightParams.lightCount] = lightInfo;
		lightParams.lightCount++;
	}

	shared_ptr<ConstantBuffer> cb = GEngine->GetConstantBuffer(Constantbuffer_Type::LIGHT);
	cb->SetData(&lightParams);
	cb->SetPipline(ShaderStage::PS);

}

void Scene::AddGameObject(shared_ptr<GameObject> GameObject, LAYER_TYPE type, string tag)
{

	if (type == LAYER_TYPE::CAMERA)
		_cameras.push_back(GameObject->GetComponent<Camera>());

	GameObject->SetLayer(type);
	_gameObjects.push_back(GameObject);

	if (tag.empty())
		tag = "Empty ID : " + to_string(GameObject->GetID());
	
	_tags[tag] = GameObject;
}

void Scene::RemoveGameObject(shared_ptr<GameObject> gameObject)
{
	auto findIt = std::find(_gameObjects.begin(), _gameObjects.end(), gameObject);
	if (findIt != _gameObjects.end())
		_gameObjects.erase(findIt);
}

void Scene::AddLight(Light* light)
{
	_lights.push_back(light);
}
