#include "AssetManager.h"

namespace Odyssey
{
	AssetHandle<Mesh> Odyssey::AssetManager::LoadMesh(const std::string& path)
	{
		uint32_t id = m_Meshes.Add(path);
		Mesh* mesh = m_Meshes[id].get();

		mesh->SetUUID(GenerateUUID());
		mesh->SetPath(path);
		mesh->SetType("Mesh");

		return AssetHandle<Mesh>(id, mesh);
	}

	std::string AssetManager::GenerateUUID()
	{
		return uuidGenerator.getUUID().bytes();
	}
}