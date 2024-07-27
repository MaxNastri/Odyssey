#pragma once
#include "Inspector.h"
#include "AssetFieldDrawer.h"
#include "StringDrawer.h"

namespace Odyssey
{
	class Material;

	class MaterialInspector : public Inspector
	{
	public:
		MaterialInspector() = default;
		MaterialInspector(Material* material);
		MaterialInspector(const std::string& guid);

	public:
		virtual void Draw() override;

	private:
		static void OnNameModified(Material* material, const std::string& name);
		static void OnFragmentShaderModified(Material* material, const std::string& guid);
		static void OnVertexShaderModified(Material* material, const std::string& guid);
		static void OnTextureModified(Material* material, const std::string& guid);

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