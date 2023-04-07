using System;
using Core;

namespace Clonemmings
{
    public class Clonemming : Entity
    {
        private TransformComponent m_Transform;
        private RigidBody2DComponent m_RigidBody;
        private ClonemmingComponent m_ClonemmingComponent;
        public void OnCreate()
        {
            Console.WriteLine($"Creating Cloneming id: {ID}");
            m_Transform = GetComponent<TransformComponent>();
            m_RigidBody = GetComponent<RigidBody2DComponent>();
            m_ClonemmingComponent = GetComponent<ClonemmingComponent>();
        }
        public void OnUpdate(float ts)
        {

        }
    }
}
