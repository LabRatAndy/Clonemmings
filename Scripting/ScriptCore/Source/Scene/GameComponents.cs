using Core;
namespace Clonemmings
{
    public class ClonemmingComponent : Component
    {
        public enum ClonmmingStatus
        {
            Walker = 0,
            Floater,
            Blocker,
            Digger,
            Miner,
            Dead,
        }

        public ClonmmingStatus Status
        {
            get => InternalCalls.ClonemmingComponent_GetStatus(Entity.ID);
            set => InternalCalls.ClonemmingComponent_SetStatus(Entity.ID, value);
        }
        public float WalkSpeed
        {
            get => InternalCalls.ClonemmingComponent_GetWalkSpeed(Entity.ID);
            set => InternalCalls.ClonemmingComponent_SetWalkSpeed(Entity.ID, value);
        }
        public float DigSpeed
        {
            get => InternalCalls.ClonemmingComponent_GetDigSpeed(Entity.ID);
            set => InternalCalls.ClonemmingComponent_SetDigSpeed(Entity.ID, value);
        }
        public float MineSpeed
        {
            get => InternalCalls.ClonemmingComponent_GetMineSpeed(Entity.ID);
            set => InternalCalls.ClonemmingComponent_SetMineSpeed(Entity.ID, value);
        }
    }

    public class ClonemmingStartComponent : Component
    {
        public int NumberOfClonemmings
        {
            get => InternalCalls.ClonemmingStartComponent_GetNumberOfClonemmings(Entity.ID);
            set => InternalCalls.ClonemmingStartComponent_SetNumberOfClonemmings(Entity.ID, value);
        }
        public int ClonemmingReleaseRate
        {
            get => InternalCalls.ClonemmingStartComponent_GetClonemmingReleaseRate(Entity.ID);
            set => InternalCalls.ClonemmingStartComponent_SetClonemmingReleaseRate(Entity.ID, value);
        }
    }

    public class ClonemmingExitComponent : Component
    {
        public int NumberOfClonemmings
        {
            get => InternalCalls.ClonemmingExitComponent_GetNumberOfClonemmings(Entity.ID);
            set => InternalCalls.ClonemmingExitComponent_SetNumberOfClonemmings(Entity.ID, value);
        }
    }

}