#include "AssetDatabase.h"
#include "AssetSerializer.h"
#include "Project.h"
#include "Asset.h"

namespace Odyssey
{
	AssetDatabase::AssetDatabase(SearchOptions& searchOptions, AssetRegistry& projectRegistry, const std::vector<AssetRegistry>& additionalRegistries)
		: m_ProjectRegistry(projectRegistry)
	{
		m_SearchOptions = searchOptions;

		std::vector<Path> supportedExtensions;
		
		for (auto& extension : s_AssetExtensions)
		{
			supportedExtensions.push_back(extension);
		}

		for (auto& [extension, sourceType] : s_SourceAssetExtensionsToType)
		{
			supportedExtensions.push_back(extension);
		}

		// Create a file tracker based on the search options so we can detect any file changes
		TrackingOptions options;
		options.TrackingPath = searchOptions.Root;
		options.Extensions = supportedExtensions;
		options.Recursive = true;
		options.Callback = [this](const Path& path, FileActionType fileAction) { OnFileAction(path, fileAction); };
		m_FileTracker = std::make_unique<FileTracker>(options);

		AddRegistry(projectRegistry);

		for (const AssetRegistry& registry : additionalRegistries)
		{
			AddRegistry(registry);
		}
		Scan();
	}

	void AssetDatabase::Scan()
	{
		ScanForAssets();
		ScanForSourceAssets();
	}

	AssetRegistry AssetDatabase::CreateRegistry()
	{
		AssetRegistry registry;

		for (auto& [guid, metadata] : m_GUIDToMetadata)
		{
			Path relativePath = std::filesystem::relative(metadata.AssetPath, m_SearchOptions.Root);
			registry.AddAsset(metadata.AssetName, metadata.AssetType, relativePath, guid);
		}

		return registry;
	}

	void AssetDatabase::AddRegistry(const AssetRegistry& registry)
	{
		for (auto& entry : registry.Entries)
		{
			bool sourceAsset = !s_AssetExtensions.contains(entry.Path.extension().string());
			AddRegistryAsset(entry.Guid, registry.RootDirectory / entry.Path, entry.Path.filename().replace_extension().string(), entry.Type, sourceAsset);
		}
	}

	void AssetDatabase::ScanForAssets()
	{
		for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(m_SearchOptions.Root))
		{
			// Skip assets we are already tracking
			if (m_AssetPathToGUID.contains(dirEntry))
				continue;

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
				for (const auto& searchExt : s_AssetExtensions)
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

							AddAsset(guid, path, name, type, false);
						}
					}
				}
			}
		}
	}

	void AssetDatabase::ScanForSourceAssets()
	{
		for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(m_SearchOptions.Root))
		{
			// Skip assets we are already tracking
			if (m_AssetPathToGUID.contains(dirEntry))
				continue;

			auto assetPath = dirEntry.path();
			auto extension = assetPath.extension().string();

			// Check if the file extension is a valid source asset
			if (dirEntry.is_regular_file() && s_SourceAssetExtensionsToType.contains(extension))
			{
				std::string name = assetPath.filename().replace_extension("").string();
				std::string type = s_SourceAssetExtensionsToType[extension];

				// Add the source asset to the database
				GUID guid = GUID::New();
				AddAsset(guid, assetPath, name, type, true);
			}
		}
	}

	void AssetDatabase::AddAsset(GUID guid, const Path& path, const std::string& assetName, const std::string& assetType, bool sourceAsset)
	{
		if (!m_GUIDToMetadata.contains(guid))
			m_GUIDToMetadata[guid] = AssetMetadata(path, assetName, assetType, sourceAsset);

		if (!path.empty() && !m_AssetPathToGUID.contains(path))
			m_AssetPathToGUID[path] = guid;

		// No contains check since we are storing multiple guids per asset type
		m_AssetTypeToGUIDs[assetType].push_back(guid);

		if (guid && !path.empty() && !assetName.empty() && !assetType.empty())
		{
			Path relativePath = std::filesystem::relative(path, m_ProjectRegistry.RootDirectory);
			m_ProjectRegistry.AddAsset(assetName, assetType, relativePath, guid);
			m_ProjectRegistry.Save();
		}
	}

	bool AssetDatabase::Contains(GUID& guid)
	{
		return m_GUIDToMetadata.contains(guid);
	}

	bool AssetDatabase::Contains(const Path& path)
	{
		return m_AssetPathToGUID.contains(path);
	}

	bool AssetDatabase::IsSourceAsset(const Path& path)
	{
		if (m_AssetPathToGUID.contains(path) && m_GUIDToMetadata.contains(m_AssetPathToGUID[path]))
			return m_GUIDToMetadata[m_AssetPathToGUID[path]].IsSourceAsset;

		return false;
	}

	void AssetDatabase::UpdateAssetName(GUID guid, const std::string& name)
	{
		if (m_GUIDToMetadata.contains(guid))
		{
			m_GUIDToMetadata[guid].AssetName = name;
			m_ProjectRegistry.UpdateAssetName(guid, name);
		}
	}

	void AssetDatabase::UpdateAssetPath(GUID guid, const Path& path)
	{
		if (m_GUIDToMetadata.contains(guid))
		{
			// Cache the old path so we can remove it
			const Path& oldPath = m_GUIDToMetadata[guid].AssetPath;

			// Update the path in our lookups
			m_GUIDToMetadata[guid].AssetPath = path;
			m_AssetPathToGUID[path] = guid;

			// Remove the old path
			m_AssetPathToGUID.erase(oldPath);

			// Update the project registry
			m_ProjectRegistry.UpdateAssetPath(guid, path);
		}
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

	const AssetMetadata& AssetDatabase::GetMetadata(GUID guid)
	{
		return m_GUIDToMetadata[guid];
	}

	void AssetDatabase::AddRegistryAsset(GUID guid, const Path& path, const std::string& assetName, const std::string& assetType, bool sourceAsset)
	{
		if (!m_GUIDToMetadata.contains(guid))
			m_GUIDToMetadata[guid] = AssetMetadata(path, assetName, assetType, sourceAsset);

		if (!m_AssetPathToGUID.contains(path))
			m_AssetPathToGUID[path] = guid;

		// No contains check since we are storing multiple guids per asset type
		m_AssetTypeToGUIDs[assetType.data()].push_back(guid);
	}

	void AssetDatabase::OnFileAction(const Path& filename, FileActionType fileAction)
	{
		if (fileAction != FileActionType::None && fileAction != FileActionType::Modified)
			Scan();
	}
}