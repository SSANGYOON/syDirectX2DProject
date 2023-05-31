#include "pch.h"
#include "ScriptGlue.h"
#include "ScriptEngine.h"

#include "UUID.h"

#include "Scene.h"
#include "Entity.h"

#include "Physics2D.h"

#include "mono/metadata/object.h"
#include "mono/metadata/reflection.h"

#include "box2d/b2_world.h"
#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_polygon_shape.h"
#include "box2d/b2_circle_shape.h"
#include "box2d/b2_contact.h"
#include "box2d/b2_distance_joint.h"
#include "CollisionManager.h"
#include "PrefabManager.h"
#include "ParentManager.h"
#include "SceneManager.h"
#include <queue>

namespace SY {

	static std::unordered_map<MonoType*, std::function<bool(Entity)>> s_EntityHasComponentFuncs;

#define HZ_ADD_INTERNAL_CALL(Name) mono_add_internal_call("SY.InternalCalls::" #Name, Name)

	static void NativeLog(MonoString* string, int parameter)
	{
		char* cStr = mono_string_to_utf8(string);
		std::string str(cStr);
		mono_free(cStr);
		std::cout << str << ", " << parameter << std::endl;
	}

	static void NativeLog_Vector(Vector3* parameter, Vector3* outResult)
	{
		//HZ_CORE_WARN("Value: {0}", *parameter);
		parameter->Normalize();
		*outResult = *parameter;
	}

	static float NativeLog_VectorDot(Vector3* parameter)
	{
		//HZ_CORE_WARN("Value: {0}", *parameter);
		return parameter->LengthSquared();
	}

	static MonoObject* GetScriptInstance(UUID entityID)
	{
		return ScriptEngine::GetManagedInstance(entityID);
	}

	static bool Entity_IsValid(UUID entityID)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		assert(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		bool ret = entity.GetContext() != nullptr;
		return ret;
	}

	static bool Entity_HasComponent(UUID entityID, MonoReflectionType* componentType)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		assert(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		assert(entity);

		MonoType* managedType = mono_reflection_type_get_type(componentType);
		assert(s_EntityHasComponentFuncs.find(managedType) != s_EntityHasComponentFuncs.end());
		return s_EntityHasComponentFuncs.at(managedType)(entity);
	}

	static uint64_t Entity_FindEntityByName(MonoString* name)
	{
		char* nameCStr = mono_string_to_utf8(name);

		Scene* scene = ScriptEngine::GetSceneContext();
		assert(scene);
		Entity entity = scene->FindEntityByName(nameCStr);
		mono_free(nameCStr);

		if (!entity)
			return 0;

		return entity.GetUUID();
	}

	static void TransformComponent_GetTranslation(UUID entityID, Vector3* outTranslation)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		assert(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		*outTranslation = entity.GetComponent<TransformComponent>().translation;
	}

