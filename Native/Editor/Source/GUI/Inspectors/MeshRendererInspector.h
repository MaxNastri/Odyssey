#pragma once
#include "Inspector.h"
#include "AssetFieldDrawer.h"
#include "GameObject.h"

namespace Odyssey
{
	class GameObject;
	class MeshRenderer;

	class MeshRendererInspector : public Inspector
	{
	public:
		MeshRendererInspector() = default;
		MeshRendererInspector(GameObject& gameObject);

	public:
		virtual void Draw() override;

	private:
		void OnMeshModified(const std::string& guid);
		void OnMaterialModified(const std::string& guid);

	private:
		GameObject m_GameObject;
		AssetFieldDrawer m_MeshDrawer;
		AssetFieldDrawer m_MaterialDrawer;
	};
}