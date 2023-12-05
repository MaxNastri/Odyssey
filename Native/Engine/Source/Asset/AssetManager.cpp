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

						std::string uuid;
						node["m_UUID"] >> uuid;

						s_AssetDatabase[uuid] = path;
					}
				}
			}
		}
	}

	AssetHandle<Mesh> Odyssey::AssetManager::LoadMesh(const std::string& path)
	{
		uint32_t id = m_Meshes.Add(path);
		Mesh* mesh = m_Meshes[id].get();
		s_LoadedAssets[mesh->GetUUID()] = id;

		return AssetHandle<Mesh>(id, mesh);
	}

	AssetHandle<Shader> AssetManager::LoadShader(const std::string& filename)
	{
		uint32_t id = m_Shaders.Add(filename);
		Shader* shader = m_Shaders[id].get();
		s_LoadedAssets[shader->GetUUID()] = id;

		return AssetHandle<Shader>(id, shader);
	}

	AssetHandle<Material> AssetManager::LoadMaterial(const std::string& assetPath)
	{
		uint32_t id = s_Materials.Add(assetPath);
		Material* material = s_Materials[id].get();
		s_LoadedAssets[material->GetUUID()] = id;

		return AssetHandle<Material>(id, material);
	}

	AssetHandle<Shader> AssetManager::LoadShaderByUUID(const std::string& uuid)
	{
		// Check if we have already loaded this asset
		if (s_LoadedAssets.find(uuid) != s_LoadedAssets.end())
		{
			// Return a handle
			uint32_t id = s_LoadedAssets[uuid];
			return AssetHandle<Shader>(id, m_Shaders[id].get());
		}

		// Load it and return a handle
		std::filesystem::path path = s_AssetDatabase[uuid];
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
			return AssetHandle<Mesh>(id, m_Meshes[id].get());
		}

		// Load it and return a handle
		std::filesystem::path path = s_AssetDatabase[guid];
		return AssetManager::LoadMesh(path.generic_string());
	}

	std::string AssetManager::GenerateUUID()
	{
		return uuidGenerator.getUUID().str();
	}
}