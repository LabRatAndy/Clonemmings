using System;
using Core;

namespace Clonemmings
{
    public class Clonemming : Entity
    {
       
        private TransformComponent m_Transform;
        private RigidBody2DComponent m_RigidBody;
        private ClonemmingComponent m_ClonemmingComponent;

        void OnCreate()
        {
            m_Transform = GetComponent<TransformComponent>();
            m_RigidBody = GetComponent<RigidBody2DComponent>();
            m_ClonemmingComponent = GetComponent<ClonemmingComponent>();
        }
        void OnUpdate(float ts)
        {
            Vector3 velocity = Vector3.Zero;
           if(m_ClonemmingComponent.Status == ClonemmingComponent.ClonmmingStatus.Walker)
            {
                if(Input.IsKeyDown(KeyCode.Left))
                {
                    velocity.X = -1.0f;
                }
                else if(Input.IsKeyDown(KeyCode.Right))
                {
                    velocity.X = 1.0f;
                }
                if (Input.IsKeyDown(KeyCode.Up))
                { 
                    velocity.Y = 1.0f; 
                }
                else if (Input.IsKeyDown(KeyCode.Down)) 
                {
                    velocity.Y = -1.0f;
                }
            }
            velocity *= m_ClonemmingComponent.WalkSpeed * ts;
            m_RigidBody.ApplyLinearImpulse(velocity.XY, true);
        }

        public void SetInitialPostion(Vector3 position, float rotation = 0.0f)
        {
            m_Transform.Translation = position;
            m_Transform.Rotation = new Vector3(0.0f, 0.0f, rotation);
            m_Transform.Scale = new Vector3(1.0f);
        }

    }
}
