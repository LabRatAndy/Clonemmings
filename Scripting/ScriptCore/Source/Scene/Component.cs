﻿namespace Core
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
                InternalCalls.TransformComponent_GetTranslation(Entity.ID, out Vector3 result);
                return result;
            }
            set
            {
                InternalCalls.TransformComponent_SetTranslation(Entity.ID, ref value);
            }
        }
        public Vector3 Rotation
        {
            get
            {
                InternalCalls.TransformComponent_GetRotation(Entity.ID, out Vector3 result);
                return result;
            }
            set
            {
                InternalCalls.TransformComponent_SetRotation(Entity.ID, ref value);
            }
        }
        public Vector3 Scale
        {
            get
            {
                InternalCalls.TransformComponent_GetScale(Entity.ID, out Vector3 result);
                return result;
            }
            set
            {
                InternalCalls.TransformComponent_SetScale(Entity.ID, ref value);
            }
        }
    }
    public class RigidBody2DComponent : Component
    {
        public enum BodyType { Static = 0, Dynamic, Kinematic};

        public Vector2 LinearVelocity
        {
            get
            {
                InternalCalls.RigidBody2DComponent_GetLinearVelocity(Entity.ID, out Vector2 result);
                return result;
            }
        }
        public BodyType Type
        {
            get => InternalCalls.RigidBody2DComponent_GetType(Entity.ID);
            set => InternalCalls.RigidBody2DComponent_SetType(Entity.ID, value);
        }
        public void ApplyLinearImpulse(Vector2 impulse, Vector2 worldpos, bool wake)
        {
            InternalCalls.RigidBody2DComponent_ApplyLinearImpulse(Entity.ID,ref impulse,ref  worldpos, wake);
        }
        public void ApplyLinearImpulse(Vector2 impulse,bool wake)
        {
            InternalCalls.RigidBody2DComponent_ApplyLinearImpulseToCentre(Entity.ID, ref impulse, wake);
        }
        public float Mass
        {
           get =>InternalCalls.RigidBody2DComponent_GetMass(Entity.ID);
        }
        public void ApplyForce(Vector2 force, Vector2 point,bool wake)
        {
            InternalCalls.RigidBody2DComponent_ApplyForce(Entity.ID, ref force, ref point, wake);
        }
        public void ApplyForce(Vector2 force, bool wake)
        {
            InternalCalls.RigidBody2DComponent_ApplyForceToCentre(Entity.ID, ref force, wake);
        }
        public void ApplyAngularImpulse(float impulse, bool wake)
        {
            InternalCalls.RigidBody2DComponent_ApplyAngularImpulse(Entity.ID, impulse, wake);
        }
        public void ApplyTorque(float torque,bool wake)
        {
            InternalCalls.RigidBody2DComponent_ApplyTorque(Entity.ID, torque, wake);
        }
        public bool HasContact
        {
            get =>InternalCalls.RigidBody2DComponent_HasContact(Entity.ID);
        }
        public bool HasContactLeft
        {
            get =>InternalCalls.RigidBody2DComponent_HasContactLeft(Entity.ID);
        }
        public bool HasContactRight
        {
            get => InternalCalls.RigidBody2DComponent_HasContactRight(Entity.ID);
        }
        public bool HasContactTop
        {
            get => InternalCalls.RigidBody2DComponent_HasContactTop(Entity.ID);
        }
        public bool HasContactBottom
        {
            get => InternalCalls.RigidBody2DComponent_HasContactBottom(Entity.ID);
        }
        public ulong GetBottomContact
        {
            get => InternalCalls.RigidBody2DComponent_GetBottomContact(Entity.ID);
        }
    }
    public class BoxCollider2DComponent : Component
    {
        public Vector2 Offset 
        {
            get 
            {
                InternalCalls.BoxCollider2DComponent_GetOffset(Entity.ID, out Vector2 result);
                return result;
            }
            set => InternalCalls.BoxCollider2DComponent_SetOffset(Entity.ID, ref value);
        }
        public Vector2 Size
        {
            get 
            {
                InternalCalls.BoxCollider2DComponent_GetSize(Entity.ID,out Vector2 result);
                return result;
            }
            set => InternalCalls.BoxCollider2DComponent_SetSize(Entity.ID, ref value);
        }
        public float Friction
        {
            get => InternalCalls.BoxCollider2DComponent_GetFriction(Entity.ID);
            set => InternalCalls.BoxCollider2DComponent_SetFriction(Entity.ID, value);            
        }
        public float Density
        {
            get => InternalCalls.BoxCollider2DComponent_GetDensity(Entity.ID);
            set => InternalCalls.BoxCollider2DComponent_SetDensity(Entity.ID, value);
        }
        public float Restitution
        {
            get => InternalCalls.BoxCollider2DComponent_GetRestitution(Entity.ID);
            set => InternalCalls.BoxCollider2DComponent_SetRestitution(Entity.ID, value);
        }
        public float RestitutionThreshold
        {
            get => InternalCalls.boxCollider2DComponent_GetRestitutionThreshold(Entity.ID);
            set => InternalCalls.BoxCollider2DComponent_SetRestitutionThreshold(Entity.ID, value);
        }
        public ushort CollisionCategory
        {
            get => InternalCalls.BoxCollider2DComponent_GetCategory(Entity.ID);
            set => InternalCalls.BoxCollider2DComponent_SetCategory(Entity.ID, value);
        }
        public ushort CollisionMask
        {
            get => InternalCalls.BoxCollider2DComponent_GetMask(Entity.ID);
            set => InternalCalls.BoxCollider2DComponent_SetMask(Entity.ID, value);
        }
    }
    public class CircleCollider2DComponent : Component
    {
        public Vector2 Offset
        {
            get
            {
                InternalCalls.CircleCollider2DComponent_GetOffset(Entity.ID,out Vector2 result);
                return result;
            }
            set => InternalCalls.CircleCollider2DComponent_SetOffset(Entity.ID, ref value);
        }
        public float Radius
        {
            get => InternalCalls.CircleCollider2DComponent_GetRadius(Entity.ID);
            set => InternalCalls.CircleCollider2DComponent_SetRadius(Entity.ID, value);
        }
        public float Friction
        {
            get => InternalCalls.CircleCollider2DComponent_GetFriction(Entity.ID);
            set => InternalCalls.CircleCollider2DComponent_SetFriction(Entity.ID, value);
        }
        public  float Density
        {
            get => InternalCalls.CircleCollider2DComponent_GetDensity(Entity.ID);
            set => InternalCalls.CircleCollider2DComponent_SetDensity(Entity.ID, value);
        }
        public float Restitution
        {
            get => InternalCalls.CircleCollider2DComponent_GetRestitution(Entity.ID);
            set => InternalCalls.CircleCollider2DComponent_SetRestitution(Entity.ID, value);
        }
        public float RestitutionThreshold
        {
            get => InternalCalls.CircleCollider2DComponent_GetRestitutionThreshold(Entity.ID);
            set => InternalCalls.CircleCollider2DComponent_SetRestitutionThreshold(Entity.ID, value);
        }
        public ushort CollisionCategory
        {
            get => InternalCalls.CircleCollider2DComponent_GetCategory(Entity.ID);
            set => InternalCalls.CircleCollider2DComponent_SetCategory(Entity.ID, value);
        }
        public ushort CollisionMask
        {
            get => InternalCalls.CircleCollider2DComponent_GetMask(Entity.ID);
            set => InternalCalls.CircleCollider2DComponent_SetMask(Entity.ID, value);
        }
    }


    public class SpriteRendererComponent : Component
    {
        public Vector4 Colour 
        {
            get
            {
                InternalCalls.SpriteRendererComponent_GetColour(Entity.ID, out Vector4 result);
                return result;
            }
            set => InternalCalls.SpriteRendererComponent_SetColour(Entity.ID,  ref value);
        }
        public float TilingFactor
        {
            get => InternalCalls.SpriteRendererComponent_GetTilingFactor(Entity.ID);
            set => InternalCalls.SpriteRendererComponent_SetTilingFactor(Entity.ID, (float)value);
        }
        //todo access textures but need to access textures from C++
    }

    public class RectangleComponent : Component
    {
        public Vector3 Translation
        {
            get
            {
                InternalCalls.RectangleComponent_GetTranslation(Entity.ID, out Vector3 result);
                return result;
            }
            set => InternalCalls.RectangleComponent_SetTranslation(Entity.ID, ref value);
        }
        public Vector3 Rotation
        {
            get
            {
                InternalCalls.RectangleComponent_GetRotation(Entity.ID, out Vector3 result);
                return result;
            }
            set => InternalCalls.RectangleComponent_SetRotation(Entity.ID, ref value);
        }
        public Vector3 Scale
        {
            get
            {
                InternalCalls.RectangleComponent_GetScale(Entity.ID, out Vector3 result);
                return result;
            }
            set => InternalCalls.RectangleComponent_SetScale(Entity.ID, ref value);
        }
        public Vector4 LineColour
        {
            get
            {
                InternalCalls.RectangleComponent_GetColour(Entity.ID, out Vector4 result);
                return result;
            }
            set => InternalCalls.RectangleComponent_SetColour(Entity.ID, ref value);
        }
        public float LineThickness
        {
            get => InternalCalls.RectangleComponent_GetLineThinkness(Entity.ID);
            set => InternalCalls.RectangleConponent_SetLineThickness(Entity.ID, value);
        }
    }


}