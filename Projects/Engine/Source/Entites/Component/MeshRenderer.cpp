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
		GUID material = m_Material ? m_Material->GetGUID().Ref() : 0;

		SerializationNode componentNode = node.AppendChild();
		componentNode.SetMap();
		componentNode.WriteData("Type", MeshRenderer::Type);
		componentNode.WriteData("Enabled", m_Enabled);
		componentNode.WriteData("m_Mesh", mesh.CRef());
		componentNode.WriteData("m_Material", material.CRef());
	}

	void MeshRenderer::Deserialize(SerializationNode& node)
	{
		GUID mesh;
		GUID material;

		node.ReadData("Enabled", m_Enabled);
		node.ReadData("m_Mesh", mesh.Ref());
		node.ReadData("m_Material", material.Ref());

		if (mesh)
			m_Mesh = AssetManager::LoadAsset<Mesh>(mesh);

		if (material)
			m_Material = AssetManager::LoadAsset<Material>(material);
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

	void MeshRenderer::SetMaterial(GUID materialGUID)
	{
		if (materialGUID)
			m_Material = AssetManager::LoadAsset<Material>(materialGUID);
		else
			m_Material.Reset();
	}
}