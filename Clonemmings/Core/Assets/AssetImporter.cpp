#include "core/Assets/AssetImporter.h"
#include "Core/Log/Log.h"
#include <map>
#include <functional>
namespace Clonemmings
{
	using AssetImportFunction = std::function<std::shared_ptr<Asset>(UUID, const AssetMetaData&)>;
	static std::map<AssetType, AssetImportFunction> s_AssetImportFunctions = { { AssetType::Texture2D,TextureImporter::ImportTexture }, {AssetType::Level, LevelImporter::ImportLevel} };

	std::shared_ptr<Asset> AssetImporter::LoadAsset(UUID assethandle, const AssetMetaData& metadata)
	{
		if (s_AssetImportFunctions.find(metadata.Type) == s_AssetImportFunctions.end())
		{
			LOGERROR("No load or import function available for AssetType {0}", AssetTypeToString(metadata.Type));
			return nullptr;
		}
		return s_AssetImportFunctions.at(metadata.Type)(assethandle, metadata);
	}
}