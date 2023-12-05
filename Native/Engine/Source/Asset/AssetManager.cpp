#include "AssetManager.h"
#include "Mesh.h"
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

				if (extension == ".asset" || extension == ".mesh" || extension == ".shader")
				{
					// Open the file as yaml
					if (std::ifstream ifs{ path })
					{
						std::string data((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
						ryml::Tree tree = ryml::parse_in_arena(ryml::to_csubstr(data));
						ryml::NodeRef node = tree.rootref();

						std::string uuid;
						node["UUID"] >> uuid;

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

		mesh->SetUUID(GenerateUUID());
		mesh->SetPath(path);
		mesh->SetType("Mesh");

		return AssetHandle<Mesh>(id, mesh);
	}

	AssetHandle<Shader> AssetManager::LoadShader(const std::string& path)
	{
		uint32_t id = m_Shaders.Add(path);
		Shader* shader = m_Shaders[id].get();

		shader->SetUUID(GenerateUUID());
		shader->SetPath(path);
		shader->SetType("Shader");

		return AssetHandle<Shader>(id, shader);
	}

	std::string AssetManager::GenerateUUID()
	{
		return uuidGenerator.getUUID().str();
	}
}