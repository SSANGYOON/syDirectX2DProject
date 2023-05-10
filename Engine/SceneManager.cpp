#include "pch.h"
#include "SceneManager.h"
#include "ParentManager.h"
#include "Scene.h"
#include "SceneSerializer.h"
#include "Entity.h"
#include <queue>

namespace SY {
    shared_ptr<Scene> SceneManager::activeScene = nullptr;
    shared_ptr<Scene> SceneManager::nextScene = nullptr;
    map<string, shared_ptr<Scene>> SceneManager::scenes = {};
    stack<thread> SceneManager::loadingThreads = {};

    static std::mutex SceneLock;
    void SceneManager::LoadScene(string path)
    {
        std::filesystem::path parentPath = std::filesystem::current_path().parent_path();
        auto cPath = parentPath / path;

        shared_ptr<Scene> newScene;

        auto it = scenes.find(path);
        if (it != scenes.end())
        {
            newScene = it->second;
        }
        else {
            newScene = make_shared<Scene>();
            SceneSerializer serializer(newScene);
            if (serializer.Deserialize(cPath.string()))
                scenes[path] = newScene;
        }
        
        //SceneLock.lock();
        nextScene = newScene;
        //SceneLock.unlock();
    }
    void SceneManager::LoadSceneAsync(string path)
    {
        loadingThreads.push(thread(SceneManager::LoadScene, path));
    }
    void SceneManager::SetStartScene(shared_ptr<Scene> scene, string name)
    {
        scenes[name] = scene;
        nextScene = scene;
    }
    void SceneManager::OnUpdate(float timeStep)
    {
        //SceneLock.lock();
        if (nextScene != activeScene)
        {
            if (activeScene != nullptr && nextScene != nullptr)
            {
                auto dontDestroy = activeScene->GetDontDestroys();
                queue<Entity> q;
                for (auto entity : dontDestroy)
                {
                    q.push(entity);
                    if (entity.HasComponent<TransformComponent>())
                    {
                        auto& transform = entity.GetComponent<TransformComponent>();
                        transform.translation = entity.GetComponent<DontDestroy>().desiredPos;
                    }
                    while (!q.empty())
                    {
                        Entity e = q.front();
                        q.pop();
                        nextScene->LoadDontDestroy(e);

                        const vector<UINT32>& children = ParentManager::GetChildren((UINT32)e);
                        for (UINT childHandle : children)
                        {
                            Entity child = { (entt::entity)childHandle, activeScene.get() };
                            q.push(child);
                        }
                    }
                }
                activeScene->OnRuntimeShift();
                nextScene->OnRuntimeStart();
            }
            else if(activeScene)
                activeScene->OnRuntimeStop();
            else if(nextScene)
                nextScene->OnRuntimeStart();
        }
        activeScene = nextScene;
        //SceneLock.unlock();
        
        while (!loadingThreads.empty())
        {
            if (loadingThreads.top().joinable())
            {
                loadingThreads.top().join();
                loadingThreads.pop();
            }
            else
                break;
        }

        activeScene->OnUpdateRuntime(timeStep);
        
    }
    void SceneManager::OnStop()
    {
        activeScene->OnRuntimeStop();
        activeScene = nullptr;
        nextScene = nullptr;
        scenes.clear();
    }
}
