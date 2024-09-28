#pragma once
#include "Asset.h"
#include "AssetSerializer.h"

namespace Odyssey
{
	struct AssetEntry
	{
	public:
		std::string Name;
		std::string Type;
		Path Path;
		GUID Guid;
	};

	class AssetRegistry
	{
	public:
		AssetRegistry() = default;
		AssetRegistry(const Path& registryPath)
		{
			RegistryPath = registryPath;
			RootDirectory = RegistryPath.parent_path();
			Load();
		}

		void AddAsset(const std::string& name, const std::string& type, const Path& path, GUID guid)
		{
			AssetEntry& entry = Entries.emplace_back();
			entry.Name = name;
			entry.Type = type;
			entry.Path = path;
			entry.Guid = guid;
		}

	public:
		void Save()
		{
			SaveTo(RegistryPath);
		}

		void SaveTo(const Path& path)
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

		void Load()
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
	public:
		Path RootDirectory;
		Path RegistryPath;
		std::vector<AssetEntry> Entries;
	};
}