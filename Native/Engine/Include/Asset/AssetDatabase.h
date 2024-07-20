#pragma once

namespace Odyssey
{
	class AssetDatabase
	{
	public:
		AssetDatabase() = default;

	public:
		void AddAsset(const std::string& guid, const std::filesystem::path& path, const std::string& assetName, const std::string& assetType);
	public:
		bool Contains(const std::string& guid);
		bool Contains(const std::filesystem::path& path);
		std::filesystem::path GUIDToAssetPath(const std::string& guid);
		std::string GUIDToAssetName(const std::string& guid);
		std::string GUIDToAssetType(const std::string& guid);
		std::string AssetPathToGUID(const std::filesystem::path& assetPath);
		std::vector<std::string> GetGUIDsOfAssetType(const std::string& assetType);
	private:
		struct AssetMetadata
		{
		public:
			std::filesystem::path AssetPath;
			std::string AssetName;
			std::string AssetType;
		};
		// [GUID, AssetMetadata]
		std::map<std::string, AssetMetadata> m_GUIDToMetadata;
		// [AssetPath, GUID]
		std::map<std::filesystem::path, std::string> m_AssetPathToGUID;
		// [AssetType, List<GUID>]
		std::map<std::string, std::vector<std::string>> m_AssetTypeToGUIDs;
	};
}