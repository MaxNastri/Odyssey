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
		MaterialInspector(Material* material);
		MaterialInspector(GUID guid);

	public:
		virtual void Draw() override;

	private:
		static void OnNameModified(Material* material, const std::string& name);
		static void OnFragmentShaderModified(Material* material, GUID guid);
		static void OnVertexShaderModified(Material* material, GUID guid);
		static void OnTextureModified(Material* material, GUID guid);

	private:
		Material* m_Material;

		StringDrawer m_NameDrawer;
		StringDrawer m_GUIDDrawer;
		AssetFieldDrawer m_FragmentShaderDrawer;
		AssetFieldDrawer m_VertexShaderDrawer;
		AssetFieldDrawer m_TextureDrawer;
		bool m_Modified = false;
	};
}