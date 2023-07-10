#include "Core/Assets/AssetManager.h"
#include "Core/Log/Log.h"
#include "Core/Assets/AssetImporter.h"
#include <yaml-cpp/yaml.h>

#include <fstream>

namespace Clonemmings
{
	static AssetType AssetTypeFromFileExtension(const std::filesystem::path& filename)
	{
		auto extension = filename.extension().generic_string();
		if (extension == "lvl") return AssetType::Level;
		if (extension == "png") return AssetType::Texture2D;
		if (extension == "bmp") return AssetType::Texture2D;
		return AssetType::None;
	}
	std::shared_ptr<Asset> AssetManager::GetAsset(UUID assethandle) const
	{
		if (!IsAssetHandleValid(assethandle))
			return nullptr;
		std::shared_ptr<Asset> asset;
		if (IsAssetLoaded(assethandle))
		{
			asset = m_LoadedAssets.at(assethandle);
		}
		else
		{
			const AssetMetaData metadata = GetMetaData(assethandle);
			asset = AssetImporter::LoadAsset(assethandle, metadata);
			if (!asset)
			{
				LOGERROR("Failed to load asset");
				return nullptr;
			}
		}
		return asset;
	}
	void AssetManager::ImportAsset(std::filesystem::path& filename)
	{
		UUID handle;
		AssetMetaData metadata;
		metadata.FilePath = filename;
		metadata.Type = AssetTypeFromFileExtension(filename);
		std::shared_ptr<Asset> asset = AssetImporter::LoadAsset(handle, metadata);
		if (asset)
		{
			asset->m_AssetHandle = handle;
			m_LoadedAssets[handle] = asset;
			m_AssetRegistry[handle] = metadata;
			SerialiseAssetRegistry();
		}
	}
	bool AssetManager::IsAssetHandleValid(UUID assethandle) const
	{
		if (assethandle == 0 && m_AssetRegistry.find(assethandle) == m_AssetRegistry.end())
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	bool AssetManager::IsAssetLoaded(UUID assethandle) const
	{
		return m_LoadedAssets.find(assethandle) != m_LoadedAssets.end();
	}
	const AssetMetaData& AssetManager::GetMetaData(UUID assethandle) const
	{
		AssetMetaData nulldata;
		auto it = m_AssetRegistry.find(assethandle);
		if (it == m_AssetRegistry.end())
			return nulldata;
		return it->second;
	}
	YAML::Emitter& operator<<(YAML::Emitter& out, std::string_view v)
	{
		out << std::string(v.data(), v.size());
		return out;
	}
	void AssetManager::SerialiseAssetRegistry()
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "AssetRegistry" << YAML::Value;
		out << YAML::BeginSeq;
		for (const auto& [handle, metadata] : m_AssetRegistry)
		{
			out << YAML::BeginMap;
			out << YAML::Key << "Handle" << YAML::Value << handle;
			std::string assetfilepath = metadata.FilePath.generic_string();
			out << YAML::Key << "FilePath" << YAML::Value << assetfilepath;
			out << YAML::Key << "Type" << YAML::Value << AssetTypeToString(metadata.Type);
			out << YAML::EndMap;
		}
		out << YAML::EndSeq;
		out << YAML::EndMap;
		std::ofstream fout("Asset/AssetReg.arg");
		fout << out.c_str();
	}

	bool AssetManager::DeserialiseAssetRegistry()
	{
		YAML::Node data;
		try
		{
			YAML::LoadFile("Asset/AssetReg.arg");
		}
		catch (YAML::ParserException e)
		{
			LOGERROR("Failed to load Asset registry file with error {0}", e.what());
			return false;
		}
		auto assetreg = data["AssetRegistry"];
		if (!assetreg)
		{
			LOGERROR("corrupt asset registry");
			return false;
		}
		for (const auto& asset : assetreg)
		{
			UUID assethandle = asset["Handle"].as<uint64_t>();
			auto& metadata = m_AssetRegistry[assethandle];
			metadata.FilePath = asset["FilePath"].as<std::string>();
			metadata.Type = AssetTypeFromSring(asset["Type"].as<std::string>());
		}
		return true;
	}
}