#pragma once
#include "Inspector.h"
#include "AssetFieldDrawer.h"
#include "StringDrawer.h"
#include "IntDrawer.h"

namespace Odyssey
{
	class Shader;

	class ShaderInspector : public Inspector
	{
	public:
		ShaderInspector() = default;
		ShaderInspector(GUID guid);

	public:
		virtual void Draw() override;

	private:
		void OnNameChanged(const std::string& name);
		void OnSourceAssetChanged(GUID sourceGUID);

	private:
		std::shared_ptr<Shader> m_Shader;
		StringDrawer m_GUIDDrawer;
		StringDrawer m_NameDrawer;
		StringDrawer m_TypeDrawer;
		StringDrawer m_ShaderCodeDrawer;
		IntDrawer<uint32_t> m_ShaderTypeDrawer;
		AssetFieldDrawer m_SourceShaderDrawer;
	};
}