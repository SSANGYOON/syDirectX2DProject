using System;
using System.Reflection;

namespace SY
{
    public class Entity
    {
        protected Entity() { ID = 0; }

        public Entity(ulong id)
        {
            ID = id;
        }

        public readonly ulong ID;

        public bool IsValid()
        {
                return InternalCalls.Entity_IsValid(ID);   
        }

        public Entity Instantiate(Vector3 position, ulong parentId)
        {
            InternalCalls.Entity_Instantiate(ID, ref position, parentId, out ulong instanceID);
            if (instanceID > 0)
                return new Entity(instanceID);
            else
                return null;
        }

        public Entity GetChild(string childTag)
        {
            InternalCalls.Entity_GetChild(ID, childTag, out ulong childId);
            if (childId > 0)
                return new Entity(childId);
            else
                return null;
        }

        public void Destroy()
        {
            StateComponent state = GetComponent<StateComponent>();
            state.State = StateComponent.EntityState.Dead;
        }

        public void DontDestroy()
        {
            Vector3 pos = Vector3.Zero;
            InternalCalls.Entity_DontDestroy(ID, ref pos);
        }

        public void DontDestroy(ref Vector3 position)
        {
            InternalCalls.Entity_DontDestroy(ID, ref position);
        }

        public void Pause()
        {
            StateComponent state = GetComponent<StateComponent>();
            state.State = StateComponent.EntityState.Pause;
        }

        public void Activate()
        {
            StateComponent state = GetComponent<StateComponent>();
            state.State = StateComponent.EntityState.Active;
        }

        public Vector3 Translation
        {
            get
            {
                InternalCalls.TransformComponent_GetTranslation(ID, out Vector3 result);
                return result;
            }
            protected set
            {
                InternalCalls.TransformComponent_SetTranslation(ID, ref value);
            }
        }

        public Vector3 Scale
        {
            get
            {
                InternalCalls.TransformComponent_GetScale(ID, out Vector3 result);
                return result;
            }
            protected set
            {
                InternalCalls.TransformComponent_SetScale(ID, ref value);
            }
        }


        public bool HasComponent<T>() where T : Component, new()
        {
            Type componentType = typeof(T);
            return InternalCalls.Entity_HasComponent(ID, componentType);
        }

        public T GetComponent<T>() where T : Component, new()
        {
            if (!HasComponent<T>())
                return null;

            T component = new T() { Entity = this };
            return component;
        }

        public Entity FindEntityByName(string name)
        {
            ulong entityID = InternalCalls.Entity_FindEntityByName(name);
            if (entityID == 0)
                return null;

            return new Entity(entityID);
        }

        public T As<T>() where T : Entity, new()
        {
            object instance = InternalCalls.GetScriptInstance(ID);
            return instance as T;
        }
    }

}
