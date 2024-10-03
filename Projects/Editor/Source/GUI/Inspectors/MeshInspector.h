#pragma once
#include "Inspector.h"
#include "Mesh.h"
#include "PropertyDrawers.h"

namespace Odyssey
{
	class MeshInspector : public Inspector
	{
	public:
		MeshInspector() = default;
		MeshInspector(GUID guid);

	public:
		virtual void Draw() override;

	private:
		void OnNameChanged(std::string_view name);
		void OnSourceAssetChanged(GUID sourceGUID);

	private:
		std::shared_ptr<Mesh> m_Mesh;
		StringDrawer m_GUIDDrawer;
		StringDrawer m_NameDrawer;
		StringDrawer m_TypeDrawer;
		StringDrawer m_VertexCountDrawer;
		StringDrawer m_IndexCountDrawer;
		AssetFieldDrawer m_SourceMeshDrawer;
	};
}