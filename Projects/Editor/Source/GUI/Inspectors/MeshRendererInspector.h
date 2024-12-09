#pragma once
#include "Inspector.h"
#include "PropertyDrawers.h"
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
		virtual bool Draw() override;

	private:
		GameObject m_GameObject;
		AssetFieldDrawer m_MeshDrawer;
		std::vector<AssetFieldDrawer> m_MaterialDrawers;
		bool m_MeshRendererEnabled;
	};
}