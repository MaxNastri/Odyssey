#include "AssetRegistry.h"

namespace Odyssey
{
	AssetRegistry::AssetRegistry(const Path& registryPath)
	{
		RegistryPath = registryPath;
		RootDirectory = RegistryPath.parent_path();
		Load();
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
		}
	}
}