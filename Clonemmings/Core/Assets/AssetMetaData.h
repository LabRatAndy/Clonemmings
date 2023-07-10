#pragma once
#include "Core/Assets/Asset.h"
#include <filesystem>

namespace Clonemmings
{
	struct AssetMetaData
	{
		AssetType Type = AssetType::None;
		std::filesystem::path FilePath;

	};
}