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
#include "CollisionManager.h"

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
		assert(entity);

		*outTranslation = entity.GetComponent<TransformComponent>().translation;
	}

	static void TransformComponent_SetTranslation(UUID entityID, Vector3* translation)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		assert(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		assert(entity);

		entity.GetComponent<TransformComponent>().translation = *translation;
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
		auto& rb = entity.GetComponent<Rigidbody2DComponent>();
		auto& tr = entity.GetComponent<TransformComponent>();

		bc2d.Size = *size;

		b2Body* body = (b2Body*)rb.RuntimeBody;
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

	static bool SpriteAnimatorComponent_Play(UUID entityID, MonoString* str)
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
			animator._currentTime = 0.f;
			return true;
		}
	}

	static bool Input_IsKeyDown(KEY_TYPE keycode)
	{
		bool result = INPUT->GetKeyState(keycode) == KEY_STATE::PRESS;
		return result;
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

		HZ_ADD_INTERNAL_CALL(Entity_HasComponent);
		HZ_ADD_INTERNAL_CALL(Entity_FindEntityByName);

		HZ_ADD_INTERNAL_CALL(TransformComponent_GetTranslation);
		HZ_ADD_INTERNAL_CALL(TransformComponent_SetTranslation);

		HZ_ADD_INTERNAL_CALL(TransformComponent_GetScale);
		HZ_ADD_INTERNAL_CALL(TransformComponent_SetScale);

		HZ_ADD_INTERNAL_CALL(Rigidbody2DComponent_ApplyLinearImpulse);
		HZ_ADD_INTERNAL_CALL(Rigidbody2DComponent_ApplyLinearImpulseToCenter);
		HZ_ADD_INTERNAL_CALL(Rigidbody2DComponent_GetLinearVelocity);
		HZ_ADD_INTERNAL_CALL(Rigidbody2DComponent_SetLinearVelocity);
		HZ_ADD_INTERNAL_CALL(Rigidbody2DComponent_GetType);
		HZ_ADD_INTERNAL_CALL(Rigidbody2DComponent_SetType);

		HZ_ADD_INTERNAL_CALL(BoxColliderComponent_GetOffset);
		HZ_ADD_INTERNAL_CALL(BoxColliderComponent_SetOffset);
		HZ_ADD_INTERNAL_CALL(BoxColliderComponent_GetSize);
		HZ_ADD_INTERNAL_CALL(BoxColliderComponent_SetSize);

		HZ_ADD_INTERNAL_CALL(SpriteAnimatorComponent_Play);

		HZ_ADD_INTERNAL_CALL(Input_IsKeyDown);

		HZ_ADD_INTERNAL_CALL(CameraComponent_GetOrthographicSize);
		HZ_ADD_INTERNAL_CALL(CameraComponent_SetOrthographicSize);
	}

}