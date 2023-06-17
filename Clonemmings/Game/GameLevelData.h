#pragma once
#include <cinttypes>

namespace Clonemmings
{
	struct GameLevelData
	{
		uint32_t MaxNumFloaters = 10;
		uint32_t MaxNumBlockers = 10;
		uint32_t MaxNumMiners = 10;
		uint32_t MaxNumDiggers = 10;
		uint32_t NumFloatersUsed = 0;
		uint32_t NumBlockersUsed = 0;
		uint32_t NumDiggersUsed = 0;
		uint32_t NumMinersUsed = 0;
		bool CanRecycleFloaters = false;
		bool CanRecycleBlockers = false;
		bool CanRecycleMiners = false;
		bool CanRecycleDiggers = false;
		float PercentageToSave = 100.0f;
		uint32_t ClonemmingsSaved = 0;
		uint32_t StartClonemmingCount = 0;
		uint32_t DeadClonemmings = 0;
		float DeadVelocityChange = 0;

		GameLevelData() = default;
		GameLevelData(const GameLevelData&) = default;

		bool UseFloater()
		{
			if (NumFloatersUsed < MaxNumFloaters)
			{
				NumFloatersUsed++;
				return true;
			}
			return false;
		}
		bool UseBlocker()
		{
			if (NumBlockersUsed < MaxNumBlockers)
			{
				NumBlockersUsed++;
				return true;
			}
			return false;
		}
		bool UseMiner()
		{
			if (NumMinersUsed < MaxNumMiners)
			{
				NumMinersUsed++;
				return true;
			}
			return false;
		}
		bool UseDigger()
		{
			if (NumDiggersUsed < MaxNumDiggers)
			{
				NumDiggersUsed++;
				return true;
			}
			return false;
		}
		bool RecycleFloater()
		{
			if (CanRecycleFloaters)
			{
				if (NumFloatersUsed >= 1)
				{
					NumFloatersUsed--;
					return true;
				}
			}
			return false;
		}
		bool RecycleBlocker()
		{
			if (CanRecycleBlockers)
			{
				if (NumBlockersUsed >= 1)
				{
					NumBlockersUsed--;
					return true;
				}
			}
			return false;
		}
		bool RecycleMiner()
		{
			if (CanRecycleMiners)
			{
				if (NumMinersUsed >= 1)
				{
					NumMinersUsed--;
					return true;
				}
			}
			return false;
		}
		bool RecycleDigger()
		{
			if (CanRecycleDiggers)
			{
				if (NumDiggersUsed >= 1)
				{
					NumDiggersUsed--;
					return true;
				}
			}
			return false;
		}
	};
}
