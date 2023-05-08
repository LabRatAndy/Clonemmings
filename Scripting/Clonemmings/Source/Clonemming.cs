using System;
using Core;

namespace Clonemmings
{
    public class Clonemming : Entity
    {
       
        private TransformComponent m_Transform;
        private RigidBody2DComponent m_RigidBody;
        private ClonemmingComponent m_ClonemmingComponent;
        private RectangleComponent m_Rectangle = null;
        void OnCreate()
        {
            m_Transform = GetComponent<TransformComponent>();
            m_RigidBody = GetComponent<RigidBody2DComponent>();
            m_ClonemmingComponent = GetComponent<ClonemmingComponent>();
        }
        void OnUpdate(float ts)
        {
            Vector3 velocity = Vector3.Zero;
            HighlightEntityWhenSelected();

            if (m_ClonemmingComponent.Status == ClonemmingComponent.ClonmmingStatus.Walker)
            {
                Vector2 linearvelocity =  m_RigidBody.LinearVelocity;
                Vector2 normalisedLV = linearvelocity.Normalise();
                InternalCalls.Native_Log("C#: normalised Linear velocity X: " + normalisedLV.X.ToString() + " Y: " + normalisedLV.Y.ToString());
                velocity.X = normalisedLV.X;
                velocity.Y = normalisedLV.Y;
                velocity *= m_ClonemmingComponent.WalkSpeed * ts;
                string result = "C#: Velocity: X " + velocity.X.ToString() + " Y " + velocity.Y.ToString();
                InternalCalls.Native_Log(result);
                m_RigidBody.ApplyLinearImpulse(velocity.XY, true);
            }
        }

        public void SetInitialPostion(Vector3 position, float rotation = 0.0f)
        {
            m_Transform.Translation = position;
            m_Transform.Rotation = new Vector3(0.0f, 0.0f, rotation);
            m_Transform.Scale = new Vector3(1.0f);
        }
        public void SetUpPhysics()
        {
            PhysicsSetup();
        }
        private void HighlightEntityWhenSelected()
        {
            if (EntitySelected() && !HasComponent<RectangleComponent>())
            {
                m_Rectangle = AddComponent<RectangleComponent>();
                m_Rectangle.Translation = m_Transform.Translation;
                m_Rectangle.Rotation = m_Transform.Rotation;
                Vector3 linescale = new Vector3(m_Transform.Scale.X, m_Transform.Scale.Y, m_Transform.Scale.Z);
                m_Rectangle.Scale = linescale;
            }
            else if (!EntitySelected() && HasComponent<RectangleComponent>())
            {
                RemoveComponent<RectangleComponent>();
                m_Rectangle = null;
            }
            else
            {
                if (m_Rectangle == null)
                    return;
                m_Rectangle.Translation = m_Transform.Translation;
                m_Rectangle.Rotation = m_Transform.Rotation;
                Vector3 linescale = new Vector3(m_Transform.Scale.X * 1.1f, m_Transform.Scale.Y * 1.1f, m_Transform.Scale.Z);
                m_Rectangle.Scale = linescale;
            }
        }
        private Vector3 RotateVelocity(float rotation,Vector2 velocity)
        {
            float sintheta = (float)Math.Sin(rotation);
            float costheta = (float)Math.Cos(rotation);
            float rotatedX = costheta * -sintheta * velocity.X;
            float rotatedy = sintheta * costheta * velocity.Y;
            return new Vector3(rotatedX, rotatedy, 0);
        }
    }
}
