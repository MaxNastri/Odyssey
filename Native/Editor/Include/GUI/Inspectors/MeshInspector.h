#pragma once
#include "Inspector.h"
#include "AssetHandle.h"
#include "StringDrawer.h"
#include "AssetFieldDrawer.h"

namespace Odyssey
{
	class Mesh;

	class MeshInspector : public Inspector
	{
	public:
		MeshInspector() = default;
		MeshInspector(const std::string& guid);

	public:
		virtual void Draw() override;

	private:
		void OnNameChanged(const std::string& name);
		void OnSourceAssetChanged(const std::string& guid);

	private:
		AssetHandle<Mesh> m_Mesh;
		StringDrawer m_GUIDDrawer;
		StringDrawer m_NameDrawer;
		StringDrawer m_TypeDrawer;
		StringDrawer m_VertexCountDrawer;
		StringDrawer m_IndexCountDrawer;
		AssetFieldDrawer m_SourceMeshDrawer;
	};
}