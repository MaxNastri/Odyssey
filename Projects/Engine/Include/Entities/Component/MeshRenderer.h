#pragma once
#include "ResourceHandle.h"
#include "AssetSerializer.h"
#include "GameObject.h"
#include "FileID.h"

namespace Odyssey
{
	class Material;
	class Mesh;

	class MeshRenderer
	{
	public:
		MeshRenderer() = default;
		MeshRenderer(const GameObject& gameObject);
		MeshRenderer(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material);

	public:
		void Serialize(SerializationNode& node);
		void Deserialize(SerializationNode& node);

	public:
		void SetMesh(std::shared_ptr<Mesh> mesh);
		void SetMaterial(std::shared_ptr<Material> material);

	public:
		std::shared_ptr<Mesh> GetMesh() { return m_Mesh; }
		std::shared_ptr<Material> GetMaterial() { return m_Material; }

	private:
		FileID m_FileID;
		GameObject m_GameObject;
		std::shared_ptr<Mesh> m_Mesh;
		std::shared_ptr<Material> m_Material;
		CLASS_DECLARATION(MeshRenderer);
	};
}