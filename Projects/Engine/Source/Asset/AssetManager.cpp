#include "AssetManager.h"
#include "Mesh.h"
#include "Material.h"
#include "Shader.h"
#include "Scene.h"
#include "Texture2D.h"
#include "SourceShader.h"
#include "AssetSerializer.h"

namespace Odyssey
{
	void AssetManager::CreateDatabase(const Path& assetsDirectory, const Path& cacheDirectory)
	{
		s_AssetsDirectory = assetsDirectory;
		s_BinaryCache = BinaryCache(cacheDirectory);

		// Scan for Assets
		AssetDatabase::SearchOptions assetSearch;
		assetSearch.Root = s_AssetsDirectory;
		assetSearch.ExclusionPaths = { };
		assetSearch.Extensions = { s_AssetExtension };
		s_AssetDatabase.ScanForAssets(assetSearch);

		// Scan for Source Assets
		ScanForSourceAssets();
	}

	AssetHandle<SourceShader> AssetManager::CreateSourceShader(const Path& sourcePath)
	{
		size_t id = s_SourceAssets.Add<SourceShader>(sourcePath);
		std::shared_ptr<SourceShader> shader = s_SourceAssets.Get<SourceShader>(id);

		// Set asset data and serialize the metafile
		shader->SetName("Default");
		shader->SetType("Shader");
		shader->SerializeMetadata();

		return AssetHandle<SourceShader>(id, shader.get());
	}

	AssetHandle<Material> AssetManager::CreateMaterial(const Path& assetPath)
	{
		// Push back an empty material
		size_t id = s_Assets.Add<Material>(assetPath);
		std::shared_ptr<Material> material = s_Assets.Get<Material>(id);

		// Set asset data
		material->SetName("Default");
		material->SetType("Material");

		// Save to disk
		material->Save();

		return AssetHandle<Material>(id, material.get());
	}

	AssetHandle<Mesh> AssetManager::CreateMesh()
	{
		// Push back an empty mesh
		size_t id = s_Assets.Add<Mesh>();
		std::shared_ptr<Mesh> mesh = s_Assets.Get<Mesh>(id);

		// Set asset data
		mesh->SetName("Default");
		mesh->SetType("Mesh");

		return AssetHandle<Mesh>(id, mesh.get());
	}

	AssetHandle<Mesh> AssetManager::CreateMesh(const Path& assetPath)
	{
		// Push back an empty mesh
		size_t id = s_Assets.Add<Mesh>(assetPath);
		std::shared_ptr<Mesh> mesh = s_Assets.Get<Mesh>(id);

		// Set asset data
		mesh->SetName("Default");
		mesh->SetType("Mesh");

		// Save to disk
		mesh->Save();

		return AssetHandle<Mesh>(id, mesh.get());
	}

	AssetHandle<Shader> AssetManager::CreateShader(const Path& assetPath)
	{
		// Push back an empty mesh
		size_t id = s_Assets.Add<Shader>(assetPath);
		std::shared_ptr<Shader> shader = s_Assets.Get<Shader>(id);

		// Set asset data
		shader->SetName("Default");
		shader->SetType("Shader");

		// Save to disk
		shader->Save();

		return AssetHandle<Shader>(id, shader.get());
	}

	AssetHandle<SourceShader> AssetManager::LoadSourceShader(GUID guid)
	{
		if (s_LoadedSourceAssets.contains(guid))
		{
			uint64_t id = s_LoadedSourceAssets[guid];
			std::shared_ptr<SourceShader> shader = s_SourceAssets.Get<SourceShader>(id);
			return AssetHandle<SourceShader>(id, shader.get());
		}

		Path sourcePath = s_SourceAssetDatabase.GUIDToAssetPath(guid);
		size_t id = s_SourceAssets.Add<SourceShader>(sourcePath);
		std::shared_ptr<SourceShader> shader = s_SourceAssets.Get<SourceShader>(id);

		// Track the source asset as loaded
		s_LoadedSourceAssets[guid] = id;

		shader->SetGUID(guid);
		shader->SetName(s_SourceAssetDatabase.GUIDToAssetName(guid));
		shader->SetType(s_SourceAssetDatabase.GUIDToAssetType(guid));

		return AssetHandle<SourceShader>(id, shader.get());
	}

	AssetHandle<Mesh> AssetManager::LoadMesh(const Path& assetPath)
	{
		// Push back a mesh loaded from the asset path
		size_t id = s_Assets.Add<Mesh>(assetPath);
		std::shared_ptr<Mesh> mesh = s_Assets.Get<Mesh>(id);

		// Track the asset
		s_LoadedAssets[mesh->GetGUID()] = id;

		return AssetHandle<Mesh>(id, mesh.get());
	}

	AssetHandle<Shader> AssetManager::LoadShader(const Path& assetPath)
	{
		// Push back a shader loaded from the asset path
		size_t id = s_Assets.Add<Shader>(assetPath);
		std::shared_ptr<Shader> shader = s_Assets.Get<Shader>(id);

		// Track the asset
		s_LoadedAssets[shader->GetGUID()] = id;

		return AssetHandle<Shader>(id, shader.get());
	}

	AssetHandle<Texture2D> AssetManager::LoadTexture2D(const Path& assetPath)
	{
		// Push back a scene loaded from the asset path
		size_t id = s_Assets.Add<Texture2D>(assetPath);
		std::shared_ptr<Texture2D> texture = s_Assets.Get<Texture2D>(id);

		// Track the asset
		s_LoadedAssets[texture->GetGUID()] = id;

		return AssetHandle<Texture2D>(id, texture.get());
	}

