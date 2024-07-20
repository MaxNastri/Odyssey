#include "AssetManager.h"
#include "Mesh.h"
#include "Material.h"
#include "Shader.h"
#include "Scene.h"
#include "Texture2D.h"
#include "AssetSerializer.h"

namespace Odyssey
{
	void AssetManager::CreateDatabase()
	{
		// Scan for Assets
		for (const auto& dirEntry : std::filesystem::recursive_directory_iterator("Assets"))
		{
			auto assetPath = dirEntry.path();
			auto extension = assetPath.extension();

			if (dirEntry.is_regular_file() &&
				(extension == s_AssetExtension || extension == s_SceneExtension))
			{
				AssetDeserializer deserializer(assetPath);
				if (deserializer.IsValid())
				{
					SerializationNode root = deserializer.GetRoot();

					std::string guid, type, name;
					root.ReadData("m_GUID", guid);
					root.ReadData("m_Type", type);
					root.ReadData("m_Name", name);
					s_AssetDatabase.AddAsset(guid, assetPath, name, type);
				}
			}
		}

		// Scan for Source Assets
		for (const auto& dirEntry : std::filesystem::recursive_directory_iterator("Source"))
		{
			auto assetPath = dirEntry.path();
			std::string extension = assetPath.extension().string();

			if (dirEntry.is_regular_file() && s_SourceAssetExtensionsToType.contains(extension))
			{
				// Convert the filename into a string
				std::string guid = GenerateGUID();
				std::string name = assetPath.filename().replace_extension("").string();
				std::string type = s_SourceAssetExtensionsToType[extension];
				s_SourceAssetDatabase.AddAsset(guid, assetPath, name, type);
			}
		}

		s_DefaultVertexShader = LoadShader(s_DefaultVertexShaderPath);
		s_DefaultFragmentShader = LoadShader(s_DefaultFragmentShaderPath);
	}

	AssetHandle<Mesh> AssetManager::CreateMesh()
	{
		// Push back an empty mesh
		size_t id = s_Assets.Add<Mesh>();
		std::shared_ptr<Mesh> mesh = s_Assets.Get<Mesh>(id);

		// Set asset data
		mesh->SetGUID(GenerateGUID());
		mesh->SetName("Default");
		mesh->SetType("Mesh");

		return AssetHandle<Mesh>(id, mesh.get());
	}

	AssetHandle<Material> AssetManager::CreateMaterial(const std::filesystem::path& assetPath)
	{
		// Push back an empty material
		size_t id = s_Assets.Add<Material>(assetPath);
		std::shared_ptr<Material > material = s_Assets.Get<Material>(id);

		// Set asset data
		material->SetGUID(GenerateGUID());
		material->SetName("Default");
		material->SetType("Material");

		// Assign default shaders
		material->SetVertexShader(s_DefaultVertexShader);
		material->SetFragmentShader(s_DefaultFragmentShader);

		// Save to disk
		material->Save();

		return AssetHandle<Material>(id, material.get());
	}

	AssetHandle<Mesh> AssetManager::CreateMesh(const std::filesystem::path& assetPath)
	{
		// Push back an empty mesh
		size_t id = s_Assets.Add<Mesh>(assetPath);
		std::shared_ptr<Mesh> mesh = s_Assets.Get<Mesh>(id);

		// Set asset data
		mesh->SetGUID(GenerateGUID());
		mesh->SetName("Default");
		mesh->SetType("Mesh");

		// Save to disk
		mesh->Save();

		return AssetHandle<Mesh>(id, mesh.get());
	}

	AssetHandle<Shader> AssetManager::CreateShader(const std::filesystem::path& assetPath)
	{
		// Push back an empty mesh
		size_t id = s_Assets.Add<Shader>(assetPath);
		std::shared_ptr<Shader> shader = s_Assets.Get<Shader>(id);

		// Set asset data
		shader->SetGUID(GenerateGUID());
		shader->SetName("Default");
		shader->SetType("Shader");

		// Save to disk
		shader->Save();

		return AssetHandle<Shader>(id, shader.get());
	}

	AssetHandle<Scene> AssetManager::CreateScene(const std::filesystem::path& assetPath)
	{
		// Push back an empty mesh
		size_t id = s_Assets.Add<Scene>(assetPath);
		std::shared_ptr<Scene> scene = s_Assets.Get<Scene>(id);

		// Set asset data
		scene->SetGUID(GenerateGUID());
		scene->SetName("Scene");
		scene->SetType("Scene");

		// Save to disk
		scene->Save();

		return AssetHandle<Scene>(id, scene.get());
	}

	AssetHandle<Mesh> Odyssey::AssetManager::LoadMesh(const std::filesystem::path& assetPath)
	{
		// Push back a mesh loaded from the asset path
		size_t id = s_Assets.Add<Mesh>(assetPath);
		std::shared_ptr<Mesh> mesh = s_Assets.Get<Mesh>(id);

		// Track the asset
		s_LoadedAssets[mesh->GetGUID()] = id;

		return AssetHandle<Mesh>(id, mesh.get());
	}

	AssetHandle<Shader> AssetManager::LoadShader(const std::filesystem::path& assetPath)
	{
		// Push back a shader loaded from the asset path
		size_t id = s_Assets.Add<Shader>(assetPath);
		std::shared_ptr<Shader> shader = s_Assets.Get<Shader>(id);

		// Track the asset
		s_LoadedAssets[shader->GetGUID()] = id;

		return AssetHandle<Shader>(id, shader.get());
	}

