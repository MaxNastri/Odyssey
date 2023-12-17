#pragma once
#include "Inspector.h"
#include "AssetFieldDrawer.h"

namespace Odyssey
{
	class GameObject;
	class MeshRenderer;

	class MeshRendererInspector : public Inspector
	{
	public:
		MeshRendererInspector() = default;
		MeshRendererInspector(GameObject* gameObject);

	public:
		virtual void Draw() override;

	private:
		static void OnMeshModified(GameObject* gameObject, const std::string& guid);

	private:
		GameObject* m_GameObject;
		MeshRenderer* m_MeshRenderer;
		AssetFieldDrawer m_MeshDrawer;
	};
}