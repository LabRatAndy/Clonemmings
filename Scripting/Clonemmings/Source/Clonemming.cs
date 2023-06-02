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
        private int m_Direction = 1; //-1 == left 1 == right
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
            InternalCalls.Native_Log("C#: on update");
            if (m_ClonemmingComponent.Status == ClonemmingComponent.ClonmmingStatus.Walker)
            {
                if(m_RigidBody.HasContactLeft)
                {
                    InternalCalls.Native_Log("C#: Left contact");
                    m_Direction = 1;
                }
                if(m_RigidBody.HasContactRight)
                {
                    InternalCalls.Native_Log("C#: Right contact");
                    m_Direction = -1;
                }
                if (m_RigidBody.HasContactBottom)
                {
                    InternalCalls.Native_Log("C#: Bottom Contact");
                    Vector2 linearvelocity = m_RigidBody.LinearVelocity;
                    InternalCalls.Native_Log("C#: Linear velocity x: " + linearvelocity.X + " y: " + linearvelocity.Y);
                    float targetvelocity = 0;   
                    switch(m_Direction)
                    {
                        case 1: targetvelocity = Math.Min(Math.Abs(linearvelocity.X) + 0.1f, m_ClonemmingComponent.WalkSpeed); break;
                        case -1: targetvelocity = -(Math.Min(Math.Abs(linearvelocity.X) + 0.1f, m_ClonemmingComponent.WalkSpeed)); break;
                    }
                    InternalCalls.Native_Log("C#: targetvelocity: " + targetvelocity);
                    float velocitychange = targetvelocity - linearvelocity.X;
                    InternalCalls.Native_Log("C#: velocitychange: " + velocitychange);
                    float impulse = m_RigidBody.Mass * velocitychange;
                    if (m_Direction == -1)
                    {
                        InternalCalls.Native_Log("C#: apply leftward impulse of: " + impulse); ;
                        m_RigidBody.ApplyLinearImpulse(new Vector2(impulse, 0), true);
                    }
                    else if (m_Direction == 1)
                    {
                        InternalCalls.Native_Log("C#: apply rightward impulse of: " + impulse);
                        m_RigidBody.ApplyLinearImpulse(new Vector2(impulse, 0), true);
                    }
                }
                InternalCalls.Native_Log("C#: passed contact checking");
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
