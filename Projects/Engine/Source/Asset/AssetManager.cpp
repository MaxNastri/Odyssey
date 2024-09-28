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
	void AssetManager::CreateDatabase(const Path& assetsDirectory, const Path& cacheDirectory)
	{
		s_AssetsDirectory = assetsDirectory;
		s_BinaryCache = std::make_unique<BinaryCache>(cacheDirectory);

		AssetRegistry assets(assetsDirectory / "AssetsRegistry.osettings");

		// Scan for Assets
		SearchOptions assetSearch;
		assetSearch.Root = s_AssetsDirectory;
		assetSearch.ExclusionPaths = { };
		s_AssetDatabase = std::make_unique<AssetDatabase>(assetSearch, assets);
	}

	BinaryBuffer AssetManager::LoadBinaryAsset(GUID guid)
	{
		return s_BinaryCache->LoadBinaryData(guid);
	}

	void AssetManager::WriteBinaryAsset(GUID guid, BinaryBuffer& buffer)
	{
		s_BinaryCache->SaveBinaryData(guid, buffer);
	}

	GUID AssetManager::CreateBinaryAsset(BinaryBuffer& buffer)
	{
		GUID guid = GUID::New();
		s_BinaryCache->SaveBinaryData(guid, buffer);
		return guid;
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