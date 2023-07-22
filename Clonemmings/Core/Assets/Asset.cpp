#include "Core/Assets/Asset.h"

namespace Clonemmings
{
	std::string AssetTypeToString(AssetType type)
	{
		switch (type)
		{
		case AssetType::None: return "AssetType::None";
		case AssetType::Texture2D: return "AssetType::Texture2D";
		case AssetType::Level: return "AssetType::Level";
		}
		return "AssetTypeInvalid";
	}
	AssetType AssetTypeFromString(const std::string& typestr)
	{
		if (typestr == "AssetType::None") return AssetType::None;
		if (typestr == "AssetType::Texture2D") return AssetType::Texture2D;
		if (typestr == "AssetType::Level") return AssetType::Level;
		return AssetType::None;
	}

}