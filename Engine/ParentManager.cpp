#include "pch.h"
#include "ParentManager.h"
#include "SceneManager.h"
#include "Scene.h"
#include "entt.hpp"
#include "Component.h"
#include "Entity.h"
#include <queue>

namespace SY {
	std::unordered_map<UINT32, vector<UINT>> ParentManager::childMap = {};

	void ParentManager::CreateHierarchy(Scene* scene)
	{
		childMap.clear();
		{
			auto view = scene->m_Registry.view<Parent>();
			for (auto e : view)
			{
				Entity entity = { e,scene };

				auto parent = scene->GetEntityByUUID(entity.GetComponent<Parent>().parentHandle);

				childMap[(UINT)parent].push_back((UINT)e);
			}
		}

		{
			queue<UINT> updatequeue;
			scene->m_Registry.view<StateComponent>(entt::exclude<Parent>).each([&updatequeue](entt::entity entity, StateComponent& sc) {
				sc.currentState = sc.state;
				for (auto e : childMap[(UINT)entity])
				{
					updatequeue.push((UINT)e);
				}
				});

			while (!updatequeue.empty())
			{
				Entity entity = { (entt::entity)(updatequeue.front()),scene };
				updatequeue.pop();
				auto& sc = entity.GetComponent<StateComponent>();
				UUID parenUUID = entity.GetComponent<Parent>().parentHandle;
				auto parent = scene->GetEntityByUUID(parenUUID);
				auto& parentSC = parent.GetComponent<StateComponent>();			
				switch (parentSC.currentState)
				{
					case EntityState::Dead:
					{
						sc.currentState = EntityState::Dead;
						break;
					}
					case EntityState::Pause:
					{
						if (sc.state == EntityState::Dead)
							sc.currentState = EntityState::Dead;
						else
							sc.currentState = EntityState::Pause;
					}
					default:
					{
						sc.currentState = sc.currentState;
						break;
					}
				}

				for (auto e : childMap[(UINT)entity])
				{
					updatequeue.push((UINT)e);
				}
			}
		}
	}

	void ParentManager::CreateToWorld(Scene* scene)
	{
		queue<UINT> updatequeue;
		scene->m_Registry.view<TransformComponent>(entt::exclude<Parent>).each([&updatequeue](entt::entity entity, TransformComponent& tr) {
			tr.localToWorld = tr.localToWorld;
			for (auto e : childMap[(UINT)entity])
			{
				updatequeue.push((UINT)e);
			}
			});

		while (!updatequeue.empty())
		{
			Entity entity = { (entt::entity)(updatequeue.front()),scene };
			updatequeue.pop();
			auto& tr = entity.GetComponent<TransformComponent>();
			UUID parenUUID = entity.GetComponent<Parent>().parentHandle;
			auto parent = scene->GetEntityByUUID(parenUUID);
			auto& parentTr = parent.GetComponent<TransformComponent>();

			tr.localToWorld = tr.localToParent * parentTr.localToWorld;

			for (auto e : childMap[(UINT)entity])
			{
				updatequeue.push((UINT)e);
			}
		}
	}

	UINT32 ParentManager::GetChild(UINT handle, int index)
	{
		assert(childMap.find(handle) != childMap.end());
		return childMap[handle][index];
	}
}
