#pragma once
#include "GUID.h"

namespace Odyssey
{
	class AssetDatabase
	{
	public:
		struct SearchOptions
		{
			std::filesystem::path Root;
			std::vector<std::filesystem::path> ExclusionPaths;
			std::vector<std::string> Extensions;
		};
	public:
		AssetDatabase() = default;

	public:
		void AddAsset(GUID guid, const std::filesystem::path& path, const std::string& assetName, const std::string& assetType);
		void ScanForAssets(SearchOptions& searchOptions);
		void PruneUnlinkedAssets();

	public:
		bool Contains(GUID& guid);
		bool Contains(const std::filesystem::path& path);
		std::filesystem::path GUIDToAssetPath(GUID guid);
		std::string GUIDToAssetName(GUID guid);
		std::string GUIDToAssetType(GUID guid);
		GUID AssetPathToGUID(const std::filesystem::path& assetPath);
		std::vector<GUID> GetGUIDsOfAssetType(const std::string& assetType);
	
	private:
		struct AssetMetadata
		{
		public:
			std::filesystem::path AssetPath;
			std::string AssetName;
			std::string AssetType;
		};

	private:
		// [GUID, AssetMetadata]
		std::map<GUID, AssetMetadata> m_GUIDToMetadata;
		// [AssetPath, GUID]
		std::map<std::filesystem::path, GUID> m_AssetPathToGUID;
		// [AssetType, List<GUID>]
		std::map<std::string, std::vector<GUID>> m_AssetTypeToGUIDs;

	};
}