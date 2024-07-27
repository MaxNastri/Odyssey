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
		MeshRenderer() = default;
		MeshRenderer(AssetHandle<Mesh> mesh, AssetHandle<Material> material);

	public:
		virtual void Serialize(SerializationNode& node) override;
		virtual void Deserialize(SerializationNode& node) override;

	public:
		void SetMesh(AssetHandle<Mesh> mesh);
		void SetMaterial(AssetHandle<Material> material);

	public:
		AssetHandle<Mesh> GetMesh() { return m_Mesh; }
		AssetHandle<Material> GetMaterial() { return m_Material; }

	private:
		AssetHandle<Mesh> m_Mesh;
		AssetHandle<Material> m_Material;
		CLASS_DECLARATION(MeshRenderer);
	};
}