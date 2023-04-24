#include "pch.h"
#include "ScriptGlue.h"
#include "ScriptEngine.h"

#include "UUID.h"

#include "Scene.h"
#include "Entity.h"

#include "Physics2D.h"

#include "mono/metadata/object.h"
#include "mono/metadata/reflection.h"
#include "box2d/b2_body.h"
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
			return true;
		}
	}

	static bool Input_IsKeyDown(KEY_TYPE keycode)
	{
		bool result = INPUT->GetKeyState(keycode) == KEY_STATE::PRESS;
		return result;
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

		HZ_ADD_INTERNAL_CALL(Rigidbody2DComponent_ApplyLinearImpulse);
		HZ_ADD_INTERNAL_CALL(Rigidbody2DComponent_ApplyLinearImpulseToCenter);
		HZ_ADD_INTERNAL_CALL(Rigidbody2DComponent_GetLinearVelocity);
		HZ_ADD_INTERNAL_CALL(Rigidbody2DComponent_SetLinearVelocity);
		HZ_ADD_INTERNAL_CALL(Rigidbody2DComponent_GetType);
		HZ_ADD_INTERNAL_CALL(Rigidbody2DComponent_SetType);
		HZ_ADD_INTERNAL_CALL(SpriteAnimatorComponent_Play);

		HZ_ADD_INTERNAL_CALL(Input_IsKeyDown);
	}

}