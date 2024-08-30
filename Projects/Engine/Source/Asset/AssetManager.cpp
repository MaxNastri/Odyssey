#include "AssetManager.h"
#include "Mesh.h"
#include "Material.h"
#include "Shader.h"
#include "Scene.h"
#include "Texture2D.h"
#include "SourceShader.h"
#include "SourceModel.h"
#include "AnimationRig.h"

namespace Odyssey
{
	void AssetManager::CreateDatabase(const Path& assetsDirectory, const Path& cacheDirectory)
	{
		s_AssetsDirectory = assetsDirectory;
		s_BinaryCache = std::make_unique<BinaryCache>(cacheDirectory);

		// Scan for Assets
		SearchOptions assetSearch;
		assetSearch.Root = s_AssetsDirectory;
		assetSearch.ExclusionPaths = { };
		assetSearch.Extensions = { s_AssetExtension };
		s_AssetDatabase = std::make_unique<AssetDatabase>(assetSearch);

		// Scan for Source Assets
		SearchOptions sourceSearch;
		sourceSearch.Root = s_AssetsDirectory;
		sourceSearch.ExclusionPaths = { };
		sourceSearch.Extensions = { };
		sourceSearch.SourceAssetsOnly = true;
		s_SourceAssetDatabase = std::make_unique<AssetDatabase>(sourceSearch);
	}

	std::shared_ptr<SourceShader> AssetManager::CreateSourceShader(const Path& sourcePath)
	{
		GUID guid = GUID::New();
		std::shared_ptr<SourceShader> shader = s_SourceAssets.Add<SourceShader>(guid, sourcePath);

		// Set asset data and serialize the metafile
		shader->Guid = guid;
		shader->SetName("Default");
		shader->SetType("SourceShader");
		shader->SerializeMetadata();

		return shader;
	}

	std::shared_ptr<SourceModel> AssetManager::CreateSourceModel(const Path& sourcePath)
	{
		GUID guid = GUID::New();
		std::shared_ptr<SourceModel> model = s_SourceAssets.Add<SourceModel>(guid, sourcePath);

		// Set asset data and serialize the metafile
		model->Guid = guid;
		model->SetName("Default");
		model->SetType("SourceModel");
		model->SerializeMetadata();

		return model;
	}

	std::shared_ptr<Material> AssetManager::CreateMaterial(const Path& assetPath)
	{
		// Create a new material asset
		GUID guid = GUID::New();
		std::shared_ptr<Material> material = s_Assets.Add<Material>(guid, assetPath);

		// Set asset data
		material->Guid = guid;
		material->SetName("Default");
		material->SetType("Material");

		// Save to disk
		material->Save();

		return material;
	}

	std::shared_ptr<Mesh> AssetManager::CreateMesh()
	{
		// Create a new mesh asset
		GUID guid = GUID::New();
		std::shared_ptr<Mesh> mesh = s_Assets.Add<Mesh>(guid);

		// Set asset data
		mesh->Guid = guid;
		mesh->SetName("Default");
		mesh->SetType("Mesh");

		return mesh;
	}

	std::shared_ptr<Mesh> AssetManager::CreateMesh(const Path& assetPath)
	{
		// Create a new mesh asset
		GUID guid = GUID::New();
		std::shared_ptr<Mesh> mesh = s_Assets.Add<Mesh>(guid, assetPath);

		// Set asset data
		mesh->Guid = guid;
		mesh->SetName("Default");
		mesh->SetType("Mesh");

		// Save to disk
		mesh->Save();

		return mesh;
	}

	std::shared_ptr<Mesh> AssetManager::CreateMesh(const Path& assetPath, std::shared_ptr<SourceModel> source)
	{
		// Create a new mesh asset
		GUID guid = GUID::New();
		std::shared_ptr<Mesh> mesh = s_Assets.Add<Mesh>(guid, assetPath, source);

		// Set asset data
		mesh->Guid = guid;
		mesh->SetName("Default");
		mesh->SetType("Mesh");
		mesh->SetSourceAsset(source->GetGUID());

		// Save to disk
		mesh->Save();

		return mesh;
	}

	std::shared_ptr<Shader> AssetManager::CreateShader(const Path& assetPath)
	{
		// Create a new shader asset
		GUID guid = GUID::New();
		std::shared_ptr<Shader> shader = s_Assets.Add<Shader>(guid, assetPath);

		// Set asset data
		shader->Guid = guid;
		shader->SetName("Default");
		shader->SetType("Shader");

		// Save to disk
		shader->Save();

		return shader;
	}

