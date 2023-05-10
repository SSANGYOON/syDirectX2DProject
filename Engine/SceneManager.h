#pragma once
#include <stack>

namespace SY {
	class Scene;
	class SceneManager
	{
	public:
		static void LoadScene(string path);
		static void LoadSceneAsync(string path);
		static void SetStartScene(shared_ptr<Scene> scene, string name);
		static void OnUpdate(float timeStep);
		static void OnStop();
		static shared_ptr<Scene> GetActiveScene() { return activeScene; }

	private:
		static shared_ptr<Scene> activeScene;
		static shared_ptr<Scene> nextScene;
		static map<string, shared_ptr<Scene>> scenes;
		static std::stack<thread> loadingThreads;
	};
}

