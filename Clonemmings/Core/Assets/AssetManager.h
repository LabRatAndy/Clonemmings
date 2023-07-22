#pragma once
#include "Core/Assets/Asset.h"
#include "Core/Assets/AssetMetaData.h"

#include <map>
#include <memory>

namespace Clonemmings
{
	class AssetManager
	{
	public:
		std::shared_ptr<Asset> GetAsset(UUID assethandle) const;
		template<typename T>
		std::shared_ptr<T> GetAssetAs(UUID assethandle) const
		{
			std::shared_ptr<Asset> asset = GetAsset(assethandle);
			return std::static_pointer_cast<T>(asset);
		}
		void ImportAsset(const std::filesystem::path& filename);
		bool IsAssetHandleValid(UUID assethandle) const;
		bool IsAssetLoaded(UUID assethandle) const;
		const AssetMetaData& GetMetaData(UUID assethandle) const;
		const std::map<UUID, AssetMetaData>& GetAssetRegistry() const { return m_AssetRegistry; }
		void SerialiseAssetRegistry();
		bool DeserialiseAssetRegistry();
	private:

		std::map<UUID, AssetMetaData> m_AssetRegistry;
		std::map<UUID, std::shared_ptr<Asset>> m_LoadedAssets;

	};
}