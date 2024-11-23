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
		void SetMaterial(GUID materialGUID);

	public:
		bool IsEnabled() { return m_Enabled; }
		Ref<Mesh> GetMesh() { return m_Mesh; }
		Ref<Material> GetMaterial() { return m_Material; }

	private:
		bool m_Enabled = true;
		GameObject m_GameObject;
		Ref<Mesh> m_Mesh;
		Ref<Material> m_Material;
		CLASS_DECLARATION(Odyssey, MeshRenderer)
	};
}