	std::shared_ptr<Shader> AssetManager::CreateShader(const Path& assetPath, std::shared_ptr<SourceShader> source)
	{
		// Create a new shader asset
		GUID guid = GUID::New();
		std::shared_ptr<Shader> shader = s_Assets.Add<Shader>(guid, assetPath, source);

		// Set asset data
		shader->Guid = guid;
		shader->SetName("Default");
		shader->SetType("Shader");

		// Save to disk
		shader->Save();

		return shader;
	}

	std::shared_ptr<AnimationRig> AssetManager::CreateAnimationRig(const Path& assetPath, std::shared_ptr<SourceModel> sourceModel)
	{
		// Create a new shader asset
		GUID guid = GUID::New();
		std::shared_ptr<AnimationRig> animationRig = s_Assets.Add<AnimationRig>(guid, assetPath, sourceModel);

		// Set asset data
		animationRig->Guid = guid;
		animationRig->SetName("Default");
		animationRig->SetType("AnimationRig");

		// Save to disk
		animationRig->Save();

		return animationRig;
	}

	std::shared_ptr<SourceShader> AssetManager::LoadSourceShader(GUID guid)
	{
		// Check if this asset has already been loaded
		if (s_LoadedSourceAssets.contains(guid))
			return s_SourceAssets.Get<SourceShader>(guid);

		// Load the source asset
		Path sourcePath = s_SourceAssetDatabase->GUIDToAssetPath(guid);
		std::shared_ptr<SourceShader> shader = s_SourceAssets.Add<SourceShader>(guid, sourcePath);

		// Track the source asset as loaded
		s_LoadedSourceAssets.emplace(guid);

		return shader;
	}

	std::shared_ptr<SourceModel> AssetManager::LoadSourceModel(GUID guid)
	{
		// Check if this asset has already been loaded
		if (s_LoadedSourceAssets.contains(guid))
			return s_SourceAssets.Get<SourceModel>(guid);

		// Load the source asset
		Path sourcePath = s_SourceAssetDatabase->GUIDToAssetPath(guid);
		std::shared_ptr<SourceModel> model = s_SourceAssets.Add<SourceModel>(guid, sourcePath);

		// Track the source asset as loaded
		 s_LoadedSourceAssets.emplace(guid);

		return model;
	}

	std::shared_ptr<Mesh> AssetManager::LoadMesh(const Path& assetPath)
	{
		// Convert the path to a guid
		GUID guid = s_AssetDatabase->AssetPathToGUID(assetPath);

		// Check if the asset is already loaded
		if (s_LoadedAssets.contains(guid))
			return s_Assets.Get<Mesh>(guid);

		// Track the asset
		s_LoadedAssets.emplace(guid);

		// Load and return the mesh asset
		return s_Assets.Add<Mesh>(guid, assetPath);
	}

	std::shared_ptr<Shader> AssetManager::LoadShader(const Path& assetPath)
	{
		// Convert the path to a guid
		GUID guid = s_AssetDatabase->AssetPathToGUID(assetPath);

		// Check if the asset is already loaded
		if (s_LoadedAssets.contains(guid))
			return s_Assets.Get<Shader>(guid);

		// Track the asset
		s_LoadedAssets.emplace(guid);

		// Load and return the mesh asset
		return s_Assets.Add<Shader>(guid, assetPath);
	}

	std::shared_ptr<Texture2D> AssetManager::LoadTexture2D(const Path& assetPath)
	{
		// Convert the path to a guid
		GUID guid = s_AssetDatabase->AssetPathToGUID(assetPath);

		// Check if the asset is already loaded
		if (s_LoadedAssets.contains(guid))
			return s_Assets.Get<Texture2D>(guid);

		// Track the asset
		s_LoadedAssets.emplace(guid);

		// Load and return the mesh asset
		return s_Assets.Add<Texture2D>(guid, assetPath);
	}

	std::shared_ptr<Material> AssetManager::LoadMaterial(const Path& assetPath)
	{
		// Convert the path to a guid
		GUID guid = s_AssetDatabase->AssetPathToGUID(assetPath);

		// Check if the asset is already loaded
		if (s_LoadedAssets.contains(guid))
			return s_Assets.Get<Material>(guid);

		// Track the asset
		s_LoadedAssets.emplace(guid);

		// Load and return the mesh asset
		return s_Assets.Add<Material>(guid, assetPath);
	}


