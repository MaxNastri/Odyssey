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
		virtual void Draw() override;

	private:
		void OnNameModified(std::string_view name);
		void OnShaderModified(GUID guid);
		void OnTextureModified(GUID guid);

	private:
		std::shared_ptr<Material> m_Material;

		StringDrawer m_NameDrawer;
		StringDrawer m_GUIDDrawer;
		AssetFieldDrawer m_ShaderDrawer;
		AssetFieldDrawer m_TextureDrawer;
		bool m_Modified = false;
	};
}