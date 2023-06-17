using Core;
using System;
using System.Text;

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
            ulong clonemmingID = 0;
            if(InternalCalls.ClonemmingExit_HasContactWithClonemming(ID, out clonemmingID))
            {
                m_Component.NumberOfClonemmings++;
                Entity entity = new Entity(clonemmingID);
                entity.Destroy();
            }
        }
    }
}