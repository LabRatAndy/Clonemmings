using System;
using Core;

namespace Clonemmings
{
    
    public class Clonemming : Entity
    {
        private readonly ushort DIGGABLEFLOOR = 1 << 7;
        private readonly ushort MINEABLEWALL = 1 << 6;
        private TransformComponent m_Transform;
        private RigidBody2DComponent m_RigidBody;
        private ClonemmingComponent m_ClonemmingComponent;
        private SpriteRendererComponent m_Sprite;
        private RectangleComponent m_Rectangle = null;
        private int m_Direction = 1; //-1 == left 1 == right
        private ClonemmingComponent.ClonmmingStatus m_previousStatus = ClonemmingComponent.ClonmmingStatus.Walker;
        private float fallvelocity = 0;
        private Vector2 m_CurrentLinearVelocity;
        private float m_DeadTime = 0.0f;
        private float m_Digtime = float.NaN;
        private float m_CurrentTimeDigging = 0.0f;
        private Entity m_Ledge = null;
        public float m_MaxSurvivableVelocityChange;
        void OnCreate()
        {
            m_Transform = GetComponent<TransformComponent>();
            m_RigidBody = GetComponent<RigidBody2DComponent>();
            m_ClonemmingComponent = GetComponent<ClonemmingComponent>();
            m_Sprite = GetComponent<SpriteRendererComponent>();
        }
        void OnUpdate(float ts)
        {
            InternalCalls.Native_Log("C#: on update");
            Vector2 linearvelocity = m_RigidBody.LinearVelocity;
            InternalCalls.Native_Log("C#: Linear velocity x: " + linearvelocity.X + " y: " + linearvelocity.Y);
            HighlightEntityWhenSelected();
            CheckIfDead(linearvelocity);
            if(m_ClonemmingComponent.Status == ClonemmingComponent.ClonmmingStatus.Dead)
            {
                InternalCalls.Native_Log("C#: Dead Clonemming !!");
                float newalpha = Math.Max(m_Sprite.Colour.W * 0.9f, 0.0f);
                InternalCalls.Native_Log("C#: dead new alpha value: " + newalpha);
                m_Sprite.Colour = new Vector4(0.0f, 1.0f, 0.0f, newalpha);
                InternalCalls.Native_Log("C#: dead time value: " + m_DeadTime);
                if(newalpha < 0.01f|| m_DeadTime >10.0)
                {
                    Destroy();
                }
                m_DeadTime += ts;
                return;
            }
            if(m_previousStatus != m_ClonemmingComponent.Status)
            {
                ProcessRecycleStatus();
            }
            if (m_ClonemmingComponent.Status == ClonemmingComponent.ClonmmingStatus.Walker)
            {
                ProcessWalker(linearvelocity);
            }
            if(m_ClonemmingComponent.Status == ClonemmingComponent.ClonmmingStatus.Blocker)
            {
                ProcessBlocker();
            }
            if(m_ClonemmingComponent.Status == ClonemmingComponent.ClonmmingStatus.Floater)
            {
                ProcessFloater(linearvelocity);
            }
            if(m_ClonemmingComponent.Status == ClonemmingComponent.ClonmmingStatus.Digger)
            {
                
                InternalCalls.Native_Log("C#: Processing miner");
                m_Ledge = new Entity(m_RigidBody.GetBottomContact);              
                if (m_Ledge.GetComponent<BoxCollider2DComponent>().CollisionCategory != DIGGABLEFLOOR)
                {
                    //cant dig through this floor os reset
                    InternalCalls.Native_Log("C#: can't dig the floor!");
                    m_ClonemmingComponent.Status = ClonemmingComponent.ClonmmingStatus.Walker;
                    m_previousStatus = ClonemmingComponent.ClonmmingStatus.Digger;
                }
                else
                {
                    InternalCalls.Native_Log("Floor is diggable");
                    InternalCalls.Native_Log("Digtime is: " + m_Digtime);
                    //set colour
                    m_Sprite.Colour = new Vector4(1.0f, 0.0f, 1.0f, 1.0f);
                    //get thicknessof ledge and work out howlong digging should take, digspeed should be in m/s ledge thickness is in meters
                    if (float.IsNaN(m_Digtime))
                    {
                        InternalCalls.Native_Log("C#: m_Digtime is NaN");
                        float ledgethickness = m_Ledge.GetComponent<TransformComponent>().Scale.Y;
                        if (m_ClonemmingComponent.DigSpeed > 0.0f)
                        {
                            m_Digtime = ledgethickness / m_ClonemmingComponent.DigSpeed;
                        }
                        else
                            m_Digtime = ledgethickness / 1.0f;
                        InternalCalls.Native_Log("C#: ledge thickness: " + ledgethickness + " m_Digtime: " + m_Digtime);
                    }
                    else
                    {
                        m_CurrentTimeDigging += ts;
                        InternalCalls.Native_Log("C#: m_currenttimedigging set: " + m_CurrentTimeDigging);
                    }
                    //compare if enough time digging has passed if so split the ledge entity
                    if (m_CurrentTimeDigging >= m_Digtime)
                    {
                        InternalCalls.Native_Log("C#: digging is finised");
                        Vector3 position = m_Transform.Translation;
                        float width = m_Transform.Scale.X + 0.1f;
                        InternalCalls.Native_Log("C#: Spliting entity: " + m_Ledge.ID + " at: " + position.ToString() + "gap width: " + width);
                        m_Ledge.HorizontalSplit(ref position, width);
                        m_ClonemmingComponent.Status = ClonemmingComponent.ClonmmingStatus.Walker;
                    }
                    m_previousStatus = ClonemmingComponent.ClonmmingStatus.Digger;
                }
                InternalCalls.Native_Log("C#: End of miner code!");
            }

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
                if (m_Rectangle == null)
                    return;
                m_Rectangle.Translation = m_Transform.Translation;
                m_Rectangle.Rotation = m_Transform.Rotation;
                Vector3 linescale = new Vector3(m_Transform.Scale.X * 1.1f, m_Transform.Scale.Y * 1.1f, m_Transform.Scale.Z);
                m_Rectangle.Scale = linescale;
            }
        }
        private Vector3 RotateVelocity(float rotation,Vector2 velocity)
        {
            float sintheta = (float)Math.Sin(rotation);
            float costheta = (float)Math.Cos(rotation);
            float rotatedX = costheta * -sintheta * velocity.X;
            float rotatedy = sintheta * costheta * velocity.Y;
            return new Vector3(rotatedX, rotatedy, 0);
        }
        private void CheckIfDead(Vector2 linearvelocity)
        {
            //check if already dead and return if so!
            if (m_ClonemmingComponent.Status == ClonemmingComponent.ClonmmingStatus.Dead) return;
            float velocitychange = Math.Abs(m_CurrentLinearVelocity.Y) - Math.Abs(linearvelocity.Y);
            //InternalCalls.Native_Log("C#: Check if dead");
            //InternalCalls.Native_Log("C#: Dead check velocity change: " + velocitychange);
            if (Math.Abs(velocitychange) >= m_MaxSurvivableVelocityChange)
            {
                //clonemming is dead
                //InternalCalls.Native_Log("C#: Clonemming is dead");
                m_ClonemmingComponent.Status = ClonemmingComponent.ClonmmingStatus.Dead;
                InternalCalls.SetDeadClonemming(ID);
                return;
            }
            m_CurrentLinearVelocity = linearvelocity;
        }  
        private void ProcessWalker(Vector2 linearvelocity)
        {
            if (m_RigidBody.HasContactLeft)
            {
                //InternalCalls.Native_Log("C#: Left contact");
                m_Direction = 1;
            }
            if (m_RigidBody.HasContactRight)
            {
                //InternalCalls.Native_Log("C#: Right contact");
                m_Direction = -1;
            }
            if (m_RigidBody.HasContactBottom)
            {
                //InternalCalls.Native_Log("C#: Bottom Contact");
                float targetvelocity = 0;
                switch (m_Direction)
                {
                    case 1: targetvelocity = Math.Min(linearvelocity.X + 0.5f, m_ClonemmingComponent.WalkSpeed); break; //Right
                    case -1: targetvelocity = Math.Max(linearvelocity.X - 0.5f, -m_ClonemmingComponent.WalkSpeed); break; //left
                }
                //InternalCalls.Native_Log("C#: targetvelocity: " + targetvelocity);
                float velocitychange = targetvelocity - linearvelocity.X;
                //InternalCalls.Native_Log("C#: velocitychange: " + velocitychange);
                float impulse = m_RigidBody.Mass * velocitychange;
                if (m_Direction == -1)
                {
                    //InternalCalls.Native_Log("C#: apply leftward impulse of: " + impulse); ;
                    m_RigidBody.ApplyLinearImpulse(new Vector2(impulse, 0), true);
                }
                else if (m_Direction == 1)
                {
                    //InternalCalls.Native_Log("C#: apply rightward impulse of: " + impulse);
                    m_RigidBody.ApplyLinearImpulse(new Vector2(impulse, 0), true);
                }
            }
            m_previousStatus = ClonemmingComponent.ClonmmingStatus.Walker;
            //InternalCalls.Native_Log("C#: passed contact checking");
        }
        private void ProcessBlocker()
        {
            InternalCalls.Native_Log("C#: Blocker");
            if(m_previousStatus == ClonemmingComponent.ClonmmingStatus.Blocker)
            {
                //don't need to do the rest if has already been done so skip!
                return;
            }
            m_RigidBody.Type = RigidBody2DComponent.BodyType.Static;
            m_Sprite.Colour = new Vector4(1.0f, 1.0f, 0.0f, 1.0f);
            m_previousStatus = ClonemmingComponent.ClonmmingStatus.Blocker;
        }
        private void ProcessFloater(Vector2 linearvelocity)
        {
            InternalCalls.Native_Log("C#: Floater");
            if (m_RigidBody.HasContactBottom)
            {
                InternalCalls.Native_Log("C#: floater touched down!");
                m_ClonemmingComponent.Status = ClonemmingComponent.ClonmmingStatus.Walker;
            }
            else
            {
                InternalCalls.Native_Log("C#: still floating");
                m_Sprite.Colour = new Vector4(0.0f, 1.0f, 1.0f, 1.0f);
                if (fallvelocity == 0)
                {
                    fallvelocity = linearvelocity.Y;
                }
                float targetvelociy = fallvelocity / 2.0f;
                float velocitychange = targetvelociy - linearvelocity.Y;
                float impulse = m_RigidBody.Mass * velocitychange;
                InternalCalls.Native_Log("C#: applying floater impulse of: " + impulse);
                m_RigidBody.ApplyLinearImpulse(new Vector2(0.0f, Math.Abs(impulse)), true);
            }
            m_previousStatus = ClonemmingComponent.ClonmmingStatus.Floater;
        }
        private void ProcessRecycleStatus()
        {
            switch (m_previousStatus)
            {
                case ClonemmingComponent.ClonmmingStatus.Blocker:
                    InternalCalls.Native_Log("C#: Recycling blocker");
                    m_RigidBody.Type = RigidBody2DComponent.BodyType.Dynamic;
                    m_Sprite.Colour = new Vector4(0, 1, 0, 1);
                    m_previousStatus = m_ClonemmingComponent.Status;
                    break;
                case ClonemmingComponent.ClonmmingStatus.Floater:
                    InternalCalls.Native_Log("C#: Recycling Floater");
                    m_Sprite.Colour = new Vector4(0, 1, 0, 1);
                    m_previousStatus = m_ClonemmingComponent.Status;
                    fallvelocity = 0.0f;
                    break;
                case ClonemmingComponent.ClonmmingStatus.Digger:
                    InternalCalls.Native_Log("C#: Recycle digger");
                    m_Sprite.Colour = new Vector4(0, 1, 0, 1);
                    m_previousStatus = m_ClonemmingComponent.Status;
                    m_Digtime = float.NaN;
                    m_CurrentTimeDigging = 0.0f;
                    m_Ledge = null;
                    break;
            }
        }
    }
}