	std::shared_ptr<Material> AssetManager::LoadMaterialByGUID(GUID guid)
	{
		// Convert the guid to a path
		Path assetPath = s_AssetDatabase->GUIDToAssetPath(guid);

		// Check if the asset is already loaded
		if (s_LoadedAssets.contains(guid))
			return s_Assets.Get<Material>(guid);

		// Track the asset
		s_LoadedAssets.emplace(guid);

		// Load and return the mesh asset
		return s_Assets.Add<Material>(guid, assetPath);
	}

	std::shared_ptr<Mesh> AssetManager::LoadMeshByGUID(GUID guid)
	{
		// Convert the guid to a path
		Path assetPath = s_AssetDatabase->GUIDToAssetPath(guid);

		// Check if the asset is already loaded
		if (s_LoadedAssets.contains(guid))
			return s_Assets.Get<Mesh>(guid);

		// Track the asset
		s_LoadedAssets.emplace(guid);

		// Load and return the mesh asset
		return s_Assets.Add<Mesh>(guid, assetPath);
	}

	std::shared_ptr<Shader> AssetManager::LoadShaderByGUID(GUID guid)
	{
		// Convert the guid to a path
		Path assetPath = s_AssetDatabase->GUIDToAssetPath(guid);

		// Check if the asset is already loaded
		if (s_LoadedAssets.contains(guid))
			return s_Assets.Get<Shader>(guid);

		// Track the asset
		s_LoadedAssets.emplace(guid);

		// Load and return the mesh asset
		return s_Assets.Add<Shader>(guid, assetPath);
	}

	std::shared_ptr<Texture2D> AssetManager::LoadTexture2DByGUID(GUID guid)
	{
		// Convert the guid to a path
		Path assetPath = s_AssetDatabase->GUIDToAssetPath(guid);

		// Check if the asset is already loaded
		if (s_LoadedAssets.contains(guid))
			return s_Assets.Get<Texture2D>(guid);

		// Track the asset
		s_LoadedAssets.emplace(guid);

		// Load and return the mesh asset
		return s_Assets.Add<Texture2D>(guid, assetPath);
	}

	std::shared_ptr<AnimationRig> AssetManager::LoadAnimationRig(GUID guid)
	{
		// Convert the guid to a path
		Path assetPath = s_AssetDatabase->GUIDToAssetPath(guid);

		// Check if the asset is already loaded
		if (s_LoadedAssets.contains(guid))
			return s_Assets.Get<AnimationRig>(guid);

		// Track the asset
		s_LoadedAssets.emplace(guid);

		// Load and return the mesh asset
		return s_Assets.Add<AnimationRig>(guid, assetPath);
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
		std::vector<GUID> assets = s_AssetDatabase->GetGUIDsOfAssetType(assetType);
		
		if (assets.empty())
			assets = s_SourceAssetDatabase->GetGUIDsOfAssetType(assetType);
		
		return assets;
	}

	GUID AssetManager::PathToGUID(const Path& path)
	{
		// Start with the asset database
		if (s_AssetDatabase->Contains(path))
			return s_AssetDatabase->AssetPathToGUID(path);

		//  Maybe its in the source asset database
		if (s_SourceAssetDatabase->Contains(path))
			return s_SourceAssetDatabase->AssetPathToGUID(path);

		return GUID::Empty();
	}

	std::string AssetManager::GUIDToName(GUID guid)
	{
		// Start with the asset database
		if (s_AssetDatabase->Contains(guid))
			return s_AssetDatabase->GUIDToAssetName(guid);

		//  Maybe its in the source asset database
		if (s_SourceAssetDatabase->Contains(guid))
			return s_SourceAssetDatabase->GUIDToAssetName(guid);

		return std::string();
	}

	std::string AssetManager::GUIDToAssetType(GUID guid)
	{
		// Start with the asset database
		if (s_AssetDatabase->Contains(guid))
			return s_AssetDatabase->GUIDToAssetType(guid);

		//  Maybe its in the source asset database
		if (s_SourceAssetDatabase->Contains(guid))
			return s_SourceAssetDatabase->GUIDToAssetType(guid);

		return std::string();
	}

	bool AssetManager::IsSourceAsset(const Path& path)
	{
		return s_SourceAssetDatabase->Contains(path);
	}
}