	static void TransformComponent_SetTranslation(UUID entityID, Vector3* translation)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		assert(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		assert(entity);

		auto& transform = entity.GetComponent<TransformComponent>();
		transform.translation = *translation;

		if (entity.HasComponent<Rigidbody2DComponent>())
		{
			auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

			b2Body* body = (b2Body*)rb2d.RuntimeBody;

			b2Vec2 b2Trans = b2Vec2(translation->x, translation->y);
			body->SetTransform(b2Trans, transform.rotation.z);
		}

		else if(entity.HasComponent<BoxCollider2DComponent>() || entity.HasComponent<CircleCollider2DComponent>()){
			b2Vec2 offset = b2Vec2_zero;
			
			if (entity.HasComponent<BoxCollider2DComponent>()) {
				auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();
				offset = { bc2d.Offset.x, bc2d.Offset.y };

				if (bc2d.RuntimeFixture){
					auto fix = (b2Fixture*)bc2d.RuntimeFixture;
					auto body = fix->GetBody();
					body->DestroyFixture(fix);
				}
			}

			if (entity.HasComponent<CircleCollider2DComponent>()) {
				auto& cc2d = entity.GetComponent<CircleCollider2DComponent>();
				offset = { cc2d.Offset.x, cc2d.Offset.y };

				if (cc2d.RuntimeFixture) {
					auto fix = (b2Fixture*)cc2d.RuntimeFixture;
					auto body = fix->GetBody();
					body->DestroyFixture(fix);
				}
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
			entity.GetContext()->AddFixture(entity, offset, angle, body, root.GetComponent<Rigidbody2DComponent>().flip);
		
		}
	}

	static void TransformComponent_GetWorldPosition(UUID entityID, Vector3* translation)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		assert(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		*translation = entity.GetComponent<TransformComponent>().localToWorld.Translation();
	}

	static void TransformComponent_GetScale(UUID entityID, Vector3* outScale)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		assert(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		assert(entity);

		*outScale = entity.GetComponent<TransformComponent>().scale;
	}

	static void TransformComponent_SetScale(UUID entityID, Vector3* scale)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		assert(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		assert(entity);

		entity.GetComponent<TransformComponent>().scale = *scale;
	}

	static void TransformComponent_GetRotation(UUID entityID, Vector3* outRotation)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		assert(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		assert(entity);

		*outRotation = entity.GetComponent<TransformComponent>().rotation;
	}

	static void TransformComponent_SetRotation(UUID entityID, Vector3* rotation)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		assert(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		assert(entity);

		auto& transform = entity.GetComponent<TransformComponent>();

		transform.rotation = *rotation;

		if (entity.HasComponent<Rigidbody2DComponent>())
		{
			auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
			b2Body* body = (b2Body*)rb2d.RuntimeBody;
			body->SetTransform(b2Vec2(transform.translation.x,transform.translation.y), transform.rotation.z);
		}
	}

	static void Rigidbody2DComponent_ApplyLinearImpulse(UUID entityID, Vector2* impulse, Vector2* point, bool wake)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		assert(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		assert(entity);

		auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
		b2Body* body = (b2Body*)rb2d.RuntimeBody;
		body->ApplyLinearImpulse(b2Vec2(impulse->x, impulse->y), b2Vec2(point->x, point->y), wake);
	}

	static void Rigidbody2DComponent_Flip(UUID entityID)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		assert(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		assert(entity);

		auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
		b2Body* body = (b2Body*)rb2d.RuntimeBody;
		rb2d.flip = !rb2d.flip;

		b2Vec2 offset = b2Vec2_zero;
		float angle = 0.f;
		float flip = rb2d.flip ? -1.f : 1.f;
		if (entity.HasComponent<BoxCollider2DComponent>())
		{
			auto& bc = entity.GetComponent<BoxCollider2DComponent>();
			offset = { bc.parentCenter.x, bc.parentCenter.y };
			angle = bc.parentAngle;
		}

		if (entity.HasComponent<CircleCollider2DComponent>())
		{
			auto& cc = entity.GetComponent<CircleCollider2DComponent>();
			offset = { cc.parentCenter.x, cc.parentCenter.y };
			angle = cc.parentAngle;
		}

		scene->AddFixture(entity, offset, angle, body, flip);
	}

	static void Rigidbody2DComponent_ApplyLinearImpulseToCenter(UUID entityID, Vector2* impulse, bool wake)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		assert(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		assert(entity);

		auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
		b2Body* body = (b2Body*)rb2d.RuntimeBody;
		body->ApplyLinearImpulseToCenter(b2Vec2(impulse->x, impulse->y), wake);
	}

	static void Rigidbody2DComponent_GetLinearVelocity(UUID entityID, Vector2* outLinearVelocity)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		assert(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		assert(entity);

		auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
		b2Body* body = (b2Body*)rb2d.RuntimeBody;
		const b2Vec2& linearVelocity = body->GetLinearVelocity();
		*outLinearVelocity = Vector2(linearVelocity.x, linearVelocity.y);
	}

	static void Rigidbody2DComponent_SetLinearVelocity(UUID entityID, Vector2* linearVelocity, bool wake)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		assert(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		assert(entity);

		auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
		b2Body* body = (b2Body*)rb2d.RuntimeBody;
		body->SetLinearVelocity(*reinterpret_cast<b2Vec2*>(linearVelocity));
	}

	static Rigidbody2DComponent::BodyType Rigidbody2DComponent_GetType(UUID entityID)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		assert(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		assert(entity);

		auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
		b2Body* body = (b2Body*)rb2d.RuntimeBody;
		return Utils::Rigidbody2DTypeFromBox2DBody(body->GetType());
	}

	static void Rigidbody2DComponent_SetType(UUID entityID, Rigidbody2DComponent::BodyType bodyType)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		assert(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		assert(entity);

		auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
		b2Body* body = (b2Body*)rb2d.RuntimeBody;
		body->SetType(Utils::Rigidbody2DTypeToBox2DBody(bodyType));
	}

	static void BoxColliderComponent_GetOffset(UUID entityID, Vector2* offset)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		assert(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		assert(entity);

		auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();
		*offset = bc2d.Offset;
	}

	static void BoxColliderComponent_SetOffset(UUID entityID, Vector2* offset)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		assert(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		assert(entity);

		auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();
		bc2d.Offset = *offset;
	}

	static void BoxColliderComponent_GetSize(UUID entityID, Vector2* size)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		assert(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		assert(entity);

		auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();
		*size = bc2d.Size;
	}

	static void BoxColliderComponent_SetSize(UUID entityID, Vector2* size)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		assert(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		assert(entity);

		auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();
		auto& tr = entity.GetComponent<TransformComponent>();

		bc2d.Size = *size;

		b2Body* body = ((b2Fixture*)bc2d.RuntimeFixture)->GetBody();
		body->DestroyFixture(body->GetFixtureList());

		b2PolygonShape boxShape;
		boxShape.SetAsBox(bc2d.Size.x * abs(tr.scale.x), bc2d.Size.y * tr.scale.y);

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

		Vector2 bodyPos = Vector2(tr.translation.x, tr.translation.y) + Vector2::Transform(bc2d.Offset, Matrix::CreateRotationZ(tr.rotation.z));
		body->SetTransform({ bodyPos.x,bodyPos.y }, tr.rotation.z);
		body->CreateFixture(&fixtureDef);
	}

	static float DistanceJointComponent_GetCurrentLength(UUID entityID)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		Entity entity = scene->GetEntityByUUID(entityID);
		assert(entity);

		auto& dj = entity.GetComponent<DistanceJointComponent>();
		return dj.GetCurrentLength();
	}

