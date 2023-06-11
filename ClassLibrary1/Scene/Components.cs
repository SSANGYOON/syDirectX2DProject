using System;
using System.Deployment.Internal;
using System.Xml;

namespace SY
{
    public abstract class Component
    {
        public Entity Entity { get; internal set; }
    }

    public class TransformComponent : Component
    {
        public Vector3 Translation
        {
            get
            {
                InternalCalls.TransformComponent_GetTranslation(Entity.ID, out Vector3 translation);
                return translation;
            }
            set
            {
                InternalCalls.TransformComponent_SetTranslation(Entity.ID, ref value);
            }
        }

        public Vector3 Scale
        {
            get
            {
                InternalCalls.TransformComponent_GetScale(Entity.ID, out Vector3 scale);
                return scale;
            }
            set
            {
                InternalCalls.TransformComponent_SetScale(Entity.ID, ref value);
            }

        }
    }

    public class Rigidbody2DComponent : Component
    {
        public enum BodyType { Static = 0, Dynamic, Kinematic }

        public bool Enable
        {
            get { return InternalCalls.Rigidbody2DComponent_GetEnable(Entity.ID); }
            set { InternalCalls.Rigidbody2DComponent_SetEnable(Entity.ID, value); }
        }
        public Vector2 LinearVelocity
        {
            get
            {
                InternalCalls.Rigidbody2DComponent_GetLinearVelocity(Entity.ID, out Vector2 velocity);
                return velocity;
            }
            set => InternalCalls.Rigidbody2DComponent_SetLinearVelocity(Entity.ID, ref value, true);
        }

        public float AngularVelocity
        {
            get { return InternalCalls.Rigidbody2DComponent_GetAngularVelocity(Entity.ID); }

            set { InternalCalls.Rigidbody2DComponent_SetAngularVelocity(Entity.ID, value); }
        }

        public BodyType Type
        {
            get => InternalCalls.Rigidbody2DComponent_GetType(Entity.ID);
            set => InternalCalls.Rigidbody2DComponent_SetType(Entity.ID, value);
        }

        public void ApplyLinearImpulse(Vector2 impulse, Vector2 worldPosition, bool wake)
        {
            InternalCalls.Rigidbody2DComponent_ApplyLinearImpulse(Entity.ID, ref impulse, ref worldPosition, wake);
        }

        public void ApplyLinearImpulse(Vector2 impulse, bool wake)
        {
            InternalCalls.Rigidbody2DComponent_ApplyLinearImpulseToCenter(Entity.ID, ref impulse, wake);
        }

        public void Flip()
        {
            InternalCalls.Rigidbody2DComponent_Flip(Entity.ID);
        }
    }

    public class StateComponent : Component
    {
        public enum EntityState { Active = 0, Pause, Dead }


        public EntityState State
        {
            get => InternalCalls.StateComponent_GetState(Entity.ID);
            set => InternalCalls.StateComponent_SetState(Entity.ID, value);
        }
    }

    public class SpriteAnimatorComponent : Component
    {
        public bool Play(string clipName, float at = 0.0f)
        {
            return InternalCalls.SpriteAnimatorComponent_Play(Entity.ID, clipName, at);
        }
    }

    public class TransformAnimatorComponent : Component 
    {
        public bool Play(string clipName, float at = 0.0f)
        { 
            return InternalCalls.TransformAnimatorComponent_Play(Entity.ID, clipName, at);
        }
    
    }

    public class SliderComponent : Component
    {
        public float maxValue
        {
           
            get {return InternalCalls.SliderComponent_GetMaxValue(Entity.ID);}
            set{InternalCalls.SliderComponent_SetMaxValue(Entity.ID, value);}
        }

        public float Value
        {

            get { return InternalCalls.SliderComponent_GetValue(Entity.ID); }
            set { InternalCalls.SliderComponent_SetValue(Entity.ID, value); }
        }

    }

    public class BoxCollider2DComponent : Component
    { 
        public Vector2 Offset
        {
            get
            {
                InternalCalls.BoxColliderComponent_GetOffset(Entity.ID, out Vector2 offset);
                return offset;
            }
            set => InternalCalls.BoxColliderComponent_SetOffset(Entity.ID, ref value);
        }

        public Vector2 Size
        {
            get
            {
                InternalCalls.BoxColliderComponent_GetSize(Entity.ID, out Vector2 size);
                return size;
            }
            set => InternalCalls.BoxColliderComponent_SetSize(Entity.ID, ref value);
        }
    }

    public class CameraComponent : Component
    {
        public Vector2 OrthographicSize
        {
            get
            {
                InternalCalls.CameraComponent_GetOrthographicSize(Entity.ID, out Vector2 orthographicSize);
                return orthographicSize;
            }
            set => InternalCalls.CameraComponent_SetOrthographicSize(Entity.ID, ref value);
        }

        public void AddOscilation(float amp)
        {
            InternalCalls.CameraComponent_AddOscilation(Entity.ID, amp);
        }
    }

