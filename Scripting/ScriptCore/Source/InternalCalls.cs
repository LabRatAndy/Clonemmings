using Clonemmings;
using System;
using System.Runtime.CompilerServices;


namespace Core
{
    public static class InternalCalls
    {
        //core stuff
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Native_Log(string text);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void SetDeadClonemming(ulong entityID);
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
        internal extern static void Entity_AddRectangleComponent(ulong entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Entity_RemoveRectangleComponent(ulong entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Entity_AddComponent(ulong entityID, Type componenttype);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Entity_RemoveComponent(ulong entityID, Type componenttype);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Entity_SplitHorizontalEntity(ulong entityID, ref Vector3 gapposition, float gapwidth);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Entity_SplitVerticalEntity(ulong entityID, ref Vector3 gapposition, float gapwidth);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static object GetInstance(ulong entityID);
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
        internal extern static float RigidBody2DComponent_GetMass(ulong entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void RigidBody2DComponent_ApplyForce(ulong entityID, ref Vector2 force, ref Vector2 point, bool wake);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void RigidBody2DComponent_ApplyForceToCentre(ulong entityID, ref Vector2 force,bool wake);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void RigidBody2DComponent_ApplyAngularImpulse(ulong entityID, float impulse, bool wake);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void RigidBody2DComponent_ApplyTorque(ulong entityID, float torque, bool wake);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool RigidBody2DComponent_HasContact(ulong entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool RigidBody2DComponent_HasContactLeft(ulong entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool RigidBody2DComponent_HasContactRight(ulong entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool RigidBody2DComponent_HasContactTop(ulong entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool RigidBody2DComponent_HasContactBottom(ulong entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static ulong RigidBody2DComponent_GetBottomContact(ulong entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static ulong RigidBody2DComponent_GetRightContact(ulong entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static ulong RigidBody2DComponent_GetLeftContact(ulong entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void BoxCollider2DComponent_GetOffset(ulong entityID, out Vector2 offset);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void BoxCollider2DComponent_SetOffset(ulong entityID, ref Vector2 offset);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void BoxCollider2DComponent_GetSize(ulong entityID, out Vector2 size);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void BoxCollider2DComponent_SetSize(ulong entityID, ref Vector2 size);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float BoxCollider2DComponent_GetFriction(ulong entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void BoxCollider2DComponent_SetFriction(ulong entityID, float friction);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float BoxCollider2DComponent_GetDensity(ulong entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void BoxCollider2DComponent_SetDensity(ulong entityID, float density);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float BoxCollider2DComponent_GetRestitution(ulong entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void BoxCollider2DComponent_SetRestitution(ulong entityID, float restitution);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float boxCollider2DComponent_GetRestitutionThreshold(ulong entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void BoxCollider2DComponent_SetRestitutionThreshold(ulong entityID, float restitutionThreshold);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static ushort BoxCollider2DComponent_GetCategory(ulong entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void BoxCollider2DComponent_SetCategory(ulong entityID, ushort category);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static ushort BoxCollider2DComponent_GetMask(ulong entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void BoxCollider2DComponent_SetMask(ulong entityID, ushort mask);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CircleCollider2DComponent_GetOffset(ulong entityID, out Vector2 offset);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CircleCollider2DComponent_SetOffset(ulong entityID, ref Vector2 offset);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float CircleCollider2DComponent_GetRadius(ulong entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CircleCollider2DComponent_SetRadius(ulong entityID, float radius);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float CircleCollider2DComponent_GetFriction(ulong entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CircleCollider2DComponent_SetFriction(ulong entityID, float friction);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float CircleCollider2DComponent_GetDensity(ulong entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CircleCollider2DComponent_SetDensity(ulong entityID, float density);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float CircleCollider2DComponent_GetRestitution(ulong entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CircleCollider2DComponent_SetRestitution(ulong entityID, float restitution);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float CircleCollider2DComponent_GetRestitutionThreshold(ulong entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CircleCollider2DComponent_SetRestitutionThreshold(ulong entityID, float restitutionThreshold);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static ushort CircleCollider2DComponent_GetCategory(ulong entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CircleCollider2DComponent_SetCategory(ulong entityID, ushort category);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static ushort CircleCollider2DComponent_GetMask(ulong entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CircleCollider2DComponent_SetMask(ulong entityID, ushort mask);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SpriteRendererComponent_GetColour(ulong entityID, out Vector4 colour);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SpriteRendererComponent_SetColour(ulong entityID, ref Vector4 colour);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float SpriteRendererComponent_GetTilingFactor(ulong entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SpriteRendererComponent_SetTilingFactor(ulong entityID, float tilingfactor);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void RectangleComponent_GetTranslation(ulong entityID, out Vector3 translation);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void RectangleComponent_SetTranslation(ulong entityID, ref Vector3 translation);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void RectangleComponent_GetRotation(ulong entityID, out Vector3 rotation);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void RectangleComponent_SetRotation(ulong entityID, ref Vector3 rotation);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void RectangleComponent_GetScale(ulong entityID, out Vector3 scale);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void RectangleComponent_SetScale(ulong entityID, ref Vector3 scale);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void RectangleComponent_GetColour(ulong entityID, out Vector4 colour);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void RectangleComponent_SetColour(ulong entityID, ref Vector4 colour);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float RectangleComponent_GetLineThinkness(ulong entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void RectangleConponent_SetLineThickness(ulong entityID, float thickness);
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
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool ClonemmingExit_HasContactWithClonemming(ulong entityID, out ulong clonemmingID);
    }
}