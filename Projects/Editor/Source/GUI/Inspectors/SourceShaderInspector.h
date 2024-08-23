#pragma once
#include "Inspector.h"
#include "StringDrawer.h"
#include "IntDrawer.h"
#include "BoolDrawer.h"

namespace Odyssey
{
	class SourceShader;

	class SourceShaderInspector : public Inspector
	{
	public:
		SourceShaderInspector() = default;
		SourceShaderInspector(GUID guid);

	public:
		virtual void Draw() override;

	private:
		void OnDstAssetPathChanged(const std::string& assetPath) { m_DstAssetPath = assetPath; }

	private:
		std::shared_ptr<SourceShader> m_Shader;
		std::string m_DstAssetPath;
		StringDrawer m_ShaderNameDrawer;
		StringDrawer m_ShaderLanguageDrawer;
		IntDrawer<uint32_t> m_ShaderTypeDrawer;
		StringDrawer m_DstAssetPathDrawer;
		BoolDrawer m_CompiledDrawer;
	};
}