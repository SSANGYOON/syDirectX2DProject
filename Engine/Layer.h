#pragma once
class GameObject;



class Layer
{
public:
	Layer();
	~Layer();

	void Start();
	void Update();
	void LateUpdate();
	void FinalUpdate();
	void Render();
	void AddGameObject(shared_ptr<GameObject> gameObject);
	void RemoveGameObject(shared_ptr<GameObject> gameObject);
	const vector<shared_ptr<GameObject>>& GetGameObjects() { return _gameObjects; }

private:
	vector<shared_ptr<GameObject>> _gameObjects;
};

