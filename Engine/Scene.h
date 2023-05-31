#pragma once

#include "EditorCamera.h"
#include "entt.hpp"
#include "UUID.h"
#include "box2d/b2_math.h"

class b2World;
class b2Body;

namespace SY {

	class Entity;

	class Scene
	{
	public:
		Scene();
		~Scene();

		static shared_ptr<Scene> Copy(shared_ptr<Scene> other);

		Entity CreateEntity(const std::string& name = std::string());
		Entity CreateEntityWithUUID(UUID uuid, const std::string& name = std::string());
		void DestroyEntity(Entity entity);

		void OnRuntimeStart();
		void OnRuntimeStop();

		void OnRuntimeShift();

		void OnSimulationStart();
		void OnSimulationStop();

		void OnUpdateRuntime(float ts);
		void OnUpdateSimulation(float ts, EditorCamera& camera);
		void OnUpdateEditor(float ts, EditorCamera& camera);
		void OnViewportResize(uint32_t width, uint32_t height);

		void AddBody(Entity entity);
		void AddFixture(Entity entity, const b2Vec2& offset, float angle, b2Body* body, float flip);

		vector<Entity> GetDontDestroys();
		void UpdateTransform();

		b2World* GetBox2dWorld() { return m_PhysicsWorld; }

		Entity DuplicateEntity(Entity entity);
		Entity LoadDontDestroy(Entity entity);
		Entity FindEntityByName(std::string_view name);
		Entity GetEntityByUUID(UUID uuid);

		Entity GetPrimaryCameraEntity();

		bool IsRunning() const { return m_IsRunning; }
		bool IsPaused() const { return m_IsPaused; }

		void SetPaused(bool paused) { m_IsPaused = paused; }

		void Step(int frames = 1);

		template<typename... Components>
		auto GetAllEntitiesWith()
		{
			return m_Registry.view<Components...>();
		}
	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);

		void OnPhysics2DStart();
		void OnPhysics2DStop();	

		void OnPhysicsUpdate(float timeStep);
		void RenderScene(EditorCamera& camera);
	private:
		entt::registry m_Registry;
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
		bool m_IsRunning = false;
		bool m_IsPaused = false;
		int m_StepFrames = 0;

		b2World* m_PhysicsWorld = nullptr;

		std::unordered_map<UUID, entt::entity> m_EntityMap;

		float accTime;

		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyPanel;
		friend class ParentManager;
		friend class PrefabManager;
		friend class InstancingManager;
	};

}
