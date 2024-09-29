#include "AssetManager.h"
#include "Mesh.h"
#include "Material.h"
#include "Shader.h"
#include "Scene.h"
#include "Texture2D.h"
#include "Cubemap.h"
#include "SourceShader.h"
#include "SourceModel.h"
#include "SourceTexture.h"
#include "AnimationRig.h"
#include "AnimationClip.h"
#include "AssetRegistry.hpp"

namespace Odyssey
{
	void AssetManager::CreateDatabase(const Path& assetsDirectory, const Path& projectRegistryPath, std::vector<Path>& additionalRegistries)
	{
		s_AssetsDirectory = assetsDirectory;

		std::vector<AssetRegistry> registries;
		AssetRegistry projectRegistry(projectRegistryPath);

		for (const Path& registry : additionalRegistries)
		{
			registries.push_back(AssetRegistry(registry));
		}

		// Scan for Assets
		SearchOptions assetSearch;
		assetSearch.Root = s_AssetsDirectory;
		assetSearch.ExclusionPaths = { };

		s_AssetDatabase = std::make_unique<AssetDatabase>(assetSearch, projectRegistry, registries);
	}

	std::vector<GUID> AssetManager::GetAssetsOfType(const std::string& assetType)
	{
		return s_AssetDatabase->GetGUIDsOfAssetType(assetType);
	}

	GUID AssetManager::PathToGUID(const Path& path)
	{
		// Start with the asset database
		if (s_AssetDatabase->Contains(path))
			return s_AssetDatabase->AssetPathToGUID(path);

		return GUID::Empty();
	}

	std::string AssetManager::GUIDToName(GUID guid)
	{
		// Start with the asset database
		if (s_AssetDatabase->Contains(guid))
			return s_AssetDatabase->GUIDToAssetName(guid);

		return std::string();
	}

	std::string AssetManager::GUIDToAssetType(GUID guid)
	{
		// Start with the asset database
		if (s_AssetDatabase->Contains(guid))
			return s_AssetDatabase->GUIDToAssetType(guid);

		return std::string();
	}

	bool AssetManager::IsSourceAsset(const Path& path)
	{
		return s_AssetDatabase->IsSourceAsset(path);
	}
}