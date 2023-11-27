#include "MeshRenderer.h"

namespace Odyssey
{
	CLASS_DEFINITION(Odyssey, MeshRenderer);

	MeshRenderer::MeshRenderer(ResourceHandle<Mesh> mesh, ResourceHandle<Material> material)
	{
		m_Mesh = mesh;
		m_Material = material;
	}

	void MeshRenderer::Serialize(ryml::NodeRef& node)
	{
		ryml::NodeRef componentNode = node.append_child();
		componentNode |= ryml::MAP;

		componentNode["Name"] << MeshRenderer::Type;
	}

	void MeshRenderer::Deserialize(ryml::ConstNodeRef node)
	{

	}

	void MeshRenderer::SetMesh(ResourceHandle<Mesh> mesh)
	{
		m_Mesh = mesh;
	}

	void MeshRenderer::SetMaterial(ResourceHandle<Material> material)
	{
		m_Material = material;
	}
}