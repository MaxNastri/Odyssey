#include "AssetDatabase.h"

namespace Odyssey
{
	void AssetDatabase::AddAsset(const std::string& guid, const std::filesystem::path& path, const std::string& assetName, const std::string& assetType)
	{
		if (!m_GUIDToAssetPath.contains(guid))
			m_GUIDToAssetPath[guid] = path;
		if (!m_GUIDToAssetName.contains(guid))
			m_GUIDToAssetName[guid] = assetName;
		if (!m_AssetPathToGUID.contains(path))
			m_AssetPathToGUID[path] = guid;

		// No contains check since we are storing multiple guids per asset type
		m_AssetTypeToGUIDs[assetType].push_back(guid);
	}

	std::filesystem::path AssetDatabase::GUIDToAssetPath(const std::string& guid)
	{
		if (m_GUIDToAssetPath.contains(guid))
			return m_GUIDToAssetPath[guid];

		return std::filesystem::path();
	}

	std::string AssetDatabase::GUIDToAssetName(const std::string& guid)
	{
		if (m_GUIDToAssetName.contains(guid))
			return m_GUIDToAssetName[guid];

		return std::string();
	}

	std::string AssetDatabase::AssetPathToGUID(const std::filesystem::path& assetPath)
	{
		if (m_AssetPathToGUID.contains(assetPath))
			return m_AssetPathToGUID[assetPath];

		return std::string();
	}

	std::vector<std::string> AssetDatabase::GetGUIDsOfAssetType(const std::string& assetType)
	{
		if (m_AssetTypeToGUIDs.contains(assetType))
			return m_AssetTypeToGUIDs[assetType];

		return std::vector<std::string>();
	}
}