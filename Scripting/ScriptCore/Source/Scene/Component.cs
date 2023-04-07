namespace Core
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
            InternalCalls.RigidBody2DComponent_ApplyLinearImpulseToCenter(Entity.ID, ref impulse, wake);
        }
    }


}