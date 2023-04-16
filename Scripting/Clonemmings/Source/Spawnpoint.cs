using Core;
using System;
namespace Clonemmings
{
    public class ClonemmingSpawnPoint : Entity
    {
        private TransformComponent m_Transform;
        private ClonemmingStartComponent m_Component;

        private float m_Time = 0.0f;
        void OnCreate()
        {
            m_Transform = GetComponent<TransformComponent>();
            m_Component = GetComponent<ClonemmingStartComponent>();
        }

        void OnUpdate(float ts)
        {
            m_Time += ts;
            if (m_Component.NumberOfClonemmings == 0)
            {
                return;
            }
            float clonemmingspersecond = m_Component.ClonemmingReleaseRate * 60.0f;
            if(m_Time > clonemmingspersecond) 
            {
                CreateNewClonemming(m_Component.NumberOfClonemmings.ToString());
                m_Component.NumberOfClonemmings--;
                m_Time = 0.0f;
            }
        }

        private new void CreateNewClonemming(string name)
        {
            Entity entity = Entity.CreateNewClonemming(name);
            Clonemming clonemming = entity.As<Clonemming>();
            if(clonemming != null) 
            {
                clonemming.SetInitialPostion(new Vector3(-8.0f, 8.0f, 0.0f));
            }
        }
    }

}
