#pragma once
#include "Inspector.h"
#include "AssetFieldDrawer.h"

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
		static void OnFragmentShaderModified(Material* material, const std::string& guid);
		static void OnVertexShaderModified(Material* material, const std::string& guid);

	private:
		Material* m_Material;
		AssetFieldDrawer m_FragmentShaderDrawer;
		AssetFieldDrawer m_VertexShaderDrawer;
	};
}