#include "AssetManager.h"
#include "Mesh.h"
#include "Material.h"
#include "Shader.h"
#include "Scene.h"
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

				if (extension == ".asset" || extension == ".mesh" || extension == ".shader" || extension == ".mat")
				{
					// Open the file as yaml
					if (std::ifstream ifs{ path })
					{
						std::string data((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
						ryml::Tree tree = ryml::parse_in_arena(ryml::to_csubstr(data));
						ryml::NodeRef node = tree.rootref();

						std::string guid;
						node["m_GUID"] >> guid;

						s_AssetDatabaseGUIDs[guid] = path;
						s_AssetDatabasePaths[path] = guid;
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

	AssetHandle<Material> AssetManager::CreateMaterial(const std::string& assetPath)
	{
		// Push back an empty material
		uint32_t id = s_Materials.Add();
		Material* material = s_Materials[id].get();

		// Set asset data
		material->SetGUID(GenerateGUID());
		material->SetName("Default");
		material->SetPath(assetPath);
		material->SetType("Material");

		// Assign default shaders
		material->SetVertexShader(s_DefaultVertexShader);
		material->SetFragmentShader(s_DefaultFragmentShader);

		// Save to disk
		material->Save();

		return AssetHandle<Material>(id, material);
	}

	AssetHandle<Mesh> AssetManager::CreateMesh(const std::string& assetPath)
	{
		// Push back an empty mesh
		uint32_t id = s_Meshes.Add();
		Mesh* mesh = s_Meshes[id].get();

		// Set asset data
		mesh->SetGUID(GenerateGUID());
		mesh->SetName("Default");
		mesh->SetPath(assetPath);
		mesh->SetType("Mesh");

		// Save to disk
		mesh->Save();

		return AssetHandle<Mesh>(id, mesh);
	}

	AssetHandle<Shader> AssetManager::CreateShader(const std::string& assetPath)
	{
		// Push back an empty mesh
		uint32_t id = s_Shaders.Add();
		Shader* shader = s_Shaders[id].get();

		// Set asset data
		shader->SetGUID(GenerateGUID());
		shader->SetName("Default");
		shader->SetPath(assetPath);
		shader->SetType("Shader");

		// Save to disk
		shader->Save();

		return AssetHandle<Shader>(id, shader);
	}

	AssetHandle<Scene> AssetManager::CreateScene(const std::string& assetPath)
	{
		// Push back an empty mesh
		uint32_t id = s_Scenes.Add();
		Scene* scene = s_Scenes[id].get();

		// Set asset data
		scene->SetGUID(GenerateGUID());
		scene->SetName("Scene");
		scene->SetPath(assetPath);
		scene->SetType("Scene");

		// Save to disk
		scene->Save();

		return AssetHandle<Scene>(id, scene);
	}

	AssetHandle<Mesh> Odyssey::AssetManager::LoadMesh(const std::string& assetPath)
	{
		// Push back a mesh loaded from the asset path
		uint32_t id = s_Meshes.Add(assetPath);
		Mesh* mesh = s_Meshes[id].get();

		// Track the asset
		s_LoadedAssets[mesh->GetGUID()] = id;

		return AssetHandle<Mesh>(id, mesh);
	}

	AssetHandle<Shader> AssetManager::LoadShader(const std::string& assetPath)
	{
		// Push back a shader loaded from the asset path
		uint32_t id = s_Shaders.Add(assetPath);
		Shader* shader = s_Shaders[id].get();

		// Track the asset
		s_LoadedAssets[shader->GetGUID()] = id;

		return AssetHandle<Shader>(id, shader);
	}

	AssetHandle<Scene> AssetManager::LoadScene(const std::string& assetPath)
	{
		// Push back a scene loaded from the asset path
		uint32_t id = s_Scenes.Add(assetPath);
		Scene* scene = s_Scenes[id].get();

		if (scene->GetGUID() == "")
			scene->SetGUID(GenerateGUID());

		if (scene->GetAssetPath() == "")
			scene->SetPath(assetPath);

		// Track the asset
		s_LoadedAssets[scene->GetGUID()] = id;

		return AssetHandle<Scene>(id, scene);
	}

	AssetHandle<Material> AssetManager::LoadMaterial(const std::string& assetPath)
	{
		uint32_t id = s_Materials.Add(assetPath);
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

	std::string AssetManager::PathToGUID(std::filesystem::path path)
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
}