using System;
using System.Runtime.CompilerServices;

namespace SY
{
    public static class InternalCalls
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Entity_IsValid(ulong entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static ulong Entity_Instantiate(ulong entityID, ref Vector3 position, ulong parentID, out ulong id);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Entity_Destroy(ulong entityID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Entity_DontDestroy(ulong entityID, ref Vector3 desiredPos);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Entity_HasComponent(ulong entityID, Type componentType);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static ulong Entity_FindEntityByName(string name);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static object GetScriptInstance(ulong entityID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_GetTranslation(ulong entityID, out Vector3 translation);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_SetTranslation(ulong entityID, ref Vector3 translation);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_GetWorldPosition(ulong entityID, out Vector3 worldPosition);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_GetScale(ulong entityID, out Vector3 scale);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_SetScale(ulong entityID, ref Vector3 scale);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_GetRotation(ulong entityID, out Vector3 rotation);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_SetRotation(ulong entityID, ref Vector3 rotation);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Rigidbody2DComponent_ApplyLinearImpulse(ulong entityID, ref Vector2 impulse, ref Vector2 point, bool wake);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Rigidbody2DComponent_GetLinearVelocity(ulong entityID, out Vector2 linearVelocity);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Rigidbody2DComponent_SetLinearVelocity(ulong entityID, ref Vector2 velocity, bool wake);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float Rigidbody2DComponent_GetAngularVelocity(ulong entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Rigidbody2DComponent_SetAngularVelocity(ulong entityID, float velocity);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static Rigidbody2DComponent.BodyType Rigidbody2DComponent_GetType(ulong entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Rigidbody2DComponent_SetType(ulong entityID, Rigidbody2DComponent.BodyType type);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Rigidbody2DComponent_ApplyLinearImpulseToCenter(ulong entityID, ref Vector2 impulse, bool wake);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Rigidbody2DComponent_Flip(ulong entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Rigidbody2DComponent_GetEnable(ulong entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Rigidbody2DComponent_SetEnable(ulong entityID, bool enable);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float DistanceJointComponent_GetCurrentLength(ulong entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float DistanceJointComponent_GetMinLength(ulong entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void DistanceJointComponent_SetMinLength(ulong entityID, float Length);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void RevoluteJointComponent_SetAngleLimit(ulong entityID, ref Vector2 limit);



        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static int Input_GetKeyState(KeyCode keycode);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool SpriteAnimatorComponent_Play(ulong entityID, string clipName, float at);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SpriteAnimatorComponent_Stop(ulong entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool TransformAnimatorComponent_Play(ulong entityID, string clipName, float at);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformAnimatorComponent_Stop(ulong entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void BoxColliderComponent_GetOffset(ulong entityID, out Vector2 offset);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void BoxColliderComponent_SetOffset(ulong entityID, ref Vector2 offset);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void BoxColliderComponent_GetSize(ulong entityID, out Vector2 size);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void BoxColliderComponent_SetSize(ulong entityID, ref Vector2 size);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CircleColliderComponent_GetOffset(ulong entityID, out Vector2 offset);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CircleColliderComponent_SetOffset(ulong entityID, ref Vector2 offset);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float CircleColliderComponent_GetRadius(ulong entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CircleColliderComponent_SetRadius(ulong entityID, float radius);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Physics_RayCast2D(ref Vector2 from, ref Vector2 to, UInt16 mask, out Collision2D col);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CameraComponent_GetOrthographicSize(ulong entityID, out Vector2 size);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CameraComponent_SetOrthographicSize(ulong entityID, ref Vector2 size);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CameraComponent_AddOscilation(ulong entityID, float amp);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CameraComponent_SetScissorRect(ulong entityID, ref Vector4 scissorRect);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CameraComponent_GetScissorRect(ulong entityID, out Vector4 scissorRect);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CameraComponent_GetFadeColor(ulong entityID, out Vector4 fadeColor);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CameraComponent_SetFadeColor(ulong entityID, ref Vector4 fadeColor);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static StateComponent.EntityState StateComponent_GetState(ulong entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void StateComponent_SetState(ulong entityID, StateComponent.EntityState type);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Timer_SetTimeScale(float timeScale);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float Timer_GetTimeScale();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void PanelComponent_GetTintColor(ulong entityID, out Vector4 color);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void PanelComponent_SetTintColor(ulong entityID, ref Vector4 color);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void IconComponent_GetTintColor(ulong entityID, out Vector4 color);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void IconComponent_SetTintColor(ulong entityID, ref Vector4 color);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_SetState(ulong entityID, uint state);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static uint ParticleSystem_GetState(ulong entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_GetGeneratePos(ulong entityID, out Vector2 pos);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_SetGeneratePos(ulong entityID, ref Vector2 pos);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static uint ParticleSystem_SetGeneratePosVariation(ulong entityID, ref Vector2 posVar);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static uint ParticleSystem_SetVelocityBegin(ulong entityID, ref Vector2 posVar);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static uint ParticleSystem_SetVelocityEnd(ulong entityID, ref Vector2 posVar);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_SetEmissionBegin(ulong entityID, ref Vector4 emissionBegin);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_SetEmissionEnd(ulong entityID, ref Vector4 emissionEnd);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_SetColorBegin(ulong entityID, ref Vector4 colorBegin);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ParticleSystem_SetColorEnd(ulong entityID, ref Vector4 colorEnd);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static uint ParticleSystem_SetAliveZone(ulong entityID, ref Vector2 aliveZone);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static uint ParticleSystem_SetLifeTime(ulong entityID, float lifeTime);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static uint ParticleSystem_SetAttachOffset(ulong entityID, ref Vector2 offset);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void IconComponent_SetTexture(ulong entityID, string color);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SlotComponent_SetItem(ulong entityID, string color);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SpriteRendererComponent_SetTexture(ulong entityID, string color);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SpriteRendererComponent_SetColor(ulong entityID, ref Vector4 color);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SpriteRendererComponent_GetColor(ulong entityID, out Vector4 color);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SpriteRendererComponent_SetEmission(ulong entityID, ref Vector4 emission);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SpriteRendererComponent_GetEmission(ulong entityID, out Vector4 emission);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SpriteRendererComponent_SetFloat(ulong entityID, uint id, float value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SpriteRendererComponent_SetVec2(ulong entityID, uint id, ref Vector2 value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SpriteRendererComponent_SetShader(ulong entityID, string key);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SpriteRendererComponent_GetTile(ulong entityID, out Vector2 tile);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SpriteRendererComponent_SetTile(ulong entityID, ref Vector2 tile);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SpriteRendererComponent_GetOffset(ulong entityID, out Vector2 offset);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void LineRenderer_SetSize(ulong entityID, ref Vector2 size);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void LineRenderer_GetSize(ulong entityID, out Vector2 size);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SliderComponent_SetMaxValue(ulong entityID, float v);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float SliderComponent_GetMaxValue(ulong entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SliderComponent_SetValue(ulong entityID, float v);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float SliderComponent_GetValue(ulong entityID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Entity_GetChild(ulong entityID, string childTag, out ulong childID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Entity_GetParent(ulong entityID, out ulong childID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SceneManager_LoadScene(string nextScenePath);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SceneManager_LoadSceneAsync(string nextScenePath);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void AudioSource_play(ulong entityID, string clip, bool loop);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void AudioSource_stop(ulong entityID);
    }
}