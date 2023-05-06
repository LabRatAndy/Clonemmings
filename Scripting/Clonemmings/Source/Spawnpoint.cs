using Core;
using System;
using System.Collections.Generic;

namespace Clonemmings
{
    public class ClonemmingSpawnPoint : Entity
    {
        private TransformComponent m_Transform;
        private ClonemmingStartComponent m_Component;
        private List<Clonemming> m_Clonemmings = null;
        private float m_Time = 0.0f;
        
        void OnCreate()
        {
            m_Transform = GetComponent<TransformComponent>();
            m_Component = GetComponent<ClonemmingStartComponent>();
            m_Clonemmings = new List<Clonemming>(m_Component.NumberOfClonemmings);
        }

        void OnUpdate(float ts)
        {
            m_Time += ts;
            if (m_Component.NumberOfClonemmings == 0)
            {
                return;
            }
            float clonemmingspersecond = 60.0f / m_Component.ClonemmingReleaseRate;
            if(m_Time > clonemmingspersecond) 
            {
                InternalCalls.Native_Log("C#: New clonemming to be created. spawnpoint.cs line 32");
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
                clonemming.SetUpPhysics();
                GC.KeepAlive(clonemming);
            }
        }
        public void RemoveClonemming(Clonemming clonemming)
        {
            m_Clonemmings.Remove(clonemming);
        }
    }

}
