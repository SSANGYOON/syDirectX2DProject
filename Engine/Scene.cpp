#include "pch.h"
#include "Engine.h"
#include "ConstantBuffer.h"
#include "Resources.h"

#include "Mesh.h"

#include "Material.h"
#include "Texture.h"
#include "Shader.h"

#include "Scene.h"
#include "Entity.h"

#include "Component.h"
#include "ScriptableEntity.h"
#include "Renderer.h"
#include "Physics2D.h"

#include "Entity.h"
#include "ParentManager.h"
#include "CollisionManager.h"

#include "box2d/b2_world.h"
#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_polygon_shape.h"
#include "box2d/b2_circle_shape.h"
#include "box2d/b2_contact.h"

#include "box2d/b2_revolute_joint.h"
#include "box2d/b2_distance_joint.h"

#include "ScriptEngine.h"
#include "Animation.h"
#include "InstancingManager.h"


namespace SY {

	Scene::Scene()
	{
	}

	Scene::~Scene()
	{
		delete m_PhysicsWorld;
	}

	template<typename... Component>
	static void CopyComponent(entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
	{
		([&]()
			{
				auto view = src.view<Component>();
				for (auto srcEntity : view)
				{
					entt::entity dstEntity = enttMap.at(src.get<IDComponent>(srcEntity).ID);

					auto& srcComponent = src.get<Component>(srcEntity);
					dst.emplace_or_replace<Component>(dstEntity, srcComponent);
				}
			}(), ...);
	}

	template<typename... Component>
	static void CopyComponent(ComponentGroup<Component...>, entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
	{
		CopyComponent<Component...>(dst, src, enttMap);
	}

	template<typename... Component>
	static void CopyComponentIfExists(Entity dst, Entity src)
	{
		([&]()
			{
				if (src.HasComponent<Component>())
					dst.AddOrReplaceComponent<Component>(src.GetComponent<Component>());
			}(), ...);
	}

	template<typename... Component>
	static void CopyComponentIfExists(ComponentGroup<Component...>, Entity dst, Entity src)
	{
		CopyComponentIfExists<Component...>(dst, src);
	}

	shared_ptr<Scene> Scene::Copy(shared_ptr<Scene> other)
	{
		shared_ptr<Scene> newScene = make_shared<Scene>();

		newScene->m_ViewportWidth = other->m_ViewportWidth;
		newScene->m_ViewportHeight = other->m_ViewportHeight;

		auto& srcSceneRegistry = other->m_Registry;
		auto& dstSceneRegistry = newScene->m_Registry;
		std::unordered_map<UUID, entt::entity> enttMap;

		// Create entities in new scene
		auto idView = srcSceneRegistry.view<IDComponent>();
		for (auto e : idView)
		{
			UUID uuid = srcSceneRegistry.get<IDComponent>(e).ID;
			const auto& name = srcSceneRegistry.get<TagComponent>(e).Tag;
			Entity newEntity = newScene->CreateEntityWithUUID(uuid, name);
			enttMap[uuid] = (entt::entity)newEntity;
		}

		// Copy components (except IDComponent and TagComponent)
		CopyComponent(AllComponents{}, dstSceneRegistry, srcSceneRegistry, enttMap);

		return newScene;
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		return CreateEntityWithUUID(UUID(), name);
	}

	Entity Scene::CreateEntityWithUUID(UUID uuid, const std::string& name)
	{
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<IDComponent>(uuid);
		auto& state = entity.AddComponent<StateComponent>();
		state.state = EntityState::Active;
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;

		m_EntityMap[uuid] = entity;

		return entity;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		if (entity.HasComponent<Rigidbody2DComponent>())
		{
			auto& rb = entity.GetComponent< Rigidbody2DComponent>();
			b2Body* body = (b2Body*)rb.RuntimeBody;

			if(m_PhysicsWorld)
				m_PhysicsWorld->DestroyBody(body);
		}
		if (entity.HasComponent<ScriptComponent>())
		{
			ScriptEngine::OnDeleteEntity(entity);
		}
		m_EntityMap.erase(entity.GetUUID());
		m_Registry.destroy(entity);
	}

	void Scene::OnRuntimeStart()
	{
		m_IsRunning = true;

		OnPhysics2DStart();

		ParentManager::CreateHierarchy(this);

		auto view = m_Registry.view<ParticleSystem>();

		for (auto e : view)
		{
			auto& p = m_Registry.get<ParticleSystem>(e);
			p.Init();
		}

		{
			ScriptEngine::OnRuntimeStart(this);
			// Instantiate all script entities

			auto view = m_Registry.view<ScriptComponent>();
			for (auto e : view)
			{
				Entity entity = { e, this };
				ScriptEngine::OnCreateEntity(entity);
			}
		}

		accTime = 0.f;
	}

	void Scene::OnRuntimeStop()
	{
		m_IsRunning = false;

		OnPhysics2DStop();

		ScriptEngine::OnRuntimeStop();
	}

	void Scene::OnRuntimeShift()
	{
		m_IsRunning = false;

		OnPhysics2DStop();
	}

	void Scene::OnSimulationStart()
	{
		OnPhysics2DStart();
	}

	void Scene::OnSimulationStop()
	{
		OnPhysics2DStop();
	}

	void Scene::OnUpdateRuntime(float timeStep)
	{
		accTime += timeStep;
		m_Registry.view<Dead>().each([this](entt::entity e, Dead& dead) {
			Entity ent = { e,this };
			DestroyEntity(ent);
			});

		ParentManager::CreateHierarchy(this);
		if (!m_IsPaused || m_StepFrames-- > 0)
		{
			auto view = m_Registry.view<ScriptComponent>(entt::exclude<Pause>);
			for (auto e : view)
			{
				Entity entity = { e, this };
				ScriptEngine::OnUpdateEntity(entity, timeStep);
			}

			OnPhysicsUpdate(timeStep);	
		}

		{
			m_Registry.view<SpriteAnimatorComponent, SpriteRendererComponent>().each([timeStep, this](entt::entity e, SpriteAnimatorComponent& animator, SpriteRendererComponent& sr)
				{	
					if (animator._currentClip == nullptr)
						return;

					auto clip = animator._currentClip;
					animator._lastTime = animator._currentTime;
					animator._currentTime += timeStep;
					
					UINT frames = clip->GetFrames();
					UINT columns = clip->GetColumns();
					float duration = clip->GetDuration();
					const Vector2& offset = clip->GetOffset();
					const Vector2& size = clip->GetSize();
					const Vector2& step = clip->GetStep();
					const Vector2& targetOffset = clip->GetTargetOffset();
					const string& nextKey = clip->GetNextKey();
					bool loop = clip->IsLoop();
					float epsilon = 1e-5f;

					UINT curFrame = UINT(min(animator._currentTime, duration - epsilon) * clip->GetFrames() / clip->GetDuration());
					Vector2 LT = offset + Vector2((curFrame % columns) * step.x, (curFrame / columns) * step.y);

					sr.Offset = LT;
					sr.tile = size;

					if (animator._currentTime > duration)
					{
						if (animator._currentTime - timeStep < duration) {
							if (animator._endEvent.find(clip->GetKey()) != animator._endEvent.end())
								ScriptEngine::OnEvent({ e, this }, animator._endEvent[clip->GetKey()]);
							if (loop)
								animator._currentTime -= duration;
							if (!nextKey.empty()) {
								animator._currentTime = 0.f;
								animator._currentClip = animator.clips[nextKey];
								int a = 0;
							}
						}
					}
				});
		}

		{
			m_Registry.view<TransformAnimatorComponent, TransformComponent>().each([timeStep, this](entt::entity e, TransformAnimatorComponent& animator, TransformComponent& transform)
				{
					if (animator._currentClip == nullptr)
						return;

					shared_ptr<TransformAnimation> clip = animator._currentClip;
					animator._lastTime = animator._currentTime;
					animator._currentTime += timeStep;

					const vector<TransformFrame>& clipFrames = clip->GetFrames();
					UINT frames = clipFrames.size();
					float duration = clip->GetDuration();
					float epsilon = 1e-5f;

					UINT curFrame = UINT(min(animator._currentTime, duration - epsilon) * (frames - 1) / duration);
					UINT nextFrame = curFrame + 1;
					float ratio = animator._currentTime / duration * (frames - 1) - curFrame;
					transform.translation = 10 * Vector3::Lerp(clipFrames[curFrame].position, clipFrames[nextFrame].position, ratio);
					transform.rotation.z = clipFrames[curFrame].angle * (1 - ratio) + clipFrames[nextFrame].angle * (ratio);
					transform.rotation.z *= XM_PI / 180.f;

				});
		}

		auto view = m_Registry.view<BackGroundColorComponent>();
		for (auto entity : view)
		{
			auto& [backGround] = view.get<BackGroundColorComponent>(entity);
			GEngine->ClearRenderTargetGroup(RENDER_TARGET_GROUP_TYPE::DEFFERED, 0, reinterpret_cast<float*>(&backGround));
		}

		CameraComponent* mainCamera = nullptr;
		Matrix cameraTransform;
		Matrix cameraProjection;
		{
			auto view = m_Registry.view<TransformComponent, CameraComponent>();
			for (auto entity : view)
			{
				auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

				if (camera.Primary)
				{
					mainCamera = &camera;
					cameraTransform = transform.localToWorld;
					break;
				}
			}
		}
		


#pragma region LightRuntime
		{
			auto view = m_Registry.view<TransformComponent, Light>();
			int lightnum = 0;
			LightCB cb;
			for (auto entity : view)
			{
				auto [transform, light] = view.get<TransformComponent, Light>(entity);
				light.info.position = transform.localToWorld.Translation();
				cb.lights[lightnum++] = light.info;
			}
			cb.lightCount = lightnum;
			auto LightBuffer = GEngine->GetConstantBuffer(Constantbuffer_Type::LIGHT);

			LightBuffer->SetData(&cb);
			LightBuffer->SetPipline(ShaderStage::PS);
		}
#pragma endregion
		if (mainCamera)
		{
			Renderer::Begin(mainCamera->Camera, cameraTransform);
			{
				GEngine->BindRenderTargetGroup(RENDER_TARGET_GROUP_TYPE::HDR);
				auto erasers = m_Registry.group<Eraser>(entt::get<TransformComponent>, entt::exclude<Pause>);
				for (auto entity : erasers)
				{
					auto [transform, eraser] = erasers.get<TransformComponent, Eraser>(entity);
					eraser.SetMaterial();
					Renderer::DrawRect(transform.localToWorld, eraser.material, entity);
				}

				Vector2 cameraViewport = { mainCamera->Camera.GetOrthographicSize() * mainCamera->Camera.GetAspectRatio(), mainCamera->Camera.GetOrthographicSize() };
				auto group = m_Registry.group<SpriteRendererComponent>(entt::get<TransformComponent>, entt::exclude<Pause>);
				vector<Entity> defferedBatches;
				vector<Entity> fowardBatches;

				VECB ve;
				ve.DeltaTime = timeStep;
				ve.time = accTime;
				ve.ViewPort = { mainCamera->Camera.GetOrthographicSize() * mainCamera->Camera.GetAspectRatio(), mainCamera->Camera.GetOrthographicSize() };

				auto veCB = GEngine->GetConstantBuffer(Constantbuffer_Type::VISUALEFFECT);
				veCB->SetData(&ve);
				veCB->SetPipline(ShaderStage::VS);
				veCB->SetPipline(ShaderStage::PS);
				veCB->SetPipline(ShaderStage::CS);

				for (auto entity : group)
				{
					auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
					if ((mainCamera->LayerBit & sprite.LayerBit) == 0)
						continue;

					Vector3 diff = transform.localToWorld.Translation() - cameraTransform.Translation();
					Vector2 scale2D = { transform.scale.x, transform.scale.y };
					Vector2 diffuseSize = Vector2::Max(sprite.tile, scale2D);
					
					if (diff.x - diffuseSize.x / 2.f < cameraViewport.x / 2.f && diff.x + diffuseSize.x / 2.f > -cameraViewport.x / 2.f &&
						diff.y - diffuseSize.y / 2.f < cameraViewport.y / 2.f && diff.y + diffuseSize.y / 2.f > -cameraViewport.y / 2.f) {
						sprite.SetMaterial();
						if (sprite.Deffered)
							defferedBatches.push_back({ entity,this });
						else
							fowardBatches.push_back({ entity,this });
					}
				}
				GEngine->BindRenderTargetGroup(RENDER_TARGET_GROUP_TYPE::DEFFERED);
				GET_SINGLE(InstancingManager)->Render(defferedBatches);
				GET_SINGLE(InstancingManager)->ClearBuffer();
				GEngine->BindRenderTargetGroup(RENDER_TARGET_GROUP_TYPE::HDR);
				Renderer::DrawDeffered();
				GET_SINGLE(InstancingManager)->Render(fowardBatches);
				GET_SINGLE(InstancingManager)->ClearBuffer();

				auto circles = m_Registry.group<CircleRendererComponent>(entt::get<TransformComponent>, entt::exclude<Pause>);

				for (auto entity : circles)
				{
					auto [transform, circle] = circles.get<TransformComponent, CircleRendererComponent>(entity);
					Renderer::DrawCircle(transform.localToWorld, circle.Color, entity);
				}

				auto particles = m_Registry.group<ParticleSystem>(entt::get<TransformComponent>, entt::exclude<Pause>);

				for (auto entity : particles)
				{
					auto [particle, transform] = particles.get<ParticleSystem, TransformComponent>(entity);
					particle.OnUpdate(timeStep, transform);
					particle.OnRender(transform, entity);
				}

				auto group2 = m_Registry.group<PanelComponent>(entt::get<RectTransformComponent>, entt::exclude<Pause>);
				for (auto entity : group2)
				{
					auto [transform, panel] = group2.get<RectTransformComponent, PanelComponent>(entity);
					panel.SetMaterial();
					Renderer::DrawPoint(transform.localToWorld, panel.material, entity);
				}

				auto group3 = m_Registry.group<SliderComponent>(entt::get<RectTransformComponent>, entt::exclude<Pause>);
				for (auto entity : group3)
				{
					auto [transform, slider] = group3.get<RectTransformComponent, SliderComponent>(entity);
					slider.SetMaterial();
					Renderer::DrawRect(transform.localToWorld, slider.material, entity);
				}

				auto group4 = m_Registry.group<SlotComponent>(entt::get<RectTransformComponent>, entt::exclude<Pause>);
				for (auto entity : group4)
				{
					auto [transform, slot] = group4.get<RectTransformComponent, SlotComponent>(entity);
					slot.SetMaterial();
					Renderer::DrawRect(transform.localToWorld, slot.material, entity);
				}

				auto group5 = m_Registry.group<IconComponent>(entt::get<RectTransformComponent>, entt::exclude<Pause>);
				for (auto entity : group5)
				{
					auto [transform, Icon] = group5.get<RectTransformComponent, IconComponent>(entity);
					Icon.SetMaterial();
					Renderer::DrawRect(transform.localToWorld, Icon.material, entity);
				}
			}

			auto post = m_Registry.view<Bloom>();
			for (auto e : post)
			{
				Entity entity = { e, this };
				auto& bloom = entity.GetComponent<Bloom>();

				Renderer::ApplyBloom(bloom.Threshold, bloom.Intensity);
				break;
			}

			if (post.size() == 0)
			{
				Renderer::ACESMap();
			}
			Renderer::End();
		}

	}

	void Scene::OnUpdateSimulation(float timeStep, EditorCamera& camera)
	{
		ParentManager::CreateHierarchy(this);
		if (!m_IsPaused || m_StepFrames-- > 0)
		{
			OnPhysicsUpdate(timeStep);
		}
		
		RenderScene(camera);
	}

	void Scene::OnUpdateEditor(float timeStep, EditorCamera& camera)
	{
		ParentManager::CreateHierarchy(this);

		UpdateTransform();

		RenderScene(camera);
	}

	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		if (m_ViewportWidth == width && m_ViewportHeight == height)
			return;

		m_ViewportWidth = width;
		m_ViewportHeight = height;

		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			auto& cameraComponent = view.get<CameraComponent>(entity);
			if (!cameraComponent.FixedAspectRatio)
				cameraComponent.Camera.SetViewportSize(width, height);
		}
	}