	AssetHandle<Material> AssetManager::LoadMaterial(const Path& assetPath)
	{
		size_t id = s_Assets.Add<Material>(assetPath);
		std::shared_ptr<Material> material = s_Assets.Get<Material>(id);
		s_LoadedAssets[material->GetGUID()] = id;

		return AssetHandle<Material>(id, material.get());
	}

	AssetHandle<Shader> AssetManager::LoadShaderByGUID(GUID guid)
	{
		// Check if we have already loaded this asset
		if (s_LoadedAssets.find(guid) != s_LoadedAssets.end())
		{
			// Return a handle
			uint64_t id = s_LoadedAssets[guid];
			std::shared_ptr<Shader> shader = s_Assets.Get<Shader>(id);
			return AssetHandle<Shader>(id, shader.get());
		}

		// Load it and return a handle
		Path path = s_AssetDatabase.GUIDToAssetPath(guid);
		return AssetManager::LoadShader(path.generic_string());
	}

	AssetHandle<Texture2D> AssetManager::LoadTexture2DByGUID(GUID guid)
	{
		// Check if we have already loaded this asset
		if (s_LoadedAssets.find(guid) != s_LoadedAssets.end())
		{
			// Return a handle
			uint64_t id = s_LoadedAssets[guid];
			std::shared_ptr<Texture2D> texture = s_Assets.Get<Texture2D>(id);
			return AssetHandle<Texture2D>(id, texture.get());
		}

		// Load it and return a handle
		Path path = s_AssetDatabase.GUIDToAssetPath(guid);
		return AssetManager::LoadTexture2D(path.string());
	}

	BinaryBuffer AssetManager::LoadBinaryAsset(GUID guid)
	{
		return s_BinaryCache.LoadBinaryData(guid);
	}

	GUID AssetManager::CreateBinaryAsset(BinaryBuffer& buffer)
	{
		GUID guid;
		s_BinaryCache.SaveBinaryData(guid, buffer);
		return guid;
	}

	std::vector<GUID> AssetManager::GetAssetsOfType(const std::string& assetType)
	{
		std::vector<GUID> assets = s_AssetDatabase.GetGUIDsOfAssetType(assetType);
		
		if (assets.empty())
			assets = s_SourceAssetDatabase.GetGUIDsOfAssetType(assetType);
		
		return assets;
	}

	GUID AssetManager::PathToGUID(const Path& path)
	{
		// Start with the asset database
		if (s_AssetDatabase.Contains(path))
			return s_AssetDatabase.AssetPathToGUID(path);

		//  Maybe its in the source asset database
		if (s_SourceAssetDatabase.Contains(path))
			return s_SourceAssetDatabase.AssetPathToGUID(path);

		return GUID::Empty();
	}

	std::string AssetManager::GUIDToName(GUID guid)
	{
		// Start with the asset database
		if (s_AssetDatabase.Contains(guid))
			return s_AssetDatabase.GUIDToAssetName(guid);

		//  Maybe its in the source asset database
		if (s_SourceAssetDatabase.Contains(guid))
			return s_SourceAssetDatabase.GUIDToAssetName(guid);

		return std::string();
	}

	std::string AssetManager::GUIDToAssetType(GUID guid)
	{
		// Start with the asset database
		if (s_AssetDatabase.Contains(guid))
			return s_AssetDatabase.GUIDToAssetType(guid);

		//  Maybe its in the source asset database
		if (s_SourceAssetDatabase.Contains(guid))
			return s_SourceAssetDatabase.GUIDToAssetType(guid);

		return std::string();
	}

	AssetHandle<Material> AssetManager::LoadMaterialByGUID(GUID guid)
	{
		// Check if we have already loaded this asset
		if (s_LoadedAssets.find(guid) != s_LoadedAssets.end())
		{
			// Return a handle
			uint64_t id = s_LoadedAssets[guid];
			std::shared_ptr<Material> material = s_Assets.Get<Material>(id);
			return AssetHandle<Material>(id, material.get());
		}

		// Load it and return a handle
		Path path = s_AssetDatabase.GUIDToAssetPath(guid);
		return AssetManager::LoadMaterial(path.generic_string());
	}

	AssetHandle<Mesh> AssetManager::LoadMeshByGUID(GUID guid)
	{
		// Check if we have already loaded this asset
		if (s_LoadedAssets.find(guid) != s_LoadedAssets.end())
		{
			// Return a handle
			uint64_t id = s_LoadedAssets[guid];
			std::shared_ptr<Mesh> mesh = s_Assets.Get<Mesh>(id);
			return AssetHandle<Mesh>(id, mesh.get());
		}

		// Load it and return a handle
		Path path = s_AssetDatabase.GUIDToAssetPath(guid);
		return AssetManager::LoadMesh(path);
	}

	void AssetManager::ScanForSourceAssets()
	{
		for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(s_AssetsDirectory))
		{
			auto assetPath = dirEntry.path();
			auto extension = assetPath.extension().string();
			auto metaPath = assetPath;
			metaPath = metaPath.replace_extension(s_MetaFileExtension);

			// Check if the file extension is a valid source asset
			// And the source asset does not have a metafile
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
							s_SourceAssetDatabase.AddAsset(sourceAsset.GetGUID(), sourceAsset.GetPath(), sourceAsset.GetName(), sourceAsset.GetType());
						}
					}
				}
				else
				{
					std::string name = assetPath.filename().replace_extension("").string();
					std::string type = s_SourceAssetExtensionsToType[extension];

					// Create a temporary source asset so we can serialize the metadata
					SourceAsset asset(assetPath);
					asset.SetMetadata(GUID(), name, type);
					asset.SerializeMetadata();

					// Add the source asset to the database
					s_SourceAssetDatabase.AddAsset(asset.GetGUID(), assetPath, name, type);
				}
			}
		}
	}
}