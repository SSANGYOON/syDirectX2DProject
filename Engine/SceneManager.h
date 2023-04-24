#pragma once


namespace SY {
	class Scene;
	class SceneManager
	{
	public:
		static shared_ptr<Scene>& GetActiveScene();
		static void ChanegeScene(string name);

		static void AddScene(string tag, shared_ptr<Scene> scene);
		static void RemoveScene(string tag);
	private:
		static shared_ptr<Scene> currentScene;
		static map<string , shared_ptr<Scene>> _scenes;
	};
}

