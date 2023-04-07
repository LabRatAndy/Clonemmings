using Core;
using System;

namespace Clonemmings
{
    public class ClonemmingExitPoint : Entity
    {
        private ClonemmingExitComponent m_Component;
        private TransformComponent m_Transform;

        public void OnCreate()
        {
            Console.WriteLine($"creating clonemming exit point ID: {ID}");
            m_Component = GetComponent<ClonemmingExitComponent>();
            m_Transform = GetComponent<TransformComponent>();
        }
        public void OnUpdate(float ts)
        {

        }
    }
}