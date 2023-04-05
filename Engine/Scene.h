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

	void AddGameObject(shared_ptr<GameObject> GameObject, LAYER_TYPE type);
	void RemoveGameObject(shared_ptr<GameObject> gameObject, LAYER_TYPE type);

	void AddLight(Light* light);

	Camera* GetMainCamera();
	Layer& GetLayer(LAYER_TYPE type) { return _layers[UINT(type)]; }

private:
	void PushLightData();

	vector<class Light*> _lights;
	vector<Layer> _layers;
	vector<class Camera*> _cameras;
};

