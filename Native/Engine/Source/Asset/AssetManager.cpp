#include "AssetManager.h"
#include "Mesh.h"
#include "Material.h"
#include "Shader.h"
#include "Scene.h"
#include "Texture2D.h"
#include "ryml.hpp"

namespace Odyssey
{
	void AssetManager::CreateDatabase()
	{
		for (const auto& dirEntry : std::filesystem::recursive_directory_iterator("Assets"))
		{
			if (dirEntry.is_regular_file())
			{
				auto path = dirEntry.path();
				auto extension = path.extension();

				if (extension == ".meta")
				{
					// Open the file as yaml
					if (std::ifstream ifs{ path })
					{
						std::string data((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
						ryml::Tree tree = ryml::parse_in_arena(ryml::to_csubstr(data));
						ryml::NodeRef node = tree.rootref();

						std::string guid;
						std::string type;
						node["m_GUID"] >> guid;
						node["m_Type"] >> type;

						std::filesystem::path assetPath = path.replace_extension("");

						if (std::filesystem::exists(assetPath))
						{
							s_AssetDatabaseGUIDs[guid] = assetPath;
							s_AssetDatabasePaths[assetPath] = guid;
							s_AssetTypeToGUIDs[type].push_back(guid);
						}
					}
				}
			}
		}

		s_DefaultVertexShader = LoadShader(s_DefaultVertexShaderPath);
		s_DefaultFragmentShader = LoadShader(s_DefaultFragmentShaderPath);
	}

	AssetHandle<Mesh> AssetManager::CreateMesh()
	{
		// Push back an empty mesh
		uint32_t id = s_Meshes.Add();
		Mesh* mesh = s_Meshes[id].get();

		// Set asset data
		mesh->SetGUID(GenerateGUID());
		mesh->SetName("Default");
		mesh->SetType("Mesh");

		return AssetHandle<Mesh>(id, mesh);
	}

	AssetHandle<Material> AssetManager::CreateMaterial(const std::filesystem::path& assetPath)
	{
		// Push back an empty material
		std::filesystem::path metaPath = AssetManager::GenerateMetaPath(assetPath);
		uint32_t id = s_Materials.Add(assetPath, metaPath);
		Material* material = s_Materials[id].get();

		// Set asset data
		material->SetGUID(GenerateGUID());
		material->SetName("Default");
		material->SetType("Material");

		// Assign default shaders
		material->SetVertexShader(s_DefaultVertexShader);
		material->SetFragmentShader(s_DefaultFragmentShader);

		// Save to disk
		material->Save();

		return AssetHandle<Material>(id, material);
	}

	AssetHandle<Mesh> AssetManager::CreateMesh(const std::filesystem::path& assetPath)
	{
		// Push back an empty mesh
		std::filesystem::path metaPath = AssetManager::GenerateMetaPath(assetPath);
		uint32_t id = s_Meshes.Add(assetPath, metaPath);
		Mesh* mesh = s_Meshes[id].get();

		// Set asset data
		mesh->SetGUID(GenerateGUID());
		mesh->SetName("Default");
		mesh->SetType("Mesh");

		// Save to disk
		mesh->Save();

		return AssetHandle<Mesh>(id, mesh);
	}

	AssetHandle<Shader> AssetManager::CreateShader(const std::filesystem::path& assetPath)
	{
		// Push back an empty mesh
		std::filesystem::path metaPath = AssetManager::GenerateMetaPath(assetPath);
		uint32_t id = s_Shaders.Add(assetPath, metaPath);
		Shader* shader = s_Shaders[id].get();

		// Set asset data
		shader->SetGUID(GenerateGUID());
		shader->SetName("Default");
		shader->SetType("Shader");

		// Save to disk
		shader->Save();

		return AssetHandle<Shader>(id, shader);
	}

	AssetHandle<Scene> AssetManager::CreateScene(const std::filesystem::path& assetPath)
	{
		// Push back an empty mesh
		std::filesystem::path metaPath = AssetManager::GenerateMetaPath(assetPath);
		uint32_t id = s_Scenes.Add(assetPath, metaPath);
		Scene* scene = s_Scenes[id].get();

		// Set asset data
		scene->SetGUID(GenerateGUID());
		scene->SetName("Scene");
		scene->SetType("Scene");

		// Save to disk
		scene->Save();

		return AssetHandle<Scene>(id, scene);
	}

	AssetHandle<Mesh> Odyssey::AssetManager::LoadMesh(const std::filesystem::path& assetPath)
	{
		// Push back a mesh loaded from the asset path
		std::filesystem::path metaPath = AssetManager::GenerateMetaPath(assetPath);
		uint32_t id = s_Meshes.Add(assetPath, metaPath);
		Mesh* mesh = s_Meshes[id].get();

		// Track the asset
		s_LoadedAssets[mesh->GetGUID()] = id;

		return AssetHandle<Mesh>(id, mesh);
	}

	AssetHandle<Shader> AssetManager::LoadShader(const std::filesystem::path& assetPath)
	{
		// Push back a shader loaded from the asset path
		std::filesystem::path metaPath = AssetManager::GenerateMetaPath(assetPath);
		uint32_t id = s_Shaders.Add(assetPath, metaPath);
		Shader* shader = s_Shaders[id].get();

		// Track the asset
		s_LoadedAssets[shader->GetGUID()] = id;

		return AssetHandle<Shader>(id, shader);
	}

	AssetHandle<Scene> AssetManager::LoadScene(const std::filesystem::path& assetPath)
	{
		//LoadTexture2D
		// Push back a scene loaded from the asset path
		std::filesystem::path metaPath = AssetManager::GenerateMetaPath(assetPath);
		uint32_t id = s_Scenes.Add(assetPath, metaPath);
		Scene* scene = s_Scenes[id].get();

		// Track the asset
		s_LoadedAssets[scene->GetGUID()] = id;

		return AssetHandle<Scene>(id, scene);
	}

	AssetHandle<Texture2D> AssetManager::LoadTexture2D(const std::filesystem::path& assetPath)
	{
		// Push back a scene loaded from the asset path
		std::filesystem::path metaPath = AssetManager::GenerateMetaPath(assetPath);
		uint32_t id = s_Textures.Add(assetPath, metaPath);
		Texture2D* texture = s_Textures[id].get();

		// Track the asset
		s_LoadedAssets[texture->GetGUID()] = id;

		return AssetHandle<Texture2D>(id, texture);
	}

	AssetHandle<Material> AssetManager::LoadMaterial(const std::filesystem::path& assetPath)
	{
		std::filesystem::path metaPath = AssetManager::GenerateMetaPath(assetPath);
		uint32_t id = s_Materials.Add(assetPath, metaPath);
		Material* material = s_Materials[id].get();
		s_LoadedAssets[material->GetGUID()] = id;

		return AssetHandle<Material>(id, material);
	}

	AssetHandle<Shader> AssetManager::LoadShaderByGUID(const std::string& guid)
	{
		// Check if we have already loaded this asset
		if (s_LoadedAssets.find(guid) != s_LoadedAssets.end())
		{
			// Return a handle
			uint32_t id = s_LoadedAssets[guid];
			return AssetHandle<Shader>(id, s_Shaders[id].get());
		}

		// Load it and return a handle
		std::filesystem::path path = s_AssetDatabaseGUIDs[guid];
		return AssetManager::LoadShader(path.generic_string());
	}

	AssetHandle<Texture2D> AssetManager::LoadTexture2DByGUID(const std::string& guid)
	{
		// Check if we have already loaded this asset
		if (s_LoadedAssets.find(guid) != s_LoadedAssets.end())
		{
			// Return a handle
			uint32_t id = s_LoadedAssets[guid];
			return AssetHandle<Texture2D>(id, s_Textures[id].get());
		}

		// Load it and return a handle
		std::filesystem::path path = s_AssetDatabaseGUIDs[guid];
		return AssetManager::LoadTexture2D(path.string());
	}

	std::vector<std::string> AssetManager::GetAssetsOfType(const std::string& type)
	{
		// TODO: insert return statement here
		if (s_AssetTypeToGUIDs.find(type) != s_AssetTypeToGUIDs.end())
			return s_AssetTypeToGUIDs[type];

		return std::vector<std::string>();
	}

	void AssetManager::UnloadScene(AssetHandle<Scene> scene)
	{
		const std::string& guid = scene.Get()->GetGUID();
		if (s_LoadedAssets.find(guid) != s_LoadedAssets.end())
		{
			s_LoadedAssets.erase(guid);
		}
		s_Scenes.Remove(scene.m_ID);
		scene.Reset();
	}

	std::string AssetManager::PathToGUID(const std::filesystem::path& path)
	{
		return s_AssetDatabasePaths[path];
	}

	AssetHandle<Material> AssetManager::LoadMaterialByGUID(const std::string& guid)
	{
		// Check if we have already loaded this asset
		if (s_LoadedAssets.find(guid) != s_LoadedAssets.end())
		{
			// Return a handle
			uint32_t id = s_LoadedAssets[guid];
			return AssetHandle<Material>(id, s_Materials[id].get());
		}

		// Load it and return a handle
		std::filesystem::path path = s_AssetDatabaseGUIDs[guid];
		return AssetManager::LoadMaterial(path.generic_string());
	}

	AssetHandle<Mesh> AssetManager::LoadMeshByGUID(const std::string& guid)
	{
		// Check if we have already loaded this asset
		if (s_LoadedAssets.find(guid) != s_LoadedAssets.end())
		{
			// Return a handle
			uint32_t id = s_LoadedAssets[guid];
			return AssetHandle<Mesh>(id, s_Meshes[id].get());
		}

		// Load it and return a handle
		std::filesystem::path path = s_AssetDatabaseGUIDs[guid];
		return AssetManager::LoadMesh(path.generic_string());
	}

	std::string AssetManager::GenerateGUID()
	{
		return s_GUIDGenerator.getUUID().str();
	}

	std::filesystem::path AssetManager::GenerateMetaPath(const std::filesystem::path& assetPath)
	{
		std::filesystem::path metaPath = assetPath;
		metaPath += ".meta";
		return metaPath;
	}
}