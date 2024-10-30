#pragma once
#include "AssetList.hpp"
#include "Enums.h"
#include "AssetDatabase.h"
#include "BinaryCache.h"
#include "GUID.h"
#include "AssetRegistry.hpp"

namespace Odyssey
{
	class Asset;
	class AnimationRig;
	class Material;
	class Mesh;
	class Shader;
	class Scene;
	class Texture2D;
	class AnimationClip;
	class Cubemap;

	class SourceAsset;
	class SourceShader;
	class SourceModel;
	class SourceTexture;

	class AssetManager
	{
	public:
		static void CreateDatabase(const Path& assetsDirectory, const Path& projectRegistryPath, std::vector<Path>& additionalRegistries);

	public:
		template<typename T, typename... Args>
		static std::shared_ptr<T> CreateAsset(const Path& assetPath, Args&&... params)
		{
			static_assert(std::is_base_of<Asset, T>::value, "T is not a dervied class of Asset.");

			// Create a new material asset
			GUID guid = GUID::New();
			std::shared_ptr<T> asset = s_Assets.Add<T>(guid, assetPath, std::forward<Args>(params)...);

			// Set asset data
			asset->Guid = guid;
			asset->SetName("Default");
			asset->SetType(T::Type);

			// Save to disk
			if (!assetPath.empty())
				asset->Save();

			return asset;
		}

		template<typename T>
		static std::shared_ptr<T> LoadSourceAsset(GUID guid)
		{
			static_assert(std::is_base_of<SourceAsset, T>::value, "T is not a dervied class of SourceAsset.");

			// Check if this asset has already been loaded
			if (s_LoadedAssets.contains(guid))
				return s_SourceAssets.Get<T>(guid);

			// Load the source asset
			Path sourcePath = s_AssetDatabase->GUIDToAssetPath(guid);
			std::shared_ptr<T> sourceAsset = s_SourceAssets.Add<T>(guid, sourcePath);

			// Set the metadata for the source asset
			const auto& metadata = s_AssetDatabase->GetMetadata(guid);
			sourceAsset->SetMetadata(guid, metadata.AssetName, metadata.AssetType);

			// Track the source asset as loaded
			s_LoadedAssets.emplace(guid);

			return sourceAsset;
		}

		template<typename T>
		static std::shared_ptr<T> LoadAsset(const Path& assetPath)
		{
			// Convert the path to a guid and load the asset
			return LoadAsset<T>(s_AssetDatabase->AssetPathToGUID(assetPath));
		}

		template<typename T>
		static std::shared_ptr<T> LoadAsset(GUID guid)
		{
			// Check if the asset is already loaded
			if (s_LoadedAssets.contains(guid))
				return s_Assets.Get<T>(guid);

			// Track the asset
			s_LoadedAssets.emplace(guid);

			// Convert the guid to a path
			Path assetPath = s_AssetDatabase->GUIDToAssetPath(guid);

			// Load and return the asset
			return s_Assets.Add<T>(guid, assetPath);
		}

	public:
		static std::vector<GUID> GetAssetsOfType(const std::string& assetType);

	public:
		static GUID PathToGUID(const Path& path);
		static std::string GUIDToName(GUID guid);
		static std::string GUIDToAssetType(GUID guid);

	public:
		static bool IsSourceAsset(const Path& path);

	private: // Assets
		inline static Path s_AssetsDirectory;
		inline static std::unique_ptr<AssetDatabase> s_AssetDatabase;
		inline static AssetList s_Assets;
		inline static SourceAssetList s_SourceAssets;
		inline static std::set<GUID> s_LoadedAssets;
	};
}