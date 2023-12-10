#include "AssetManager.h"
#include "Mesh.h"
#include "Material.h"
#include "Shader.h"
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

						s_AssetDatabase[guid] = path;
					}
				}
			}
		}

		s_DefaultVertexShader = LoadShader(s_DefaultVertexShaderPath);
		s_DefaultFragmentShader = LoadShader(s_DefaultFragmentShaderPath);
	}

	AssetHandle<Material> AssetManager::CreateMaterial(const std::string& assetPath)
	{
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
		material->Save(assetPath);

		return AssetHandle<Material>(id, material);
	}

	AssetHandle<Mesh> AssetManager::CreateMesh(const std::string& assetPath)
	{
		// Create an empty mesh at the desired location

		uint32_t id = s_Meshes.Add();
		Mesh* mesh = s_Meshes[id].get();

		// Set asset data
		mesh->SetGUID(GenerateGUID());
		mesh->SetName("Default");
		mesh->SetPath(assetPath);
		mesh->SetType("Mesh");

		// Save to disk
		mesh->Save(assetPath);

		return AssetHandle<Mesh>(id, mesh);
	}

	AssetHandle<Mesh> Odyssey::AssetManager::LoadMesh(const std::string& path)
	{
		uint32_t id = s_Meshes.Add(path);
		Mesh* mesh = s_Meshes[id].get();
		s_LoadedAssets[mesh->GetGUID()] = id;

		return AssetHandle<Mesh>(id, mesh);
	}

	AssetHandle<Shader> AssetManager::LoadShader(const std::string& filename)
	{
		uint32_t id = s_Shaders.Add(filename);
		Shader* shader = s_Shaders[id].get();
		s_LoadedAssets[shader->GetGUID()] = id;

		return AssetHandle<Shader>(id, shader);
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
		std::filesystem::path path = s_AssetDatabase[guid];
		return AssetManager::LoadShader(path.generic_string());
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
		std::filesystem::path path = s_AssetDatabase[guid];
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
		std::filesystem::path path = s_AssetDatabase[guid];
		return AssetManager::LoadMesh(path.generic_string());
	}

	std::string AssetManager::GenerateGUID()
	{
		return s_GUIDGenerator.getUUID().str();
	}
}