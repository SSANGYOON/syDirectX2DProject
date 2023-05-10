#pragma once
#include <unordered_map>

namespace SY {
	class Scene;
	class ParentManager
	{
	public:
		static void CreateHierarchy(Scene* scene);
		static void CreateToWorld(Scene* scene);
		static void AddHiearchy(Scene* scene, uint64_t child);
		static const vector<UINT32>& GetChildren(UINT handle);
		static UINT64 GetChild(UINT64 parent, string child, Scene* scene);

	private:
		static std::unordered_map<UINT, vector<UINT32>> childMap;
	};
}

