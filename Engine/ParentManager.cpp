#include "pch.h"
#include "ParentManager.h"
#include "SceneManager.h"
#include "Scene.h"
#include "entt.hpp"
#include "Component.h"
#include "Entity.h"
#include <queue>
#include "ScriptEngine.h"
#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"
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
						auto prev = sc.currentState;
						sc.currentState = sc.state;
						break;
					}
				}

				if (sc.currentState == EntityState::Pause && !entity.HasComponent<Pause>()) {
					entity.AddOrReplaceComponent<Pause>();

					if (entity.HasComponent<Rigidbody2DComponent>())
					{
						auto& rb = entity.GetComponent<Rigidbody2DComponent>();
						b2Body* body = (b2Body*)rb.RuntimeBody;

						if (body)
							body->SetEnabled(false);
					}

					else if (entity.HasComponent<BoxCollider2DComponent>() || entity.HasComponent<CircleCollider2DComponent>())
					{
						b2Fixture* fix = nullptr;

						if (entity.HasComponent<BoxCollider2DComponent>()) {
							auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();
							fix = (b2Fixture*)bc2d.RuntimeFixture;
						}
						
						else{
							auto& cc2d = entity.GetComponent<CircleCollider2DComponent>();
							fix = (b2Fixture*)cc2d.RuntimeFixture;
						}

						if (fix) {
							auto body = fix->GetBody();
							body->DestroyFixture(fix);
							if(entity.HasComponent<BoxCollider2DComponent>())
								entity.GetComponent<BoxCollider2DComponent>().RuntimeFixture = nullptr;
							else
								entity.GetComponent<CircleCollider2DComponent>().RuntimeFixture = nullptr;
						}
					}
				}
				else if (sc.currentState == EntityState::Active && entity.HasComponent<Pause>()) {
					entity.RemoveComponent<Pause>();

					if (entity.HasComponent<Rigidbody2DComponent>())
					{
						auto& rb = entity.GetComponent<Rigidbody2DComponent>();
						b2Body* body = (b2Body*)rb.RuntimeBody;
						auto& tr = entity.GetComponent<TransformComponent>();

						if (body) {
							body->SetEnabled(true);
							body->SetTransform({ tr.translation.x,tr.translation.y }, tr.rotation.z);
						}
					}

					else if (entity.HasComponent<BoxCollider2DComponent>() || entity.HasComponent<CircleCollider2DComponent>())
					{
						b2Vec2 offset = b2Vec2_zero;

						if (entity.HasComponent<BoxCollider2DComponent>()) {
							auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();
							b2Vec2  offset = { bc2d.Offset.x, bc2d.Offset.y };
						}

						else
						{
							auto& cc2d = entity.GetComponent<CircleCollider2DComponent>();
							b2Vec2  offset = { cc2d.Offset.x, cc2d.Offset.y };
						}
						float angle = 0.f;

						b2Body* body = nullptr;
						Entity root = entity;

						while (root.IsValid())
						{
							if (root.HasComponent<Rigidbody2DComponent>()) {
								body = (b2Body*)root.GetComponent<Rigidbody2DComponent>().RuntimeBody;
								break;
							}
							else {
								auto& trans = root.GetComponent<TransformComponent>();
								b2Rot r(trans.rotation.z);

								offset += {trans.translation.x* r.c - trans.translation.y * r.s, trans.translation.x* r.s + trans.translation.y * r.c};
								angle += trans.rotation.z;

								root = root.GetContext()->GetEntityByUUID(root.GetComponent<Parent>().parentHandle);
							}
						}
						if (body) {
							float flip = root.GetComponent<Rigidbody2DComponent>().flip ? -1.f : 1.f;
							entity.GetContext()->AddFixture(entity, offset, angle, body, root.GetComponent<Rigidbody2DComponent>().flip);
						}
					}
				}
				if (sc.currentState == EntityState::Dead) {
					entity.AddOrReplaceComponent<Dead>();
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

			if (childMap.find((UINT)parent) == childMap.end())
				childMap[(UINT)parent] = vector<UINT>();

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
