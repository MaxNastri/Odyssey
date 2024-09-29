#pragma once
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

	public:
		void Serialize(SerializationNode& node);
		void Deserialize(SerializationNode& node);

	public:
		void SetMesh(GUID meshGUID);
		void SetMaterial(GUID materialGUID);

	public:
		GUID GetMesh() { return m_Mesh; }
		GUID GetMaterial() { return m_Material; }

	private:
		GameObject m_GameObject;
		GUID m_Mesh;
		GUID m_Material;
		CLASS_DECLARATION(Odyssey, MeshRenderer)
	};
}