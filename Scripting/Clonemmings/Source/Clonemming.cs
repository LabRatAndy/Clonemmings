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

           if(EntitySelected())
            {
                if(Input.IsKeyDown(KeyCode.Left))
                {
                    InternalCalls.Native_Log("Key left pressed");
                    velocity.X = -1.0f;
                }
                else if(Input.IsKeyDown(KeyCode.Right))
                {
                    InternalCalls.Native_Log("key right pressed");
                    velocity.X = 1.0f;
                }
                if (Input.IsKeyDown(KeyCode.Up))
                {
                    InternalCalls.Native_Log("key up pressed");
                    velocity.Y = 1.0f; 
                }
                else if (Input.IsKeyDown(KeyCode.Down)) 
                {
                    InternalCalls.Native_Log("key down pressed");
                    velocity.Y = -1.0f;
                }
            }
            velocity *= m_ClonemmingComponent.WalkSpeed * ts;
            string result = "C#: Velocity: X " + velocity.X.ToString() + " Y " + velocity.Y.ToString();
            //InternalCalls.Native_Log(result);
            m_RigidBody.ApplyLinearImpulse(velocity.XY, true);
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
                m_Rectangle.Translation = m_Transform.Translation;
                m_Rectangle.Rotation = m_Transform.Rotation;
                Vector3 linescale = new Vector3(m_Transform.Scale.X * 1.1f, m_Transform.Scale.Y * 1.1f, m_Transform.Scale.Z);
                m_Rectangle.Scale = linescale;
            }
        }
    }
}
