#pragma once
#include "Inspector.h"
#include "Shader.h"
#include "PropertyDrawers.h"

namespace Odyssey
{
	class ShaderInspector : public Inspector
	{
	public:
		ShaderInspector() = default;
		ShaderInspector(GUID guid);

	public:
		virtual bool Draw() override;

	private:
		void OnNameChanged(std::string_view name);
		void OnSourceAssetChanged(GUID sourceGUID);

	private:
		std::shared_ptr<Shader> m_Shader;
		StringDrawer m_GUIDDrawer;
		StringDrawer m_NameDrawer;
		IntDrawer<uint32_t> m_ShaderTypeDrawer;
		AssetFieldDrawer m_SourceShaderDrawer;
	};
}