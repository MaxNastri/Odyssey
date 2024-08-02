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

	MeshRenderer::MeshRenderer(AssetHandle<Mesh> mesh, AssetHandle<Material> material)
	{
		m_Mesh = mesh;
		m_Material = material;
	}

	void MeshRenderer::Serialize(SerializationNode& node)
	{
		SerializationNode componentNode = node.AppendChild();
		componentNode.SetMap();
		componentNode.WriteData("Type", MeshRenderer::Type);
		componentNode.WriteData("m_Mesh", m_Mesh.Get()->GetGUID());
		componentNode.WriteData("m_Material", m_Material.Get()->GetGUID());
	}

	void MeshRenderer::Deserialize(SerializationNode& node)
	{
		std::string meshGUID;;
		std::string materialGUID;

		node.ReadData("m_Mesh", meshGUID);
		node.ReadData("m_Material", materialGUID);

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