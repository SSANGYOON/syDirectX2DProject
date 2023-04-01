#pragma once
class Scene;
class GameObject;
class SceneManager
{
	DECLARE_SINGLE(SceneManager)

public:
	void Update();
	void Render();
	void LoadScene(const wstring& SceneName);

public:
	shared_ptr<Scene> GetActiveScene() { return _activeScene; }
	GameObject* Instantiate(LAYER_TYPE type);

private:
	shared_ptr<Scene> _activeScene;
};

