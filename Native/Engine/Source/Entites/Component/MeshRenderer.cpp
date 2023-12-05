#include "MeshRenderer.h"
#include "Mesh.h"

namespace Odyssey
{
	CLASS_DEFINITION(Odyssey, MeshRenderer);

	MeshRenderer::MeshRenderer(AssetHandle<Mesh> mesh, ResourceHandle<Material> material)
	{
		m_Mesh = mesh;
		m_Material = material;
	}

	void MeshRenderer::Serialize(ryml::NodeRef& node)
	{
		ryml::NodeRef componentNode = node.append_child();
		componentNode |= ryml::MAP;

		componentNode["Name"] << MeshRenderer::Type;
		componentNode["Mesh"] << m_Mesh.Get()->GetUUID();
	}

	void MeshRenderer::Deserialize(ryml::ConstNodeRef node)
	{

	}

	void MeshRenderer::SetMesh(AssetHandle<Mesh> mesh)
	{
		m_Mesh = mesh;
	}

	void MeshRenderer::SetMaterial(ResourceHandle<Material> material)
	{
		m_Material = material;
	}
}