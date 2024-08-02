#pragma once
#include "ResourceHandle.h"
#include "AssetHandle.h"
#include "AssetSerializer.h"
#include "GameObject.h"

namespace Odyssey
{
	class Material;
	class Mesh;

	class MeshRenderer
	{
	public:
		MeshRenderer() = default;
		MeshRenderer(const GameObject& gameObject);
		MeshRenderer(AssetHandle<Mesh> mesh, AssetHandle<Material> material);

	public:
		void Serialize(SerializationNode& node);
		void Deserialize(SerializationNode& node);

	public:
		void SetMesh(AssetHandle<Mesh> mesh);
		void SetMaterial(AssetHandle<Material> material);

	public:
		AssetHandle<Mesh> GetMesh() { return m_Mesh; }
		AssetHandle<Material> GetMaterial() { return m_Material; }

	private:
		GameObject m_GameObject;
		AssetHandle<Mesh> m_Mesh;
		AssetHandle<Material> m_Material;
		CLASS_DECLARATION(MeshRenderer);
	};
}