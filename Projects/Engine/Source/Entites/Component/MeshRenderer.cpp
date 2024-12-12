#include "MeshRenderer.h"
#include "Mesh.h"
#include "Material.h"
#include "AssetManager.h"

namespace Odyssey
{
	MeshRenderer::MeshRenderer(const GameObject& gameObject)
		: m_GameObject(gameObject)
	{
	}

	void MeshRenderer::Serialize(SerializationNode& node)
	{
		GUID mesh = m_Mesh ? m_Mesh->GetGUID().Ref() : 0;

		std::vector<uint64_t> materialGUIDs;
		materialGUIDs.resize(m_Materials.size());

		for (size_t i = 0; i < m_Materials.size(); i++)
		{
			materialGUIDs[i] = m_Materials[i]->GetGUID();
		}

		SerializationNode componentNode = node.AppendChild();
		componentNode.SetMap();
		componentNode.WriteData("Type", MeshRenderer::Type);
		componentNode.WriteData("Enabled", m_Enabled);
		componentNode.WriteData("Mesh", mesh.CRef());
		componentNode.WriteData("Materials", materialGUIDs);
	}

	void MeshRenderer::Deserialize(SerializationNode& node)
	{
		GUID mesh;
		std::vector<uint64_t> materials;

		node.ReadData("Enabled", m_Enabled);
		node.ReadData("Mesh", mesh.Ref());
		node.ReadData("Materials", materials);

		if (mesh)
			m_Mesh = AssetManager::LoadAsset<Mesh>(mesh);

		m_Materials.resize(materials.size());
		for (size_t i = 0; i < materials.size(); i++)
		{
			m_Materials[i] = AssetManager::LoadAsset<Material>(GUID(materials[i]));
		}
	}

	void MeshRenderer::SetEnabled(bool enabled)
	{
		m_Enabled = enabled;
	}

	void MeshRenderer::SetMesh(GUID meshGUID)
	{
		if (meshGUID)
			m_Mesh = AssetManager::LoadAsset<Mesh>(meshGUID);
		else
			m_Mesh.Reset();
	}

	void MeshRenderer::SetMaterial(GUID materialGUID, size_t submesh)
	{
		if (submesh >= m_Materials.size())
			m_Materials.insert(m_Materials.begin() + submesh, nullptr);

		if (materialGUID)
			m_Materials[submesh] = AssetManager::LoadAsset<Material>(materialGUID);
		else
			m_Materials[submesh].Reset();
	}
	void MeshRenderer::RemoveMaterial(int32_t index)
	{
		if (m_Materials.size() > 0)
		{
			if (index == -1)
				index = (int32_t)m_Materials.size() - 1;

			m_Materials.erase(m_Materials.begin() + index);
		}
	}
}