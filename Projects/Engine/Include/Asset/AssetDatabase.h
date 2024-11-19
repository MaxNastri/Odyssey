#pragma once
#include "GUID.h"
#include "FileTracker.h"
#include "AssetRegistry.h"

namespace Odyssey
{
	struct SearchOptions
	{
		Path Root;
		std::vector<Path> ExclusionPaths;
		std::vector<Path> Extensions;
		bool SourceAssetsOnly = false;
	};

	struct AssetMetadata
	{
	public:
		Path AssetPath;
		std::string AssetName;
		std::string AssetType;
		bool IsSourceAsset = false;
	};

	class AssetDatabase
	{
	public:
		AssetDatabase() = default;
		AssetDatabase(SearchOptions& searchOptions, AssetRegistry& projectRegistry, const std::vector<AssetRegistry>& additionalRegistries);

	public:
		void Scan();
		AssetRegistry CreateRegistry();
		void AddRegistry(const AssetRegistry& registry);

	private:
		void ScanForAssets();
		void ScanForSourceAssets();

	public:
		void AddAsset(GUID guid, const Path& path, const std::string& assetName, const std::string& assetType, bool sourceAsset);
		bool Contains(GUID& guid);
		bool Contains(const Path& path);
		bool IsSourceAsset(const Path& path);

	public:
		void UpdateAssetName(GUID guid, const std::string& name);
		void UpdateAssetPath(GUID guid, const Path& path);

	public:
		Path GUIDToAssetPath(GUID guid);
		std::string GUIDToAssetName(GUID guid);
		std::string GUIDToAssetType(GUID guid);
		GUID AssetPathToGUID(const Path& assetPath);
		std::vector<GUID> GetGUIDsOfAssetType(const std::string& assetType);
		const AssetMetadata& GetMetadata(GUID guid);

	private:
		void AddRegistryAsset(GUID guid, const Path& path, const std::string& assetName, const std::string& assetType, bool sourceAsset);
		void OnFileAction(const Path& filename, FileActionType fileAction);

	protected:
		std::unique_ptr<FileTracker> m_FileTracker;
		SearchOptions m_SearchOptions;
		AssetRegistry& m_ProjectRegistry;

		// [GUID, AssetMetadata]
		std::map<GUID, AssetMetadata> m_GUIDToMetadata;
		// [AssetPath, GUID]
		std::map<Path, GUID> m_AssetPathToGUID;
		// [AssetType, List<GUID>]
		std::map<std::string, std::vector<GUID>> m_AssetTypeToGUIDs;

	private:
		inline static std::set<std::string> s_AssetExtensions =
		{
			".asset", ".shader", ".mesh", ".prefab", ".rune"
		};

		inline static std::map<std::string, std::string> s_SourceAssetExtensionsToType =
		{
			{".glsl", "Odyssey.SourceShader"},
			{".hlsl", "Odyssey.SourceShader"},
			{".fbx", "Odyssey.SourceModel"},
			{".gltf", "Odyssey.SourceModel"},
			{".glb", "Odyssey.SourceModel"},
			{".png", "Odyssey.SourceTexture"},
			{".jpg", "Odyssey.SourceTexture"},
		};
	};
}