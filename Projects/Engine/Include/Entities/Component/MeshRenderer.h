#pragma once
#include "AssetSerializer.h"
#include "GameObject.h"
#include "Mesh.h"
#include "Material.h"

namespace Odyssey
{
	class MeshRenderer
	{
	public:
		MeshRenderer() = default;
		MeshRenderer(const GameObject& gameObject);

	public:
		void Serialize(SerializationNode& node);
		void Deserialize(SerializationNode& node);

	public:
		void SetEnabled(bool enabled);
		void SetMesh(GUID meshGUID);
		void SetMaterial(GUID materialGUID, size_t submesh = 0);

	public:
		bool IsEnabled() { return m_Enabled; }
		Ref<Mesh> GetMesh() { return m_Mesh; }
		Ref<Material> GetMaterial(size_t submesh = 0) { return m_Materials[submesh]; }
		std::vector<Ref<Material>>& GetMaterials() { return m_Materials; }

	private:
		bool m_Enabled = true;
		GameObject m_GameObject;
		Ref<Mesh> m_Mesh;
		std::vector<Ref<Material>> m_Materials;
		CLASS_DECLARATION(Odyssey, MeshRenderer)
	};
}