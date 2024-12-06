#pragma once
#include "Inspector.h"
#include "PropertyDrawers.h"
#include "Material.h"

namespace Odyssey
{
	class MaterialInspector : public Inspector
	{
	public:
		MaterialInspector() = default;
		MaterialInspector(GUID guid);

	public:
		virtual bool Draw() override;

	private:
		Ref<Material> m_Material;
		bool m_Dirty = false;

		StringDrawer m_NameDrawer;
		StringDrawer m_GUIDDrawer;
		AssetFieldDrawer m_ShaderDrawer;
		AssetFieldDrawer m_ColorTextureDrawer;
		AssetFieldDrawer m_NormalTextureDrawer;
	};
}