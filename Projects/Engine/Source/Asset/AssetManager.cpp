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
#include "AssetRegistry.h"
#include "Project.h"

namespace Odyssey
{
	void AssetManager::CreateDatabase(Settings settings)
	{
		s_AssetsDirectory = settings.AssetsDirectory;

		std::vector<AssetRegistry> registries;

		for (const Path& registry : settings.AdditionalRegistries)
		{
			registries.push_back(AssetRegistry(registry));
		}

		// Scan for Assets
		SearchOptions assetSearch;
		assetSearch.Root = s_AssetsDirectory;
		assetSearch.ExclusionPaths = { };
		assetSearch.Extensions = settings.AssetExtensions;
		assetSearch.SourceExtensionsMap = settings.SourceAssetExtensionMap;

		s_AssetDatabase = std::make_unique<AssetDatabase>(assetSearch, Project::GetActiveAssetRegistry(), registries);
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

	void AssetManager::UpdateAssetName(GUID guid, const std::string& name)
	{
		s_AssetDatabase->UpdateAssetName(guid, name);
	}

	void AssetManager::UpdateAssetPath(GUID guid, const Path& path)
	{
		s_AssetDatabase->UpdateAssetPath(guid, path);
	}

	bool AssetManager::IsSourceAsset(const Path& path)
	{
		return s_AssetDatabase->IsSourceAsset(path);
	}
}