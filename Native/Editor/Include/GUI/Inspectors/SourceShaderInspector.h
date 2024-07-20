#pragma once
#include "Inspector.h"
#include "AssetHandle.h"
#include "StringDrawer.h"
#include "BoolDrawer.h"

namespace Odyssey
{
	class SourceShader;

	class SourceShaderInspector : public Inspector
	{
	public:
		SourceShaderInspector() = default;
		SourceShaderInspector(const std::string& guid);

	public:
		virtual void Draw() override;

	private:
		AssetHandle<SourceShader> m_Shader;
		StringDrawer m_ShaderNameDrawer;
		StringDrawer m_ShaderLanguageDrawer;
		BoolDrawer m_CompiledDrawer;
	};
}