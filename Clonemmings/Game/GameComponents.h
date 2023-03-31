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