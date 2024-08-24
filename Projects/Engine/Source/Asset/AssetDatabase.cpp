#include "AssetDatabase.h"
#include "AssetSerializer.h"
#include "Project.h"
#include "Asset.h"

namespace Odyssey
{
	AssetDatabase::AssetDatabase(SearchOptions& searchOptions)
	{
		m_SearchOptions = searchOptions;

		// Create a file tracker based on the search options so we can detect any file changes
		TrackingOptions options;
		options.Direrctory = searchOptions.Root;
		options.Extensions = searchOptions.Extensions;
		options.Recursive = true;
		options.Callback = [this](const Path& path, FileActionType fileAction) { OnFileAction(path, fileAction); };
		m_FileTracker = std::make_unique<FileTracker>(options);

		Scan();
	}

	void AssetDatabase::Scan()
	{
		if (m_SearchOptions.SourceAssetsOnly)
			ScanForSourceAssets();
		else
			ScanForAssets();
	}

	void AssetDatabase::ScanForAssets()
	{
		// Clear the existing lookups
		m_GUIDToMetadata.clear();
		m_AssetPathToGUID.clear();
		m_AssetPathToGUID.clear();

		for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(m_SearchOptions.Root))
		{
			bool excluded = false;

			if (!m_SearchOptions.ExclusionPaths.empty())
			{
				// Check if this path matches anything in the exclusion list
				for (const auto& path : m_SearchOptions.ExclusionPaths)
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
				for (const auto& searchExt : m_SearchOptions.Extensions)
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

	void AssetDatabase::ScanForSourceAssets()
	{
		// Clear the existing lookups
		m_GUIDToMetadata.clear();
		m_AssetPathToGUID.clear();
		m_AssetPathToGUID.clear();

		for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(m_SearchOptions.Root))
		{
			auto assetPath = dirEntry.path();
			auto extension = assetPath.extension().string();
			auto metaPath = assetPath;
			metaPath = metaPath.replace_extension(s_MetaFileExtension);

			// Check if the file extension is a valid source asset
			if (dirEntry.is_regular_file() && s_SourceAssetExtensionsToType.contains(extension))
			{
				if (std::filesystem::exists(metaPath))
				{
					// Deserialize the meta file
					// Add the asset to the database with the source file extension
					if (AssetDeserializer deserializer = AssetDeserializer(metaPath))
					{
						SourceAsset sourceAsset = SourceAsset::CreateFromMetafile(metaPath);

						if (sourceAsset.HasMetadata())
						{
							AddAsset(sourceAsset.GetGUID(), sourceAsset.GetPath(), sourceAsset.GetName(), sourceAsset.GetType());
						}
					}
				}
				else
				{
					std::string name = assetPath.filename().replace_extension("").string();
					std::string type = s_SourceAssetExtensionsToType[extension];

					// Create a temporary source asset so we can serialize the metadata
					SourceAsset asset(assetPath);
					asset.SetMetadata(GUID::New(), name, type);
					asset.SerializeMetadata();

					// Add the source asset to the database
					AddAsset(asset.GetGUID(), assetPath, name, type);
				}
			}
		}
	}

	void AssetDatabase::AddAsset(GUID guid, const Path& path, const std::string& assetName, const std::string& assetType)
	{
		if (!m_GUIDToMetadata.contains(guid))
			m_GUIDToMetadata[guid] = AssetMetadata(path, assetName, assetType);

		if (!m_AssetPathToGUID.contains(path))
			m_AssetPathToGUID[path] = guid;

		// No contains check since we are storing multiple guids per asset type
		m_AssetTypeToGUIDs[assetType].push_back(guid);
	}

	bool AssetDatabase::Contains(GUID& guid)
	{
		return m_GUIDToMetadata.contains(guid);
	}

	bool AssetDatabase::Contains(const Path& path)
	{
		return m_AssetPathToGUID.contains(path);
	}

	Path AssetDatabase::GUIDToAssetPath(GUID guid)
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

	GUID AssetDatabase::AssetPathToGUID(const Path& assetPath)
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
	
	void AssetDatabase::OnFileAction(const Path& filename, FileActionType fileAction)
	{
		if (fileAction != FileActionType::None && fileAction != FileActionType::Modified)
			Scan();
	}
}