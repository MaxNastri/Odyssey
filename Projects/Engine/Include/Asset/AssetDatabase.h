#pragma once
#include "GUID.h"
#include "FileTracker.h"
#include "AssetRegistry.hpp"

namespace Odyssey
{
	struct SearchOptions
	{
		Path Root;
		std::vector<Path> ExclusionPaths;
		std::vector<Path> Extensions;
		bool SourceAssetsOnly = false;
	};

	class AssetDatabase
	{
	public:
		AssetDatabase() = default;
		AssetDatabase(SearchOptions& searchOptions);
		AssetDatabase(SearchOptions& searchOptions, const std::vector<AssetRegistry>& registries);

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
		Path GUIDToAssetPath(GUID guid);
		std::string GUIDToAssetName(GUID guid);
		std::string GUIDToAssetType(GUID guid);
		GUID AssetPathToGUID(const Path& assetPath);
		std::vector<GUID> GetGUIDsOfAssetType(const std::string& assetType);

	private:
		void OnFileAction(const Path& filename, FileActionType fileAction);

	protected:
		struct AssetMetadata
		{
		public:
			std::filesystem::path AssetPath;
			std::string AssetName;
			std::string AssetType;
			bool IsSourceAsset = false;
		};

	protected:
		std::unique_ptr<FileTracker> m_FileTracker;
		SearchOptions m_SearchOptions;

		// [GUID, AssetMetadata]
		std::map<GUID, AssetMetadata> m_GUIDToMetadata;
		// [AssetPath, GUID]
		std::map<Path, GUID> m_AssetPathToGUID;
		// [AssetType, List<GUID>]
		std::map<std::string, std::vector<GUID>> m_AssetTypeToGUIDs;

	private:
		inline static std::string s_AssetExtension = ".asset";
		inline static std::string s_MetaFileExtension = ".meta";

		inline static std::set<std::string> s_AssetExtensions =
		{
			".asset", ".shader", ".mesh", ".scene", ".prefab",
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