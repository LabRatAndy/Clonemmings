using System;
using System.Reflection.Emit;

namespace Core
{
    public class Entity
    {
        protected Entity() { ID = 0; }

        public Entity(ulong id)
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
            if(ID == 0) 
            {
                InternalCalls.Native_Log("C#: Has Component called by entity with invalid ID");
                return false;
            }
            return InternalCalls.Entity_HasComponent(ID, componenttype);
        }
        public T GetComponent<T>() where T : Component, new()
        {
            InternalCalls.Native_Log("C#: GetComponent called");
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
            if (uuid ==0)
            {
                InternalCalls.Native_Log("C#: Clonemming has invalid ID");
            }
            return new Entity(uuid);
        }
        public static Entity CreateNewEntity(string name)
        {
             ulong uuid = InternalCalls.Entity_CreateNewEntity(name);
             return new Entity(uuid);
        }
        protected void PhysicsSetup()
        {
            InternalCalls.Entity_SetupPhysics(ID);
        }
        public bool EntitySelected()
        {
            return InternalCalls.Entity_IsSelectedEntity(ID);
        }
        public void AddRectangleComponent()
        {
            InternalCalls.Entity_AddRectangleComponent(ID);
        }
        public void RemoveRectangleComponent()
        {
            InternalCalls.Entity_RemoveRectangleComponent(ID);
        }
        public T AddComponent<T>() where T : Component, new() 
        {
            Type componenttype = typeof(T);
            InternalCalls.Entity_AddComponent(ID, componenttype);
            return GetComponent<T>();
        }
        public void RemoveComponent<T>() where T: Component
        {
            Type type = typeof(T);
            InternalCalls.Entity_RemoveComponent(ID, type);
        }
        public void Destroy()
        {
            InternalCalls.Entity_DestroyEntity(ID);
        }
        public void HorizontalSplit(ref Vector3 gapposition, float gapwidth)
        {
            InternalCalls.Entity_SplitHorizontalEntity(ID, ref gapposition, gapwidth);
        }
        public void VerticalSplit(ref Vector3 gapposition, float gapwidth)
        {
            InternalCalls.Entity_SplitVerticalEntity(ID, ref gapposition, gapwidth);
        }
   }
}