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