    public class PanelComponent : Component 
    {
        public Vector4 TintColor
        {
            get
            {
                InternalCalls.PanelComponent_GetTintColor(Entity.ID, out Vector4 tintColor);
                return tintColor;
            }
            set => InternalCalls.PanelComponent_SetTintColor(Entity.ID, ref value);
        }
    }

    public class IconComponent : Component
    {
        public Vector4 TintColor
        {
            get
            {
                InternalCalls.IconComponent_GetTintColor(Entity.ID, out Vector4 tintColor);
                return tintColor;
            }
            set => InternalCalls.IconComponent_SetTintColor(Entity.ID, ref value);
        }

        public string Texture
        {
            set { InternalCalls.IconComponent_SetTexture(Entity.ID, value); }
        }
    }

    public class SlotComponent : Component
    {
        public string Texture
        {
            set { InternalCalls.SlotComponent_SetItem(Entity.ID, value); }
        }
    }

    public class SpriteRendererComponent : Component
    {
        public string Texture
        {
            set { InternalCalls.SpriteRendererComponent_SetTexture(Entity.ID, value); }
        }

        public Vector4 Color
        {
            get { InternalCalls.SpriteRendererComponent_GetColor(Entity.ID, out Vector4 color);
                return color;
            }
            set { InternalCalls.SpriteRendererComponent_SetColor(Entity.ID, ref value); }
        }

        public Vector4 Emission
        {
            get
            {
                InternalCalls.SpriteRendererComponent_GetEmission(Entity.ID, out Vector4 emission);
                return emission;
            }
            set { InternalCalls.SpriteRendererComponent_SetEmission(Entity.ID, ref value); }
        }

        public void SetFloat(uint id, float value)
        {
            InternalCalls.SpriteRendererComponent_SetFloat(Entity.ID, id, value);
        }

        public void SetVec2(uint id, ref Vector2 value)
        {
            InternalCalls.SpriteRendererComponent_SetVec2(Entity.ID, id, ref value);
        }
    }


    public class ParticleSystem : Component
    {
        public enum ParticleState
        { 
            NORMAL,
            UPDATE_ONLY,
            PAUSE,
        }
        public uint State
        {
            set { InternalCalls.ParticleSystem_SetState(Entity.ID, (uint)value); }
            get { return InternalCalls.ParticleSystem_GetState(Entity.ID); }
        }

        public Vector2 Position
        { 
            get { InternalCalls.ParticleSystem_GetGeneratePos(Entity.ID, out Vector2 pos);
                return pos;
            }
            set { InternalCalls.ParticleSystem_SetGeneratePos(Entity.ID, ref value); }
        }

        public Vector2 PositionVariation
        {
            set { InternalCalls.ParticleSystem_SetGeneratePosVariation(Entity.ID, ref value); }
        }

        public Vector2 VelocityBegin
        {
            set { InternalCalls.ParticleSystem_SetVelocityBegin(Entity.ID, ref value); }
        }

        public Vector2 VelocityEnd
        {
            set { InternalCalls.ParticleSystem_SetVelocityEnd(Entity.ID, ref value); }
        }

        public Vector2 AliveZone
        {
            set { InternalCalls.ParticleSystem_SetAliveZone(Entity.ID, ref value); }
        }

        public float LifeTime
        {
            set { InternalCalls.ParticleSystem_SetLifeTime(Entity.ID, value); }
        }

        public Vector4 EmissionBegin
        {
            set { InternalCalls.ParticleSystem_SetEmissionBegin(Entity.ID, ref value); }
        }

        public Vector4 EmissionEnd
        {
            set { InternalCalls.ParticleSystem_SetEmissionEnd(Entity.ID, ref value); }
        }

        public Vector4 ColorBegin
        {
            set { InternalCalls.ParticleSystem_SetColorBegin(Entity.ID, ref value); }
        }

        public Vector4 ColorEnd
        {
            set { InternalCalls.ParticleSystem_SetColorEnd(Entity.ID, ref value); }
        }
    }

    public class DistanceJointComponent : Component
    {
        public float CurrentLength
        {
            get { return InternalCalls.DistanceJointComponent_GetCurrentLength(Entity.ID); }
        }

        public float MinLength
        {
            get { return InternalCalls.DistanceJointComponent_GetMinLength(Entity.ID); }
            set { InternalCalls.DistanceJointComponent_SetMinLength(Entity.ID, value); }
        }
    }

    public class LineRenderer : Component
    {
        public Vector2 Size
        {
            get { InternalCalls.LineRenderer_GetSize(Entity.ID, out Vector2 size);
                return size;}
            set {InternalCalls.LineRenderer_SetSize(Entity.ID, ref value);}
        }
    }

    public class Physics
    {
        public static void RayCast(ref Vector2 from, ref Vector2 to, UInt16 mask, out Collision2D col)
        {
            InternalCalls.Physics_RayCast2D(ref from, ref to, mask, out col);
        }
    }
}