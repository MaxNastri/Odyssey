#pragma once
#include "Enums.h"
#include "AssetDatabase.h"
#include "BinaryCache.h"
#include "GUID.h"
#include "AssetRegistry.h"
#include "AssetList.h"
#include "Material.h"

namespace Odyssey
{
	class Asset;
	class AnimationRig;
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
		struct Settings
		{
			Path AssetsDirectory;
			std::vector<Path> AdditionalRegistries;
			std::vector<std::string> AssetExtensions;
			std::map<std::string, std::string> SourceAssetExtensionMap;
		};

	public:
		static void CreateDatabase(Settings settings);

	public:
		template<typename T, typename... Args>
		static Ref<T> CreateAsset(const Path& assetPath, Args&&... params)
		{
			static_assert(std::is_base_of<Asset, T>::value, "T is not a dervied class of Asset.");

			// Create a new material asset
			GUID guid = GUID::New();
			std::string name = assetPath.filename().replace_extension("").string();

			s_LoadedAssets.insert(guid);

			Ref<Asset> asset = s_Assets.emplace_back(new T(assetPath, std::forward<Args>(params)...));
			s_GUIDToIndex[guid] = s_Assets.size() - 1;

			// Set asset data
			asset->m_GUID = guid;
			asset->SetName(name);
			asset->m_Type = T::Type;

			// Save to disk
			if (!assetPath.empty())
				asset->Save();

			s_AssetDatabase->AddAsset(guid, assetPath, name, T::Type, false);

			return asset.As<T>();
		}

		template<typename T>
		static Ref<T> LoadSourceAsset(GUID guid)
		{
			static_assert(std::is_base_of<SourceAsset, T>::value, "T is not a dervied class of SourceAsset.");

			Path sourcePath = s_AssetDatabase->GUIDToAssetPath(guid);
			if (sourcePath.empty())
				return nullptr;

			Ref<T> sourceAsset = new T(sourcePath);

			// Set the metadata for the source asset
			const auto& metadata = s_AssetDatabase->GetMetadata(guid);
			sourceAsset->SetMetadata(guid, metadata.AssetName, metadata.AssetType);

			return sourceAsset;
		}

		template<typename T>
		static Ref<T> LoadAsset(GUID guid)
		{
			if (s_LoadedAssets.contains(guid))
				return s_Assets[s_GUIDToIndex[guid]].As<T>();

			// Convert the guid to a path
			Path assetPath = s_AssetDatabase->GUIDToAssetPath(guid);

			s_LoadedAssets.insert(guid);

			Ref<Asset>& asset = s_Assets.emplace_back(new T(assetPath));
			s_GUIDToIndex[guid] = s_Assets.size() - 1;

			// Load and return the asset
			return asset.As<T>();
		}

		template<typename T>
		static Ref<T> LoadAsset(const Path& assetPath)
		{
			// Convert the path to a guid and load the asset
			return LoadAsset<T>(s_AssetDatabase->AssetPathToGUID(assetPath));
		}

		template<typename T>
		static Ref<T> LoadInstance(const Path& assetPath)
		{
			return Ref<T>(new T(assetPath));
		}

		template<typename T>
		static Ref<T> LoadInstance(GUID guid)
		{
			// Convert the path to a guid and load the asset
			return LoadInstance<T>(s_AssetDatabase->GUIDToAssetPath(guid));
		}

	public:
		static std::vector<GUID> GetAssetsOfType(const std::string& assetType);

	public:
		static GUID PathToGUID(const Path& path);
		static std::string GUIDToName(GUID guid);
		static std::string GUIDToAssetType(GUID guid);

	public:
		static void UpdateAssetName(GUID guid, const std::string& name);
		static void UpdateAssetPath(GUID guid, const Path& path);

	public:
		static bool IsSourceAsset(const Path& path);

	private: // Assets
		inline static Path s_AssetsDirectory;
		inline static std::unique_ptr<AssetDatabase> s_AssetDatabase;

	private:
		inline static std::set<GUID> s_LoadedAssets;
		inline static std::vector<Ref<Asset>> s_Assets;
		inline static std::map<GUID, size_t> s_GUIDToIndex;
	};
}