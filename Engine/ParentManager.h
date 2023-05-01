#pragma once

namespace SY {
	class Scene;
	class ParentManager
	{
	public:
		static void CreateToWorld(Scene* scene);

		static vector<UINT32>& GetChildren(UINT handle) { return childMap[handle]; }
		static UINT32 GetChild(UINT handle, int index);

	private:
		static std::unordered_map<UINT, vector<UINT32>> childMap;
	};
}

