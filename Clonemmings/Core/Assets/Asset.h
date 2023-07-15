#pragma once
#include "Core/Application/UUID.h"
#include <string>
namespace  Clonemmings
{
	enum class AssetType : uint16_t
	{
		None = 0,
		Texture2D,
		Level
	};

	std::string AssetTypeToString(AssetType type);
	AssetType AssetTypeFromString(const std::string& typestr);
	
	class Asset
	{
	public: 
		UUID m_AssetHandle;
		virtual AssetType GetType() const = 0;
	};
}
