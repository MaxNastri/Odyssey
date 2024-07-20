#pragma once
#include "Inspector.h"
#include "StringDrawer.h"

namespace Odyssey
{
	class SourceShaderInspector : public Inspector
	{
	public:
		SourceShaderInspector() = default;
		SourceShaderInspector(const std::filesystem::path& sourcePath);

	public:
		virtual void Draw() override;

	private:
		std::filesystem::path m_SourcePath;
		ReadOnlyStringDrawer m_SourceDrawer;
		ReadOnlyStringDrawer m_CompileDrawer;
		bool m_Compiled = false;
	};
}