#include "EditorContext.h"

namespace Odyssey
{
	EditorContext::EditorContext()
	{
		ShaderCompiler::Options shaderOptions;
		shaderOptions.Optimize = false;
		m_ShaderCompiler = std::make_unique<ShaderCompiler>(shaderOptions);
	}

	EditorContext* EditorContext::GetInstance()
	{
		// Static local variable initialization is thread-safe
		// and will be initialized only once.
		static EditorContext instance{};
		return &instance;
	}
}