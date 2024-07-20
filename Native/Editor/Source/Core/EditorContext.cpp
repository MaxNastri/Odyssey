#include "EditorContext.h"

namespace Odyssey
{
	EditorContext::EditorContext()
	{
		ShaderCompiler::CompilerSettings shaderOptions;
		shaderOptions.Optimize = false;
		m_GUIManager = std::make_unique<GUIManager>();
	}

	EditorContext* EditorContext::GetInstance()
	{
		// Static local variable initialization is thread-safe
		// and will be initialized only once.
		static EditorContext instance{};
		return &instance;
	}
}