	static float DistanceJointComponent_GetMinLength(UUID entityID)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		Entity entity = scene->GetEntityByUUID(entityID);
		assert(entity);

		auto& dj = entity.GetComponent<DistanceJointComponent>();
		return dj.GetMinLength();
	}

	static void DistanceJointComponent_SetMinLength(UUID entityID, float length)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		Entity entity = scene->GetEntityByUUID(entityID);
		assert(entity);

		auto& dj = entity.GetComponent<DistanceJointComponent>();
		dj.SetMinLength(length);
	}

	static bool SpriteAnimatorComponent_Play(UUID entityID, MonoString* str, float at = 0.f)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		char* cStr = mono_string_to_utf8(str);
		mono_free(cStr);
		std::string clipKey(cStr);
		Entity entity = scene->GetEntityByUUID(entityID);
		auto & animator= entity.GetComponent<SpriteAnimatorComponent>();

		if (animator.clips.find(clipKey) == animator.clips.end())
			return false;
		else {
			animator._currentClip = animator.clips.find(clipKey)->second;
			animator._currentTime = at;
			return true;
		}
	}

	static bool TransformAnimatorComponent_Play(UUID entityID, MonoString* str, float at = 0.f)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		char* cStr = mono_string_to_utf8(str);
		mono_free(cStr);
		std::string clipKey(cStr);
		Entity entity = scene->GetEntityByUUID(entityID);
		auto& animator = entity.GetComponent<TransformAnimatorComponent>();

		if (animator.clips.find(clipKey) == animator.clips.end())
			return false;
		else {
			animator._currentClip = animator.clips.find(clipKey)->second;
			animator._currentTime = at;
			return true;
		}
	}

	static ParticleState ParticleSystem_GetState(UUID entityID)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		Entity entity = scene->GetEntityByUUID(entityID);
		auto& particle = entity.GetComponent<ParticleSystem>();

		return particle.state;
	}

	static void ParticleSystem_SetState(UUID entityID, UINT state)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		Entity entity = scene->GetEntityByUUID(entityID);
		auto& particle = entity.GetComponent<ParticleSystem>();

		particle.state = (ParticleState)state;
	}

	static void ParticleSystem_SetGeneratePos(UUID entityID, Vector2* genPos)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		Entity entity = scene->GetEntityByUUID(entityID);
		auto& particle = entity.GetComponent<ParticleSystem>();

		particle.Position = *genPos;
	}

	static void ParticleSystem_SetGeneratePosVariation(UUID entityID, Vector2* variation)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		Entity entity = scene->GetEntityByUUID(entityID);
		auto& particle = entity.GetComponent<ParticleSystem>();

		particle.PositionVariation = *variation;
	}

	static KEY_STATE Input_GetKeyState(KEY_TYPE keycode)
	{
		return INPUT->GetKeyState(keycode);
	}

	static void CameraComponent_GetOrthographicSize(UUID entityID, Vector2* OrthographicSize)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		assert(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		assert(entity);

		assert(entity.HasComponent<CameraComponent>());

		CameraComponent& cam = entity.GetComponent<CameraComponent>();
		float size = cam.Camera.GetOrthographicSize();
		float aspectRatio = cam.Camera.GetAspectRatio();
		Vector2 camSize(aspectRatio * size, size);
		*OrthographicSize = camSize;
	}

	static void CameraComponent_SetOrthographicSize(UUID entityID, Vector3* targetSize)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		assert(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		assert(entity);

		assert(entity.HasComponent<CameraComponent>());

		CameraComponent& cam = entity.GetComponent<CameraComponent>();

		cam.Camera.SetAspectRatio(targetSize->x / targetSize->y);
		cam.Camera.SetOrthographicSize(targetSize->y);
	}

	static void CameraComponent_AddOscilation(UUID entityID, float amp)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		assert(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		assert(entity);

		assert(entity.HasComponent<CameraComponent>());

		CameraComponent& cam = entity.GetComponent<CameraComponent>();

		cam.oscillationAmp = min(10.f, cam.oscillationAmp + amp);
	}

	static EntityState StateComponent_GetState(UUID entityID)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		assert(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		assert(entity);

		return entity.GetComponent<StateComponent>().state;

	}

	static void StateComponent_SetState(UUID entityID, EntityState state)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		assert(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		assert(entity);

		auto& Sc = entity.GetComponent<StateComponent>();
		Sc.state = state;
	}

	static void PanelComponent_GetTintColor(UUID entityID, Vector4* color)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		assert(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		assert(entity);

		auto& panel = entity.GetComponent<PanelComponent>();
		*color = panel.color;
	}

	static void PanelComponent_SetTintColor(UUID entityID, Vector4* color)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		assert(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		assert(entity);

		auto& panel = entity.GetComponent<PanelComponent>();
		panel.color = *color;
	}

	static void IconComponent_GetTintColor(UUID entityID, Vector4* color)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		assert(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		assert(entity);

		auto& icon = entity.GetComponent<IconComponent>();
		*color = icon.tint;
	}

	static void IconComponent_SetTintColor(UUID entityID, Vector4* color)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		assert(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		assert(entity);

		auto& icon = entity.GetComponent<IconComponent>();
		icon.tint = *color;
	}

	static void IconComponent_SetTexture(UUID entityID, MonoString* texturePath)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		char* cPath = mono_string_to_utf8(texturePath);
		mono_free(cPath);
		std::string path(cPath);
		Entity entity = scene->GetEntityByUUID(entityID);
		assert(entity);
		auto& icon = entity.GetComponent<IconComponent>();

		icon.icon = GET_SINGLE(Resources)->Load<Texture>(stow(path), stow(path));
	}

	static void SlotComponent_SetItem(UUID entityID, MonoString* itemPath)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		char* cPath = mono_string_to_utf8(itemPath);
		mono_free(cPath);
		std::string path(cPath);
		Entity entity = scene->GetEntityByUUID(entityID);
		assert(entity);
		auto& slot = entity.GetComponent<SlotComponent>();

		slot.item = GET_SINGLE(Resources)->Load<Texture>(stow(path), stow(path));
	}

	static void SpriteRendererComponent_SetTexture(UUID entityID, MonoString* texturePath)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		char* cPath = mono_string_to_utf8(texturePath);
		mono_free(cPath);
		std::string path(cPath);
		Entity entity = scene->GetEntityByUUID(entityID);
		assert(entity);
		auto& sp = entity.GetComponent<SpriteRendererComponent>();

		sp.Diffuse = GET_SINGLE(Resources)->Load<Texture>(stow(path), stow(path), false);
		sp.tile = sp.Diffuse->GetSize();
	}

	static void SpriteRendererComponent_SetColor(UUID entityID, Vector4* color)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		Entity entity = scene->GetEntityByUUID(entityID);
		assert(entity);
		auto& sp = entity.GetComponent<SpriteRendererComponent>();

		sp.Color = *color;
	}

	static void SpriteRendererComponent_SetEmission(UUID entityID, Vector4* emission)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		Entity entity = scene->GetEntityByUUID(entityID);
		assert(entity);
		auto& sp = entity.GetComponent<SpriteRendererComponent>();

		sp.Emission = *emission;
	}

	static void SpriteRendererComponent_GetColor(UUID entityID, Vector4* color)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		Entity entity = scene->GetEntityByUUID(entityID);
		assert(entity);

		auto& sp = entity.GetComponent<SpriteRendererComponent>();
		*color = sp.Color;
	}

	static void SpriteRendererComponent_GetEmission(UUID entityID, Vector4* emission)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		Entity entity = scene->GetEntityByUUID(entityID);
		assert(entity);

		auto& sp = entity.GetComponent<SpriteRendererComponent>();
		*emission = sp.Emission;
	}


	static float Timer_GetTimeScale()
	{
		return TIME->GetTimeScale();
	}

	static void Timer_SetTimeScale(float timeScale)
	{
		return TIME->SetTimeScale(timeScale);
	}


	
	static void Entity_Instantiate(uint64_t id, Vector3* position, uint64_t parentID, uint64_t* instanceId)
	{
		Entity instance = PrefabManager::Instantiate(ScriptEngine::GetSceneContext(), id, parentID);
		
		

		
		
		if(instance) {
			queue<UINT> initQueue;
			initQueue.push(instance);

			while (!initQueue.empty()) {
				Entity ent = { (entt::entity)initQueue.front(), ScriptEngine::GetSceneContext() };
				initQueue.pop();

				if (ent.HasComponent<ScriptComponent>())
					ScriptEngine::OnInstantiateEntity(ent, id);

				if (ent.HasComponent<Rigidbody2DComponent>())
					ScriptEngine::GetSceneContext()->AddBody(ent);

				if (ent.HasComponent<ParticleSystem>())
				{
					auto& p = ent.GetComponent<ParticleSystem>();
					p.Init();
				}

				for (UINT e : ParentManager::GetChildren(ent))
					initQueue.push(e);
			}

			if (instance.HasComponent<TransformComponent>())
			{
				auto& tr = instance.GetComponent<TransformComponent>();
				tr.translation = *position;
			}
			else
			{
				auto& tr = instance.GetComponent<RectTransformComponent>();
				tr.translation = *position;
			}

			*instanceId = instance.GetUUID();
		}
		else
			*instanceId = 0;
	}

	static void Entity_GetChild(uint64_t id, MonoString* tag, uint64_t* instanceId)
	{
		char* cTag = mono_string_to_utf8(tag);
		mono_free(cTag);
		std::string stag(cTag);
		auto child = ParentManager::GetChild(id, stag, ScriptEngine::GetSceneContext());
		
		if (child)
			*instanceId = child;
		else
			*instanceId = 0;
	}

	static void Entity_GetParent(uint64_t id, uint64_t* instanceId)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		assert(scene);
		Entity entity = scene->GetEntityByUUID(id);
		assert(entity);

		auto& parent = entity.GetComponent<Parent>();

		*instanceId = parent.parentHandle;
	}

	static void Entity_Destroy(uint64_t id)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		assert(scene);
		Entity entity = scene->GetEntityByUUID(id);
		assert(entity);

		auto& state = entity.GetComponent<StateComponent>();
		state.state = EntityState::Dead;
	}

	static void Entity_DontDestroy(uint64_t id, Vector3* desiredPos)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		assert(scene);
		Entity entity = scene->GetEntityByUUID(id);
		assert(entity);

		auto& dontDestroy = entity.AddOrReplaceComponent<DontDestroy>();
		if (desiredPos == nullptr)
		{
			if (entity.HasComponent<TransformComponent>())
				dontDestroy.desiredPos = entity.GetComponent< TransformComponent>().translation;
			else
			{
				assert(entity.HasComponent<RectTransformComponent>());
				dontDestroy.desiredPos = entity.GetComponent<RectTransformComponent>().translation;
			}
			
		}
		else
		{
			dontDestroy.desiredPos = *desiredPos;
		}
	}

	static void SceneManager_LoadScene(MonoString* monoPath)
	{
		char* cPath = mono_string_to_utf8(monoPath);
		mono_free(cPath);
		std::string sPath(cPath);
		SceneManager::LoadScene(sPath);
	}

	static void SceneManager_LoadSceneAsync(MonoString* monoPath)
	{
		char* cPath = mono_string_to_utf8(monoPath);
		mono_free(cPath);
		std::string sPath(cPath);
		SceneManager::LoadScene(sPath);
	}

	template<typename... Component>
	static void RegisterComponent()
	{
		([]()
			{
				std::string_view typeName = typeid(Component).name();
				size_t pos = typeName.find_last_of(':');
				std::string_view structName = typeName.substr(pos + 1);
				std::string managedTypename = std::format("SY.{}", structName);

				MonoType* managedType = mono_reflection_type_from_name(managedTypename.data(), ScriptEngine::GetCoreAssemblyImage());
				if (!managedType)
				{
					//HZ_CORE_ERROR("Could not find component type {}", managedTypename);
					return;
				}
				s_EntityHasComponentFuncs[managedType] = [](Entity entity) { return entity.HasComponent<Component>(); };
			}(), ...);
	}

	template<typename... Component>
	static void RegisterComponent(ComponentGroup<Component...>)
	{
		RegisterComponent<Component...>();
	}

	void ScriptGlue::RegisterComponents()
	{
		s_EntityHasComponentFuncs.clear();
		RegisterComponent(AllComponents{});
	}

	void ScriptGlue::RegisterFunctions()
	{
		HZ_ADD_INTERNAL_CALL(NativeLog);
		HZ_ADD_INTERNAL_CALL(NativeLog_Vector);
		HZ_ADD_INTERNAL_CALL(NativeLog_VectorDot);

		HZ_ADD_INTERNAL_CALL(GetScriptInstance);

		HZ_ADD_INTERNAL_CALL(Entity_IsValid);
		HZ_ADD_INTERNAL_CALL(Entity_HasComponent);
		HZ_ADD_INTERNAL_CALL(Entity_FindEntityByName);
		HZ_ADD_INTERNAL_CALL(Entity_GetChild);
		HZ_ADD_INTERNAL_CALL(Entity_GetParent);

		HZ_ADD_INTERNAL_CALL(TransformComponent_GetTranslation);
		HZ_ADD_INTERNAL_CALL(TransformComponent_SetTranslation);

		HZ_ADD_INTERNAL_CALL(TransformComponent_GetRotation);
		HZ_ADD_INTERNAL_CALL(TransformComponent_SetRotation);

		HZ_ADD_INTERNAL_CALL(TransformComponent_GetWorldPosition);

		HZ_ADD_INTERNAL_CALL(TransformComponent_GetScale);
		HZ_ADD_INTERNAL_CALL(TransformComponent_SetScale);

		HZ_ADD_INTERNAL_CALL(Rigidbody2DComponent_ApplyLinearImpulse);
		HZ_ADD_INTERNAL_CALL(Rigidbody2DComponent_ApplyLinearImpulseToCenter);
		HZ_ADD_INTERNAL_CALL(Rigidbody2DComponent_GetLinearVelocity);
		HZ_ADD_INTERNAL_CALL(Rigidbody2DComponent_SetLinearVelocity);
		HZ_ADD_INTERNAL_CALL(Rigidbody2DComponent_GetType);
		HZ_ADD_INTERNAL_CALL(Rigidbody2DComponent_SetType);
		HZ_ADD_INTERNAL_CALL(Rigidbody2DComponent_Flip);
		HZ_ADD_INTERNAL_CALL(BoxColliderComponent_GetOffset);
		HZ_ADD_INTERNAL_CALL(BoxColliderComponent_SetOffset);
		HZ_ADD_INTERNAL_CALL(BoxColliderComponent_GetSize);
		HZ_ADD_INTERNAL_CALL(BoxColliderComponent_SetSize);

		HZ_ADD_INTERNAL_CALL(DistanceJointComponent_GetCurrentLength);
		HZ_ADD_INTERNAL_CALL(DistanceJointComponent_GetMinLength);
		HZ_ADD_INTERNAL_CALL(DistanceJointComponent_SetMinLength);

		HZ_ADD_INTERNAL_CALL(SpriteAnimatorComponent_Play);
		HZ_ADD_INTERNAL_CALL(TransformAnimatorComponent_Play);

		HZ_ADD_INTERNAL_CALL(Input_GetKeyState);

		HZ_ADD_INTERNAL_CALL(ParticleSystem_SetState);
		HZ_ADD_INTERNAL_CALL(ParticleSystem_GetState);

		HZ_ADD_INTERNAL_CALL(ParticleSystem_SetGeneratePos);
		HZ_ADD_INTERNAL_CALL(ParticleSystem_SetGeneratePosVariation);

		HZ_ADD_INTERNAL_CALL(CameraComponent_GetOrthographicSize);
		HZ_ADD_INTERNAL_CALL(CameraComponent_SetOrthographicSize);
		HZ_ADD_INTERNAL_CALL(CameraComponent_AddOscilation);
		HZ_ADD_INTERNAL_CALL(StateComponent_GetState);
		HZ_ADD_INTERNAL_CALL(StateComponent_SetState);

		HZ_ADD_INTERNAL_CALL(Timer_GetTimeScale);
		HZ_ADD_INTERNAL_CALL(Timer_SetTimeScale);

		HZ_ADD_INTERNAL_CALL(Entity_Instantiate);
		HZ_ADD_INTERNAL_CALL(Entity_Destroy);
		HZ_ADD_INTERNAL_CALL(Entity_DontDestroy);

		HZ_ADD_INTERNAL_CALL(PanelComponent_GetTintColor);
		HZ_ADD_INTERNAL_CALL(PanelComponent_SetTintColor);

		HZ_ADD_INTERNAL_CALL(IconComponent_GetTintColor);
		HZ_ADD_INTERNAL_CALL(IconComponent_SetTintColor);

		HZ_ADD_INTERNAL_CALL(IconComponent_SetTexture);
		HZ_ADD_INTERNAL_CALL(SlotComponent_SetItem);

		HZ_ADD_INTERNAL_CALL(SpriteRendererComponent_SetTexture);
		HZ_ADD_INTERNAL_CALL(SpriteRendererComponent_SetColor);
		HZ_ADD_INTERNAL_CALL(SpriteRendererComponent_GetColor);
		HZ_ADD_INTERNAL_CALL(SpriteRendererComponent_SetEmission);
		HZ_ADD_INTERNAL_CALL(SpriteRendererComponent_GetEmission);

		HZ_ADD_INTERNAL_CALL(SceneManager_LoadScene);
		HZ_ADD_INTERNAL_CALL(SceneManager_LoadSceneAsync);
	}

}