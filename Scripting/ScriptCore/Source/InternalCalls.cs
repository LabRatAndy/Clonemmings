using Clonemmings;
using System;
using System.Runtime.CompilerServices;


namespace Core
{
    public static class InternalCalls
    {
        //core stuff
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void Native_Log(string text);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Entity_HasComponent(ulong entityID, Type componenttype);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static ulong Entity_FindEntityByName(string name);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static ulong Entity_CreateNewEntity(string name);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static ulong Entity_CreateNewClonemming(string name);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Entity_SetupPhysics(ulong entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Entity_DestroyEntity(ulong entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Entity_IsSelectedEntity(ulong entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static object GetInstance(ulong entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_GetTranslation(ulong entityID, out Vector3 translation);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_SetTranslation(ulong entityID, ref Vector3 translation);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_GetRotation(ulong entityID, out Vector3 rotation);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_SetRotation(ulong entityID, ref Vector3 rotation);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_GetScale(ulong entityID, out Vector3 scale);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_SetScale(ulong entityID, ref Vector3 scale);       
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void RigidBody2DComponent_ApplyLinearImpulse(ulong entityID, ref Vector2 impulse, ref Vector2 point, bool wake);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void RigidBody2DComponent_ApplyLinearImpulseToCentre(ulong entityID, ref Vector2 impulse, bool wake);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void RigidBody2DComponent_GetLinearVelocity(ulong entityID, out Vector2 linearvelocity);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static RigidBody2DComponent.BodyType RigidBody2DComponent_GetType(ulong entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void RigidBody2DComponent_SetType(ulong entityID, RigidBody2DComponent.BodyType type);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SpriteRendererComponent_GetColour(ulong entityID, out Vector4 colour);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SpriteRendererComponent_SetColour(ulong entityID, ref Vector4 colour);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float SpriteRendererComponent_GetTilingFactor(ulong entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SpriteRendererComponent_SetTilingFactor(ulong entityID, float tilingfactor);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Input_IsKeyDown(KeyCode keycode);


        //game components 
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static ClonemmingComponent.ClonmmingStatus ClonemmingComponent_GetStatus(ulong entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ClonemmingComponent_SetStatus(ulong entityID, ClonemmingComponent.ClonmmingStatus status);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float ClonemmingComponent_GetWalkSpeed(ulong entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ClonemmingComponent_SetWalkSpeed(ulong entityID, float walkspeed);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float ClonemmingComponent_GetDigSpeed(ulong entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ClonemmingComponent_SetDigSpeed(ulong entityID, float digspeed);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float ClonemmingComponent_GetMineSpeed(ulong entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float ClonemmingComponent_SetMineSpeed(ulong entityID, float minespeed);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static int ClonemmingStartComponent_GetNumberOfClonemmings(ulong entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ClonemmingStartComponent_SetNumberOfClonemmings(ulong entityID, int numberofclonemmings);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static int ClonemmingStartComponent_GetClonemmingReleaseRate(ulong entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ClonemmingStartComponent_SetClonemmingReleaseRate(ulong entityID, int releaserate);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static int ClonemmingExitComponent_GetNumberOfClonemmings(ulong entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ClonemmingExitComponent_SetNumberOfClonemmings(ulong entityID, int numberofclonemmings);
    }
}