#pragma once
#include "Inspector.h"
#include "AssetFieldDrawer.h"
#include "StringDrawer.h"
#include "GUID.h"

namespace Odyssey
{
	class Material;

	class MaterialInspector : public Inspector
	{
	public:
		MaterialInspector() = default;
		MaterialInspector(GUID guid);

	public:
		virtual void Draw() override;

	private:
		void OnNameModified(const std::string& name);
		void OnFragmentShaderModified(GUID guid);
		void OnVertexShaderModified(GUID guid);
		void OnTextureModified(GUID guid);

	private:
		std::shared_ptr<Material> m_Material;

		StringDrawer m_NameDrawer;
		StringDrawer m_GUIDDrawer;
		AssetFieldDrawer m_FragmentShaderDrawer;
		AssetFieldDrawer m_VertexShaderDrawer;
		AssetFieldDrawer m_TextureDrawer;
		bool m_Modified = false;
	};
}