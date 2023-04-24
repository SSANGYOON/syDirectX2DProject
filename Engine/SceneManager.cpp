#include "pch.h"
#include "SceneManager.h"
#include "Scene.h"


namespace SY {
    shared_ptr<Scene> SceneManager::currentScene = nullptr;
    map<string, shared_ptr<Scene>> SceneManager::_scenes = {};

    shared_ptr<Scene>& SceneManager::GetActiveScene()
    {
        return currentScene;
    }

    void SceneManager::ChanegeScene(string name)
    {
        assert(_scenes.find(name) != _scenes.end());
        currentScene = _scenes[name];
    }

    void SceneManager::RemoveScene(string tag)
    {
        auto it = _scenes.find(tag);
        if (it != _scenes.end())
            _scenes.erase(tag);
    }

    void SceneManager::AddScene(string tag, shared_ptr<Scene> scene)
    {
        _scenes[tag] = scene;
        if (currentScene == nullptr)
            currentScene = scene;
    }
}
