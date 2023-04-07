using Core;
using System;
namespace Clonemmings
{
    public class ClonemmingSpawnPoint : Entity
    {
        private TransformComponent m_Transform;
        private ClonemmingStartComponent m_Component;

        public void OnCreate()
        {
            Console.WriteLine($"Creating spawn point ID: {ID}");
            m_Transform = GetComponent<TransformComponent>();
            m_Component = GetComponent<ClonemmingStartComponent>();
        }

        public void OnUpdate(float ts)
        {

        }
    }

}
