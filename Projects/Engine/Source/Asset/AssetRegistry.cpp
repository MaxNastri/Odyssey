#include "AssetRegistry.h"

namespace Odyssey
{
	AssetRegistry::AssetRegistry(const Path& registryPath)
	{
		RegistryPath = registryPath;
		RootDirectory = RegistryPath.parent_path();
		Load();

		FolderTracker::Options options;
		options.Extensions = { };
		options.Recursive = true;
		options.Callback = [this](const Path& oldPath, const Path& newPath, FileActionType fileAction) { OnFileAction(oldPath, newPath, fileAction); };
		m_TrackingID = FileManager::Get().TrackFolder(RootDirectory, options);
	}

	AssetRegistry::~AssetRegistry()
	{
		FileManager::Get().UntrackFolder(m_TrackingID);
	}

	void AssetRegistry::AddAsset(const std::string& name, const std::string& type, const Path& path, GUID guid)
	{
		// Make sure this isn't a dupe
		for (auto& entry : Entries)
		{
			if (entry.Guid == guid)
				return;
		}

		AssetEntry& entry = Entries.emplace_back();
		entry.Name = name;
		entry.Type = type;
		entry.Path = path;
		entry.Guid = guid;
	}

	void AssetRegistry::UpdateAssetName(GUID guid, const std::string& name)
	{
		for (AssetEntry& entry : Entries)
		{
			if (entry.Guid == guid)
			{
				entry.Name = name;
				Save();
				break;
			}
		}
	}

	void AssetRegistry::UpdateAssetPath(GUID guid, const Path& path)
	{
		for (AssetEntry& entry : Entries)
		{
			if (entry.Guid == guid)
			{
				entry.Path = path;
				Save();
				break;
			}
		}
	}

	void AssetRegistry::UpdateAssetPath(const Path& oldPath, const Path& newPath)
	{
		for (AssetEntry& entry : Entries)
		{
			if (entry.Path == oldPath)
			{
				entry.Path = newPath;
				Save();
				break;
			}
		}
	}

	void AssetRegistry::Save()
	{
		SaveTo(RegistryPath);
	}

	void AssetRegistry::SaveTo(const Path& path)
	{
		AssetSerializer serializer;
		SerializationNode root = serializer.GetRoot();

		SerializationNode entriesNode = root.CreateSequenceNode("Entries");

		for (auto& entry : Entries)
		{
			SerializationNode entryNode = entriesNode.AppendChild();
			entryNode.SetMap();

			std::string entryPath = entry.Path.string();
			std::replace(entryPath.begin(), entryPath.end(), '\\', '/');
			entryNode.WriteData("Name", entry.Name);
			entryNode.WriteData("Type", entry.Type);
			entryNode.WriteData("Path", entryPath);
			entryNode.WriteData("GUID", entry.Guid.CRef());
		}

		serializer.WriteToDisk(path);
	}

	void AssetRegistry::Load()
	{
		Entries.clear();

		AssetDeserializer deserializer(RegistryPath);
		if (deserializer.IsValid())
		{
			SerializationNode root = deserializer.GetRoot();

			SerializationNode entriesNode = root.GetNode("Entries");
			assert(entriesNode.IsSequence());
			assert(entriesNode.HasChildren());

			for (size_t i = 0; i < entriesNode.ChildCount(); ++i)
			{
				SerializationNode entryNode = entriesNode.GetChild(i);
				assert(entryNode.IsMap());

				AssetEntry& entry = Entries.emplace_back();

				std::string path;
				entryNode.ReadData("Name", entry.Name);
				entryNode.ReadData("Type", entry.Type);
				entryNode.ReadData("Path", path);
				entryNode.ReadData("GUID", entry.Guid.Ref());
				entry.Path = path;
			}

			PruneEntries();
		}
	}

	void AssetRegistry::PruneEntries()
	{
		size_t removed = 0;

		for (int64_t i = (int64_t)Entries.size() - 1; i >= 0; i--)
		{
			AssetEntry& entry = Entries[i];

			if (!std::filesystem::exists(RootDirectory / entry.Path))
			{
				Entries.erase(Entries.begin() + i);
				removed++;
				continue;
			}
		}

		if (removed > 0)
			Save();
	}

	void AssetRegistry::OnFileAction(const Path& oldFilename, const Path& newFilename, FileActionType fileAction)
	{
		if (fileAction == FileActionType::Moved && !oldFilename.empty() && !newFilename.empty())
		{
			UpdateAssetPath(oldFilename, newFilename);
		}
	}
}