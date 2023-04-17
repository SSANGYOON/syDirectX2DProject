#pragma once
#include "Entity.h"
#include "Light.h"

class Layer;
class GameObject;
class Camera;
class Light;
class Scene : public Entity
{
public:
	Scene();
	virtual ~Scene();

	virtual void Start();

	void Update();
	void FinalUpdate();
	void Render();

	void AddGameObject(shared_ptr<GameObject> GameObject, LAYER_TYPE type, string tag = "");
	void RemoveGameObject(shared_ptr<GameObject> gameObject);

	void AddLight(Light* light);

	void SetSceneSize(Vector2 sceneSize) { _sceneSize = sceneSize; }
	const Vector2& GetSceneSize() { return _sceneSize; }

	GameObject* FindGameObject(string tag);
	vector<shared_ptr<GameObject>>& GetObjects() { return _gameObjects; }
	Camera* GetMainCamera();

private:
	void hiearchy();
	void PushLightData();

	vector<class Light*> _lights;
	vector<shared_ptr<GameObject>> _gameObjects;
	unordered_map<string, shared_ptr<GameObject>> _tags;
	vector<class Camera*> _cameras;

	Vector2 _sceneSize;
};