	vector<Entity> Scene::GetDontDestroys()
	{
		vector<Entity> dontDestroys;

		auto view = m_Registry.view<DontDestroy>();
		for (auto entity : view)
		{
			dontDestroys.emplace_back(Entity{ entity, this });
		}
		
		return dontDestroys;
	}

	void Scene::UpdateTransform()
	{
		auto view = m_Registry.view<TransformComponent>();
		for (auto e : view)
		{
			Entity entity = { e, this };
			auto& transform = entity.GetComponent<TransformComponent>();
			transform.CreateToParent();
			if (!entity.HasComponent<Parent>())
				transform.localToWorld = transform.localToParent;
		}

		ParentManager::CreateToWorld(this);
	}

	Entity Scene::GetPrimaryCameraEntity()
	{
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			const auto& camera = view.get<CameraComponent>(entity);
			if (camera.Primary)
				return Entity{ entity, this };
		}
		return {};
	}

	void Scene::Step(int frames)
	{
		m_StepFrames = frames;
	}

	Entity Scene::DuplicateEntity(Entity entity)
	{
		std::string name = entity.GetName();
		Entity newEntity = CreateEntity(name);
		CopyComponentIfExists(AllComponents{}, newEntity, entity);
		return newEntity;
	}

	Entity Scene::LoadDontDestroy(Entity entity)
	{
		assert(entity.GetContext() != this);

		std::string name = entity.GetName();
		Entity newEntity = CreateEntityWithUUID(entity.GetUUID(), name);
		CopyComponentIfExists(AllComponents{}, newEntity, entity);
		return newEntity;
	}

	Entity Scene::FindEntityByName(std::string_view name)
	{
		auto view = m_Registry.view<TagComponent>();
		for (auto entity : view)
		{
			const TagComponent& tc = view.get<TagComponent>(entity);
			if (tc.Tag == name)
				return Entity(entity, this);
		}
		return {};
	}

	Entity Scene::GetEntityByUUID(UUID uuid)
	{
		if (m_EntityMap.find(uuid) != m_EntityMap.end())
			return { m_EntityMap.at(uuid), this };

		return {};
	}

	void Scene::OnPhysics2DStart()
	{
		m_PhysicsWorld = new b2World({ 0.0f, -700.f });

		m_PhysicsWorld->SetContactListener(new CollisionListener());

		m_PhysicsWorld->SetAllowSleeping(false);
		auto view = m_Registry.view<Rigidbody2DComponent>();
		for (auto e : view)
		{
			Entity entity = { e, this };
			AddBody(entity);
		}

		auto revjointView = m_Registry.view<RevoluteJointComponent>();

		for (auto e : revjointView)
		{
			Entity entity = { e, this };

			auto& joint = entity.GetComponent<RevoluteJointComponent>();

			Entity opponent = GetEntityByUUID(joint.Opponent);

			b2Body* rb = (b2Body*)entity.GetComponent<Rigidbody2DComponent>().RuntimeBody;
			b2Body* opponentRb = (b2Body*)opponent.GetComponent<Rigidbody2DComponent>().RuntimeBody;
		
			b2RevoluteJointDef jointDef = {};

			jointDef.bodyA = rb;
			jointDef.bodyB = opponentRb;
			jointDef.collideConnected = false;
			jointDef.enableLimit = joint.EnableLimit;
			jointDef.localAnchorA = { joint.LocalAnchor.x, joint.LocalAnchor.y };
			jointDef.localAnchorB = { joint.OpponentLocalAnchor.x, joint.OpponentLocalAnchor.y };
			jointDef.lowerAngle = joint.AngleRange.x /180.f * XM_PI;
			jointDef.upperAngle = joint.AngleRange.y / 180.f * XM_PI;

			jointDef.referenceAngle = 0;

			m_PhysicsWorld->CreateJoint(&jointDef);
		}

		auto disjointView = m_Registry.view<DistanceJointComponent>();

		for (auto e : disjointView)
		{
			Entity entity = { e, this };

			auto& joint = entity.GetComponent<DistanceJointComponent>();

			Entity opponent = GetEntityByUUID(joint.Opponent);

			b2Body* rb = (b2Body*)entity.GetComponent<Rigidbody2DComponent>().RuntimeBody;
			b2Body* opponentRb = (b2Body*)opponent.GetComponent<Rigidbody2DComponent>().RuntimeBody;

			b2DistanceJointDef jointDef = {};

			jointDef.bodyA = rb;
			jointDef.bodyB = opponentRb;
			jointDef.collideConnected = false;
			jointDef.localAnchorA = { joint.LocalAnchor.x, joint.LocalAnchor.y };
			jointDef.localAnchorB = { joint.OpponentLocalAnchor.x, joint.OpponentLocalAnchor.y };
			jointDef.minLength = joint.minLength;
			jointDef.maxLength = joint.maxLength;

			jointDef.stiffness = joint.stiffness;
			jointDef.damping = joint.damping;

			joint.b2Joint = (void*)m_PhysicsWorld->CreateJoint(&jointDef);
		}
	}

	void Scene::OnPhysics2DStop()
	{
		delete m_PhysicsWorld;
		m_PhysicsWorld = nullptr;
	}

	void Scene::AddBody(Entity entity)
	{
		auto& transform = entity.GetComponent<TransformComponent>();
		auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

		b2BodyDef bodyDef;
		bodyDef.linearDamping = rb2d.LinearDamping;
		bodyDef.angularDamping = rb2d.AngularDamping;
		bodyDef.type = Utils::Rigidbody2DTypeToBox2DBody(rb2d.Type);
		bodyDef.position.Set(transform.translation.x, transform.translation.y);
		bodyDef.angle = transform.rotation.z;
		bodyDef.gravityScale = !rb2d.DisableGravity;
		b2Body* body = m_PhysicsWorld->CreateBody(&bodyDef);

		if (entity.HasComponent<Pause>())
			body->SetEnabled(false);

		body->SetFixedRotation(rb2d.FixedRotation);
		rb2d.RuntimeBody = body;
		if (entity.HasComponent<BoxCollider2DComponent>())
		{
			auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();

			b2PolygonShape boxShape;
			b2Vec2 center;
			center.x = bc2d.Offset.x;
			center.y = bc2d.Offset.y;
			boxShape.SetAsBox(bc2d.Size.x, bc2d.Size.y, center, 0);

			b2FixtureDef fixtureDef;
			fixtureDef.shape = &boxShape;
			fixtureDef.density = bc2d.Density;
			fixtureDef.friction = bc2d.Friction;
			fixtureDef.restitution = bc2d.Restitution;
			fixtureDef.restitutionThreshold = bc2d.RestitutionThreshold;
			fixtureDef.isSensor = bc2d.isSensor;
			fixtureDef.filter.categoryBits = bc2d.categoryBits;
			fixtureDef.filter.maskBits = bc2d.maskBits;

			auto userData = b2FixtureUserData();
			userData.pointer = (uintptr_t)(UINT)entity;
			fixtureDef.userData = userData;

			body->CreateFixture(&fixtureDef);
		}

		if (entity.HasComponent<CircleCollider2DComponent>())
		{
			auto& cc2d = entity.GetComponent<CircleCollider2DComponent>();

			b2CircleShape circleShape;
			circleShape.m_p.Set(cc2d.Offset.x, cc2d.Offset.y);
			circleShape.m_radius = cc2d.Radius;

			b2FixtureDef fixtureDef;
			fixtureDef.shape = &circleShape;
			fixtureDef.density = cc2d.Density;
			fixtureDef.friction = cc2d.Friction;
			fixtureDef.restitution = cc2d.Restitution;
			fixtureDef.restitutionThreshold = cc2d.RestitutionThreshold;
			fixtureDef.isSensor = cc2d.isSensor;
			fixtureDef.filter.categoryBits = cc2d.categoryBits;
			fixtureDef.filter.maskBits = cc2d.maskBits;

			auto userData = b2FixtureUserData();
			userData.pointer = (uintptr_t)(UINT)entity;
			fixtureDef.userData = userData;

			body->CreateFixture(&fixtureDef);
		}
	}

	void Scene::OnPhysicsUpdate(float timeStep)
	{
		const int32_t velocityIterations = 12;
		const int32_t positionIterations = 4;

		m_PhysicsWorld->Step(timeStep, velocityIterations, positionIterations);

		auto view = m_Registry.view<Rigidbody2DComponent>();
		for (auto e : view)
		{
			Entity entity = { e, this };
			auto& transform = entity.GetComponent<TransformComponent>();
			auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

			b2Body* body = (b2Body*)rb2d.RuntimeBody;

			const auto& position = body->GetPosition();
			Vector2 transformPos = { position.x,position.y };
			float angle = body->GetAngle();

			transform.translation.x = transformPos.x;
			transform.translation.y = transformPos.y;
			transform.rotation.z = angle;
		}
		b2Contact* contact = m_PhysicsWorld->GetContactManager().m_contactList;

		while(contact){

			if (contact->IsEnabled() && contact->IsTouching())
			{
				b2Fixture* f1 = contact->GetFixtureA();
				b2Fixture* f2 = contact->GetFixtureB();

				uintptr_t p1 = f1->GetUserData().pointer;
				uintptr_t p2 = f2->GetUserData().pointer;

				SY::Entity entity1 = { (entt::entity)p1, ScriptEngine::GetSceneContext() };
				SY::Entity entity2 = { (entt::entity)p2, ScriptEngine::GetSceneContext() };

				string tag1 = entity1.GetComponent<SY::TagComponent>().Tag;
				string tag2 = entity2.GetComponent<SY::TagComponent>().Tag;

				if (entity1.HasComponent<ScriptComponent>())
				{
					const b2Filter& filter = f2->GetFilterData();
					Collision col = {};
					col.entityID = entity2.GetUUID();
					col.CollisionLayer = filter.categoryBits;
					if (f1->IsSensor())
						ScriptEngine::OnTriggerStay(entity1, &col);
					else
						ScriptEngine::OnCollisionStay(entity1, &col);
				}
				if (entity2.HasComponent<ScriptComponent>())
				{
					const b2Filter& filter = f1->GetFilterData();
					Collision col = {};
					col.entityID = entity1.GetUUID();
					col.CollisionLayer = filter.categoryBits;
					if (f1->IsSensor())
						ScriptEngine::OnTriggerStay(entity2, &col);
					else
						ScriptEngine::OnCollisionStay(entity2, &col);
				}
			}
			contact = contact->GetNext();
		}

		UpdateTransform();
	}

	void Scene::RenderScene(EditorCamera& camera)
	{

#pragma region LightEdit
		{
			auto view = m_Registry.view<TransformComponent, Light>();
			int lightnum = 0;
			LightCB cb;
			for (auto entity : view)
			{
				auto [transform, light] = view.get<TransformComponent, Light>(entity);
				light.info.position = transform.localToWorld.Translation();
				cb.lights[lightnum++] = light.info;
			}
			cb.lightCount = lightnum;
			auto LightBuffer = GEngine->GetConstantBuffer(Constantbuffer_Type::LIGHT);

			LightBuffer->SetData(&cb);
			LightBuffer->SetPipline(ShaderStage::PS);
		}
#pragma endregion
		Renderer::Begin(camera);
		{
			auto group = m_Registry.group<SpriteRendererComponent>(entt::get<TransformComponent>, entt::exclude<Pause>);
			vector<Entity> defferedBatches;
			vector<Entity> fowardBatches;
			for (auto entity : group)
			{
				auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
				sprite.SetMaterial();
				if (sprite.Deffered)
					defferedBatches.push_back({ entity,this });
				else
					fowardBatches.push_back({ entity,this });
			}

			GEngine->BindRenderTargetGroup(RENDER_TARGET_GROUP_TYPE::DEFFERED);
			GET_SINGLE(InstancingManager)->Render(defferedBatches);
			GET_SINGLE(InstancingManager)->ClearBuffer();
			GEngine->BindRenderTargetGroup(RENDER_TARGET_GROUP_TYPE::HDR);
			Renderer::DrawDeffered();
			GET_SINGLE(InstancingManager)->Render(fowardBatches);
			GET_SINGLE(InstancingManager)->ClearBuffer();

			auto circles = m_Registry.group<CircleRendererComponent>(entt::get<TransformComponent>, entt::exclude<Pause>);

			for (auto entity : circles)
			{
				auto [transform, circle] = circles.get<TransformComponent, CircleRendererComponent>(entity);
				Renderer::DrawCircle(transform.localToWorld, circle.Color, entity);
			}

			CONTEXT->OMSetRenderTargets(0, nullptr, nullptr);

			auto post = m_Registry.view<Bloom>();
			for (auto e : post)
			{
				Entity entity = { e, this };
				auto& bloom = entity.GetComponent<Bloom>();

				Renderer::ApplyBloom(bloom.Intensity, bloom.Threshold);
				break;
			}

			if (post.size() == 0)
			{
				Renderer::ACESMap();
			}
		}

		Renderer::End();
	}

	template<typename T>
	void Scene::OnComponentAdded(Entity entity, T& component)
	{
		static_assert(sizeof(T) != 0);
	}

	template<>
	void Scene::OnComponentAdded<IDComponent>(Entity entity, IDComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<SpriteAnimatorComponent>(Entity entity, SpriteAnimatorComponent& component)
	{
		if (!entity.HasComponent<SpriteRendererComponent>())
			entity.AddComponent<SpriteRendererComponent>();
	}

	template<>
	void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<Rigidbody2DComponent>(Entity entity, Rigidbody2DComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<BoxCollider2DComponent>(Entity entity, BoxCollider2DComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<CircleCollider2DComponent>(Entity entity, CircleCollider2DComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<Parent>(Entity entity, Parent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<StateComponent>(Entity entity, StateComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<Pause>(Entity entity, Pause& component)
	{
		ScriptEngine::OnPaused(entity);
	}

	template<>
	void Scene::OnComponentAdded<Dead>(Entity entity, Dead& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<RectTransformComponent>(Entity entity, RectTransformComponent& component)
	{
		
	}

	template<>
	void Scene::OnComponentAdded<DontDestroy>(Entity entity, DontDestroy& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<Light>(Entity entity, Light& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<RevoluteJointComponent>(Entity entity, RevoluteJointComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<DistanceJointComponent>(Entity entity, DistanceJointComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<Eraser>(Entity entity, Eraser& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<ParticleSystem>(Entity entity, ParticleSystem& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<Bloom>(Entity entity, Bloom& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<CircleRendererComponent>(Entity entity, CircleRendererComponent& component)
	{

	}
}