#pragma once
#include "Entity.h"
#include <unordered_set>

namespace SY {
	class PrefabManager
	{
	public:
		static void Init();
		static Entity Instantiate(Scene* targetScene, UUID id, UUID parent = 0);
		static Entity GetPrefabIdFromName(string name);
		static string GetPrefabName(UUID id);
	private:
		static HRESULT loadPrefab(std::filesystem::directory_entry entry);
		static unique_ptr<Scene> prefabScene;
		static std::unordered_map<UINT, vector<UINT32>> childMap;
	};
}

