#pragma once
#include "Component.h"
#include "ResourceHandle.h"
#include "AssetHandle.h"

namespace Odyssey
{
	class Material;
	class Mesh;

	class MeshRenderer : public Component
	{
	public:
		MeshRenderer(AssetHandle<Mesh> mesh, ResourceHandle<Material> material);

	public:
		virtual void Serialize(ryml::NodeRef& node) override;
		virtual void Deserialize(ryml::ConstNodeRef node) override;

	public:
		void SetMesh(AssetHandle<Mesh> mesh);
		void SetMaterial(ResourceHandle<Material> material);

	public:
		AssetHandle<Mesh> GetMesh() { return m_Mesh; }
		ResourceHandle<Material> GetMaterial() { return m_Material; }

	private:
		AssetHandle<Mesh> m_Mesh;
		ResourceHandle<Material> m_Material;
		CLASS_DECLARATION(MeshRenderer);
	};
}