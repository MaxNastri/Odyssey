#include "MeshRenderer.h"
#include "Mesh.h"
#include "Material.h"
#include "AssetManager.h"

namespace Odyssey
{
	CLASS_DEFINITION(Odyssey, MeshRenderer);

	MeshRenderer::MeshRenderer(const GameObject& gameObject)
		: m_GameObject(gameObject)
	{
	}

	void MeshRenderer::Serialize(SerializationNode& node)
	{
		SerializationNode componentNode = node.AppendChild();
		componentNode.SetMap();
		componentNode.WriteData("Type", MeshRenderer::Type);
		componentNode.WriteData("m_Mesh", m_Mesh.CRef());
		componentNode.WriteData("m_Material", m_Material.CRef());
	}

	void MeshRenderer::Deserialize(SerializationNode& node)
	{
		node.ReadData("m_Mesh", m_Mesh.Ref());
		node.ReadData("m_Material", m_Material.Ref());
	}

	void MeshRenderer::SetMesh(GUID meshGUID)
	{
		m_Mesh = meshGUID;
	}

	void MeshRenderer::SetMaterial(GUID materialGUID)
	{
		m_Material = materialGUID;
	}
}