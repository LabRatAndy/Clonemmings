#pragma once
#include <cinttypes>
namespace Clonemmings
{
	struct  ClonemmingComponent
	{
		enum class ClonemingStatus
		{
			Walker = 0,
			Floater,
			Blocker,
			Digger,
			Miner,
			Dead
		};

		ClonemingStatus Status = ClonemingStatus::Walker;
		float WalkSpeed = 5.0f;
		float DigSpeed = 1.0f;
		float MineSpeed = 1.0f;

		ClonemmingComponent() = default;
		ClonemmingComponent(const ClonemmingComponent&) = default;
	};

	struct ClonemmingStartComponent
	{
		uint32_t NumberOfClonemmings = 0;
		uint32_t ClonemmingReleaseRate = 0;

		ClonemmingStartComponent() = default;
		ClonemmingStartComponent(const ClonemmingStartComponent&) = default;
	};

	struct ClonemmingExitComponent
	{
		uint32_t NumberOfClonemmings = 0;

		ClonemmingExitComponent() = default;
		ClonemmingExitComponent(const ClonemmingExitComponent&) = default;
	};
}