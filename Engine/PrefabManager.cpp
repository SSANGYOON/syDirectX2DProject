#include "pch.h"
#include "PrefabManager.h"
#include "Scene.h"
#include "SceneSerializer.h"
#include <queue>
#include "IDComponent.h"
#include "ParentManager.h"

namespace SY {
	unique_ptr<Scene> PrefabManager::prefabScene = nullptr;
	std::unordered_map<UINT32, vector<UINT>> PrefabManager::childMap = std::unordered_map<UINT32, vector<UINT>>();
	void PrefabManager::Init()
	{
		prefabScene = make_unique<Scene>();

		std::filesystem::path projectPath = std::filesystem::current_path().parent_path();
		std::filesystem::path prefabDir = projectPath.wstring() + L"\\Resources\\assets\\prefabs";

		auto iter = filesystem::directory_iterator(prefabDir);

		for (auto& p : iter)
		{
			if (!p.is_directory())
				loadPrefab(p);
		}

		childMap.clear();
		{
			auto view = prefabScene->m_Registry.view<Parent>();
			for (auto e : view)
			{
				Entity entity = { e,prefabScene.get()};
				auto parent = prefabScene->GetEntityByUUID(entity.GetComponent<Parent>().parentHandle);
				childMap[(UINT)parent].push_back((UINT)e);
			}
		}
	}

	Entity PrefabManager::Instantiate(Scene* targetScene, UUID prefabId, UUID parent)
	{
		Entity entity = prefabScene->GetEntityByUUID(prefabId);
		Entity Instance = targetScene->DuplicateEntity(entity);
		map<uint64_t, uint64_t> uuidMap;

		uuidMap[prefabId] = Instance.GetUUID();
		queue<uint64_t> copyQueue;

		for (auto e : childMap[(UINT)entity])
		{
			copyQueue.push((UINT)e);
		}

		while (!copyQueue.empty())
		{
			entity = { (entt::entity)(copyQueue.front()),prefabScene.get()};
			copyQueue.pop();
			Entity inst = targetScene->DuplicateEntity(entity);

			uuidMap[entity.GetUUID()] = inst.GetUUID();
			auto& parentId = inst.GetComponent<Parent>();
			parentId.parentHandle = uuidMap[parentId.parentHandle];
			ParentManager::AddHiearchy(targetScene, inst.GetUUID());

			for (auto e : childMap[(UINT)entity])
			{
				copyQueue.push((UINT)e);
			}
		}
		if (parent) {
			auto& p = Instance.AddComponent<Parent>();
			p.parentHandle = parent;
		}
		ParentManager::AddHiearchy(targetScene, Instance.GetUUID());
		return Instance;
	}
	Entity PrefabManager::GetPrefabIdFromName(string name)
	{
		return prefabScene->FindEntityByName(name);
	}

	string PrefabManager::GetPrefabName(UUID id)
	{
		Entity ent = prefabScene->GetEntityByUUID(id);

		if (ent)
			return ent.GetComponent<TagComponent>().Tag;
		else
			return "";
	}

	HRESULT PrefabManager::loadPrefab(std::filesystem::directory_entry entry)
	{
		filesystem::path prefabPath = filesystem::path(entry);
		map<uint64_t, uint64_t> uuidMap;
		if (prefabPath.extension().string().compare(".pref") == 0)
		{
			YAML::Node data;
			try
			{
				data = YAML::LoadFile(wtos(prefabPath));
			}
			catch (YAML::ParserException e)
			{
				assert(false);
				return S_FALSE;
			}

			for (const auto& entity : data["Entities"]) {
				uint64_t uuid = entity["Entity"].as<uint64_t>();
			}

			for (auto entity : data["Entities"]) {
				uint64_t uuid = entity["Entity"].as<uint64_t>();
				Entity instance = SceneSerializer::DeserializeEntity(prefabScene.get(), entity);
			}
		}

		return S_OK;
	}
}