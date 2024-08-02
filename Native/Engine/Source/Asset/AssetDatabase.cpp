#include "AssetDatabase.h"
#include "AssetSerializer.h"

namespace Odyssey
{
	void AssetDatabase::AddAsset(GUID guid, const std::filesystem::path& path, const std::string& assetName, const std::string& assetType)
	{
		if (!m_GUIDToMetadata.contains(guid))
			m_GUIDToMetadata[guid] = AssetMetadata(path, assetName, assetType);

		if (!m_AssetPathToGUID.contains(path))
			m_AssetPathToGUID[path] = guid;

		// No contains check since we are storing multiple guids per asset type
		m_AssetTypeToGUIDs[assetType].push_back(guid);
	}

	void AssetDatabase::ScanForAssets(SearchOptions& searchOptions)
	{
		for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(searchOptions.Root))
		{
			bool excluded = false;

			if (!searchOptions.ExclusionPaths.empty())
			{
				// Check if this path matches anything in the exclusion list
				for (const auto& path : searchOptions.ExclusionPaths)
				{
					auto pathCompare = std::filesystem::relative(dirEntry, path);
					if (!pathCompare.empty() && pathCompare.native()[0] != '.')
					{
						excluded = true;
						break;
					}
				}
			}

			// Skip if this path should be excluded
			if (excluded)
				continue;

			auto path = dirEntry.path();
			auto extension = path.extension();

			if (dirEntry.is_regular_file())
			{
				for (const auto& searchExt : searchOptions.Extensions)
				{
					// Check if this is a valid extension
					if (extension == searchExt)
					{
						AssetDeserializer deserializer(path);
						if (deserializer.IsValid())
						{
							SerializationNode root = deserializer.GetRoot();

							// Deserialize the asset's metadata
							GUID guid;
							std::string type, name;
							root.ReadData("m_GUID", guid.Ref());
							root.ReadData("m_Type", type);
							root.ReadData("m_Name", name);
							AddAsset(guid, path, name, type);
						}
					}
				}
			}
		}
	}

	bool AssetDatabase::Contains(GUID& guid)
	{
		return m_GUIDToMetadata.contains(guid);
	}

	bool AssetDatabase::Contains(const std::filesystem::path& path)
	{
		return m_AssetPathToGUID.contains(path);
	}

	std::filesystem::path AssetDatabase::GUIDToAssetPath(GUID guid)
	{
		if (m_GUIDToMetadata.contains(guid))
			return m_GUIDToMetadata[guid].AssetPath;

		return std::filesystem::path();
	}

	std::string AssetDatabase::GUIDToAssetName(GUID guid)
	{
		if (m_GUIDToMetadata.contains(guid))
			return m_GUIDToMetadata[guid].AssetName;

		return std::string();
	}

	std::string AssetDatabase::GUIDToAssetType(GUID guid)
	{
		if (m_GUIDToMetadata.contains(guid))
			return m_GUIDToMetadata[guid].AssetType;

		return std::string();
	}

	GUID AssetDatabase::AssetPathToGUID(const std::filesystem::path& assetPath)
	{
		if (m_AssetPathToGUID.contains(assetPath))
			return m_AssetPathToGUID[assetPath];

		return GUID::Empty();
	}

	std::vector<GUID> AssetDatabase::GetGUIDsOfAssetType(const std::string& assetType)
	{
		if (m_AssetTypeToGUIDs.contains(assetType))
			return m_AssetTypeToGUIDs[assetType];

		return std::vector<GUID>();
	}
}