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
            Log("C#: on update");
            Vector2 linearvelocity = m_RigidBody.LinearVelocity;
            Log("C# OnUpdate: Linear velocity x: " + linearvelocity.X + " y: " + linearvelocity.Y);
            HighlightEntityWhenSelected();
            CheckIfDead(linearvelocity);
            if(m_ClonemmingComponent.Status == ClonemmingComponent.ClonmmingStatus.Dead)
            {
                ProcessDead(ts);
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
                ProcessDigger(ts);                
            }
            if(m_ClonemmingComponent.Status==ClonemmingComponent.ClonmmingStatus.Miner)
            {
                ProcessMiner(linearvelocity, ts);   
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
            Log("C#: Walker");
            if (m_RigidBody.HasContactLeft)
            {
                Log("C# ProcessWalker: Left contact");
                m_Direction = 1;
            }
            if (m_RigidBody.HasContactRight)
            {
                Log("C# ProcessWalker: Right contact");
                m_Direction = -1;
            }
            if (m_RigidBody.HasContactBottom)
            {
                Log("C# ProcessWalker: Bottom Contact");
                float targetvelocity = 0;
                switch (m_Direction)
                {
                    case 1: targetvelocity = Math.Min(linearvelocity.X + 0.5f, m_ClonemmingComponent.WalkSpeed); break; //Right
                    case -1: targetvelocity = Math.Max(linearvelocity.X - 0.5f, -m_ClonemmingComponent.WalkSpeed); break; //left
                }
                Log("C# ProcessWalker: targetvelocity: " + targetvelocity);
                float velocitychange = targetvelocity - linearvelocity.X;
                Log("C# ProcessWalker: velocitychange: " + velocitychange);
                float impulse = m_RigidBody.Mass * velocitychange;
                if (m_Direction == -1)
                {
                    Log("C# ProcessWalker: apply leftward impulse of: " + impulse); ;
                    m_RigidBody.ApplyLinearImpulse(new Vector2(impulse, 0), true);
                }
                else if (m_Direction == 1)
                {
                    Log("C# ProcessWalker: apply rightward impulse of: " + impulse);
                    m_RigidBody.ApplyLinearImpulse(new Vector2(impulse, 0), true);
                }
            }
            m_previousStatus = ClonemmingComponent.ClonmmingStatus.Walker;
            Log("C# ProcessWalker: passed contact checking");
        }
        private void ProcessBlocker()
        {
            Log("C#: Blocker");
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
            Log("C# ProcessFloater: Floater");
            if (m_RigidBody.HasContactBottom)
            {
                Log("C# ProcessFloater: floater touched down!");
                m_ClonemmingComponent.Status = ClonemmingComponent.ClonmmingStatus.Walker;
            }
            else
            {
                Log("C# ProcessFloater: still floating");
                m_Sprite.Colour = new Vector4(0.0f, 1.0f, 1.0f, 1.0f);
                if (fallvelocity == 0)
                {
                    fallvelocity = linearvelocity.Y;
                }
                float targetvelociy = fallvelocity / 2.0f;
                float velocitychange = targetvelociy - linearvelocity.Y;
                float impulse = m_RigidBody.Mass * velocitychange;
                Log("C# ProcessFloater: applying floater impulse of: " + impulse);
                m_RigidBody.ApplyLinearImpulse(new Vector2(0.0f, Math.Abs(impulse)), true);
            }
            m_previousStatus = ClonemmingComponent.ClonmmingStatus.Floater;
        }
        private void ProcessDigger(float ts)
        {
            Log("C# ProcessDigger: Processing Digger");
            m_Ledge = new Entity(m_RigidBody.GetBottomContact);
            if ((m_Ledge.GetComponent<BoxCollider2DComponent>().CollisionCategory & DIGGABLEFLOOR) != DIGGABLEFLOOR)
            {
                //cant dig through this floor os reset
                Log("C# ProcessDigger: can't dig the floor!");
                Log("C# Process Digger: Collision Category is: " + m_Ledge.GetComponent<BoxCollider2DComponent>().CollisionCategory);
                m_ClonemmingComponent.Status = ClonemmingComponent.ClonmmingStatus.Walker;
                m_previousStatus = ClonemmingComponent.ClonmmingStatus.Digger;
            }
            else
            {
                Log("C# ProcessDigger:Floor is diggable");
                Log("C# ProcessDigger: Digtime is: " + m_Digtime);
                //set colour
                m_Sprite.Colour = new Vector4(1.0f, 0.0f, 1.0f, 1.0f);
                //get thicknessof ledge and work out howlong digging should take, digspeed should be in m/s ledge thickness is in meters
                if (float.IsNaN(m_Digtime))
                {
                    Log("C# ProcessDigger: m_Digtime is NaN");
                    float ledgethickness = m_Ledge.GetComponent<TransformComponent>().Scale.Y;
                    if (m_ClonemmingComponent.DigSpeed > 0.0f)
                    {
                        m_Digtime = ledgethickness / m_ClonemmingComponent.DigSpeed;
                    }
                    else
                        m_Digtime = ledgethickness / 1.0f;
                    Log("C# ProcessDigger: ledge thickness: " + ledgethickness + " m_Digtime: " + m_Digtime);
                }
                else
                {
                    m_CurrentTimeDigging += ts;
                    Log("C# ProcessDigger: m_currenttimedigging set: " + m_CurrentTimeDigging);
                }
                //compare if enough time digging has passed if so split the ledge entity
                if (m_CurrentTimeDigging >= m_Digtime)
                {
                    Log("C# ProcessDigger: digging is finised");
                    Vector3 position = m_Transform.Translation;
                    float width = m_Transform.Scale.X + 0.1f;
                    Log("C# ProcessDigger: Spliting entity: " + m_Ledge.ID + " at: " + position.ToString() + "gap width: " + width);
                    m_Ledge.HorizontalSplit(ref position, width);
                    m_ClonemmingComponent.Status = ClonemmingComponent.ClonmmingStatus.Walker;
                }
                m_previousStatus = ClonemmingComponent.ClonmmingStatus.Digger;
            }
            Log("C# ProcessDigger: End of digger code!");
        }
        void ProcessMiner(Vector2 linearvelocity, float ts)
        {
            Log("C# ProcessMiner: Processing Miner");
            m_Ledge = null;
            //are we touching a wall yet
            if (m_RigidBody.HasContactRight)
            {
                m_Ledge = new Entity(m_RigidBody.GetRightContact);
            }
            if (m_RigidBody.HasContactLeft)
            {
                m_Ledge = new Entity(m_RigidBody.GetLeftContact);
            }
            if (m_Ledge == null)
            {
                Log("C# ProcessMiner: not contacting any walls");
                ProcessWalker(linearvelocity);
            }
            else if ((m_Ledge.GetComponent<BoxCollider2DComponent>().CollisionCategory & MINEABLEWALL) == MINEABLEWALL) 
            {
                //we are touching a mineable wall
                Log("C# ProcessMiner: wall is mineable");
                Log("C# ProcessMiner: Minetime is: " + m_Digtime);
                //set colour
                m_Sprite.Colour = new Vector4(1.0f, 0.0f, 1.0f, 1.0f);
                //get thickness of ledge and work out howlong digging should take, mine speed should be in m/s wall thickness is in meters
                if (float.IsNaN(m_Digtime))
                {
                    Log("C# ProcessMiner: m_Minetime is NaN");
                    float wallthickness = m_Ledge.GetComponent<TransformComponent>().Scale.X;
                    if (m_ClonemmingComponent.DigSpeed > 0.0f)
                    {
                        m_Digtime = wallthickness / m_ClonemmingComponent.MineSpeed;
                    }
                    else
                        m_Digtime = wallthickness / 1.0f;
                    Log("C# ProcessMiner: ledge thickness: " + wallthickness + " m_Digtime: " + m_Digtime);
                }
                else
                {
                    m_CurrentTimeDigging += ts;
                    Log("C# ProcessMiner: m_currenttimedigging set: " + m_CurrentTimeDigging);
                }
                //compare if enough time digging has passed if so split the ledge entity
                if (m_CurrentTimeDigging >= m_Digtime)
                {
                    Log("C# ProcessMiner: mining is finised");
                    Vector3 position = m_Transform.Translation;
                    float width = m_Transform.Scale.Y + 0.1f;
                    Log("C# ProcessMiner: Spliting entity: " + m_Ledge.ID + " at: " + position.ToString() + "gap width: " + width);
                    m_Ledge.VerticalSplit(ref position, width);
                    m_ClonemmingComponent.Status = ClonemmingComponent.ClonmmingStatus.Walker;
                }
            }
            else
            {
                //we are not touching a mineable wall carry on walking until we hit a wall
                Log("C# ProcessMiner: wall is not minable");
                ProcessWalker(linearvelocity);
            }
            m_previousStatus = ClonemmingComponent.ClonmmingStatus.Miner;
        }
        private void ProcessDead(float ts)
        {
            Log("C# ProcessDead: Dead Clonemming !!");
            float newalpha = Math.Max(m_Sprite.Colour.W * 0.9f, 0.0f);
            Log("C# ProcessDead: dead new alpha value: " + newalpha);
            m_Sprite.Colour = new Vector4(0.0f, 1.0f, 0.0f, newalpha);
            Log("C# ProcessDead: dead time value: " + m_DeadTime);
            if (newalpha < 0.01f || m_DeadTime > 10.0)
            {
                Destroy();
            }
            m_DeadTime += ts;
        }
        private void ProcessRecycleStatus()
        {
            switch (m_previousStatus)
            {
                case ClonemmingComponent.ClonmmingStatus.Blocker:
                    Log("C# ProcessRecycleStatus: Recycling blocker");
                    m_RigidBody.Type = RigidBody2DComponent.BodyType.Dynamic;
                    m_Sprite.Colour = new Vector4(0, 1, 0, 1);
                    m_previousStatus = m_ClonemmingComponent.Status;
                    break;
                case ClonemmingComponent.ClonmmingStatus.Floater:
                    Log("C# ProcessRecycleStatus: Recycling Floater");
                    m_Sprite.Colour = new Vector4(0, 1, 0, 1);
                    m_previousStatus = m_ClonemmingComponent.Status;
                    fallvelocity = 0.0f;
                    break;
                case ClonemmingComponent.ClonmmingStatus.Digger:
                    Log("C# ProcessRecycleStatus: Recycle digger");
                    m_Sprite.Colour = new Vector4(0, 1, 0, 1);
                    m_previousStatus = m_ClonemmingComponent.Status;
                    m_Digtime = float.NaN;
                    m_CurrentTimeDigging = 0.0f;
                    m_Ledge = null;
                    break;
                case ClonemmingComponent.ClonmmingStatus.Miner:
                    Log("C# ProcessRecycleStatus: Recycle miner");
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
