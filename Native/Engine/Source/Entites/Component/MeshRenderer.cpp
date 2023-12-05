#include "MeshRenderer.h"
#include "Mesh.h"
#include "Material.h"
#include "AssetManager.h"

namespace Odyssey
{
	CLASS_DEFINITION(Odyssey, MeshRenderer);

	MeshRenderer::MeshRenderer(AssetHandle<Mesh> mesh, AssetHandle<Material> material)
	{
		m_Mesh = mesh;
		m_Material = material;
	}

	void MeshRenderer::Serialize(ryml::NodeRef& node)
	{
		ryml::NodeRef componentNode = node.append_child();
		componentNode |= ryml::MAP;
		
		componentNode["Name"] << MeshRenderer::Type;
		componentNode["m_Mesh"] << m_Mesh.Get()->GetUUID();
		componentNode["m_Material"] << m_Material.Get()->GetUUID();
	}

	void MeshRenderer::Deserialize(ryml::ConstNodeRef node)
	{
		std::string meshGUID;;
		std::string materialGUID;

		node["m_Mesh"] >> meshGUID;
		node["m_Material"] >> materialGUID;

		m_Material = AssetManager::LoadMaterialByGUID(materialGUID);
		m_Mesh = AssetManager::LoadMeshByGUID(meshGUID);
	}

	void MeshRenderer::SetMesh(AssetHandle<Mesh> mesh)
	{
		m_Mesh = mesh;
	}

	void MeshRenderer::SetMaterial(AssetHandle<Material> material)
	{
		m_Material = material;
	}
}