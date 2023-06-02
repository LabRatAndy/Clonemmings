using Core;
using System;

namespace Clonemmings
{
    public class ClonemmingExitPoint : Entity
    {
        private ClonemmingExitComponent m_Component;
        private TransformComponent m_Transform;

        void OnCreate()
        {
            m_Component = GetComponent<ClonemmingExitComponent>();
            m_Transform = GetComponent<TransformComponent>();
        }
        void OnUpdate(float ts)
        {

        }
    }
}