	AssetHandle<Scene> AssetManager::LoadScene(const std::filesystem::path& assetPath)
	{
		// Push back a scene loaded from the asset path
		size_t id = s_Assets.Add<Scene>(assetPath);
		std::shared_ptr<Scene> scene = s_Assets.Get<Scene>(id);

		// Track the asset
		s_LoadedAssets[scene->GetGUID()] = id;

		return AssetHandle<Scene>(id, scene.get());
	}

	AssetHandle<Texture2D> AssetManager::LoadTexture2D(const std::filesystem::path& assetPath)
	{
		// Push back a scene loaded from the asset path
		size_t id = s_Assets.Add<Texture2D>(assetPath);
		std::shared_ptr<Texture2D> texture = s_Assets.Get<Texture2D>(id);

		// Track the asset
		s_LoadedAssets[texture->GetGUID()] = id;

		return AssetHandle<Texture2D>(id, texture.get());
	}

	AssetHandle<Material> AssetManager::LoadMaterial(const std::filesystem::path& assetPath)
	{
		size_t id = s_Assets.Add<Material>(assetPath);
		std::shared_ptr<Material> material = s_Assets.Get<Material>(id);
		s_LoadedAssets[material->GetGUID()] = id;

		return AssetHandle<Material>(id, material.get());
	}

	AssetHandle<Shader> AssetManager::LoadShaderByGUID(const std::string& guid)
	{
		// Check if we have already loaded this asset
		if (s_LoadedAssets.find(guid) != s_LoadedAssets.end())
		{
			// Return a handle
			uint32_t id = s_LoadedAssets[guid];
			std::shared_ptr<Shader> shader = s_Assets.Get<Shader>(id);
			return AssetHandle<Shader>(id, shader.get());
		}

		// Load it and return a handle
		std::filesystem::path path = s_AssetDatabase.GUIDToAssetPath(guid);
		return AssetManager::LoadShader(path.generic_string());
	}

	AssetHandle<Texture2D> AssetManager::LoadTexture2DByGUID(const std::string& guid)
	{
		// Check if we have already loaded this asset
		if (s_LoadedAssets.find(guid) != s_LoadedAssets.end())
		{
			// Return a handle
			uint32_t id = s_LoadedAssets[guid];
			std::shared_ptr<Texture2D> texture = s_Assets.Get<Texture2D>(id);
			return AssetHandle<Texture2D>(id, texture.get());
		}

		// Load it and return a handle
		std::filesystem::path path = s_AssetDatabase.GUIDToAssetPath(guid);
		return AssetManager::LoadTexture2D(path.string());
	}

	std::vector<std::string> AssetManager::GetAssetsOfType(const std::string& type)
	{
		return s_AssetDatabase.GetGUIDsOfAssetType(type);
	}

	void AssetManager::UnloadScene(AssetHandle<Scene> scene)
	{
		const std::string& guid = scene.Get()->GetGUID();
		if (s_LoadedAssets.find(guid) != s_LoadedAssets.end())
		{
			s_LoadedAssets.erase(guid);
		}
		s_Assets.Remove(scene.m_ID);
		scene.Reset();
	}

	std::string AssetManager::PathToGUID(const std::filesystem::path& path)
	{
		return s_AssetDatabase.AssetPathToGUID(path);
	}

	std::string AssetManager::GUIDToName(const std::string& guid)
	{
		return s_AssetDatabase.GUIDToAssetName(guid);
	}

	std::string AssetManager::GUIDToAssetType(const std::string& guid)
	{
		return s_AssetDatabase.GUIDToAssetType(guid);
	}

	AssetHandle<Material> AssetManager::LoadMaterialByGUID(const std::string& guid)
	{
		// Check if we have already loaded this asset
		if (s_LoadedAssets.find(guid) != s_LoadedAssets.end())
		{
			// Return a handle
			uint32_t id = s_LoadedAssets[guid];
			std::shared_ptr<Material> material = s_Assets.Get<Material>(id);
			return AssetHandle<Material>(id, material.get());
		}

		// Load it and return a handle
		std::filesystem::path path = s_AssetDatabase.GUIDToAssetPath(guid);
		return AssetManager::LoadMaterial(path.generic_string());
	}

	AssetHandle<Mesh> AssetManager::LoadMeshByGUID(const std::string& guid)
	{
		// Check if we have already loaded this asset
		if (s_LoadedAssets.find(guid) != s_LoadedAssets.end())
		{
			// Return a handle
			uint32_t id = s_LoadedAssets[guid];
			std::shared_ptr<Mesh> mesh = s_Assets.Get<Mesh>(id);
			return AssetHandle<Mesh>(id, mesh.get());
		}

		// Load it and return a handle
		std::filesystem::path path = s_AssetDatabase.GUIDToAssetPath(guid);
		return AssetManager::LoadMesh(path.generic_string());
	}

	std::string AssetManager::GenerateGUID()
	{
		std::string guid = s_GUIDGenerator.getUUID().str();
		while (s_AssetDatabase.Contains(guid) || s_SourceAssetDatabase.Contains(guid))
		{
			guid = s_GUIDGenerator.getUUID().str();
		}
		return guid;
	}
}