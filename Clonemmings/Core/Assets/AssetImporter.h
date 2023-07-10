#pragma once
#include "Core/Assets/AssetMetaData.h"
#include "Core/Renderer/Texture.h"
#include "Core/Scene/Scene.h"
namespace Clonemmings
{
	class AssetImporter
	{
	public:
		static std::shared_ptr<Asset> LoadAsset(UUID assethandle, const AssetMetaData& metadata);
	};

	class TextureImporter
	{
	public:
		static std::shared_ptr<Texture> ImportTexture(UUID assethandle, const AssetMetaData& metadata);
		static std::shared_ptr<Texture> LoadTexture(const std::filesystem::path& filename);
	};
	class LevelImporter
	{
	public:
		static std::shared_ptr<Scene> ImportLevel(UUID assethandle, const AssetMetaData& metadata);
		static void LoadLevel(const std::filesystem::path& filename, std::shared_ptr<Scene> newscene);
	};
}
