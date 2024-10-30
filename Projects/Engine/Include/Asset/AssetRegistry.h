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
		AssetRegistry(const Path& registryPath);

	public:
		void AddAsset(const std::string& name, const std::string& type, const Path& path, GUID guid);
		void UpdateAssetName(GUID guid, const std::string& name);
		void UpdateAssetPath(GUID guid, const Path& path);

	public:
		void Save();
		void SaveTo(const Path& path);
		void Load();

	public:
		Path RootDirectory;
		Path RegistryPath;
		std::vector<AssetEntry> Entries;
	};
}