#include "core/Assets/AssetImporter.h"
#include "Core/Log/Log.h"
#include "Core/Application/Assert.h"
#include "Core/Scene/SceneSerialiser.h"
#include <stb_image.h>
#include <map>
#include <functional>
namespace Clonemmings
{
	using AssetImportFunction = std::function<std::shared_ptr<Asset>(UUID, const AssetMetaData&)>;
	static std::map<AssetType, AssetImportFunction> s_AssetImportFunctions =
	{
		{AssetType::Texture2D, TextureImporter::ImportTexture},
		{AssetType::Level, LevelImporter::ImportLevel}
	};

	std::shared_ptr<Asset> AssetImporter::LoadAsset(UUID assethandle, const AssetMetaData& metadata)
	{
		if (s_AssetImportFunctions.find(metadata.Type) == s_AssetImportFunctions.end())
		{
			LOGERROR("No load or import function available for AssetType {0}", AssetTypeToString(metadata.Type));
			return nullptr;
		}
		return s_AssetImportFunctions.at(metadata.Type)(assethandle, metadata);
	}

	std::shared_ptr<Texture> TextureImporter::ImportTexture(UUID assethandle, const AssetMetaData& metadata)
	{
		return LoadTexture(metadata.FilePath);
	}
	
	std::shared_ptr<Texture> TextureImporter::LoadTexture(const std::filesystem::path& filename)
	{
		ASSERT(std::filesystem::exists(filename), "Texture file does not exist!");
		TextureSettings settings;
		int width = 0;
		int height = 0;
		int channels = 0;
		stbi_set_flip_vertically_on_load(1);
		Buffer data;
		data.Data = stbi_load(filename.string().c_str(), &width, &height, &channels, 0);
		if (data.Data = nullptr)
		{
			LOGERROR("Failed to load texture file: {0}", filename.string());
			data.Release();
			return nullptr;
		}
		data.Size = width * height * channels;
		settings.Width = width;
		settings.Height = height;
		switch (channels)
		{
		case 3:
			settings.Format = ImageFormat::RGB8;
			break;
		case 4:
			settings.Format = ImageFormat::RGBA8;
			break;
		default:
			ASSERT(false, "Unsupported image format");
		}
		std::shared_ptr<Texture> texture = std::make_shared<Texture>(settings, data);
		data.Release();
		return texture;

	}
	std::shared_ptr<Scene> LevelImporter::ImportLevel(UUID assethandle, const AssetMetaData& metadata)
	{
		std::shared_ptr<Scene> newscene = nullptr;
		SceneSerialiser serialiser(newscene);
		serialiser.Deserialise(metadata.FilePath.generic_string());
		return newscene;
	}
}