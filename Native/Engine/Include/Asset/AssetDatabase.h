#pragma once

namespace Odyssey
{
	struct SourceAssetDatabase
	{

	};

	class AssetDatabase
	{
	public:
		AssetDatabase() = default;

	public:
		void AddAsset(const std::string& guid, const std::filesystem::path& path, const std::string& assetName, const std::string& assetType);
	public:
		std::filesystem::path GUIDToAssetPath(const std::string& guid);
		std::string GUIDToAssetName(const std::string& guid);
		std::string AssetPathToGUID(const std::filesystem::path& assetPath);
		std::vector<std::string> GetGUIDsOfAssetType(const std::string& assetType);
	private:
		// [GUID, AssetPath]
		std::map<std::string, std::filesystem::path> m_GUIDToAssetPath;
		// [GUID, AssetName]
		std::map<std::string, std::string> m_GUIDToAssetName;
		// [AssetPath, GUID]
		std::map<std::filesystem::path, std::string> m_AssetPathToGUID;
		// [AssetType, List<GUID>]
		std::map<std::string, std::vector<std::string>> m_AssetTypeToGUIDs;
	};
}