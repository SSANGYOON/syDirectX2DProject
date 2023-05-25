#include "pch.h"
#include "ParentManager.h"
#include "SceneManager.h"
#include "Scene.h"
#include "entt.hpp"
#include "Component.h"
#include "Entity.h"
#include <queue>
#include "ScriptEngine.h"

namespace SY {
	std::unordered_map<UINT32, vector<UINT>> ParentManager::childMap = std::unordered_map<UINT32, vector<UINT>>();

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
			scene->m_Registry.view<StateComponent>(entt::exclude<Parent>).each([&updatequeue, scene](entt::entity entity, StateComponent& sc) {
				Entity ent = { entity,scene };

				sc.currentState = sc.state;

				if (sc.currentState == EntityState::Pause && !ent.HasComponent<Pause>())
					ent.AddOrReplaceComponent<Pause>();
				else if (sc.currentState == EntityState::Active && ent.HasComponent<Pause>()) {
					ent.RemoveComponent<Pause>();
					ScriptEngine::OnActivated(ent);
				}
					
				
				for (auto e : childMap[(UINT)entity])
				{
					updatequeue.push((UINT)e);
				}

				if(sc.currentState == EntityState::Dead)
					ent.AddOrReplaceComponent<Dead>();
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
						break;
					}
					default:
					{
						sc.currentState = sc.state;
						break;
					}
				}

				if (sc.currentState == EntityState::Pause && !entity.HasComponent<Pause>())
					entity.AddOrReplaceComponent<Pause>();
				else if (sc.currentState == EntityState::Active && entity.HasComponent<Pause>())
					entity.RemoveComponent<Pause>();
				if (sc.currentState == EntityState::Dead)
					entity.AddOrReplaceComponent<Dead>();

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

		scene->m_Registry.view<RectTransformComponent>(entt::exclude<Parent>).each([&updatequeue](entt::entity entity, RectTransformComponent& tr) {
			tr.worldTranslation = tr.translation;
			tr.CreatToWorld();
			for (auto e : childMap[(UINT)entity])
			{
				updatequeue.push((UINT)e);
			}
			});

		while (!updatequeue.empty())
		{
			Entity entity = { (entt::entity)(updatequeue.front()),scene };
			updatequeue.pop();
			auto& tr = entity.GetComponent<RectTransformComponent>();
			UUID parenUUID = entity.GetComponent<Parent>().parentHandle;
			auto parent = scene->GetEntityByUUID(parenUUID);
			auto& parentTr = parent.GetComponent<RectTransformComponent>();

			tr.worldTranslation = parentTr.worldTranslation + tr.translation;
			tr.CreatToWorld();
			for (auto e : childMap[(UINT)entity])
			{
				updatequeue.push((UINT)e);
			}
		}
	}

	void ParentManager::AddHiearchy(Scene* scene, uint64_t childID)
	{
		auto child = scene->GetEntityByUUID(childID);
		if (child.HasComponent<Parent>())
		{
			
			auto parent = scene->GetEntityByUUID(child.GetComponent<Parent>().parentHandle);

			childMap[(UINT)parent].push_back((UINT)child);
		}
	}

	const vector<UINT32>& ParentManager::GetChildren(UINT handle)
	{
		if (childMap.find(handle) == childMap.end())
		{
			childMap[handle] = {};
		}

		return childMap[handle];
	}

	uint64_t ParentManager::GetChild(UINT64 parentID, string child, Scene* scene)
	{
		auto parent = scene->GetEntityByUUID(parentID);

		auto& childmaps = childMap[(UINT)parent];
		for (auto c : childmaps)
		{
			Entity childEntity = { (entt::entity)c, scene };

			auto& tag = childEntity.GetComponent<TagComponent>();

			if (tag.Tag == child)
			{
				return childEntity.GetUUID();
			}
		}
		return 0;
	}
}
