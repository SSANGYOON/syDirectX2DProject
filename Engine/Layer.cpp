#include "pch.h"
#include "Layer.h"
#include "GameObject.h"

Layer::Layer()
{
}

Layer::~Layer()
{
}

void Layer::Start()
{
	for (auto obj : _gameObjects)
	{
		if (obj == nullptr)
			continue;

		obj->Start();
	}
}

void Layer::Update()
{
	for (auto obj : _gameObjects)
	{
		if (obj == nullptr)
			continue;

		obj->Update();
	}
}

void Layer::LateUpdate()
{
	for (auto obj : _gameObjects)
	{
		if (obj == nullptr)
			continue;

		obj->LateUpdate();
	}
}

void Layer::FinalUpdate()
{
	for (auto obj : _gameObjects)
	{
		if (obj == nullptr)
			continue;

		obj->FinalUpdate();
	}
}

void Layer::Render()
{
	for (auto obj : _gameObjects)
	{
		if (obj == nullptr)
			continue;

		obj->Render();
	}
}

void Layer::AddGameObject(shared_ptr<GameObject> gameObject)
{
	_gameObjects.push_back(gameObject);
}

void Layer::RemoveGameObject(shared_ptr<GameObject> gameObject)
{
	auto findIt = std::find(_gameObjects.begin(), _gameObjects.end(), gameObject);
	if (findIt != _gameObjects.end())
		_gameObjects.erase(findIt);
}
