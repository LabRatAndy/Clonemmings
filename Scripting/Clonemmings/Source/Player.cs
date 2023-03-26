using System;
using Core;
namespace Clonemmings
{
    public class Player : Entity
    {
        private TransformComponent m_Transform;
        private RigidBody2DComponent m_RigidBody;

        public float Speed;
        public float Time = 0.0f;

        void OnCreate()
        {
            Console.WriteLine($"player.OnCreate - {ID}");
            m_Transform = GetComponent<TransformComponent>();
            m_RigidBody = GetComponent<RigidBody2DComponent>();
        }

        void OnUpdate(float ts)
        {
            Time += ts;
            float speed = Speed;
            Vector3 velocity = Vector3.Zero;
            if(Input.IsKeyDown(KeyCode.W))
            {
                velocity.Y = 1.0f;
            }
            else if(Input.IsKeyDown(KeyCode.S))
            {
                velocity.Y = -1.0f;
            }
            if(Input.IsKeyDown(KeyCode.D))
            {
                velocity.X = 1.0f;
            }
            else if(Input.IsKeyDown(KeyCode.A))
            {
                velocity.X = -1.0f;
            }

            Entity cameraentity = FindEntityByName("Camera");
            if(cameraentity != null) 
            {
                Camera camera = cameraentity.As<Camera>();
                if(Input.IsKeyDown(KeyCode.Q))
                {
                    camera.DistanceFromPlayer += speed * Time * ts; 
                }
                else if( Input.IsKeyDown(KeyCode.E))
                {
                    camera.DistanceFromPlayer -= speed * Time * ts;
                }
            }
            velocity *= speed * ts;
            m_RigidBody.ApplyLinearImpulse(velocity.XY, true);
        }
    }
}