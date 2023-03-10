#pragma once
#include "Entity.h"

class Layer;
class GameObject;
class Camera;
class Scene : public Entity
{
public:
	Scene();
	virtual ~Scene();

	virtual void Start();
	void Update();
	void LateUpdate();
	void FinalUpdate();
	void Render();

	void AddGameObject(shared_ptr<GameObject> GameObject, LAYER_TYPE type);
	void RemoveGameObject(shared_ptr<GameObject> gameObject, LAYER_TYPE type);

	shared_ptr<Camera> GetMainCamera() { return _cameras[0]; }
	Layer& GetLayer(LAYER_TYPE type) { return _layers[UINT(type)]; }

private:
	vector<Layer> _layers;
	vector<shared_ptr<class Camera>> _cameras;
};

