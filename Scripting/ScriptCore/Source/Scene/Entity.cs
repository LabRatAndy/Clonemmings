using System;
using System.Reflection.Emit;

namespace Core
{
    public class Entity
    {
        protected Entity() { ID = 0; }

        internal Entity(ulong id)
        {
            ID = id;
        }

        public readonly ulong ID;

        public Vector3 Translation
        {
            get
            {
                InternalCalls.TransformComponent_GetTranslation(ID, out Vector3 result);
                return result;
            }
            set
            {
                InternalCalls.TransformComponent_SetTranslation(ID, ref value);
            }
        }
        public bool HasComponent<T>() where T:Component ,new()
        {
            Type componenttype = typeof(T);
            return InternalCalls.Entity_HasComponent(ID, componenttype);
        }
        public T GetComponent<T>() where T : Component, new()
        {
            if(!HasComponent<T>())
            {
                return null;
            }
            T component = new T() { Entity = this };
            return component;
        }
        public Entity FindEntityByName(string name)
        {
            ulong uuid = InternalCalls.Entity_FindEntityByName(name);
            if(uuid ==0)
            {
                return null;
            }
            return new Entity(uuid);
        }
        public T As<T>() where T : Entity, new()
        {
            object instance = InternalCalls.GetInstance(ID);
            return instance as T;
        }
        public static Entity CreateNewClonemming(string name)
        {
            ulong uuid = InternalCalls.Entity_CreateNewClonemming(name);
            return new Entity(uuid);
        }
        public static Entity CreateNewEntity(string name)
        {
             ulong uuid = InternalCalls.Entity_CreateNewEntity(name);
             return new Entity(uuid);
        }
   }
}